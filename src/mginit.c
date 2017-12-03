/* 
** $Id: mginit.c,v 1.22 2007-08-30 01:20:10 wyan Exp $
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <minigui/common.h>
#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "config.h"
#include "softkeywindow.h"
#ifdef ENABLE_MLSSHELL
#include "mlskbd.h"
#endif

#include "layout.h"
#include "layouthlp.h"
#include "dragmanager.h"
#include "sence.h"
#include "common.h"
#include "animate/instruction.h"
#include "animate/animate.h"
#include "desktop.h"
#include "busywin.h"
#include "sharedbuff.h"

//#define FUN_DEBUG
#include <my_debug.h>

#define SFKBD_GETSTATE      3
#define SFKBD_REQID    (MAX_SYS_REQID + 9)
static HWND hSoftKeypad;

static int nr_clients = 0;

static void on_new_del_client (int op, int cli)
{       

    FUN_START ();

#if 1
    if (op == LCO_NEW_CLIENT) {
        nr_clients ++;
    }   
    else if (op == LCO_DEL_CLIENT) {
        nr_clients --;
        if (nr_clients == 0) {
        }               
        else if (nr_clients < 0) {
        }

        if (strcmp (mgClients [cli].name, "screen_saver")) {
            SetCurrentSence (SENCE_DESKTOP);
        }
        else if (!IsWindowVisible (g_hMainWnd)) {
		    ShowWindow (g_hMainWnd, SW_SHOWNORMAL);
        }
    }
    else
        printf ("Serious error: incorrect operations.\n");
#endif

    FUN_END();
}

BOOL g_bAnimateShow = FALSE;
BOOL g_animateflag = FALSE;
BOOL g_bBusyWin = FALSE;
HWND g_hMainWnd;
HDC g_hMainDC;

/////////////////////////////////////////
//screen saver:
struct {
	int svr_wait_time;
	int svr_left; //screen saver waiting time left
	pid_t pid;
	char* save_command;
}screen_saver;

extern pid_t exec_app (const char* file_name, const char* app_name);

#define ResetScreenSaver()  (screen_saver.svr_left = screen_saver.svr_wait_time)
#define DecreaseScreenSaver()  do{ \
	if(screen_saver.svr_wait_time > 0 && screen_saver.pid <= 0) { \
		screen_saver.svr_left -= 1; \
		if(screen_saver.svr_left <= 0){ \
			StartScreenSaver(); \
		} \
	} \
}while(0)

static void StartScreenSaver()
{
	if(screen_saver.pid <= 0 && screen_saver.save_command != NULL) { 
		screen_saver.pid = exec_app(screen_saver.save_command, screen_saver.save_command); 
		if(screen_saver.pid > 0){ 
			SendMessage (hSoftKeypad, MSG_IME_CLOSE, 1, 0);
			ShowWindow (g_hMainWnd, SW_HIDE); 
		} 
	} 
}

static void StopScreenSaver()
{
	if(screen_saver.pid > 0){
		kill (screen_saver.pid, SIGKILL);
		screen_saver.pid = 0;
		screen_saver.svr_left = screen_saver.svr_wait_time;
	} 
}

static void GetScreenSaverInfoFromEtc()
{
	char szvalue[128];
#define GET_STR(key) (GetValueFromEtcFile(CFGFILE, "screensaver", key, szvalue, sizeof(szvalue)-1)==ETC_OK? \
	strdup(szvalue):NULL)
#define GET_INT(key, value) GetIntValueFromEtcFile(CFGFILE, "screensaver", key, &value)
	
	StopScreenSaver();
	if(screen_saver.save_command) free(screen_saver.save_command);
	memset(&screen_saver, 0 ,sizeof(screen_saver));
	GET_INT("waittime",screen_saver.svr_wait_time);
	screen_saver.svr_left = screen_saver.svr_wait_time;
	screen_saver.save_command = GET_STR("command");

#undef GET_STR
#undef GET_INT
}




///////////////////////////////////////
//timer drag
struct drag_info_t{
	BOOL is_drag_on;
	HWND hwnd;
	int time;
	WPARAM wParam;
	LPARAM lParam;
};

static struct drag_info_t drag_info = {0, 0, 0, 0};

static void drag_event_hook (PMSG msg)
{
    FUN_START ();
	if(msg->message >= MSG_FIRSTMOUSEMSG 
		&& msg->message <= MSG_LASTMOUSEMSG)
	{
		if(msg->message == MSG_LBUTTONDOWN)
		{
			drag_info.is_drag_on = TRUE;
			drag_info.hwnd = msg->hwnd;
			drag_info.time = 200;
			drag_info.wParam = msg->wParam;
			drag_info.lParam = msg->lParam;
		}
		else if(msg->message == MSG_MOUSEMOVE){
			if(drag_info.is_drag_on){
				if(drag_info.hwnd != msg->hwnd 
					|| (drag_info.wParam != msg->wParam)){
					register int diff_x, diff_y;
					diff_x = LOWORD(msg->lParam) - LOWORD(drag_info.lParam);
					diff_y = HIWORD(msg->lParam) - HIWORD(drag_info.lParam);
					if(diff_x < -10 || diff_x > 10 || diff_y < -10 || diff_y > 10){
						drag_info.is_drag_on = FALSE;
					}
				}
			}
		}
		else if (msg->message < MSG_NCMOUSEOFF)
		{
			if(drag_info.is_drag_on)
			{
				drag_info.is_drag_on = FALSE; //cancel
			}
		}
	}
    FUN_END();
}

static BOOL drag_time_proc(HWND hwnd, int id, DWORD count)
{
    FUN_START ();

	if(drag_info.is_drag_on)
	{
		drag_info.time -= 100;
		if(  drag_info.time <= 0  )
		{
			PostMessage(drag_info.hwnd, MSG_DRAG_ACTIVE,
				drag_info.wParam, drag_info.lParam);
			drag_info.is_drag_on = FALSE;
		}
	}

    if (nr_clients == 0 && !g_animateflag && !g_bBusyWin)
	    DecreaseScreenSaver();

    FUN_END();
	return TRUE;
}
//////////////////////////////////////////////


static int DemoWinProc(HWND hWnd,int message, WPARAM wParam, LPARAM lParam)
{
    FUN_START ();
    if (message == MSG_CREATE)
    {
        //* wxg add
        if (g_curSence && g_curSence->state != ssBlocked)
            g_hMainDC = GetClientDC(hWnd);
        // */
    }
    else if (g_curSence && g_curSence->state != ssBlocked){
         SenceProc(g_curSence,message, wParam, lParam);
    }

    if(message != MSG_ERASEBKGND)
        return DefaultMainWinProc(hWnd,message,wParam,lParam); 
    FUN_END();
}

