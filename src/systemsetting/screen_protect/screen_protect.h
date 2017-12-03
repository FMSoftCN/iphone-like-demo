
#ifndef SCREEN_SAVER
#define SCREEN_SAVER
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>

#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
HWND CreateScreenSaver (void);
PLOGFONT g_pLogFont; 
HWND g_hScreenSaver;



#endif
