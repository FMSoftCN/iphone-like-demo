/*
** $Id: timeset.h  2008-07-29 09:38:24Z xwyan $
**
**timeset.h: head file of timeset.c
**
** Copyright (C) 2008 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: CaiLiangda.
*/

#ifndef __TIMESET_H_
#define __TIMESET_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "moncal.h"
#include "../sysmain.h"
time_t __mg_time (time_t * timer);
time_t __mg_mktime (struct tm * timeptr);
struct tm *__mg_localtime (const time_t * timer);
HWND CreateDateWindow (void);

#ifdef  __cplusplus
}
#endif

#endif    // __MONTHCALENDAR_IMPL_H__
