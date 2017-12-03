#include <minigui/common.h>
#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "common.h" 
#include "../sharedbuff.h"

#ifdef  ENABLE_LANDSCAPE
#define OTHERS_BMP_BG    "res/menu.png"
#else
#define OTHERS_BMP_BG    "res/others_bg_pt.png" /*portrait*/
#endif

//#define OTHERS_TIMER    500
#define OTHERS_TIMER    100

static BITMAP backbitmap;

void display_text(HDC hdc)
{
    LOGFONT* lrc_font;
    LOGFONT* old_font;
    RECT rect;
#if 1
    lrc_font = CreateLogFont (NULL, "Arial", "UTF-8",
            FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 30, 0);
#else
    lrc_font = CreateLogFont (NULL, "song", "GB2312",
            FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 30, 0);
#endif

    old_font = SelectFont(hdc, lrc_font);
    SetTextColor(hdc, COLOR_lightwhite);
    SetBkMode(hdc, BM_TRANSPARENT);
    SetRect(&rect, IPHONE_MAIN_WIDTH/2-100 , IPHONE_MAIN_HEIGHT/2-100, 
            IPHONE_MAIN_WIDTH/2+100 , IPHONE_MAIN_HEIGHT/2);

#if 1
    DrawText(hdc, "Unimplemented!", -1, &rect,
            DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_WORDBREAK|DT_NOCLIP);
#else
    DrawText(hdc, "应用程序未添加", -1, &rect,
            DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_WORDBREAK|DT_NOCLIP);
#endif

    SelectFont(hdc, old_font);
    DestroyLogFont(lrc_font);
}

static int OthersWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) {
        case MSG_CREATE:
            if (LoadBitmap (HDC_SCREEN, &backbitmap, OTHERS_BMP_BG))
                return -1;
            UpdateWindow(hWnd, TRUE);
            return 0;

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            display_text(hdc);
            EndPaint (hWnd, hdc);
            SetTimer(hWnd, 100, OTHERS_TIMER);
            return 0;
        case MSG_TIMER:
            exit(0);
            break;
        case MSG_ERASEBKGND:
            hdc = (HDC)wParam;
            const RECT* clip = (const RECT*) lParam;
            BOOL fGetDC = FALSE, fSecondary = TRUE;
            RECT rcTemp;

            if (hdc == 0) {
                //hdc = GetClientDC (hWnd);
#if 0
                if ((hdc = GetSecondaryDC(hWnd)) == HDC_SCREEN) {
                    hdc = GetClientDC(hWnd);
                    fSecondary = FALSE;
                }
#else
                hdc = GetSecondaryClientDC(hWnd);
#endif
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
#if 0
                if (fSecondary)
                    ReleaseSecondaryDC(hWnd, hdc);
                else
                    ReleaseDC (hdc);
#else
                ReleaseSecondaryDC(hWnd, hdc);
#endif
            }
            return 0;
#if 0
        case MSG_CLOSE:
            KillTimer(hWnd, 100);
            UnloadBitmap (&backbitmap);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
#endif
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "others" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_NONE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Unimplemented";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = OthersWinProc;
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
    ShowWindow (hMainWnd, SW_SHOWNORMAL);
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

