/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#include "draw.h"

#define INTERSECT(x,y,w,h,r)  (MAX(0, MIN((x)+(w),(r).x_org+(r).width)  - MAX((x),(r).x_org)) \
                             * MAX(0, MIN((y)+(h),(r).y_org+(r).height) - MAX((y),(r).y_org)))
#define MIN(a,b)              ((a) < (b) ? (a) : (b))
#define MAX(a,b)              ((a) > (b) ? (a) : (b))
#define DEFFONT "fixed" /* xft example: "Monospace-11" */

typedef struct Item Item;
struct Item {
	char *text;
	Item *left, *right;
};

static void appenditem(Item *item, Item **list, Item **last);
static void calcoffsets(void);
static void cleanup(void);
static char *cistrstr(const char *s, const char *sub);
static void drawmenu(void);
static void grabkeyboard(void);
static void insert(const char *str, ssize_t n);
static void keypress(XKeyEvent *ev);
static void matchstr(void);
static void matchtok(void);
static void matchfuzzy(void);
static char *strchri(const char *s, int c);
static size_t nextrune(int inc);
static size_t utf8length();
static void paste(void);
static void readstdin(void);
static void run(void);
static void setup(void);
static void usage(void);
static void read_resourses(void);
static char text[BUFSIZ] = "";
static char originaltext[BUFSIZ] = "";
static int bh, mw, mh;
static int inputw, promptw;
static size_t cursor = 0;
static const char *font = NULL;
static const char *prompt = NULL;
static const char *normbgcolor = NULL;
static const char *normfgcolor = NULL;
static const char *selbgcolor  = NULL;
static const char *selfgcolor  = NULL;
static const char *dimcolor = NULL; 
static char *name = "dmenu";
static char *class = "Dmenu";
static char *dimname = "dimenu";
static unsigned int lines = 0, line_height = 0;
static int xoffset = 0;
static int yoffset = 0;
static int width = 0;
#ifdef XINERAMA
static int snum = -1;
#endif
static ColorSet *normcol;
static ColorSet *selcol;
static ColorSet *dimcol;
static Atom clip, utf8;
static Bool topbar = True;
static Bool running = True;
static Bool filter = False;
static Bool maskin = False;
static Bool noinput = False;
static int ret = 0;
static Bool quiet = False;
static DC *dc;
static Item *items = NULL;
static Item *matches, *matchend;
static Item *prev, *curr, *next, *sel;
static Window win, dim;
static XIC xic;
static double opacity = 1.0, dimopacity = 0.0;

#define OPAQUE 0xffffffff
#define OPACITY "_NET_WM_WINDOW_OPACITY"

static int (*fstrncmp)(const char *, const char *, size_t) = strncmp;
static char *(*fstrstr)(const char *, const char *) = strstr;
static void (*match)(void) = matchstr;
static char *(*fstrchr)(const char *, const int) = strchr;

