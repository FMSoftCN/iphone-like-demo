#include <stdio.h>
#include <stdlib.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include "common-animate/p-code.h"
#include "common-animate/animate.h"

static void draw_animate (HDC hdc, ANIMATE* ani)
{
    HDC memdc = (HDC)ani->img;
	SetMemDCAlpha(memdc, MEMDC_FLAG_SRCALPHA, GetAnimateA(ani));
	BitBlt(memdc, 0, 0, GetAnimateW(ani), GetAnimateH(ani), hdc, 
            GetAnimateX(ani), GetAnimateY(ani), 0);
}

static void draw_bkgnd (HDC hdc, const RECT* rc, void* param)
{
    BitBlt ((HDC)param, 0, 0, RECTWP(rc), RECTHP(rc), hdc, rc->left, rc->top, 0);
}

static void on_finished (ANIMATE_SENCE* as)
{
    if (as->param)
        DeleteCompatibleDC ((HDC)as->param);

    DeleteCompatibleDC ((HDC)as->normal->img);
    free (as->normal);
    as->normal = NULL;
}

void StartAlphaAnimate (int interval, int all_times, HDC hdc, const BITMAP* bitmap, const RECT* rc)
{
    TIME_LINE* tl;
    ANIMATE* a;
    ANIMATE_SENCE* as;
    int w = RECTWP(rc); 
    int h = RECTHP(rc); 
    HDC bkmemdc = CreateCompatibleDCEx (hdc, w, h);
    HDC fgmemdc = CreateCompatibleDCEx (hdc, w, h);

    BitBlt (hdc, rc->left, rc->top, RECTWP(rc), RECTHP(rc), bkmemdc, 0, 0, 0);
    FillBoxWithBitmap (fgmemdc, 0, 0, w, h, bitmap);

    as = CreateAnimateSence (interval, all_times,
            hdc, draw_animate, draw_bkgnd, on_finished, rc, (void*)bkmemdc);

    a = (ANIMATE*) calloc (1, sizeof(ANIMATE));
    a->img = (void*)fgmemdc;
	SetAnimateW(a, w);
	SetAnimateH(a, h);
    SetAnimateA(a, 0);
	InsertAnimate(as, a, FALSE);	

	tl = CreateTimeLine((int)a);
	TLAlphaTo(tl, a, 255, all_times/interval - 3);
    TLRun (tl, all_times/interval - 3);
	StartTimeLine(as, tl, NULL, 0, NULL);
	StartTimerAnimateSence(as);
}


#if  1
//usage:
static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            {
                BITMAP bkbmp;
                BITMAP fgbmp;
                RECT rc = g_rcScr;

                //LoadBitmap (HDC_SCREEN, &bkbmp, "fall_night.jpg");
                //LoadBitmap (HDC_SCREEN, &fgbmp, "plain_tree.jpg");
                LoadBitmap (HDC_SCREEN, &fgbmp, "res/memo_bk.png");
                LoadBitmap (HDC_SCREEN, &bkbmp, "res/watch_sw.png");
                FillBoxWithBitmap (HDC_SCREEN, rc.left, rc.top, RECTW(rc), RECTH(rc), &bkbmp);

                StartAlphaAnimate (50, 5000, HDC_SCREEN, &fgbmp, &rc, 0, 255);
            }
            return 0;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            break;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "alpha animate" , 0 , 0);
#endif

#if 1
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "alpha animate";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = HelloWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 0;
    CreateInfo.by = 0;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
#endif
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

#endif

