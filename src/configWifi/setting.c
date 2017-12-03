/* 
** $Id: button.c,v 1.14 2006/05/10 09:51:33 xwyan Exp $
**
** Listing 20.1
**
** button.c: Sample program for MiniGUI Programming Guide
**     Usage of BUTTON control.
**
** Copyright (C) 2004 ~ 2006 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define MSG_ENABLECTK   MSG_USER+1
#define MSG_DISABLECTK  MSG_USER+2

#define IDC_STATICIP                110
#define IDC_DHCPIP                  111
#define IDC_IPADRESS                112
#define IDC_NETMASK                 113
#define IDC_GATEWAY                 114
#define IDC_DNS						115
        
#define IDC_ESSID					116
#define IDC_PWD						117

#define IDC_TEST_LOCALIP            118
#define IDC_TEST_GATEWAY            119
#define IDC_TEST_HOST               120
#define IDC_TEST_DNS                121
#define IDC_TEST_SIDE               122  
#define IDC_SEARCH                  123

static char ip[16];
static char netmask[16];
static char gateway[16];

static char dns[16];
static char search[512];

static int isDHCP = -1;

static DLGTEMPLATE DlgPWD =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 200, 140,
    "Set KEY",
    0, 0,
    4, NULL,
    0
};

static CTRLDATA CtrlPWD[] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 15, 175, 20, 
        IDC_ESSID, 
       "essid:",
        0
    },
   	{
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        10, 40, 180, 20, 
        IDC_PWD,
        NULL,
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        10, 80, 80, 28,
        IDOK, 
        "OK",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        100, 80, 80, 28,
        IDCANCEL,
        "Cancel",
        0
    },
};

static DLGTEMPLATE DlgYourTaste =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 240, 195,
    "Setting IP",
    0, 0,
    15, NULL,
    0
};

static CTRLDATA CtrlYourTaste[] =
{ 
    {
        "static",
        WS_VISIBLE | SS_GROUPBOX, 
        5, 0, 150, 40,
        IDC_STATIC,
        "Setting",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_TABSTOP | WS_GROUP,
        10, 15, 60, 20,
        IDC_DHCPIP,
        "DHCP",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON, 
        80, 15, 60, 20,
        IDC_STATICIP, 
        "STATIC",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 40, 55, 15, 
        IDC_STATIC, 
       "IP Address:",
        0
    },
   	{
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        70, 40, 140, 20, 
        IDC_IPADRESS,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 63, 55, 15, 
        IDC_STATIC, 
       "NETMASK",
        0
    },
   	{
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        70, 63, 140, 20, 
        IDC_NETMASK,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 85, 55, 15, 
        IDC_STATIC, 
		"GateWay:",
        0
    },
   	{
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        70, 85, 140, 20, 
        IDC_GATEWAY,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 107, 55, 15,
        IDC_STATIC, 
		"DNS:",
        0
    },
   	{
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        70, 107, 140, 20, 
        IDC_DNS,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 129, 55, 15,
        IDC_STATIC, 
		"Search:",
        0
    },
   	{
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
        70, 129, 140, 20, 
        IDC_SEARCH,
        NULL,
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        40, 151, 70, 18,
        IDOK, 
        "OK",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        120, 151, 70, 18,
        IDCANCEL,
        "Cancel",
        0
    },
};

static void dhcp_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
		if(	SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED)
		{
			SendMessage(GetParent(hwnd), MSG_DISABLECTK, 0, 0); 
			isDHCP = 1;
		}
    }
}

static void static_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
		if(	SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED)
		{
			SendMessage(GetParent(hwnd), MSG_ENABLECTK, 0, 0); 
			isDHCP = 0;
		}
    }
}

static int DialogBoxProc2 (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
		case MSG_INITDIALOG:
			{
				SetWindowText(GetDlgItem(hDlg, IDC_GATEWAY), gateway);
				SetWindowText(GetDlgItem(hDlg, IDC_IPADRESS), ip);
				SetWindowText(GetDlgItem(hDlg, IDC_NETMASK), netmask);
				SetWindowText(GetDlgItem(hDlg, IDC_DNS), dns);
				SetWindowText(GetDlgItem(hDlg, IDC_SEARCH), search);

				SetNotificationCallback (GetDlgItem (hDlg, IDC_STATICIP),
						static_notif_proc);
				SetNotificationCallback (GetDlgItem (hDlg, IDC_DHCPIP),
						dhcp_notif_proc);

				if (isDHCP == -1)
				{
					SendMessage(GetDlgItem (hDlg, IDC_DHCPIP), 
							BM_SETCHECK, BST_UNCHECKED, 0); 
					SendMessage(GetDlgItem (hDlg, IDC_STATICIP),
							BM_SETCHECK, BST_UNCHECKED, 0); 
					SendMessage(hDlg, MSG_DISABLECTK, 0, 0); 
				} else {
					if (isDHCP) 
					{
						SendMessage(GetDlgItem (hDlg, IDC_DHCPIP), 
								BM_SETCHECK, BST_CHECKED, 0); 
						SendMessage(GetDlgItem (hDlg, IDC_STATICIP),
								BM_SETCHECK, BST_UNCHECKED, 0); 
						SendMessage(hDlg, MSG_DISABLECTK, 0, 0); 
					} else {
						SendMessage(GetDlgItem (hDlg, IDC_STATICIP), 
								BM_SETCHECK, BST_CHECKED, 0); 
						SendMessage(GetDlgItem (hDlg, IDC_DHCPIP), 
								BM_SETCHECK, BST_UNCHECKED, 0); 
						SendMessage(hDlg, MSG_ENABLECTK, 0, 0); 
					}
				}
			}
        return 1;

	case MSG_CLOSE:
		EndDialog (hDlg, wParam);
		break;

	case MSG_ENABLECTK:
		EnableWindow(GetDlgItem (hDlg, IDC_IPADRESS), TRUE);
		EnableWindow(GetDlgItem (hDlg, IDC_NETMASK), TRUE);
		EnableWindow(GetDlgItem (hDlg, IDC_GATEWAY), TRUE);
		break;

	case MSG_DISABLECTK: 
		EnableWindow(GetDlgItem (hDlg, IDC_IPADRESS), FALSE);
		EnableWindow(GetDlgItem (hDlg, IDC_NETMASK), FALSE);
		EnableWindow(GetDlgItem (hDlg, IDC_GATEWAY), FALSE);
		break;

    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:

			if (isDHCP == 1)
				if_using_dhcp();
			else
			{
				GetWindowText(GetDlgItem(hDlg, IDC_GATEWAY),
						gateway, sizeof(gateway));
				GetWindowText(GetDlgItem(hDlg, IDC_IPADRESS),
						ip, sizeof(ip));
				GetWindowText(GetDlgItem(hDlg, IDC_NETMASK),
						netmask, sizeof(netmask));
				if_set_ip(ip);
				if_set_netmask(netmask);
				if_set_gateway(gateway);
			}

			{
				char dns_tmp[16];
				char search_tmp[512];
				GetWindowText(GetDlgItem(hDlg, IDC_DNS),
						dns_tmp, sizeof(dns_tmp));
				GetWindowText(GetDlgItem(hDlg, IDC_SEARCH),
						search_tmp, sizeof(search_tmp));

				if (strcmp(dns_tmp, dns) != 0)
				{
					GetWindowText(GetDlgItem(hDlg, IDC_DNS),
							dns, sizeof(dns));
					if_set_dns(dns);
				}

				if (strcmp(search_tmp, search) != 0)
				{
					GetWindowText(GetDlgItem(hDlg, IDC_SEARCH),
							search, sizeof(search));
					if_set_search(search);
				}
			}
        case IDCANCEL:
            EndDialog (hDlg, wParam);
            break;
        }
        break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

#if 0
int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "button" , 0 , 0);
#endif
    
    DlgYourTaste.controls = CtrlYourTaste;
    
    DialogBoxIndirectParam (&DlgYourTaste, HWND_DESKTOP, DialogBoxProc2, 0L);

    return 0;
}
#else
void Setting_create(const char* ssid, int isHaveKey, HWND parent)
{
    DlgYourTaste.controls = CtrlYourTaste;
    DialogBoxIndirectParam (&DlgYourTaste, parent, DialogBoxProc2, 0L);
}
#endif

static int DialogPWDProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITDIALOG:
		printf("init the set KEY Dialog \n");
			SetWindowText(GetDlgItem(hDlg, IDC_ESSID), "Please Input The Key:");
        return 1;
	case MSG_CLOSE:
		EndDialog (hDlg, wParam);
		break;

    case MSG_COMMAND:
        switch (wParam) {
			case IDOK:
				{
					char key[320]; 
					GetWindowText(GetDlgItem(hDlg, IDC_PWD), key, sizeof(key));
					ap_set_key(key);
				}
        case IDCANCEL:
            EndDialog (hDlg, wParam);
            break;
        }
        break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void Setting_Key(const char* ssid_pwd, HWND parent)
{
	printf("will setting key \n");
    DlgPWD.controls = CtrlPWD;
    DialogBoxIndirectParam (&DlgPWD, parent, DialogPWDProc, 0L);
}

static DLGTEMPLATE DlgTEST =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 200, 200,
    "Test Net",
    0, 0,
    8, NULL,
    0
};

static CTRLDATA CtrlTEST[] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 5, 175, 20, 
        IDC_STATIC, 
       "Test The NetWork ",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 25, 170, 20,
        IDC_TEST_LOCALIP, 
        "Testing the local ip",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 45, 170, 20,
        IDC_TEST_GATEWAY, 
        "",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 65, 170, 20,
        IDC_TEST_DNS, 
        "",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 85, 170, 20,
        IDC_TEST_HOST, 
        "",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE, 
        10, 105, 170, 20,
        IDC_TEST_SIDE, 
        "",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        10, 135, 80, 28,
        IDOK, 
        "OK",
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        100, 135, 80, 28,
        IDCANCEL,
        "Cancel",
        0
    },
};

int testItem = -1;
/*
 * testItem = 1 -> ip
 * 2 -> gateway
 * 3 -> dns 
 * 4 -> host
 * 5 -> side
 * */