int
main(int argc, char *argv[]) {
	Bool fast = False;
	int i;

	for(i = 1; i < argc; i++)
		/* these options take no arguments */
		if(!strcmp(argv[i], "-v")) {      /* prints version information */
			puts("dmenu-"VERSION", © 2006-2012 dmenu engineers, see LICENSE for details");
			exit(EXIT_SUCCESS);
		}
		else if(!strcmp(argv[i], "-b"))   /* appears at the bottom of the screen */
			topbar = False;
 		else if(!strcmp(argv[i], "-q"))
 			quiet = True;
		else if(!strcmp(argv[i], "-f"))   /* grabs keyboard before reading stdin */
			fast = True;
		else if(!strcmp(argv[i], "-z"))   /* enable fuzzy matching */
			match = matchfuzzy;
 		else if(!strcmp(argv[i], "-r"))
 			filter = True;
		else if(!strcmp(argv[i], "-i")) { /* case-insensitive item matching */
			fstrncmp = strncasecmp;
			fstrstr = cistrstr;
			fstrchr = strchri;
		}
      else if(!strcmp(argv[i], "-mask")) /* password-style input */
         maskin = True;
      else if(!strcmp(argv[i], "-noinput"))
         noinput = True;

		else if(!strcmp(argv[i], "-t"))
			match = matchtok;
		else if(i+1 == argc)
			usage();
		/* these options take one argument */
 		else if(!strcmp(argv[i], "-x"))
 			xoffset = atoi(argv[++i]);
 		else if(!strcmp(argv[i], "-y"))
 			yoffset = atoi(argv[++i]);
 		else if(!strcmp(argv[i], "-w"))
 			width = atoi(argv[++i]);
		else if(!strcmp(argv[i], "-l"))   /* number of lines in vertical list */
			lines = atoi(argv[++i]);
		else if(!strcmp(argv[i], "-h"))   /* minimum height of single line */
			line_height = atoi(argv[++i]);
		#ifdef XINERAMA
		else if(!strcmp(argv[i], "-s"))   /* screen number for dmenu to appear in */
			snum = atoi(argv[++i]);
		#endif
		else if (!strcmp(argv[i], "-name")) /* dmenu window name */
			name = argv[++i];
		else if (!strcmp(argv[i], "-class")) /* dmenu window class */
			class = argv[++i];
		else if (!strcmp(argv[i], "-o"))  /* opacity */
			opacity = atof(argv[++i]);
		else if (!strcmp(argv[i], "-dim"))  /* dim opacity */
			dimopacity = atof(argv[++i]);	
		else if (!strcmp(argv[i], "-dc")) /* dim color */
			dimcolor = argv[++i];
		else if(!strcmp(argv[i], "-p"))   /* adds prompt to left of input field */
			prompt = argv[++i];
		else if(!strcmp(argv[i], "-fn"))  /* font or font set */
			font = argv[++i];
		else if(!strcmp(argv[i], "-nb"))  /* normal background color */
			normbgcolor = argv[++i];
		else if(!strcmp(argv[i], "-nf"))  /* normal foreground color */
			normfgcolor = argv[++i];
		else if(!strcmp(argv[i], "-sb"))  /* selected background color */
			selbgcolor = argv[++i];
		else if(!strcmp(argv[i], "-sf"))  /* selected foreground color */
			selfgcolor = argv[++i];
		else
			usage();

	dc = initdc();
 	read_resourses();
	initfont(dc, font ? font : DEFFONT);
	normcol = initcolor(dc, normfgcolor, normbgcolor);
	selcol = initcolor(dc, selfgcolor, selbgcolor);
	dimcol = initcolor(dc, dimcolor, dimcolor);

   if(noinput) {
      grabkeyboard();
   }
   else if(fast) {
      grabkeyboard();
      readstdin();
   }
   else {
      readstdin();
      grabkeyboard();
   }
	setup();
	run();

	cleanup();
	return ret;
}

/* Set font and colors from X resources database if they are not set
 * from command line */
