/* 
** $Id: CoverFlow.c 616 2008-08-25 03:17:42Z weiym $
**
** Listing 2.1
**
** CoverFlow.c: Sample program for MiniGUI Programming Guide
**      The first MiniGUI application.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "CoverFlow.h"
#include <agg_coverflow.h>
static char* g_pBitmapName[] = {"CoverFlow1.bmp", "CoverFlow2.bmp", "CoverFlow3.bmp",
                          "CoverFlow4.bmp", "CoverFlow5.bmp", "CoverFlow6.bmp",
                          "CoverFlow7.bmp", "CoverFlow1.bmp", "CoverFlow2.bmp"};

static int CoverFlowProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    int i;

    switch (message) {
        case MSG_CREATE:
            break;

        case MSG_PAINT:
#if 1 
            hdc = BeginPaint (hWnd);
            InitCoverFlow (g_pBitmapName, 9, 640, 480);
            ShowCoverFlow (HDC_SCREEN);
            EndPaint (hWnd, hdc);
#endif
            return 0;

        case MSG_LBUTTONDOWN:
            {
                CoverFlowMoveFromRightToLeftSingle (HDC_SCREEN);
            }
            break;

        case MSG_RBUTTONDOWN:
            {
                CoverFlowMoveFromLeftToRightSingle (HDC_SCREEN);
            }
            break;
        case MSG_KEYDOWN:
            {
                switch( wParam )
                {
                    case SCANCODE_CURSORBLOCKLEFT:
                        CoverFlowMoveFromRightToLeftSingle (HDC_SCREEN);
                        break;
                    case SCANCODE_CURSORBLOCKRIGHT:
                        CoverFlowMoveFromLeftToRightSingle (HDC_SCREEN);
                        break;
                    default:
                        break;
                }
            }
            break;

//        case MSG_KEYDOWN:
//            ShowCoverFlow ();
//            return 0;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
    int i;

    
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "coverflow" , 0 , 0);
#endif



    CreateInfo.dwStyle = 
        WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = HL_ST_CAP;
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = CoverFlowProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 640;
    CreateInfo.by = 480;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

