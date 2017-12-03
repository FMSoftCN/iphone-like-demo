/*
 * the time manager
 *
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include "phone_icon.h"

#define DESKTOP_TIMER_ID 100
#define DESKTOP_CHECK_DRAG_TIMER_SPEED 20
#define DESKTOP_ANIMATE_TIMER_SPEED 16 

//#define USE_OS_TIMER

#define DEBUG 0

#if DEBUG 
#define BEGIN_TIME() do { \
	struct timeval tv1, tv2; \
	gettimeofday(&tv1,NULL);

#define END_TIME() \
	gettimeofday(&tv2,NULL); \
	printf("%s:%d: time=%d usec\n",__FUNCTION__,__LINE__, \
	(tv2.tv_sec-tv1.tv_sec)*1000000+tv2.tv_usec-tv1.tv_usec); \
}while(0);
#else
#define BEGIN_TIME()
#define END_TIME()
#endif

#define MSG_MYTIMER MSG_USER+100
extern HWND g_hMainWnd;

BOOL InitAnimateTimer();

void TimerCallback(int timerId);

typedef void (*POnAllAnimateFinished)();

extern POnAllAnimateFinished OnAllAnimateFinished;

typedef struct _ANIMATE_INTF{
	BOOL (*isEnd)(struct _ANIMATE_INTF*);
	BOOL (*equal)(struct _ANIMATE_INTF*, void*);
	void (*showCurFrame)(struct _ANIMATE_INTF*, HDC);
	void (*nextFrame)(struct _ANIMATE_INTF*);
	void (*free)(struct _ANIMATE_INTF*);
}ANIMATE_INTF;

typedef struct _ANIMATE_OBJ{
	ANIMATE_INTF *intf;
}ANIMATE_OBJ;

#define AM_CALL(aobj,func)  ((ANIMATE_INTF*)(ANIMATE_OBJ*)(aobj)->intf)->func

#define AmIsEnd(aobj)  AM_CALL((ANIMATE_OBJ*)aobj,isEnd)(aobj)
#define AmEqual(aobj,key) AM_CALL((ANIMATE_OBJ*)aobj,equal)(aobj,key)
#define AmShowCurFrame(aobj,hdc) AM_CALL((ANIMATE_OBJ*)aobj,showCurFrame)(aobj,hdc)
#define AmNextFrame(aobj) AM_CALL(aobj,nextFrame)((ANIMATE_OBJ*)aobj)
#define AmFree(aobj) AM_CALL(aobj,free)((ANIMATE_OBJ*)aobj)

#define TIME_PRI_LOW  0
#define TIME_PRI_MID  1
#define TIME_PRI_HIG  2
#define TIME_PRI_VERY_HIG 3
BOOL InsertAnimate(int priority,ANIMATE_OBJ* aobj, BOOL autoDelete);

BOOL DeleteAnimate(ANIMATE_OBJ* obj);

typedef struct _ANIMATE_BKCLEAN{
	RECT rtBk;
	void *user_data;
	void (*clearBkground)(HDC,PRECT,void*);
	struct _ANIMATE_BKCLEAN * next;
}ANIMATE_BKCLEAN;

typedef struct _animate_obj_link{
	ANIMATE_OBJ * obj;
	int flag;
	struct _animate_obj_link* next;
}animate_obj_link;

BOOL InsertAnimateBkClean(ANIMATE_BKCLEAN* bkclean);

BOOL DeleteAnimateBkClean(ANIMATE_BKCLEAN* bkclean);

ANIMATE_OBJ* FindAnimateObj(void* key, int priority/*=-1*/);

BOOL ChangePriority(void* key, int new_priority);
ANIMATE_OBJ* CreateIconAnimate(PHONE_ICON *icon);


void ResetTimerInterval(int ms);

#endif