void
read_resourses(void) {
	XrmDatabase xdb;
	char* xrm;
	char* datatype[20];
	XrmValue xvalue;

	XrmInitialize();
	xrm = XResourceManagerString(dc->dpy);
	if( xrm != NULL ) {
		xdb = XrmGetStringDatabase(xrm);
		if( font == NULL && XrmGetResource(xdb, "dmenu.font", "*", datatype, &xvalue) == True )
			font = strdup(xvalue.addr);
		if( normfgcolor == NULL && XrmGetResource(xdb, "dmenu.foreground", "*", datatype, &xvalue) == True )
			normfgcolor = strdup(xvalue.addr);
		if( normbgcolor == NULL && XrmGetResource(xdb, "dmenu.background", "*", datatype, &xvalue) == True )
			normbgcolor = strdup(xvalue.addr);
		if( selfgcolor == NULL && XrmGetResource(xdb, "dmenu.selforeground", "*", datatype, &xvalue) == True )
			selfgcolor = strdup(xvalue.addr);
		if( selbgcolor == NULL && XrmGetResource(xdb, "dmenu.selbackground", "*", datatype, &xvalue) == True )
			selbgcolor = strdup(xvalue.addr);
		if( dimcolor == NULL && XrmGetResource(xdb, "dmenu.dimcolor", "*", datatype, &xvalue) == True )
			dimcolor = strdup(xvalue.addr);
		if( XrmGetResource(xdb, "dmenu.opacity", "*", datatype, &xvalue) == True )
			opacity = atof(strdup(xvalue.addr));
		XrmDestroyDatabase(xdb);
	}
	/* Set default colors if they are not set */
	if( normbgcolor == NULL )
		normbgcolor = "#2f2d38";
	if( normfgcolor == NULL )
		normfgcolor = "#ffffff";
	if( selbgcolor == NULL )
		selbgcolor  = "#8b97b1";
	if( selfgcolor == NULL )
		selfgcolor  = "#ffffff";
	if( dimcolor == NULL )
		dimcolor = "#000000";
	if( !opacity )
		opacity = 1.0;
}

void
appenditem(Item *item, Item **list, Item **last) {
	if(*last)
		(*last)->right = item;
	else
		*list = item;

	item->left = *last;
	item->right = NULL;
	*last = item;
}

void
calcoffsets(void) {
	int i, n;

	if(lines > 0)
		n = lines * bh;
	else
		n = mw - (promptw + inputw + textw(dc, "<") + textw(dc, ">"));
	/* calculate which items will begin the next page and previous page */
	for(i = 0, next = curr; next; next = next->right)
		if((i += (lines > 0) ? bh : MIN(textw(dc, next->text), n)) > n)
			break;
	for(i = 0, prev = curr; prev && prev->left; prev = prev->left)
		if((i += (lines > 0) ? bh : MIN(textw(dc, prev->left->text), n)) > n)
			break;
}

char *
cistrstr(const char *s, const char *sub) {
	size_t len;

	for(len = strlen(sub); *s; s++)
		if(!strncasecmp(s, sub, len))
			return (char *)s;
	return NULL;
}

void
cleanup(void) {
    freecol(dc, normcol);
    freecol(dc, selcol);
		freecol(dc, dimcol);
    XDestroyWindow(dc->dpy, win);
		if(dimopacity > 0)
			XDestroyWindow(dc->dpy, dim);
    XUngrabKeyboard(dc->dpy, CurrentTime);
    freedc(dc);
}

const char *
createmaskinput(char *maskinput, int length)
{
   if (length <= 0) {
      *maskinput = '\0';
   } else {
      memset(maskinput, '*', length);
      maskinput[length] = '\0';
   }

   return (maskinput);
}

void
drawmenu(void) {
	int curpos;
   char maskinput[sizeof text];
   int length = maskin ? utf8length() : cursor;
	Item *item;

	dc->x = 0;
	dc->y = 0;
	dc->h = bh;
	drawrect(dc, 0, 0, mw, mh, True, normcol->BG);

	if(prompt && *prompt) {
		dc->w = promptw;
		drawtext(dc, prompt, selcol);
		dc->x = dc->w;
	}


	/* draw input field */
	dc->w = (lines > 0 || !matches) ? mw - dc->x : inputw;
	drawtext(dc, maskin ? createmaskinput(maskinput, length) : text, normcol);
	if((curpos = textnw(dc, maskin ? maskinput : text, length) + dc->font.height/2) < dc->w)
		drawrect(dc, curpos, (dc->h - dc->font.height)/2 + 1, 1, dc->font.height -1, True, normcol->FG);

    if(!quiet || strlen(text) > 0) {    
        if(lines > 0) {
            /* draw vertical list */
            dc->w = mw - dc->x;
            for(item = curr; item != next; item = item->right) {
                dc->y += dc->h;
                drawtext(dc, item->text, (item == sel) ? selcol : normcol);
            }
        }
        else if(matches) {
            /* draw horizontal list */
            dc->x += inputw;
            dc->w = textw(dc, "<");
            if(curr->left)
                drawtext(dc, "<", normcol);
            for(item = curr; item != next; item = item->right) {
                dc->x += dc->w;
                dc->w = MIN(textw(dc, item->text), mw - dc->x - textw(dc, ">"));
                drawtext(dc, item->text, (item == sel) ? selcol : normcol);
            }
            dc->w = textw(dc, ">");
            dc->x = mw - dc->w;
            if(next)
                drawtext(dc, ">", normcol);
        }
    }
	mapdc(dc, win, mw, mh);
}