void  testingNet(HWND hDlg)
{
	switch (testItem)
	{
		case 1:
			check_network(ip);
			break;
		case 2:
			check_network(gateway);
			break;
		case 3:
			check_network(dns);
			break;
		case 4:
			check_network("202.108.33.32");
			break;
		case 5:
			check_network("www.minigui.com");
			break;
	}




	/*
	int res = 0;
	if (!isDHCP)
	{
		//testing ip
		res = check_network(ip);
		if (res != 0)
			SetWindowText(GetDlgItem(hDlg, IDC_TEST_LOCALIP), "test loaclip fail");
		else
			SetWindowText(GetDlgItem(hDlg, IDC_TEST_LOCALIP), "test loaclip ok");

		SetWindowText(GetDlgItem(hDlg, IDC_TEST_GATEWAY), "testing gateway" );
		res = check_network(gateway);
		if (res != 0)
			SetWindowText(GetDlgItem(hDlg, IDC_TEST_GATEWAY), "test gateway fail");
		else
			SetWindowText(GetDlgItem(hDlg, IDC_TEST_GATEWAY), "test gateway ok");

		SetWindowText(GetDlgItem(hDlg, IDC_TEST_DNS), "testing dns ");
		res = check_network(dns);
		if (res != 0)
			SetWindowText(GetDlgItem(hDlg, IDC_TEST_DNS), "test dns fail");
		else
			SetWindowText(GetDlgItem(hDlg, IDC_TEST_DNS), "test dns ok");

	} else {

		SetWindowText(GetDlgItem(hDlg, IDC_TEST_LOCALIP), "ignore test loaclip (dhcp)");
		SetWindowText(GetDlgItem(hDlg, IDC_TEST_GATEWAY), "ignore testing gateway(dhcp)" );
		SetWindowText(GetDlgItem(hDlg, IDC_TEST_DNS), "ignore testing dns(dhcp)");
	}


	SetWindowText(GetDlgItem(hDlg, IDC_TEST_HOST), "connecting 202.108.33.32 ");
	res = check_network("202.108.33.32");
	if (res != 0)
		SetWindowText(GetDlgItem(hDlg, IDC_TEST_HOST), "connect 202.108.33.32 fail");
	else
		SetWindowText(GetDlgItem(hDlg, IDC_TEST_HOST), "connect 202.108.33.32 ok");

	SetWindowText(GetDlgItem(hDlg, IDC_TEST_SIDE), "testing minigui website ");
	res = check_network("www.minigui.com");
	if (res != 0)
		SetWindowText(GetDlgItem(hDlg, IDC_TEST_SIDE), "test minigui website fail");
	else
		SetWindowText(GetDlgItem(hDlg, IDC_TEST_SIDE), "test minigui website ok");
		*/
}

