/* 
** $Id: mginit.c,v 1.22 2007-08-30 01:20:10 wyan Exp $
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
**
** License: GPL
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "busywin.h"

static int times;
static POINT pts [] = 
{
    {20, 10},
    {27, 13},
    {30, 20},
    {27, 27},
    {20, 30},
    {13, 27},
    {10, 20},
    {13, 13},
};

static RGB rgbs [] = 
{
    {0, 0, 0},
    {32, 32, 32},
    {64, 64, 64},
    {96, 96, 96},
    {128, 128, 128},
    {160, 160, 160},
    {192, 192, 192},
    {224, 224, 224},
};
#define NR_PTS TABLESIZE(pts)

static int BusyWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    int i;
    HDC hdc;

    switch (message) {
        case MSG_TIMER:
            times ++;
            InvalidateRect (hWnd, NULL, FALSE);
            break;
            
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            SetPenColor (hdc, COLOR_darkgray);
            Circle (hdc, 20, 20, 20);
            for (i = 0; i < NR_PTS; i++) {
                SetBrushColor (hdc, RGB2Pixel (hdc, 
                    rgbs [i%NR_PTS].r, rgbs [i%NR_PTS].g, rgbs [i%NR_PTS].b));
                FillCircle (hdc, pts [(times + i)%NR_PTS].x, pts [(times + i)%NR_PTS].y, 3);
            }

            EndPaint (hWnd, hdc);
            return 0;

        case MSG_CLOSE:
            KillTimer (hWnd, 100);
            DestroyMainWindow (hWnd);
            MainWindowCleanup (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

HWND CreateBusyWindow (HWND hosting)
{
    MAINWINCREATE CreateInfo;
    BLOCKHEAP cliprc_heap;
    CLIPRGN circle_rgn;
    HWND busy_win;

    CreateInfo.dwStyle = WS_NONE;
    CreateInfo.dwExStyle = WS_EX_TOOLWINDOW;
    CreateInfo.spCaption = "Busy window";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = BusyWinProc;
    CreateInfo.lx = (240-40)/2;
    CreateInfo.ty = (320-40)/2;
    CreateInfo.rx = CreateInfo.lx + 41;
    CreateInfo.by = CreateInfo.ty + 41;
    CreateInfo.iBkColor = COLOR_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = hosting;
    
    busy_win = CreateMainWindow (&CreateInfo);
    
    if (busy_win == HWND_INVALID)
        return HWND_INVALID;

    InitFreeClipRectList (&cliprc_heap, 50);
    InitClipRgn (&circle_rgn, &cliprc_heap);
    InitCircleRegion (&circle_rgn, 20, 20, 20);

    if (!SetWindowRegion (busy_win, &circle_rgn))
        printf ("Error calling SetWindowRegion. \n");

    EmptyClipRgn (&circle_rgn);
    DestroyFreeClipRectList (&cliprc_heap);

    return busy_win;
}