void
grabkeyboard(void) {
	int i;

	/* try to grab keyboard, we may have to wait for another process to ungrab */
	for(i = 0; i < 1000; i++) {
		if(XGrabKeyboard(dc->dpy, DefaultRootWindow(dc->dpy), True,
		                 GrabModeAsync, GrabModeAsync, CurrentTime) == GrabSuccess)
			return;
		usleep(1000);
	}
	eprintf("cannot grab keyboard\n");
}

void
insert(const char *str, ssize_t n) {
	if(strlen(text) + n > sizeof text - 1)
		return;
	/* move existing text out of the way, insert new text, and update cursor */
	memmove(&text[cursor + n], &text[cursor], sizeof text - cursor - MAX(n, 0));
	if(n > 0)
		memcpy(&text[cursor], str, n);
	cursor += n;
	match();
}

void
keypress(XKeyEvent *ev) {
	char buf[32];
	int len;
	KeySym ksym = NoSymbol;
	Status status;

	len = XmbLookupString(xic, ev, buf, sizeof buf, &ksym, &status);
	if(status == XBufferOverflow)
		return;
	if(ev->state & ControlMask)
		switch(ksym) {
		case XK_a: ksym = XK_Home;      break;
		case XK_b: ksym = XK_Left;      break;
		case XK_c: ksym = XK_Escape;    break;
		case XK_d: ksym = XK_Delete;    break;
		case XK_e: ksym = XK_End;       break;
		case XK_f: ksym = XK_Right;     break;
		case XK_g: ksym = XK_Escape;    break;
		case XK_h: ksym = XK_BackSpace; break;
		case XK_i: ksym = XK_Tab;       break;
		case XK_j: /* fallthrough */
		case XK_J: ksym = XK_Return;    break;
		case XK_m: /* fallthrough */
		case XK_M: ksym = XK_Return;    break;
		case XK_n: ksym = XK_Down;      break;
		case XK_p: ksym = XK_Up;        break;

		case XK_k: /* delete right */
			text[cursor] = '\0';
			match();
			break;
		case XK_u: /* delete left */
			insert(NULL, 0 - cursor);
			break;
		case XK_w: /* delete word */
			while(cursor > 0 && text[nextrune(-1)] == ' ')
				insert(NULL, nextrune(-1) - cursor);
			while(cursor > 0 && text[nextrune(-1)] != ' ' && text[nextrune(-1)] != '/')
				insert(NULL, nextrune(-1) - cursor);
			break;
		case XK_y: /* fallthrough */
		case XK_Y: /* paste selection */
			XConvertSelection(dc->dpy, (ev->state & ShiftMask) ? clip : XA_PRIMARY,
			                  utf8, utf8, win, CurrentTime);
			return;
		default:
			return;
		}
	else if(ev->state & Mod1Mask)
		switch(ksym) {
		case XK_g: ksym = XK_Home;  break;
		case XK_G: ksym = XK_End;   break;
		case XK_h: ksym = XK_Up;    break;
		case XK_j: ksym = XK_Next;  break;
		case XK_k: ksym = XK_Prior; break;
		case XK_l: ksym = XK_Down;  break;
		default:
			return;
		}
	switch(ksym) {
	default:
		if(!iscntrl(*buf))
         insert(buf, len);
		break;
	case XK_Delete:
		if(text[cursor] == '\0')
			return;
		cursor = nextrune(+1);
		/* fallthrough */
	case XK_BackSpace:
		if(cursor == 0)
			return;
		insert(NULL, nextrune(-1) - cursor);
		break;
	case XK_End:
		if(text[cursor] != '\0') {
			cursor = strlen(text);
			break;
		}
		if(next) {
			/* jump to end of list and position items in reverse */
			curr = matchend;
			calcoffsets();
			curr = prev;
			calcoffsets();
			while(next && (curr = curr->right))
				calcoffsets();
		}
		sel = matchend;
		break;
	case XK_Escape:
        ret = EXIT_FAILURE;
        running = False;
	case XK_Home:
		if(sel == matches) {
			cursor = 0;
			break;
		}
		sel = curr = matches;
		calcoffsets();
		break;
	case XK_Left:
		if(cursor > 0 && (!sel || !sel->left || lines > 0)) {
			cursor = nextrune(-1);
			break;
		}
		if(lines > 0)
			return;
		/* fallthrough */
	case XK_Up:
		if(sel && sel->left && (sel = sel->left)->right == curr) {
			curr = prev;
			calcoffsets();
		}
		break;
	case XK_Next:
		if(!next)
			return;
		sel = curr = next;
		calcoffsets();
		break;
	case XK_Prior:
		if(!prev)
			return;
		sel = curr = prev;
		calcoffsets();
		break;
	case XK_Return:
	case XK_KP_Enter:
 		if((ev->state & ShiftMask) || !sel)
 			puts(text);
 		else if(!filter)
 			puts(sel->text);
 		else {
 			for(Item *item = sel; item; item = item->right)
 				puts(item->text);
 			for(Item *item = matches; item != sel; item = item->right)
 				puts(item->text);
 		}
		ret = EXIT_SUCCESS;
		running = False;
	case XK_Right:
		if(text[cursor] != '\0') {
			cursor = nextrune(+1);
			break;
		}
		if(lines > 0)
			return;
		/* fallthrough */
	case XK_Down:
		if(sel && sel->right && (sel = sel->right) == next) {
			curr = next;
			calcoffsets();
		}
		break;
	case XK_Tab:
		if(!sel)
			return;
		if(strcmp(text, sel->text)) {
			strncpy(originaltext, text, sizeof originaltext);
			strncpy(text, sel->text, sizeof text);
			cursor = strlen(text);
		} else {
			if(sel && sel->right) {
				sel = sel->right;
				strncpy(text, sel->text, sizeof text);
				cursor = strlen(text);
			}
			else {
				strncpy(text, originaltext, sizeof text);
				cursor = strlen(text);
				match();
			}
		}
		break;
	case XK_ISO_Left_Tab:
		if(!sel)
			return;
		if(strcmp(text, sel->text)) {
			sel = matchend;
			strncpy(originaltext, text, sizeof originaltext);
			strncpy(text, sel->text, sizeof text);
			cursor = strlen(text);
		} else {
			if(sel && sel->left) {
				sel = sel->left;
				strncpy(text, sel->text, sizeof text);
				cursor = strlen(text);
			}
			else {
				strncpy(text, originaltext, sizeof text);
				cursor = strlen(text);
				match();
			}
		} 
		break;
	}
	drawmenu();
}