void  SetTestingRes(HWND hDlg, int res)
{
	switch (testItem)
	{
		case 1:
			if (res != 0)
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_LOCALIP), 
						"test loaclip fail");
			else
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_LOCALIP),
						"test loaclip ok");

			SetWindowText(GetDlgItem(hDlg, IDC_TEST_GATEWAY),
					"testing gateway" );
			break;
		case 2:
			if (res != 0)
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_GATEWAY),
						"test gateway fail");
			else
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_GATEWAY),
						"test gateway ok");

			SetWindowText(GetDlgItem(hDlg, IDC_TEST_DNS),
					"testing dns ");
			break;
		case 3:
			if (res != 0)
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_DNS),
						"test dns fail");
			else
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_DNS),
						"test dns ok");
			SetWindowText(GetDlgItem(hDlg, IDC_TEST_HOST),
					"connecting 202.108.33.32 ");
			break;
		case 4:
			if (res != 0)
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_HOST),
						"connect 202.108.33.32 fail");
			else
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_HOST),
						"connect 202.108.33.32 ok");

			SetWindowText(GetDlgItem(hDlg, IDC_TEST_SIDE),
					"testing minigui website ");
			break;
		case 5:
			if (res != 0)
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_SIDE),
						"test minigui website fail");
			else
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_SIDE), 
						"test minigui website ok");
			break;
	}

}