HWND CreateDemoWindow()
{
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DemoWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    return  CreateMainWindow (&CreateInfo);
}

static void child_wait (int sig)
{
    FUN_START ();
    int pid;
    int status;

    while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
        printf ("--pid=%d--signal=%d--\n", pid, WTERMSIG (status));
    }
    FUN_END();
}

static int my_event_hook (PMSG msg)
{
    FUN_START ();
	if((msg->message >= MSG_FIRSTMOUSEMSG && msg->message <= MSG_LASTMOUSEMSG) || (msg->message >= MSG_FIRSTKEYMSG && msg->message <= MSG_LASTKEYMSG))
	{
        ResetScreenSaver();
		StopScreenSaver();
	}

    if (msg->message == MSG_KEYUP) {
		if(msg->wParam == SCANCODE_F4)
			SendMessage(hSoftKeypad, MSG_IME_CLOSE, 1, 0);
        DemoWinProc(msg->hwnd,msg->message,msg->wParam,msg->lParam);
    }

    FUN_END();
    return HOOK_GOON;
}

static void proceed_sfkbd(BOOL skbd_is_shown)
{
    FUN_START ();
	if(skbd_is_shown) {
#ifdef ENABLE_MLSSHELL
		MLSKeyboardShow(FALSE);
		MLSKeyboardSetActive(FALSE);
#endif
	} else {
#ifdef ENABLE_MLSSHELL
		MLSKeyboardSetActive(TRUE);
#endif
	}
    FUN_END();
}

static int sfkbd_handler (int cli, int clifd, void* buff, size_t len)
{
	int req = *(int*)buff;
	int ret;
	switch (req) {
		case SFKBD_GETSTATE:
			ret = SendMessage(hSoftKeypad, MSG_IME_GETSTATUS, 0, 0);
			break;
		case 0:
			//ret = SendMessage(hSoftKeypad, MSG_IME_CLOSE, 0, 0);
			ret = SendMessage(hSoftKeypad, MSG_IME_CLOSE, 1, 1);
			break;
		case 1:
			//ret = SendMessage(hSoftKeypad, MSG_IME_OPEN, 0, 0);
			ret = SendMessage(hSoftKeypad, MSG_IME_OPEN, 1, 1);
			break;
	}

	return ServerSendReply (clifd, &ret, sizeof(int));
}