char *
strchri(const char *s, int c) {
	char *u, *l;
	if(!isalpha(c)) return strchr(s, c);
	if(isupper(c)) {
		u = strchr(s, c);
		l = strchr(s, tolower(c));
	}
	else {
		l = strchr(s, c);
		u = strchr(s, toupper(c));
	}
}



void
matchstr(void) {
	static char **tokv = NULL;
	static int tokn = 0;

	char buf[sizeof text], *s;
	int i, tokc = 0;
	size_t len;
	Item *item, *lprefix, *lsubstr, *prefixend, *substrend;

	strcpy(buf, text);
	/* separate input text into tokens to be matched individually */
	for(s = strtok(buf, " "); s; tokv[tokc-1] = s, s = strtok(NULL, " "))
		if(++tokc > tokn && !(tokv = realloc(tokv, ++tokn * sizeof *tokv)))
			eprintf("cannot realloc %u bytes\n", tokn * sizeof *tokv);
	len = tokc ? strlen(tokv[0]) : 0;

	matches = lprefix = lsubstr = matchend = prefixend = substrend = NULL;
	for(item = items; item && item->text; item++) {
		for(i = 0; i < tokc; i++)
			if(!fstrstr(item->text, tokv[i]))
				break;
		if(i != tokc) /* not all tokens match */
			continue;
		/* exact matches go first, then prefixes, then substrings */
		if(!tokc || !fstrncmp(tokv[0], item->text, len+1))
			appenditem(item, &matches, &matchend);
		else if(!fstrncmp(tokv[0], item->text, len))
			appenditem(item, &lprefix, &prefixend);
		else
			appenditem(item, &lsubstr, &substrend);
	}
	if(lprefix) {
		if(matches) {
			matchend->right = lprefix;
			lprefix->left = matchend;
		}
		else
			matches = lprefix;
		matchend = prefixend;
	}
	if(lsubstr) {
		if(matches) {
			matchend->right = lsubstr;
			lsubstr->left = matchend;
		}
		else
			matches = lsubstr;
		matchend = substrend;
	}
	curr = sel = matches;
	calcoffsets();
}

