/* UPDATE
* GitHub COPY Custom 16.09.2017
* by https://github.com/appath/
* See LICENSE file for copyright and license details. 
*/

/* include modules */
#include "patch/selfrestart.c"
#include "patch/moveresize.c"
#include "patch/gaplessgrid.c"
#include "patch/zoomswap.c"

/* mappings */
#define XF86AudioMute			0x1008ff12
#define XF86AudioLowerVolume	        0x1008ff11
#define XF86AudioRaiseVolume	        0x1008ff13
#define XF86TouchpadToggle		0x1008ffa9

/* appearance */
static const char *fonts[] = {
	"siji"
};

/* border pixel of windows */
static const unsigned int borderpx 		= 0;
/* snap pixel */
static const unsigned int snap 			= 8;
/* allows you to set the field value */
static const unsigned int tagpadding 		= 12;
/* the rule checks the spaces inside and around the syntax elements */
static const unsigned int tagspacing 		= 4;
/* gap pixel between windows */
static const unsigned int gappx			= 8;
/* 1: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systraypinning 	= 0;
/* systray spacing */
static const unsigned int systrayspacing 	= 2;
/* 1: if pinning fails, display systray on the first monitor */
/* 0: display systray on the last monitor */
static const int systraypinningfailfirst 	= 1;
/* 1 means no systray */
static const int showsystray 			= 0;
/* 0 means no bar */
static const int showbar 			= 1;
/* 1 means bottom bar */
static const int topbar 			= 0;

#define NUMCOLORS 9
static const char colors[NUMCOLORS][MAXCOLORS][9] = {
	// border	 foreground	  background
	{ "#ffffff",	 "#f5f5f5", 	  "#000507" },  // 0 = normal
	{ "#181818", 	 "#f5f5f5", 	  "#8b97b1" },  // 1 = selected
	{ "#b43030", 	 "#f5f5f5", 	  "#b23450" },  // 2 = red / urgent
	{ "#181818", 	 "#f5f5f5", 	  "#14161A" },  // 3 = green / occupied
	{ "#212121", 	 "#ab7438", 	  "#0b0606" },  // 4 = yellow
	{ "#212121", 	 "#475971", 	  "#0b0606" },  // 5 = blue
	{ "#212121", 	 "#694255", 	  "#0b0606" },  // 6 = magenta
	{ "#212121", 	 "#3e6868", 	  "#0b0606" },  // 7 = cyan
	{ "#212121",	 "#cfa696", 	  "#0b0606" },  // 8 = grey
};

/* tagging */
static const char *tags[] = { "\ue241 ", "\ue1e2 ", "\ue1e0 ", "\ue1e5 ", "\ue1dd ", "\ue1da ", "\ue19e " };

static const Rule rules[] = {
	/* class					instance	title	tags mask	iscentered	isfloating	monitor */
	{ "URxvt",						NULL,	NULL,	0,			1,			1,			-1 },
	{ "Gimp",     					NULL,	NULL,	1 << 5,		0,			1,			-1 },
	{ "Gimp-2.8",     				NULL,	NULL,	1 << 5,		0,			1,			-1 },
	{ "Inkscape",     				NULL,	NULL,	1 << 5,		0,			1,			-1 },
	{ "mpv",     					NULL,	NULL,	1 << 4,		0,			1,			-1 },
	{ "Geany",     					NULL,	NULL,	1 << 3,		0,			1,			-1 },
	{ "libreoffice*",     			NULL,	NULL,	1 << 3,		0,			1,			-1 },
	{ "Firefox-esr",  				NULL,	NULL,	1 << 1,		0, 			0,			-1 },
	{ "Firefox",  					NULL,	NULL,	1 << 1,		0, 			0,			-1 },
	{ "Transmission-gtk",  			NULL,	NULL,	1 << 1,		0, 			0,			-1 },
	{ "Thunar",  					NULL,	NULL,	1 << 2,		0, 			0,			-1 },
	{ "File-roller",  				NULL,	NULL,	1 << 2,		0, 			0,			-1 },
	{ "Viewnior",					NULL,	NULL,	NULL,		1,			1,			-1 },
	{ "Lxtask",						NULL,	NULL,	1 << 6,		1,			1,			-1 },
	{ "Lxappearance",				NULL,	NULL,	1 << 6,		1,			1,			-1 },
};

