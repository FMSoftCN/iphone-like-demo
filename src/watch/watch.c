#include <stdio.h>
#include <string.h>
#include <time.h>

#include <minigui/common.h>
#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "common.h" 
#include "../sharedbuff.h" 

#ifdef  ENABLE_LANDSCAPE
#define W_BOX_X     117
#define W_BOX_Y     111
#define W_BMP_BG    "res/watch_bg.png"
#else
#define W_BOX_X     77
#define W_BOX_Y     148
#define W_BMP_BG    "res/watch_bg_pt.png" /*portrait*/
#endif

#define W_BOX_W     (80/8)
#define W_BOX_H     14
#define W_BMP_BN    "res/watch_num.png"
#define W_DIGI_W    10 

typedef struct _WATCH_INFO{
    int hourH;
    int hourL;
    int minH;
    int minL;
    int secH;
    int secL;
    RECT hH;
    RECT hL;
    RECT pH;
    RECT mH;
    RECT mL;
    RECT pL;
    RECT sH;
    RECT sL;
}WATCH_INFO;

static BITMAP backbitmap;
static BITMAP digibitmap;
static WATCH_INFO g_watch;

static void get_current_time(void)
{
    time_t timep;
    struct tm *p;
    time(&timep);
    p=localtime(&timep);
    g_watch.hourH = p->tm_hour/10;
    g_watch.hourL = p->tm_hour%10;
    g_watch.minH = p->tm_min/10;
    g_watch.minL = p->tm_min%10;
    g_watch.secH = p->tm_sec/10;
    g_watch.secL = p->tm_sec%10;
}

static void init_all_rect(void)
{
    SetRect(&g_watch.hH, W_BOX_X, W_BOX_Y, W_BOX_X + W_BOX_W, W_BOX_Y + W_BOX_H);
    SetRect(&g_watch.hL, W_BOX_X + W_BOX_W, W_BOX_Y, W_BOX_X + 2*W_BOX_W, W_BOX_Y + W_BOX_H);
    SetRect(&g_watch.pH, W_BOX_X + 2*W_BOX_W, W_BOX_Y, W_BOX_X + 3*W_BOX_W, W_BOX_Y + W_BOX_H);
    SetRect(&g_watch.mH, W_BOX_X + 3*W_BOX_W, W_BOX_Y, W_BOX_X + 4*W_BOX_W, W_BOX_Y + W_BOX_H);
    SetRect(&g_watch.mL, W_BOX_X + 4*W_BOX_W, W_BOX_Y, W_BOX_X + 5*W_BOX_W, W_BOX_Y + W_BOX_H);
    SetRect(&g_watch.pL, W_BOX_X + 5*W_BOX_W, W_BOX_Y, W_BOX_X + 6*W_BOX_W, W_BOX_Y + W_BOX_H);
    SetRect(&g_watch.sH, W_BOX_X + 6*W_BOX_W, W_BOX_Y, W_BOX_X + 7*W_BOX_W, W_BOX_Y + W_BOX_H);
    SetRect(&g_watch.sL, W_BOX_X + 7*W_BOX_W, W_BOX_Y, W_BOX_X + 8*W_BOX_W, W_BOX_Y + W_BOX_H);
}