void
matchtok(void) {
	char buf[sizeof text];
	char **tokv, *s;
	int tokc, i;
	Item *item, *end;

	tokc = 0;
	tokv = NULL;
	strcpy(buf, text);
	for(s = strtok(buf, " "); s; tokv[tokc-1] = s, s = strtok(NULL, " "))
		if(!(tokv = realloc(tokv, ++tokc * sizeof *tokv)))
			eprintf("cannot realloc %u bytes\n", tokc * sizeof *tokv);

	matches = end = NULL;
	for(item = items; item->text; item++) {
		for(i = 0; i < tokc; i++)
			if(!fstrstr(item->text, tokv[i]))
				break;
		if(i == tokc)
			appenditem(item, &matches, &end);
	}
	free(tokv);
	curr = prev = next = sel = matches;
	calcoffsets();
}

void
matchfuzzy(void) {
	int i;
	size_t len;
	Item *item;
	char *pos;
	
	len = strlen(text);
	matches = matchend = NULL;
	for(item = items; item && item->text; item++) {
		i = 0;
		for(pos = fstrchr(item->text, text[i]); pos && text[i]; i++, pos = fstrchr(pos+1, text[i]));
		if(i == len) appenditem(item, &matches, &matchend);
	}

	curr = sel = matches;
	calcoffsets();
}

size_t
nextrune(int inc) {
	ssize_t n;

	/* return location of next utf8 rune in the given direction (+1 or -1) */
	for(n = cursor + inc; n + inc >= 0 && (text[n] & 0xc0) == 0x80; n += inc);
	return n;
}

/* UTF-8 length for password */
size_t
utf8length()
{
   ssize_t n = cursor - 1, length = 0;

   while (n >= 0) {
      for (; n - 1 >= 0 && (text[n] & 0xc0) == 0x80; n--)
         ;
      n--;
      length++;
   }

   return (length);
}

void
paste(void) {
	char *p, *q;
	int di;
	unsigned long dl;
	Atom da;

	/* we have been given the current selection, now insert it into input */
	XGetWindowProperty(dc->dpy, win, utf8, 0, (sizeof text / 4) + 1, False,
	                   utf8, &da, &di, &dl, &dl, (unsigned char **)&p);
	insert(p, (q = strchr(p, '\n')) ? q-p : (ssize_t)strlen(p));
	XFree(p);
	drawmenu();
}

