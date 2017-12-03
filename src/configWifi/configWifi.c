/*
** $Id: listbox.c,v 1.15 2006/05/10 09:51:33 xwyan Exp $
** 
** Listing  21.1
**
** listbox.c: Sample program for MiniGUI Programming Guide
**      The usage of LISTBOX control.
** 
** Copyright (C) 2004 ~ 2006 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "readiwlist.h"
#include "setting.h"
#include "iwfunc.h"
#include "readiwlist.h"

#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
#include "mgime.h"
#include "keyboard.h"
GHANDLE ime_handle;
#endif

#define NET_DLAG_W 240
#define NET_DLAG_H 200

#define IDL_NET   311
#define IDC_REFRESH 111 
#define IDC_SETTING 112 
#define IDC_SETKEY  113 
#define IDC_TESTNET 114

HICON hIcon1;
static DLGTEMPLATE DlgDelFiles =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE | WS_EX_NOCLOSEBOX,
    0, 0, 240, 190,
    "WiFi Network",
    0, 0,
    6, NULL,
    0
};

static CTRLDATA CtrlDelFiles[] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 5, 120, 15, 
        IDC_STATIC, 
       "Choose one network:",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        135, 5, 80, 15,
		IDC_TESTNET,
        "Test Net",
        0
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_AUTOCHECKBOX | LBS_NOTIFY | LBS_CHECKBOX | LBS_USEICON,
        10, 25, 215, 110,
        IDL_NET,
        "",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        32, 142, 53, 18,
		IDC_REFRESH, 
        "Refresh",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        95, 142, 50, 18,
		IDC_SETKEY, 
        "SetKEY",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        155, 142, 50, 18,
		IDC_SETTING,
        "SetIP",
        0
    },
};

static void fill_boxes (HWND hDlg, const char* ap_str)
{
	if (!ap_str)
		return;
	WIRELESS_AP * ap_list = NULL;
	get_all_ap (&ap_list, ap_str);

	WIRELESS_AP * tmp = ap_list;
#if 1
    SendDlgItemMessage (hDlg, IDL_NET, LB_RESETCONTENT, 0, (LPARAM)0);
	if (!tmp)
	{
		LISTBOXITEMINFO lbii;
		lbii.string = "no any network";
		lbii.cmFlag = CMFLAG_BLANK;
		lbii.hIcon = 0;
		SendDlgItemMessage (hDlg, IDL_NET, LB_ADDSTRING, 0, (LPARAM)&lbii);
	}

    while (tmp) {
#if 1
            LISTBOXITEMINFO lbii;
		/*char *tmpstr =asprintf("%s--%s",tmp->essid, tmp->haveKey?"Need Key":" ");
			  free(tmpstr); */
            lbii.string = tmp->essid;
            lbii.cmFlag = CMFLAG_BLANK;

			if (tmp->haveKey)
				lbii.hIcon = hIcon1;
			else 
				lbii.hIcon = 0;

            SendDlgItemMessage (hDlg, IDL_NET, LB_ADDSTRING, 0, (LPARAM)&lbii);
#else
            LISTBOXITEMINFO  *lbii =(LISTBOXITEMINFO *) malloc(sizeof(LISTBOXITEMINFO)) ;
            lbii->string = tmp->essid;
            lbii->cmFlag = CMFLAG_BLANK;

			if (tmp->haveKey)
				lbii->hIcon = hIcon1;
			else 
				lbii->hIcon = 0;

            SendDlgItemMessage (hDlg, IDL_NET, LB_ADDSTRING, 0, lbii);
#endif
			tmp = tmp->next;
    }
#endif
	free_all_ap(ap_list);
}

static void net_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
	LISTBOXITEMINFO lbii;
	if (nc == LBN_CLICKCHECKMARK) {
		int currentIndex = SendMessage(hwnd, LB_GETCURSEL, 0, 0);
		{

		char * text = (char*)malloc(sizeof(text)*300);
		if (SendMessage(hwnd, LB_GETTEXT, currentIndex, (LPARAM)text) == LB_OKAY)
			//here will run iwconfig wlan0 ssid= text
			ap_set_ssid(text);
		free(text);
			LISTBOXITEMINFO lbii;
			SendMessage(hwnd, LB_GETITEMDATA, currentIndex, (LPARAM)&lbii); 

			if (lbii.hIcon == hIcon1)
				EnableWindow(GetDlgItem (GetParent(hwnd), IDC_SETKEY), TRUE);
			else
				EnableWindow(GetDlgItem (GetParent(hwnd), IDC_SETKEY), FALSE);
		}

		int count = SendMessage(hwnd, LB_GETCOUNT, 0, 0);
		int i = 0;
		for (i =0; i< count; i++)
		{
			if (i == currentIndex)
				continue;
			SendMessage(hwnd, LB_SETCHECKMARK, i, CMFLAG_BLANK);
		}
	}
	if (nc == LBN_DBLCLK) {
		int currentIndex = SendMessage(hwnd, LB_GETCURSEL, 0, 0);
		if (SendMessage(hwnd, LB_GETCHECKMARK, currentIndex, 0) == CMFLAG_CHECKED)
		{
			SendMessage(hwnd, LB_GETITEMDATA, currentIndex, (LPARAM)&lbii); 

			if (lbii.hIcon == hIcon1)
				Setting_create(lbii.string, 1 ,hwnd);
			else
				Setting_create(lbii.string, 0 ,hwnd);
		}
	}
}


