/* 
** $Id: mginit.c,v 1.22 2007-08-30 01:20:10 wyan Exp $
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "sharedbuff.h"

#define MSG_SHAREBUFFER_READY           5000

void* AttachSharedBuffer (void)
{
    int shmid;
    void* memptr;
    size_t sz_shared_buffer = GetGDCapability (HDC_SCREEN, GDCAP_PITCH) * RECTH (g_rcScr);

    shmid = shmget (0x731128, sz_shared_buffer, 0); 
    if (shmid == -1) { 
        goto error;
    } 

    memptr = shmat (shmid, 0, 0);
    if (memptr == (char*)-1)
        goto error;

    return memptr;

error:
    perror ("AttachSharedBuffer");
    return NULL;
}

void NotifyServerSharedBufferReady (void)
{
    int status = REQ_BUFFER_READY;

    REQUEST req = {SHARED_BUFF_REQID, &status, sizeof (int)};
    ClientRequest (&req, NULL, 0);
}

HDC CreateSecondaryDCFromShareBuffer (void* shared_buffer)
{
    BITMAP bmp;

    if (shared_buffer == NULL)
        return HDC_SCREEN;

    bmp.bmType = BMP_TYPE_NORMAL;
    bmp.bmBitsPerPixel = GetGDCapability (HDC_SCREEN, GDCAP_DEPTH);
    bmp.bmBytesPerPixel = GetGDCapability (HDC_SCREEN, GDCAP_BPP);
    bmp.bmAlpha = 0;
    bmp.bmColorKey = 0;
    bmp.bmWidth = RECTW (g_rcScr);
    bmp.bmHeight = RECTH (g_rcScr);
    bmp.bmPitch = GetGDCapability (HDC_SCREEN, GDCAP_PITCH);
    bmp.bmBits = shared_buffer;
    //bmp.bmAlphaPixelFormat = NULL;

    return CreateMemDCFromBitmap (HDC_SCREEN, &bmp);
}

void ShowWindowUsingShareBuffer (HWND hwnd)
{
    MSG msg;
    HDC private_dc = CreateSecondaryDCFromShareBuffer (AttachSharedBuffer());
    HDC curr_prv_dc;

    curr_prv_dc = GetSecondaryDC (hwnd);
    if (curr_prv_dc == HDC_SCREEN) {
        SetSecondaryDC (hwnd, private_dc, NULL);
    }

    ShowWindow (hwnd, SW_SHOWNORMAL);

    /* wait for MSG_IDLE */
    while (GetMessage(&msg, hwnd)) {
        if (msg.message == MSG_IDLE)
            break;
        DispatchMessage(&msg);
    }

    if (curr_prv_dc == HDC_SCREEN) {
        /* restore */
        SetSecondaryDC (hwnd, HDC_SCREEN, NULL);
    }
    else {
        /* blit to shared buffer */
        BitBlt (curr_prv_dc, 0, 0, RECTW (g_rcScr), RECTH (g_rcScr),
                private_dc, 0, 0, 0);
    }

    SendMessage (hwnd, MSG_SHAREBUFFER_READY, 0, 0);
    NotifyServerSharedBufferReady ();
    //DeleteSecondaryDC (private_dc);
    DeleteMemDC(private_dc);
}