void
readstdin(void) {
	char buf[sizeof text], *p, *maxstr = NULL;
	size_t i, max = 0, size = 0;

	/* read each line from stdin and add it to the item list */
	for(i = 0; fgets(buf, sizeof buf, stdin); i++) {
		if(i+1 >= size / sizeof *items)
			if(!(items = realloc(items, (size += BUFSIZ))))
				eprintf("cannot realloc %u bytes:", size);
		if((p = strchr(buf, '\n')))
			*p = '\0';
		if(!(items[i].text = strdup(buf)))
			eprintf("cannot strdup %u bytes:", strlen(buf)+1);
		if(strlen(items[i].text) > max)
			max = strlen(maxstr = items[i].text);
	}
	if(items)
		items[i].text = NULL;
	inputw = maxstr ? textw(dc, maxstr) : 0;
	lines = MIN(lines, i);
}

void
run(void) {
	XEvent ev;

	while(running && !XNextEvent(dc->dpy, &ev)) {
		if(XFilterEvent(&ev, win))
			continue;
		switch(ev.type) {
		case Expose:
			if(ev.xexpose.count == 0)
				mapdc(dc, win, mw, mh);
			break;
		case KeyPress:
			keypress(&ev.xkey);
			break;
		case SelectionNotify:
			if(ev.xselection.property == utf8)
				paste();
			break;
		case VisibilityNotify:
			if(ev.xvisibility.state != VisibilityUnobscured)
				XRaiseWindow(dc->dpy, win);
			break;
		}
	}
}