static int DialogTESTProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {

    case MSG_INITDIALOG:
			if (!isDHCP)
				testItem = 1;
			else
			{
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_LOCALIP),
						"ignore test loaclip (dhcp)");
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_GATEWAY), 
						"ignore testing gateway(dhcp)" );
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_DNS), 
						"ignore testing dns(dhcp)");
				SetWindowText(GetDlgItem(hDlg, IDC_TEST_HOST),
						"connecting 202.108.33.32 ");
				testItem = 4;
			}

			testingNet(hDlg);
			SetTimer (hDlg, 100, 100);
        return 1;

	case MSG_CLOSE:
		EndDialog (hDlg, wParam);
		break;

	case MSG_TIMER:
		{
			int res = get_check_network();
			if (res != -3)
			{
				if (testItem == -1) {
					KillTimer(hDlg, 100);
					break;
				}

				SetTestingRes (hDlg, res);
				if (testItem == 5) {
					KillTimer(hDlg, 100);
					testItem = -1;
				}
				if (testItem != -1) {
					testItem++;
					testingNet(hDlg);
				}
			}
		}
		break;
    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
            EndDialog (hDlg, wParam);
            break;
        }
        break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void Setting_TestingNet(HWND parent)
{
	testItem = -1;
    DlgTEST.controls = CtrlTEST;
    DialogBoxIndirectParam (&DlgTEST, parent, DialogTESTProc, 0L);
}
#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

