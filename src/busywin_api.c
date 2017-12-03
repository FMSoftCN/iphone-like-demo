/* 
** $Id: busy_win_api.c,v 1.22 2007-08-30 01:20:10 wyan Exp $
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
**
** License: GPL
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "busywin.h"

extern HWND g_hBusyWnd;
extern BOOL g_bBusyWin;

int ShowBusyWindow (BOOL bShow)
{
    if (mgIsServer) {
        if (bShow) {
            g_bBusyWin = TRUE;
            SetTimer (g_hBusyWnd, 100, 10);
            ShowWindow (g_hBusyWnd, SW_SHOWNORMAL);
        }
        else {
            g_bBusyWin = FALSE;
            KillTimer (g_hBusyWnd, 100);
            ShowWindow (g_hBusyWnd, SW_HIDE);
        }
    }
    else {
        int ret;
        REQUEST req = {BUSYWIN_REQID, &bShow, sizeof (BOOL)};

        ClientRequest (&req, &ret, sizeof (int));
    }

    return 0;
}

