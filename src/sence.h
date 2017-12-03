/*
 * sence.h the sence of iphone-like demo
 *
 *
 */

#ifndef SENCE_H
#define SENCE_H

#include "layout.h"
#include "layouthlp.h"

typedef struct _SENCE{
	int id;
	int state;
	int (*senceProc)(struct _SENCE*, int ,WPARAM,LPARAM);
	void (*switchOut)(struct _SENCE* _this);
	void (*switchIn)(struct _SENCE* _this);
}SENCE;

#define SENCE_CALL(sence,func)  ((SENCE*)(sence))->func
#define SenceProc(sence, message, wParam, lParam) \
	SENCE_CALL(sence,senceProc)((SENCE*)sence, message, (WPARAM)wParam,(LPARAM)lParam)
#define SenceSwitchOut(sence)  SENCE_CALL(sence,switchOut)(sence)
#define SenceSwitchIn(sence) SENCE_CALL(sence,switchIn)(sence);

enum senceState{
	ssNormal = 0,
	ssBlocked
};

BOOL InitSence();
void SetCurrentSence(int newId);

extern SENCE* g_curSence;

extern HWND g_hMainWnd;
extern HDC g_hMainDC;

#define SENCE_DESKTOP 0
#define SENCE_APP     1

#endif
