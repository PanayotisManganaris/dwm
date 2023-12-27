/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>
/* appearance */
static const unsigned int borderpx  = 10;       /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Liberation Mono:pixelsize=24:antialias=true:autohint=true" };
static const char dmenufont[]       = "Liberation Mono:pixelsize=24:antialias=true:autohint=true";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_purple[]      = "#68217a";
static const char *colors[][3]      = {
        /*               fg         bg         border   */
        [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
        [SchemeSel]  = { col_gray4, col_purple, col_purple },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
        /* xprop(1):
         *      WM_CLASS(STRING) = instance, class
         *      WM_NAME(STRING) = title
         */
        /* class      instance    title       tags mask     isfloating   monitor */
        { "Gimp",     NULL,       NULL,       0,            1,           -1 },
        { "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
        /* symbol     arrange function */
        { "[]=",      tile },    /* first entry is default */
        { "[M]",      monocle },
        { "[@]",      spiral },
        { "[\\]",     dwindle },
        { "H[]",      deck },
        { "TTT",      bstack },
        { "===",      bstackhoriz },
        { "HHH",      grid },
        { "###",      nrowgrid },
        { "---",      horizgrid },
        { ":::",      gaplessgrid },
        { "|M|",      centeredmaster },
        { ">M>",      centeredfloatingmaster },
        { "><>",      NULL },    /* no layout function means floating behavior */
        { NULL,       NULL },
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
static const char *dmenucmd[]    = { "dmenu_run",
                                     "-m", dmenumon, "-fn", dmenufont,
                                     "-nb", col_gray1, "-nf", col_gray3,
                                     "-sb", col_purple, "-sf", col_gray4, NULL };
static const char *termcmd[]     = { "st", NULL };
static const char *dwmdiecmd[]   = { "pkill", "dwm", NULL };
static const char *emacsclient[] = { "emacsclient", "-c", NULL };
static const char *upvol[]   = { "/home/panos/.local/bin/updatevolume.sh", "+", NULL };
static const char *downvol[] = { "/home/panos/.local/bin/updatevolume.sh", "-", NULL };
static const char *mutevol[] = { "/home/panos/.local/bin/updatevolume.sh", "m", NULL };
static const char *mutemic[] = { "pactl", "set-source-mute", "@DEFAULT_SOURCE@", "toggle", NULL };
// notification management commands
static const char *close_note[] = { "/sbin/dunstctl", "close", NULL };
static const char *close_note_all[] = { "/sbin/dunstctl", "close-all", NULL };
static const char *history_note[] = { "/sbin/dunstctl", "history-pop", NULL };
static const char *pause_note[] = { "/sbin/dunstctl", "set-paused toggle", NULL };
static const char *context_note[] = { "/sbin/dunstctl", "context", NULL };

static Key keys[] = {
        /* modifier                     key                       function        argument */
        //apps
        { MODKEY,                       XK_x,                     spawn,          {.v = dmenucmd    } },
        { MODKEY|ShiftMask,             XK_Return,                spawn,          {.v = termcmd     } },
        { MODKEY,                       XK_e,                     spawn,          {.v = emacsclient } },
        //facade
        { MODKEY,                       XK_b,                     togglebar,      {0} },
        //focus stack and space
        { MODKEY,                       XK_n,                     focusstack,     {.i = +1 } },
        { MODKEY,                       XK_p,                     focusstack,     {.i = -1 } },
        { MODKEY|ShiftMask,             XK_n,                     incnmaster,     {.i = +1 } },
        { MODKEY|ShiftMask,             XK_p,                     incnmaster,     {.i = -1 } },
        { MODKEY|ShiftMask,             XK_bracketleft,           setmfact,       {.f = -0.05} },
        { MODKEY|ShiftMask,             XK_bracketright,          setmfact,       {.f = +0.05} },
        { MODKEY,                       XK_bracketleft,           setcfact,       {.f = +0.25} },
        { MODKEY,                       XK_bracketright,          setcfact,       {.f = -0.25} },
        { MODKEY,                       XK_backslash,             setcfact,       {.f =  0.00} },
        { MODKEY,                       XK_Return,                zoom,           {0} },
        { MODKEY,                       XK_Tab,                   view,           {0} },
        { MODKEY|ShiftMask,             XK_c,                     killclient,     {0} },
        //gaps
        { MODKEY,                       XK_u,                     incrgaps,       {.i = +1 } },
        { MODKEY|ShiftMask,             XK_u,                     incrgaps,       {.i = -1 } },
        { MODKEY,                       XK_i,                     incrigaps,      {.i = +1 } },
        { MODKEY|ShiftMask,             XK_i,                     incrigaps,      {.i = -1 } },
        { MODKEY,                       XK_o,                     incrogaps,      {.i = +1 } },
        { MODKEY|ShiftMask,             XK_o,                     incrogaps,      {.i = -1 } },
        { MODKEY|Mod1Mask,              XK_7,                     incrihgaps,     {.i = +1 } },
        { MODKEY|Mod1Mask|ShiftMask,    XK_7,                     incrihgaps,     {.i = -1 } },
        { MODKEY|Mod1Mask,              XK_8,                     incrivgaps,     {.i = +1 } },
        { MODKEY|Mod1Mask|ShiftMask,    XK_8,                     incrivgaps,     {.i = -1 } },
        { MODKEY|Mod1Mask,              XK_9,                     incrohgaps,     {.i = +1 } },
        { MODKEY|Mod1Mask|ShiftMask,    XK_9,                     incrohgaps,     {.i = -1 } },
        { MODKEY|Mod1Mask,              XK_0,                     incrovgaps,     {.i = +1 } },
        { MODKEY|Mod1Mask|ShiftMask,    XK_0,                     incrovgaps,     {.i = -1 } },
        { MODKEY|Mod1Mask,              XK_6,                     togglegaps,     {0} },
        { MODKEY|Mod1Mask|ShiftMask,    XK_6,                     defaultgaps,    {0} },
        //Layouts
        { MODKEY,                       XK_t,                     setlayout,      {.v = &layouts[0]} },
        { MODKEY,                       XK_m,                     setlayout,      {.v = &layouts[1]} },
        { MODKEY,                       XK_s,                     setlayout,      {.v = &layouts[2]} },
        { MODKEY,                       XK_space,                 setlayout,      {0} },
        { MODKEY|ShiftMask,             XK_space,                 togglefloating, {0} },
        //desktops + tags
        { MODKEY,                       XK_0,                     view,           {.ui = ~0 } },
        { MODKEY|ShiftMask,             XK_0,                     tag,            {.ui = ~0 } },
        { MODKEY,                       XK_comma,                 focusmon,       {.i = -1 } },
        { MODKEY,                       XK_period,                focusmon,       {.i = +1 } },
        { MODKEY|ShiftMask,             XK_comma,                 tagmon,         {.i = -1 } },
        { MODKEY|ShiftMask,             XK_period,                tagmon,         {.i = +1 } },
        TAGKEYS(XK_1, 0)
        TAGKEYS(XK_2, 1)
        TAGKEYS(XK_3, 2)
        TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4)
        TAGKEYS(XK_6, 5)
        TAGKEYS(XK_7, 6)
        TAGKEYS(XK_8, 7)
        TAGKEYS(XK_9, 8)
        //dwm's fate
        { MODKEY|ShiftMask,             XK_q,                     quit,          {0} },
        { MODKEY|Mod1Mask|ShiftMask,    XK_q,                     spawn,         {.v = dwmdiecmd} },
        //media
        { 0,                            XF86XK_AudioLowerVolume,  spawn,         {.v = downvol } },
        { 0,                            XF86XK_AudioMute,         spawn,         {.v = mutevol } },
        { 0,                            XF86XK_AudioRaiseVolume,  spawn,         {.v = upvol   } },
        { 0,                            XF86XK_AudioMicMute,      spawn,         {.v = mutemic } },
        //dunst notifications
        { MODKEY,                       XK_grave,                 spawn,         {.v = close_note     } },
        { MODKEY|ShiftMask,             XK_grave,                 spawn,         {.v = close_note_all } },
        { MODKEY,                       XK_BackSpace,             spawn,         {.v = history_note   } },
        { MODKEY,                       XK_equal,                 spawn,         {.v = pause_note     } },
        { MODKEY,                       XK_backslash,             spawn,         {.v = context_note   } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
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