static int DelFilesBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	/*	if(message != 322)
		printf("messgae %d \n", message);
		*/
	switch (message) {
		case MSG_CREATE:
			{
				CreateWindow (CTRL_STATIC ,"Choose one network:", 
						WS_CHILD | WS_VISIBLE, 
						IDC_STATIC , 10, 5, 120, 15,  hDlg, 0 );
				CreateWindow ("button", "Test Net",
						WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
						IDC_TESTNET, 135, 5, 80, 15,  hDlg, 0 );

				CreateWindow (CTRL_LISTBOX, "", 
						WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_AUTOCHECKBOX | LBS_NOTIFY | LBS_CHECKBOX | LBS_USEICON,
						IDL_NET, 10, 25, 215, 110,  hDlg, 0 );

				CreateWindow ("button", "Refresh",
						WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
						IDC_REFRESH, 32, 142, 53, 18,  hDlg, 0 );

				CreateWindow ("button", "SetKEY",
						WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
						IDC_SETKEY, 95, 142, 50, 18,  hDlg, 0 );

				CreateWindow ("button", "SetIP",
						WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
						IDC_SETTING, 155, 142, 50, 18,  hDlg, 0 );

				EnableWindow(GetDlgItem (hDlg, IDC_SETKEY), FALSE);
				hIcon1 = LoadIconFromFile(HDC_SCREEN, "./res/ico.ico", 1);
				if (hIcon1 == 0)
					printf("load ico fail \n");
				SetNotificationCallback (GetDlgItem (hDlg, IDL_NET), net_notif_proc);
				get_ap_list();
				SetTimer (hDlg, 100, 100);
				printf("asdsdasd \n");
				break;
			}

		case MSG_TIMER:
			{
				char * ap_str = get_ap_list_str();
				if (ap_str)
				{
					KillTimer (hDlg, 100);
					fill_boxes (hDlg, ap_str);
				}
			}
			break;

        case MSG_CLOSE:
            DestroyAllControls ( hDlg );
            DestroyMainWindow ( hDlg );
            return 0;
		case MSG_COMMAND:
			switch (wParam) {
				case IDC_SETKEY:
					{
#if 1
						int i = 0;
						HWND list = GetDlgItem (hDlg, IDL_NET);
						int count = SendMessage(list, LB_GETCOUNT, 0, 0);
						for (i =0; i< count; i++)
						{
							if (SendMessage(list, LB_GETCHECKMARK, i, 0) == CMFLAG_CHECKED)
							{
								LISTBOXITEMINFO lbii;
								SendMessage(list, LB_GETITEMDATA, i, (LPARAM)&lbii); 
								if (lbii.hIcon)
									Setting_Key(NULL, hDlg);
								break;
							}
						}
#endif
						break;
					}
				case IDC_SETTING :
					Setting_create("", 0 ,hDlg);
					break;

				case IDC_TESTNET: 
					Setting_TestingNet(hDlg);
					break;
				case IDC_REFRESH:
					get_ap_list();
					SetTimer (hDlg, 100, 100);
					break;
				case IDOK:
					EndDialog (hDlg, wParam);
					break;
				case IDCANCEL:
					EndDialog (hDlg, wParam);
					break;
			}
			break;
	}
    
    return DefaultMainWinProc(hDlg, message, wParam, lParam);
    //return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "listbox" , 0 , 0);
#endif
#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
    HWND sk_ime_hwnd;
    MGI_SOFTKEYBOARD_INFO sk_info;

    memset (&sk_info, 0, sizeof (MGI_SOFTKEYBOARD_INFO));
#endif 

#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)

    ime_handle = mgiCreateIMEContainer (2, FALSE);
    sk_ime_hwnd = mgiCreateSoftKeyboardIME (&sk_info, NULL);

    mgiAddIMEWindow (ime_handle, sk_ime_hwnd, "softkeyboard");
    mgiAddSKBTranslateOps (sk_ime_hwnd, &quanpin_trans_ops);
    mgiSetActiveIMEWindow (ime_handle, "softkeyboard");
#endif
    
    CreateInfo.dwStyle = WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE | WS_EX_NOCLOSEBOX;
    CreateInfo.spCaption = "WiFi Network";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DelFilesBoxProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = NET_DLAG_W;
    CreateInfo.by = NET_DLAG_H;
	CreateInfo.iBkColor = GetWindowElementColor (BKC_DIALOG);
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
	{
		printf("HWND_INVALID \n");
        return -1;
	}

    ShowWindow (hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);
    return 0;
#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
        mgiDestroyIMEContainer (ime_handle);
#endif
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

