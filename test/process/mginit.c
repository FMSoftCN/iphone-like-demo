/*
** $Id: mginit.c 643 2007-04-19 09:17:59Z liupeng $
**
** The mginit program of feynman PMP.
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2006/11/17
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
//#include <minigui/mgext.h>
#include <minigui/fixedmath.h>

#include "config.h"
#include "softkeywindow.h"
#ifdef ENABLE_MLSSHELL
#include "mlskbd.h"
#endif

#ifndef _MGRM_PROCESSES
    #error "*******************************************************************"
    #error "**** Your MiniGUI is configured as MiniGUI-Threads.            ****"
    #error "**** This program is a server of MiniGUI-Processes,            ****"
    #error "****   it can only run on MiniGUI-Processes,                   ****"
    #error "****   not on MiniGUI-Threads.                                 ****"
    #error "**** If you want to configure MiniGUI as MiniGUI-Processes,    ****"
    #error "****   please run `./configure --enable-procs'                 ****"
    #error "****   when configuring MiniGUI                                ****"
    #error "****   and build and reinstall MiniGUI.                        ****"
    #error "**** Note that this is not a fatal error.                      ****"
    #error "*******************************************************************"
#else

static HWND hSoftKeypad;

static pid_t exec_app (const char* file_name, const char* app_name)
{
    pid_t pid = 0;

    if ((pid = vfork ()) > 0) {
        fprintf (stderr, "new child, pid: %d.\n", pid);
    }
    else if (pid == 0) {
        execl (file_name, app_name, NULL);
        perror ("execl");
        _exit (1);
    }
    else {
        perror ("vfork");
    }

    return pid;
}

static int my_event_hook (PMSG msg)
{
    if (msg->message == MSG_KEYUP) {
		printf("key up\n");
    }

    return HOOK_GOON;
}

static void proceed_mls_kbd(BOOL skbd_is_shown)
{
#ifdef ENABLE_MLSSHELL
	if(skbd_is_shown) {
		MLSKeyboardShow(FALSE);
		MLSKeyboardSetActive(FALSE);
	} else {
		MLSKeyboardSetActive(TRUE);
	}
#endif
}

int MiniGUIMain (int args, const char* arg[])
{
    MSG msg;
	static HWND hSoftKeypad;
    
    if (!ServerStartup (0, 0, 0)) {
        fprintf (stderr, 
                 "Can not start the server of MiniGUI-Processes: mginit.\n");
        return 1;
    }

    if ((hSoftKeypad = CreateSoftKeypad (proceed_mls_kbd)) == HWND_INVALID) {
        fprintf (stderr, "Can not create soft key pad.\n");
        return 2;
    }

#ifdef ENABLE_MLSSHELL
	RECT mlskbd = {160, 0, 240, 320};
	RECT mrc = {235, 0, 240, 320};
    if ((MLSKeyboardInit(mlskbd, mrc, 0, 128, 5, my_event_hook)) < 0) {
        fprintf (stderr, "Can not create MLShadow keyboard.\n");
        return 3;
    }
#else
   	SetServerEventHook (my_event_hook);
#endif

    exec_app ("./notebook", "notebook");

    while (GetMessage (&msg, hSoftKeypad)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }

    return 0;
}

#endif

