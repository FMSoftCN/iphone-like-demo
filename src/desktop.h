
#ifndef DESKTOP_H
#define DESKTOP_H

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>


typedef struct _DESKTOP_CONTEXT{
	PBITMAP bmp_bkgnd;
}DESKTOP_CONTEXT;

void SetDesktopBkgnd(PBITMAP bmpbk);

extern DESKTOPOPS mg_dsk_ops;

#endif

