/*
 ** $Id: calendar.c 224 2007-07-03 09:38:24Z xwyan $
 **
 ** calendar.c: Calendar module for Feynman PMP solution.
 **
 ** Copyright (C) 2007 Feynman Software, all rights reserved.
 **
 ** Use of this source package is subject to specific license terms
 ** from Beijing Feynman Software Technology Co., Ltd.
 **
 ** URL: http://www.minigui.com
 **
 ** Current maintainer: Zhang Hua .
 */

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <minigui/common.h>
#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "monthcal.h"
#include "monthcalendar_impl.h"
#include "common.h"
#include "../sharedbuff.h"

#define IDC_CALENDAR     206


static LRESULT CalendarProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static BITMAP bmpbk;
	static char* bk_name;
    switch ( message ) {

        case MSG_CREATE:
            {
                HWND calendar_hwnd = CreateWindowEx ( "monthcalendar", "PLAY", 
                        WS_CHILD | WS_VISIBLE, 
                        IDC_CALENDAR, WS_EX_LFRDR_CUSTOM1, 0, 0, g_rcScr.right, g_rcScr.bottom, hWnd, 0 );
				char szvalue[128];
				if(GetValueFromEtcFile(CFGFILE, "calendar", "bkgnd",szvalue,sizeof(szvalue)-1) == ETC_OK)
				{
					if(LoadBitmapFromFile(HDC_SCREEN, &bmpbk, szvalue) == 0
						&& RegisterResFromBitmap(szvalue,&bmpbk))
					{
						SetWindowElementAttr(calendar_hwnd,WE_LFSKIN_WND_BKGND,(DWORD)(bk_name=strdup(szvalue)));
						InvalidateRect(calendar_hwnd, NULL, TRUE);
					}
				}
                SetFocusChild (calendar_hwnd );
                break;
            }
#if 0
        case MSG_PAINT:
            {
                SYSTEMTIME date;
                HDC hdc = BeginPaint ( hWnd );
                SendMessage ( GetDlgItem ( hWnd, IDC_CALENDAR ), MCM_GETCURDATE, 0, ( LPARAM ) &date );
                EndPaint ( hWnd, hdc );
                return 0;
            }  
#endif
        case MSG_CLOSE:
            DestroyAllControls ( hWnd );
            DestroyMainWindow ( hWnd );
			if(bk_name){
				free(bk_name);
				UnloadBitmap(&bmpbk);
			}
            return 0;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void _calendar_init_create_info ( PMAINWINCREATE pCreateInfo )
{
    pCreateInfo->dwStyle = WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "Calendar";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor ( 0 );
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = CalendarProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = g_rcScr.right;
    pCreateInfo->by = g_rcScr.bottom;
    pCreateInfo->iBkColor = COLOR_lightgray;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    RegisterMonthCalendarControl();

	SetDefaultWindowElementRenderer("skin");	

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "Calendar" , 0 , 0);
#endif

    _calendar_init_create_info ( &CreateInfo );

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

#if 0
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
#else
    ShowWindowUsingShareBuffer (hMainWnd);
#endif

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