static void draw_time(HDC hdc)
{
    FillBoxWithBitmapPart(hdc, g_watch.hH.left, g_watch.hH.top, 
            10, RECTH(g_watch.hH), 0, RECTH(g_watch.hH), &digibitmap, g_watch.hourH*W_DIGI_W, 0);
    FillBoxWithBitmapPart(hdc, g_watch.hL.left, g_watch.hL.top, 
            10, RECTH(g_watch.hL), 0, RECTH(g_watch.hL), &digibitmap, g_watch.hourL*W_DIGI_W, 0);
    FillBoxWithBitmapPart(hdc, g_watch.pH.left, g_watch.pH.top, 
            9, RECTH(g_watch.pH), 0, RECTH(g_watch.pH), &digibitmap, 10*W_DIGI_W, 0);
    FillBoxWithBitmapPart(hdc, g_watch.mH.left, g_watch.mH.top, 
            10, RECTH(g_watch.mH), 0, RECTH(g_watch.mH), &digibitmap, g_watch.minH*W_DIGI_W, 0);
    FillBoxWithBitmapPart(hdc, g_watch.mL.left, g_watch.mL.top, 
            10, RECTH(g_watch.mL), 0, RECTH(g_watch.mL), &digibitmap, g_watch.minL*W_DIGI_W, 0);
    FillBoxWithBitmapPart(hdc, g_watch.pL.left, g_watch.pL.top, 
            9, RECTH(g_watch.pL), 0, RECTH(g_watch.pL), &digibitmap, 10*W_DIGI_W, 0);
    FillBoxWithBitmapPart(hdc, g_watch.sH.left, g_watch.sH.top, 
            10, RECTH(g_watch.sH), 0, RECTH(g_watch.sH), &digibitmap, g_watch.secH*W_DIGI_W, 0);
    FillBoxWithBitmapPart(hdc, g_watch.sL.left, g_watch.sL.top, 
            10, RECTH(g_watch.sL), 0, RECTH(g_watch.sL), &digibitmap, g_watch.secL*W_DIGI_W, 0);
}

static LRESULT WatchWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) {
        case MSG_CREATE:
            if (LoadBitmap (HDC_SCREEN, &backbitmap, W_BMP_BG))
                return -1;
            if (LoadBitmap (HDC_SCREEN, &digibitmap, W_BMP_BN))
                return -1;

            memset(&g_watch, 0, sizeof(WATCH_INFO));
            init_all_rect();
            get_current_time();
            UpdateWindow(hWnd, TRUE);
            return 0;

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            draw_time(hdc);
            EndPaint (hWnd, hdc);
            SetTimer(hWnd, 100, 100);
            return 0;
        case MSG_ERASEBKGND:
            hdc = (HDC)wParam;
            const RECT* clip = (const RECT*) lParam;
            BOOL fGetDC = FALSE;
            RECT rcTemp;

            if (hdc == 0) {
                hdc = GetSecondaryClientDC(hWnd);
                fGetDC = TRUE;
            }

            if (clip) {
                rcTemp = *clip;
                ScreenToClient(hWnd, &rcTemp.left, &rcTemp.top);
                ScreenToClient(hWnd, &rcTemp.right, &rcTemp.bottom);
                IncludeClipRect(hdc, &rcTemp);
            }

            FillBoxWithBitmap(hdc, 0, 0, 0, 0, &backbitmap);
            if (fGetDC){
                ReleaseSecondaryDC(hWnd, hdc);
            }
            return 0;
        case MSG_TIMER:
            get_current_time();
            InvalidateRect(hWnd, &g_watch.sL, TRUE);
            if (!g_watch.secL)
                InvalidateRect(hWnd, &g_watch.sH, TRUE);
            if (!g_watch.secH)
                InvalidateRect(hWnd, &g_watch.mL, TRUE);
            if (!g_watch.minL)
                InvalidateRect(hWnd, &g_watch.mH, TRUE);
            if (!g_watch.minH)
                InvalidateRect(hWnd, &g_watch.hL, TRUE);
            if (!g_watch.hourL)
                InvalidateRect(hWnd, &g_watch.hH, TRUE);
            break;

        case MSG_CLOSE:
            UnloadBitmap (&backbitmap);
            UnloadBitmap (&digibitmap);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

#include "../sharedbuff.h"

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "watch" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_NONE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Watch";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = WatchWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = IPHONE_MAIN_WIDTH;
    CreateInfo.by = IPHONE_MAIN_HEIGHT;
    CreateInfo.iBkColor = PIXEL_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

#if 0
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
#else
    ShowWindowUsingShareBuffer (hMainWnd);
#endif

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

