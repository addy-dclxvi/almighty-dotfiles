/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "drw.h"
#include "util.h"

#define UTF_INVALID 0xFFFD
#define UTF_SIZ     4

static const unsigned char utfbyte[UTF_SIZ + 1] = {0x80,    0, 0xC0, 0xE0, 0xF0};
static const unsigned char utfmask[UTF_SIZ + 1] = {0xC0, 0x80, 0xE0, 0xF0, 0xF8};
static const long utfmin[UTF_SIZ + 1] = {       0,    0,  0x80,  0x800,  0x10000};
static const long utfmax[UTF_SIZ + 1] = {0x10FFFF, 0x7F, 0x7FF, 0xFFFF, 0x10FFFF};

static long
utf8decodebyte(const char c, size_t *i)
{
	for (*i = 0; *i < (UTF_SIZ + 1); ++(*i))
		if (((unsigned char)c & utfmask[*i]) == utfbyte[*i])
			return (unsigned char)c & ~utfmask[*i];
	return 0;
}

static size_t
utf8validate(long *u, size_t i)
{
	if (!BETWEEN(*u, utfmin[i], utfmax[i]) || BETWEEN(*u, 0xD800, 0xDFFF))
		*u = UTF_INVALID;
	for (i = 1; *u > utfmax[i]; ++i)
		;
	return i;
}

static size_t
utf8decode(const char *c, long *u, size_t clen)
{
	size_t i, j, len, type;
	long udecoded;

	*u = UTF_INVALID;
	if (!clen)
		return 0;
	udecoded = utf8decodebyte(c[0], &len);
	if (!BETWEEN(len, 1, UTF_SIZ))
		return 1;
	for (i = 1, j = 1; i < clen && j < len; ++i, ++j) {
		udecoded = (udecoded << 6) | utf8decodebyte(c[i], &type);
		if (type)
			return j;
	}
	if (j < len)
		return 0;
	*u = udecoded;
	utf8validate(u, len);

	return len;
}

Drw *
drw_create(Display *dpy, int screen, Window root, unsigned int w, unsigned int h)
{
	Drw *drw;

	drw = ecalloc(1, sizeof(Drw));
	drw->dpy = dpy;
	drw->screen = screen;
	drw->root = root;
	drw->w = w;
	drw->h = h;
	drw->drawable = XCreatePixmap(dpy, root, w, h, DefaultDepth(dpy, screen));
	drw->gc = XCreateGC(dpy, root, 0, NULL);
	drw->fontcount = 0;
	XSetLineAttributes(dpy, drw->gc, 1, LineSolid, CapButt, JoinMiter);

	return drw;
}

void
drw_resize(Drw *drw, unsigned int w, unsigned int h)
{
	drw->w = w;
	drw->h = h;
	if (drw->drawable)
		XFreePixmap(drw->dpy, drw->drawable);
	drw->drawable = XCreatePixmap(drw->dpy, drw->root, w, h, DefaultDepth(drw->dpy, drw->screen));
}

void
drw_free(Drw *drw)
{
	size_t i;

	for (i = 0; i < drw->fontcount; i++)
		drw_font_free(drw->fonts[i]);
	XFreePixmap(drw->dpy, drw->drawable);
	XFreeGC(drw->dpy, drw->gc);
	free(drw);
}

/* This function is an implementation detail. Library users should use
 * drw_font_create instead.
 */
static Fnt *
drw_font_xcreate(Drw *drw, const char *fontname, FcPattern *fontpattern)
{
	Fnt *font;
	XftFont *xfont = NULL;
	FcPattern *pattern = NULL;

	if (fontname) {
		/* Using the pattern found at font->xfont->pattern does not yield same
		 * the same substitution results as using the pattern returned by
		 * FcNameParse; using the latter results in the desired fallback
		 * behaviour whereas the former just results in
		 * missing-character-rectangles being drawn, at least with some fonts.
		 */
		if (!(xfont = XftFontOpenName(drw->dpy, drw->screen, fontname))) {
			fprintf(stderr, "error, cannot load font: '%s'\n", fontname);
			return NULL;
		}
		if (!(pattern = FcNameParse((FcChar8 *) fontname))) {
			fprintf(stderr, "error, cannot load font: '%s'\n", fontname);
			XftFontClose(drw->dpy, xfont);
			return NULL;
		}
	} else if (fontpattern) {
		if (!(xfont = XftFontOpenPattern(drw->dpy, fontpattern))) {
			fprintf(stderr, "error, cannot load font pattern.\n");
			return NULL;
		}
	} else {
		die("no font specified.\n");
	}

	font = ecalloc(1, sizeof(Fnt));
	font->xfont = xfont;
	font->pattern = pattern;
	font->ascent = xfont->ascent;
	font->descent = xfont->descent;
	font->h = font->ascent + font->descent;
	font->dpy = drw->dpy;

	return font;
}

