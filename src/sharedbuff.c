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
#include <string.h>

#include <errno.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "sharedbuff.h"

#define SHM_PARAM 0666

static size_t sz_shared_buffer;
static void* memptr;

static void delete_shared_buffer (void)
{
    int shmid = shmget (0x731128, sz_shared_buffer, 0); 

    shmdt (memptr);
    shmctl (shmid, IPC_RMID, NULL);
}

void* CreateSharedBuffer (void)
{
    int shmid;

    sz_shared_buffer = GetGDCapability (HDC_SCREEN, GDCAP_PITCH) * RECTH (g_rcScr);

    shmid = shmget (0x731128, sz_shared_buffer, SHM_PARAM | IPC_CREAT | IPC_EXCL); 
    if (errno == EEXIST)
        shmid = shmget (0x731128, sz_shared_buffer, 0); 

    if (shmid == -1) { 
        goto error;
    }

    memptr = shmat (shmid, 0, 0);
    if (memptr == (char*)-1)
        goto error;
    else
        memset (memptr, 0, sz_shared_buffer);

    atexit (delete_shared_buffer);

    return memptr;

error:
    perror ("CreateSharedBuffer");
    return NULL;
}