/* layout(s) */
static const float mfact = 0.52;	/* factor of master area size [0.05..0.95] */
static const int nmaster = 1;		/* number of clients in master area */
static const int resizehints = 0;	/* 1 means respect size hints in tiled resizals */

/* tagging */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "\ue006",   NULL },   	/* no layout function means floating behavior */
	{ "\ue002",   tile },    	/* first entry is default */
	{ "\ue000",   monocle },        /* monocle is good for maximizing the preservation and focusing of the window */
	{ "\ue003",   htile },          /* first entry is default */
	{ "\ue00a",   gaplessgrid },    /* organizes windows in the grid */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
/* dmenu custom */
static char dmenumon[2] 		= "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] 		= { "dmenu_run", "-b", "-i", "-p", "Search", "-fn", "Noto Sans:size=8", "-w", "360", "-h", "20", "-x", "24", "-y", "48", "-dim", "0.4", NULL };
static const char *termcmd[] 		= { "urxvt", NULL, };
static const char *rubycmd[] 		= { "urxvt", "-name", "Ruby", "-geometry", "122x55", "-e", "irb", NULL, };
static const char *lfmcmd[] 		= { "urxvt", "-name", "Total Commander", "-geometry", "95x45", "-e", "lfm", NULL, };
static const char *vol_up[] 	        = { "pulseaudio-ctl", "up", NULL };
static const char *vol_down[] 		= { "pulseaudio-ctl", "down", NULL };
static const char *vol_toggle[]  	= { "pulseaudio-ctl", "mute", NULL };
static const char *togtouchpad[] 	= { "touchpad", NULL };