Fnt*
drw_font_create(Drw *drw, const char *fontname)
{
	return drw_font_xcreate(drw, fontname, NULL);
}

void
drw_load_fonts(Drw* drw, const char *fonts[], size_t fontcount)
{
	size_t i;
	Fnt *font;

	for (i = 0; i < fontcount; i++) {
		if (drw->fontcount >= DRW_FONT_CACHE_SIZE) {
			die("font cache exhausted.\n");
		} else if ((font = drw_font_xcreate(drw, fonts[i], NULL))) {
			drw->fonts[drw->fontcount++] = font;
		}
	}
}

void
drw_font_free(Fnt *font)
{
	if (!font)
		return;
	if (font->pattern)
		FcPatternDestroy(font->pattern);
	XftFontClose(font->dpy, font->xfont);
	free(font);
}

Clr *
drw_clr_create(Drw *drw, const char *clrname)
{
	Clr *clr;

	clr = ecalloc(1, sizeof(Clr));
	if (!XftColorAllocName(drw->dpy, DefaultVisual(drw->dpy, drw->screen),
	                       DefaultColormap(drw->dpy, drw->screen),
	                       clrname, &clr->rgb))
		die("error, cannot allocate color '%s'\n", clrname);
	clr->pix = clr->rgb.pixel;

	return clr;
}

void
drw_clr_free(Clr *clr)
{
	free(clr);
}

void
drw_setscheme(Drw *drw, ClrScheme *scheme)
{
	drw->scheme = scheme;
}

int
drw_get_width(Drw *drw, int numcolors, const char *text)
{
	int i;
	Fnt *curfont = drw->fonts[0];
	int w = drw_text(drw, 0, 0, 0, 0, text, 0) + curfont->h;

	for (i = 0; i < strlen(text); i++) {
		if (text[i] > 0 && text[i] <= numcolors) {
			/* we found a color code
			 * drw_text counted it as a normal character and added one character's width
			 * we aren't going to render this character, so we remove one character's width */
			w -= curfont->xfont->max_advance_width;

			if (i == 0 || i + 1 == strlen(text)) {
				/* we're on the first or the last character of the string
				 * drw_text already added one character's height (divided by 2) as padding to the beginning and end
				 * we don't want to double this padding, so we skip this character */
				continue;
			}

			if (text[i - 1] > 0 && text[i - 1] <= numcolors) {
				/* the previous character was also a color code
				 * we already added padding in the previous iteration
				 * we don't want to double this padding, so we skip this character */
				continue;
			}

			/* we are somewhere in the middle of the string and the color has changed
			 * we want to add one character's height (divided by 2) as padding to the end of the previous colored text
			 * and to the beginning of the new colored text */
			w += curfont->h;
		}
	}

  return w;
}

void
drw_colored_text(Drw *drw, ClrScheme *scheme, int numcolors, int x, int y, unsigned int w, unsigned int h, char *text)
{
	if (!drw || !drw->fontcount || !drw->scheme)
		return;

	char *buf = text, *ptr = buf, c = 1;
	int i;

	while (*ptr) {
		for (i = 0; *ptr < 0 || *ptr > numcolors; i++, ptr++);
		if (!*ptr)
			break;
		c = *ptr;
		*ptr = 0;
		if (i)
			x = drw_text(drw, x, y, w, h, buf, 0) + drw->fonts[0]->h;
		*ptr = c;
		drw_setscheme(drw, &scheme[c-1]);
		buf = ++ptr;
	}
	drw_text(drw, x, y, w, h, buf, 0);
}

void
drw_rect(Drw *drw, int x, int y, unsigned int w, unsigned int h, int filled, int empty, int invert)
{
	if (!drw->scheme)
		return;
	XSetForeground(drw->dpy, drw->gc, invert ? drw->scheme->bg->pix : drw->scheme->fg->pix);
	if (filled)
		XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w + 1, h + 1);
	else if (empty)
		XDrawRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
}