/* get bkground and set bkbmp.*/
void SetStartBkBmp(void)
{
    char szValue[256];
    LAYOUT_BKGROUND bkg;

    if (mg_dsk_ops.init)
        mg_dsk_ops.init();
     
    if(GetValueFromEtcFile(CFGFILE, TOP_LAYOUT, "bkground", 
                szValue, sizeof(szValue)-1) == ETC_OK)
    {
        ParseBkground(szValue, &bkg);
    }
    if (bkg.type == lbtBmpBrush)
        SetDesktopBkgnd(bkg.data.tilBmp);
}

HWND g_hBusyWnd;

static int busywin_handler (int cli, int clifd, void* buff, size_t len)
{
    int req = *((int*)buff);
    int ret = 1;

    if (req) {
        g_bBusyWin = TRUE;
        ShowWindow (g_hBusyWnd, SW_SHOWNORMAL);
        SetTimer (g_hBusyWnd, 100, 10);
    }
    else {
        g_bBusyWin = FALSE;
        KillTimer (g_hBusyWnd, 100);
        ShowWindow (g_hBusyWnd, SW_HIDE);
    }

    return ServerSendReply (clifd, &ret, sizeof(int));
}

void* g_pSharedBuffer;

BOOL g_bAppStarted;

static int sharedbuffer_handler (int cli, int clifd, void* buff, size_t len)
{
    int req = *((int*)buff);

    switch (req) {
        case REQ_BUFFER_READY:
            g_bAppStarted = TRUE;
            break;
        default:
            break;
    }

    return ServerSendReply (clifd, NULL, 0);
}

extern int tscal_win_entry (void);
int MiniGUIMain (int args, const char* arg[])
{
    MSG msg;
	struct sigaction siga;


    siga.sa_handler = child_wait;
    siga.sa_flags  = 0;
    memset (&siga.sa_mask, 0, sizeof(sigset_t));
    sigaction (SIGCHLD, &siga, NULL);

    OnNewDelClient = on_new_del_client;

    SetStartBkBmp();
	SetCustomDesktopOperationSet(&mg_dsk_ops);
    if (!ServerStartup (0 , 0 , 0)) {
        fprintf (stderr, 
                 "Can not start the server of MiniGUI-Processes: mginit.\n");
        return 1;
    }
    
	//SetCustomDesktopOperationSet(&mg_dsk_ops);

    if(!RegisterRequestHandler (SFKBD_REQID, sfkbd_handler)) {
        fprintf (stderr, "Can not register sfkbd handler.\n");
        return 2;
	}

#ifdef _MGIAL_IPAQ_H5400
    tscal_win_entry(); 
#endif
  
    if ((hSoftKeypad = CreateSoftKeypad (proceed_sfkbd)) == HWND_INVALID) {
        fprintf (stderr, "Can not create soft key pad.\n");
        return 3;
    }

#ifdef ENABLE_MLSSHELL
	RECT mlskbd = {160, 0, 240, 320};
	RECT mrc = {235, 0, 240, 320};
    if ((MLSKeyboardInit(mlskbd, mrc, 0, 128, 5, my_event_hook)) < 0) {
        fprintf (stderr, "Can not create MLShadow keyboard.\n");
        return 4;
    }
#else
   	SetServerEventHook (my_event_hook);
#endif

	if(!InitSence()) {
		fprintf(stderr, "Cannot initalize the sence\n");
		exit(-1);
	}

	GetScreenSaverInfoFromEtc();

	g_hMainWnd = CreateDemoWindow();
	SetTimerEx (HWND_NULL, 100, 100, drag_time_proc);

    g_hBusyWnd = CreateBusyWindow (g_hMainWnd);
    if (g_hBusyWnd != HWND_INVALID) {
        if (!RegisterRequestHandler (BUSYWIN_REQID, busywin_handler)) {
            fprintf (stderr, "Can not register busywin handler.\n");
            return 3;
	    }
    }

    g_pSharedBuffer = CreateSharedBuffer ();
    if (g_pSharedBuffer) {
        if (!RegisterRequestHandler (SHARED_BUFF_REQID, sharedbuffer_handler)) {
            fprintf (stderr, "Can not register shared buffer handler.\n");
            return 4;
	    }
    }

    while (GetMessage (&msg, HWND_DESKTOP)) {
		drag_event_hook (&msg);
		TranslateMessage (&msg);
        DispatchMessage (&msg);
    }

   return 0;
}