static Key keys[] = {
	/* modifier			key			function		argument */
	{ MODKEY|ShiftMask,		XK_p,			spawn,			{.v = dmenucmd } },
	{ MODKEY,	         	XK_Return,		spawn,			{.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_m,                   spawn,                  {.v = lfmcmd } },
	{ MODKEY,			XK_r,			spawn,			{.v = rubycmd } },
	{ MODKEY|ShiftMask,		XK_b,			togglebar,		{0} },
	{ MODKEY,			XK_Right,		focusstack,		{.i = +1 } },
	{ MODKEY,			XK_Left,		focusstack,		{.i = -1 } },
	{ MODKEY,			XK_i,			incnmaster,		{.i = +1 } },
	{ MODKEY,			XK_d,			incnmaster,		{.i = -1 } },
	{ MODKEY|ShiftMask,		XK_Left,		setmfact,		{.f = -0.05} },
	{ MODKEY|ShiftMask,		XK_Right,		setmfact,		{.f = +0.05} },
	{ MODKEY|ShiftMask,		XK_Up,			setcfact,		{.f = +0.25} },
	{ MODKEY|ShiftMask,		XK_Down,		setcfact,		{.f = -0.25} },
	{ MODKEY|ShiftMask,		XK_o,			setcfact,		{.f =  0.00} },
	{ MODKEY,			XK_Down,		pushdown,		{0} },
    	{ MODKEY,			XK_Up,			pushup,			{0} },
	{ MODKEY|ShiftMask,		XK_Return,		zoom,			{0} },
	{ MODKEY,			XK_Tab,			view,			{0} },
	{ MODKEY|ShiftMask,		XK_c,			killclient,		{0} },
	{ MODKEY,			XK_t,			setlayout,		{.v = &layouts[1]} },
	{ MODKEY,			XK_f,			setlayout,		{.v = &layouts[0]} },
	{ MODKEY,			XK_m,			setlayout,		{.v = &layouts[2]} },
	{ MODKEY,			XK_b,			setlayout,		{.v = &layouts[3]} },
	{ MODKEY,			XK_g,			setlayout,		{.v = &layouts[4]} },
	{ MODKEY,			XK_space,		setlayout,		{0} },
    	{ MODKEY,			XK_u,			togglefullscreen,	{0} },
	{ MODKEY|ShiftMask,		XK_space,		togglefloating,		{0} },
	{ MODKEY,			XK_0,			view,			{.ui = ~0 } },
	{ MODKEY|ShiftMask,		XK_0,			tag,			{.ui = ~0 } },
	{ MODKEY,			XK_comma,		focusmon,		{.i = -1 } },
	{ MODKEY,			XK_period,		focusmon,		{.i = +1 } },
	{ MODKEY|ShiftMask,		XK_comma,		tagmon,			{.i = -1 } },
	{ MODKEY|ShiftMask,		XK_period,		tagmon,			{.i = +1 } },
	{ MODKEY|ControlMask,			XK_Up,			moveresize,		{.v = "0x -25y 0w 0h"} },
	{ MODKEY|ControlMask,			XK_Down,		moveresize,		{.v = "0x 25y 0w 0h"} },
	{ MODKEY|ControlMask,			XK_Left,		moveresize,		{.v = "-25x 0y 0w 0h"} },
	{ MODKEY|ControlMask,			XK_Right,		moveresize,		{.v = "25x 0y 0w 0h"} },
	{ MODKEY|ShiftMask,		XK_Up,			moveresize,		{.v = "0x 0y 0w -25h"} },
	{ MODKEY|ShiftMask,		XK_Down,		moveresize,		{.v = "0x 0y 0w 25h"} },
	{ MODKEY|ShiftMask,		XK_Left,		moveresize,		{.v = "0x 0y -25w 0h"} },
	{ MODKEY|ShiftMask,		XK_Right,		moveresize,		{.v = "0x 0y 25w 0h"} },
	TAGKEYS(			XK_1,						0)
	TAGKEYS(			XK_2,						1)
	TAGKEYS(			XK_3,						2)
	TAGKEYS(			XK_4,						3)
	TAGKEYS(			XK_5,						4)
	TAGKEYS(			XK_6,						5)
	TAGKEYS(			XK_7,						6)
	{ MODKEY|ShiftMask,		XK_q,			quit,			{0} },
	{ MODKEY|ShiftMask,		XK_r,			self_restart,		{0} },
	{ 0,				XF86AudioRaiseVolume,	spawn,			{.v = vol_up } },
    	{ 0,				XF86AudioLowerVolume,	spawn,			{.v = vol_down } },
    	{ 0,				XF86AudioMute,		spawn,			{.v = vol_toggle } },
    	{ 0,				XF86TouchpadToggle,	spawn,			{.v = togtouchpad } },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click		event mask	button		function		argument */
	{ ClkLtSymbol,		0,		Button1,	setlayout,		{0} },
	{ ClkLtSymbol,		0,		Button3,	setlayout,		{.v = &layouts[2]} },
	{ ClkStatusText,	0,		Button2,	spawn,			{.v = termcmd } },
	{ ClkClientWin,		MODKEY,		Button1,	movemouse,		{0} },
	{ ClkClientWin,		MODKEY,		Button2,	togglefloating,		{0} },
	{ ClkClientWin,		MODKEY, 	Button3,	resizemouse,		{0} },
	{ ClkTagBar,		0,		Button1,	view,			{0} },
	{ ClkTagBar,		0,		Button3,	toggleview,		{0} },
	{ ClkTagBar,		MODKEY,		Button1,	tag,			{0} },
	{ ClkTagBar,		MODKEY,		Button3,	toggletag,		{0} },
};
/* the end */