int
drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, const char *text, int invert)
{
	char buf[1024];
	int tx, ty, th;
	Extnts tex;
	XftDraw *d = NULL;
	Fnt *curfont, *nextfont;
	size_t i, len;
	int utf8strlen, utf8charlen, render;
	long utf8codepoint = 0;
	const char *utf8str;
	FcCharSet *fccharset;
	FcPattern *fcpattern;
	FcPattern *match;
	XftResult result;
	int charexists = 0;

	if (!drw->scheme || !drw->fontcount)
		return 0;

	if (!(render = x || y || w || h)) {
		w = ~w;
	} else {
		XSetForeground(drw->dpy, drw->gc, invert ?
		               drw->scheme->fg->pix : drw->scheme->bg->pix);
		XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
		d = XftDrawCreate(drw->dpy, drw->drawable,
		                  DefaultVisual(drw->dpy, drw->screen),
		                  DefaultColormap(drw->dpy, drw->screen));
	}

	curfont = drw->fonts[0];
	while (1) {
		utf8strlen = 0;
		utf8str = text;
		nextfont = NULL;
		while (*text) {
			utf8charlen = utf8decode(text, &utf8codepoint, UTF_SIZ);
			for (i = 0; i < drw->fontcount; i++) {
				charexists = charexists || XftCharExists(drw->dpy, drw->fonts[i]->xfont, utf8codepoint);
				if (charexists) {
					if (drw->fonts[i] == curfont) {
						utf8strlen += utf8charlen;
						text += utf8charlen;
					} else {
						nextfont = drw->fonts[i];
					}
					break;
				}
			}

			if (!charexists || (nextfont && nextfont != curfont))
				break;
			else
				charexists = 0;
		}

		if (utf8strlen) {
			drw_font_getexts(curfont, utf8str, utf8strlen, &tex);
			/* shorten text if necessary */
			for (len = MIN(utf8strlen, (sizeof buf) - 1); len && (tex.w > w - drw->fonts[0]->h || w < drw->fonts[0]->h); len--)
				drw_font_getexts(curfont, utf8str, len, &tex);

			if (len) {
				memcpy(buf, utf8str, len);
				buf[len] = '\0';
				if (len < utf8strlen)
					for (i = len; i && i > len - 3; buf[--i] = '.');

				if (render) {
					th = curfont->ascent + curfont->descent;
					ty = y + (h / 2) - (th / 2) + curfont->ascent;
					tx = x + (h / 2);
					XftDrawStringUtf8(d, invert ? &drw->scheme->bg->rgb : &drw->scheme->fg->rgb, curfont->xfont, tx, ty, (XftChar8 *)buf, len);
				}
				x += tex.w;
				w -= tex.w;
			}
		}

		if (!*text) {
			break;
		} else if (nextfont) {
			charexists = 0;
			curfont = nextfont;
		} else {
			/* Regardless of whether or not a fallback font is found, the
			 * character must be drawn.
			 */
			charexists = 1;

			if (drw->fontcount >= DRW_FONT_CACHE_SIZE)
				continue;

			fccharset = FcCharSetCreate();
			FcCharSetAddChar(fccharset, utf8codepoint);

			if (!drw->fonts[0]->pattern) {
				/* Refer to the comment in drw_font_xcreate for more
				 * information. */
				die("the first font in the cache must be loaded from a font string.\n");
			}

			fcpattern = FcPatternDuplicate(drw->fonts[0]->pattern);
			FcPatternAddCharSet(fcpattern, FC_CHARSET, fccharset);
			FcPatternAddBool(fcpattern, FC_SCALABLE, FcTrue);

			FcConfigSubstitute(NULL, fcpattern, FcMatchPattern);
			FcDefaultSubstitute(fcpattern);
			match = XftFontMatch(drw->dpy, drw->screen, fcpattern, &result);

			FcCharSetDestroy(fccharset);
			FcPatternDestroy(fcpattern);

			if (match) {
				curfont = drw_font_xcreate(drw, NULL, match);
				if (curfont && XftCharExists(drw->dpy, curfont->xfont, utf8codepoint)) {
					drw->fonts[drw->fontcount++] = curfont;
				} else {
					drw_font_free(curfont);
					curfont = drw->fonts[0];
				}
			}
		}
	}
	if (d)
		XftDrawDestroy(d);

	return x;
}

void
drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h)
{
	XCopyArea(drw->dpy, drw->drawable, win, drw->gc, x, y, w, h, x, y);
	XSync(drw->dpy, False);
}

void
drw_font_getexts(Fnt *font, const char *text, unsigned int len, Extnts *tex)
{
	XGlyphInfo ext;

	XftTextExtentsUtf8(font->dpy, font->xfont, (XftChar8 *)text, len, &ext);
	tex->h = font->h;
	tex->w = ext.xOff;
}

unsigned int
drw_font_getexts_width(Fnt *font, const char *text, unsigned int len)
{
	Extnts tex;

	drw_font_getexts(font, text, len, &tex);

	return tex.w;
}

Cur *
drw_cur_create(Drw *drw, int shape)
{
	Cur *cur;

	cur = ecalloc(1, sizeof(Cur));
	cur->cursor = XCreateFontCursor(drw->dpy, shape);

	return cur;
}

void
drw_cur_free(Drw *drw, Cur *cursor)
{
	if (!cursor)
		return;
	XFreeCursor(drw->dpy, cursor->cursor);
	free(cursor);
}
