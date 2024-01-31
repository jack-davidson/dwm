/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

#ifdef lt0
#define DEVICE "Master"
#endif

#ifdef dt0
#define DEVICE "PCM"
#endif

/* appearance */
static unsigned int borderpx  = 2;        /* border pixel of windows and bar */
static unsigned int snap      = 32;       /* snap pixel */
static int showbar            = 1;        /* 0 means no bar */
static int statuspadding      = 8;
static int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";

static char foreground[]     = "#3d3d3d";
static char background[]     = "#AAAAAA";
static char normborder[] = "#000000";

static char selfg[]      = "#FFFFAA";
static char selbg[]      = "#3d3d3d";
static char selborder[]  = "#AAAAAA";

static char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { foreground, background, normborder }, /* Unselected */
	[SchemeSel]  = { selfg,  selbg,  selborder  }, /* Selected */
};

/* tagging */
static const char *tags[] = { "A", "S", "D", "F", "G", "Z"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class          instance    title       tags mask     isfloating   monitor */
	{ "Slack",        NULL,       NULL,       1 << 4,       0,           -1 },
	{ "qutebrowser",  NULL,       NULL,       1 << 3,       0,           -1 },
	{ "kitty",     NULL,       NULL,       0     ,       1,           -1 },
	{ "QjackCtl",     NULL,       NULL,       1     ,       1,           -1 },
	{ "QjackCtl",     NULL,       "Graph",    1,            0,           -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "><>",      NULL },    /* no layout function means floating behavior */
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
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run",
    "-fn", dmenufont,
    "-nb", background, "-nf", foreground,
    "-sb", selbg, "-sf", selfg, 
    "-nhb", background, "-nhf", selfg, 
    "-shb", selbg, "-shf", background, 
    NULL };
static const char *termcmd[]  = { "kitty", NULL };
static const char *screenshotcmd[] = {"shot.sh", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,	                XK_c,      spawn,          {.v = screenshotcmd } },
	{ 0,              XF86XK_AudioRaiseVolume, spawn,          SHCMD("amixer set "DEVICE" 5%+; kill -44 $(pidof dwmblocks)") },
	{ 0,              XF86XK_AudioLowerVolume, spawn,          SHCMD("amixer set "DEVICE" 5%-; kill -44 $(pidof dwmblocks)") },
	{ 0,              XF86XK_AudioMute, 	   spawn,          SHCMD("amixer set "DEVICE" toggle; kill -44 $(pidof dwmblocks)") },
	{ 0,	          XF86XK_MonBrightnessUp,  spawn,	   SHCMD("xbacklight -inc 5; kill -45 $(pidof dwmblocks)") },
	{ 0,	          XF86XK_MonBrightnessDown,spawn,	   SHCMD("xbacklight -dec 5; kill -45 $(pidof dwmblocks)") },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_u,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_q,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_w,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_e,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_a,                      0)
	TAGKEYS(                        XK_s,                      1)
	TAGKEYS(                        XK_d,                      2)
	TAGKEYS(                        XK_f,                      3)
	TAGKEYS(                        XK_g,                      4)
	TAGKEYS(                        XK_z,                      5)
	TAGKEYS(                        XK_x,                      6)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

/* implementing resource classes
 *
 * ResourcePref border[] = {
 *      { "focused", STRING, &selborder },
 * };
 *
 * ResourcePref bar[] = {
 *      { "show", INTEGER, &selborder },
 *      { "top",  INTEGER, &topbar },
 * };
 *
 * ResourceClass resources[] = {
 * { "border", STRING, &border },
 * { "bar",    STRING, &bar },
 * }
 */

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "background",        STRING,   &background },
		{ "normborder",    STRING,  &normborder },
		{ "foreground",        STRING,   &foreground },
		{ "selbg",         STRING,  &selbg },
		{ "selborder",     STRING,  &selborder },
		{ "selfg",         STRING,  &selfg },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};