void
setup(void) {
	int x, y, screen = DefaultScreen(dc->dpy);
	Screen *defScreen = DefaultScreenOfDisplay(dc->dpy);
	int dimx, dimy, dimw, dimh;
	Window root = RootWindow(dc->dpy, screen);
	XSetWindowAttributes swa;
	XIM xim;

#ifdef XINERAMA
	int n;
	XineramaScreenInfo *info;
#endif

	clip = XInternAtom(dc->dpy, "CLIPBOARD",   False);
	utf8 = XInternAtom(dc->dpy, "UTF8_STRING", False);

	/* calculate menu geometry */
	bh = (line_height > dc->font.height + 2) ? line_height : dc->font.height + 2;
	lines = MAX(lines, 0);
	mh = (lines + 1) * bh;
#ifdef XINERAMA
	if((info = XineramaQueryScreens(dc->dpy, &n))) {
		int a, j, di, i = 0, area = 0;
		unsigned int du;
		Window w, pw, dw, *dws;
		XWindowAttributes wa;

		if(snum > -1 && snum < n) {
			x = info[snum].x_org;
			y = info[snum].y_org + (topbar ? yoffset : info[i].height - mh - yoffset);
			mw = info[snum].width;
			
			dimx = info[snum].x_org;
			dimy = info[snum].y_org;
			dimw = info[snum].width;
			dimh = info[snum].height;
		}
		else {
			XGetInputFocus(dc->dpy, &w, &di);
			if(w != root && w != PointerRoot && w != None) {
				/* find top-level window containing current input focus */
				do {
					if(XQueryTree(dc->dpy, (pw = w), &dw, &w, &dws, &du) && dws)
						XFree(dws);
				} while(w != root && w != pw);
				/* find xinerama screen with which the window intersects most */
				if(XGetWindowAttributes(dc->dpy, pw, &wa))
					for(j = 0; j < n; j++)
						if((a = INTERSECT(wa.x, wa.y, wa.width, wa.height, info[j])) > area) {
							area = a;
							i = j;
						}
			}
			/* no focused window is on screen, so use pointer location instead */
			if(!area && XQueryPointer(dc->dpy, root, &dw, &dw, &x, &y, &di, &di, &du))
				for(i = 0; i < n; i++)
					if(INTERSECT(x, y, 1, 1, info[i]))
						break;

			x = info[i].x_org;
			y = info[i].y_org + (topbar ? yoffset : info[i].height - mh - yoffset);
			mw = info[i].width;

			dimx = info[i].x_org;
			dimy = info[i].y_org;
			dimw = info[i].width;
			dimh = info[i].height;
		}
		XFree(info);
	}
	else
#endif
	{
		x = 0;
		y = topbar ? 0 : DisplayHeight(dc->dpy, screen) - mh - yoffset;
		mw = DisplayWidth(dc->dpy, screen);
		
		dimx = 0;
		dimy = 0;
		dimw = WidthOfScreen(defScreen); 
		dimh = HeightOfScreen(defScreen);
	}

	x += xoffset;
	mw = width ? width : mw;
	promptw = (prompt && *prompt) ? textw(dc, prompt) : 0;
	inputw = MIN(inputw, mw/3);
	match();
	
	swa.override_redirect = True;
	
	/* create dim window */
	if(dimopacity > 0) {
		swa.background_pixel = dimcol->BG;
		swa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
		dim = XCreateWindow(dc->dpy, root, dimx, dimy, dimw, dimh, 0,
	                    DefaultDepth(dc->dpy, screen), CopyFromParent,
	                    DefaultVisual(dc->dpy, screen),
	                    CWOverrideRedirect | CWBackPixel | CWEventMask, &swa);
		XClassHint dimhint = { .res_name = dimname, .res_class = class };
  	XSetClassHint(dc->dpy, dim, &dimhint);
  
		dimopacity = MIN(MAX(dimopacity, 0), 1);
  	unsigned int dimopacity_set = (unsigned int)(dimopacity * OPAQUE);
  	XChangeProperty(dc->dpy, dim, XInternAtom(dc->dpy, OPACITY, False),
											XA_CARDINAL, 32, PropModeReplace,
											(unsigned char *) &dimopacity_set, 1L);
	
		XMapRaised(dc->dpy, dim);
	}
	
	/* create menu window */
	swa.background_pixel = normcol->BG;
	swa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
	win = XCreateWindow(dc->dpy, root, x, y, mw, mh, 0,
	                    DefaultDepth(dc->dpy, screen), CopyFromParent,
	                    DefaultVisual(dc->dpy, screen),
	                    CWOverrideRedirect | CWBackPixel | CWEventMask, &swa);
  XClassHint hint = { .res_name = name, .res_class = class };
  XSetClassHint(dc->dpy, win, &hint);

  opacity = MIN(MAX(opacity, 0), 1);
  unsigned int opacity_set = (unsigned int)(opacity * OPAQUE);
  XChangeProperty(dc->dpy, win, XInternAtom(dc->dpy, OPACITY, False),
											XA_CARDINAL, 32, PropModeReplace,
											(unsigned char *) &opacity_set, 1L);
	
	/* open input methods */
	xim = XOpenIM(dc->dpy, NULL, NULL, NULL);
	xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
	                XNClientWindow, win, XNFocusWindow, win, NULL);

	XMapRaised(dc->dpy, win);
	resizedc(dc, mw, mh);
	drawmenu();
}

void
usage(void) {
	fputs("usage: dmenu [-b] [-q] [-f] [-r] [-i] [-z] [-t] [-mask] [-noinput]\n"
				"             [-s screen] [-name name] [-class class] [ -o opacity]\n"
				"             [-dim opcity] [-dc color] [-l lines] [-p prompt] [-fn font]\n"
	      "             [-x xoffset] [-y yoffset] [-h height] [-w width]\n"
	      "             [-nb color] [-nf color] [-sb color] [-sf color] [-v]\n", stderr);
	exit(EXIT_FAILURE);
}
