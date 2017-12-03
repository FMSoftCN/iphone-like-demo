/*
 ** $ld: wifiset.c 
 *
 *
 *
 ** Copyright (C) 2004 ~ 2007 Feynman Software.
 *
 ** License: GPL
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "wifiset.h"
#include "ipset.h"

#define IDC_MANWIFI_DONE      300
#define IDC_MANWIFI_CANCEL    301
#define IDC_MANWIFI_ESSID     302
#define IDC_MANWIFI_KEY       303

static HDC g_hWiFiManualBKG;
static BITMAP g_stWiFiManualBKG;
static BITMAP g_stWiFiManualCancel;
static BITMAP g_stWiFiManualDone;
static char* exec;

static int WiFiManualProc (HWND hWnd, int nMessage, WPARAM wParam, LPARAM lParam)
{
    switch (nMessage)
    {
        case MSG_CREATE:
            {
                CreateWindow ("MGD_BUTTON","",
                        WS_CHILD | WS_VISIBLE,
                        IDC_MANWIFI_CANCEL,
                        4, 21, 42, 21,
                        hWnd, 
                        (DWORD)&g_stWiFiManualCancel);
                CreateWindow ("MGD_BUTTON", "",
                        WS_CHILD | WS_VISIBLE,
                        IDC_MANWIFI_DONE,
                        196, 21,40, 21,
                        hWnd, 
                        (DWORD)&g_stWiFiManualDone);

                CreateWindowEx (CTRL_SLEDIT, "",
                        WS_CHILD | WS_VISIBLE,
                        WS_EX_TRANSPARENT,
                        IDC_MANWIFI_ESSID,
                        83, 73, 110, 20,
                        hWnd, 0);

                CreateWindowEx (CTRL_SLEDIT, "",
                        WS_CHILD | WS_VISIBLE | ES_PASSWORD, 
                        WS_EX_TRANSPARENT,
                        IDC_MANWIFI_KEY,
                        83, 122, 110, 20,
                        hWnd, 0);
            }
            break;
        case MSG_ERASEBKGND:
            {
                HDC hdc = (HDC)wParam;
                const RECT* clip = (const RECT*) lParam;
                BOOL fGetDC = FALSE;
                RECT rcTemp;

                if (hdc == 0){
                    //hdc = BeginPaint (hWnd);
                    hdc = GetSecondaryClientDC(hWnd);
                    fGetDC = TRUE;
                }
                if (clip){
                    rcTemp = *clip;
                    ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
                    ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
                    IncludeClipRect (hdc, &rcTemp);
                }
                BitBlt (g_hWiFiManualBKG, 0, 0, 0, 0, hdc, 0, 0, 0 );
                if (fGetDC) {
                    ReleaseSecondaryDC(hWnd, hdc);
                    //EndPaint (hWnd, hdc);
                }
                return 0;
            }
        case MSG_CLOSE:
             CloseSYSIMEMethod (FALSE);
             SendMessage (g_hSysMain, MSG_CLOSE_APP, 0, 1);
             return 0;
        case BUTTON_KEYDOWN:
             {
                 switch (LOWORD (wParam))
                 {
                     case IDC_MANWIFI_DONE :
                         {
                            char buffessid [32];
                            char buffpwd [32];

                            memset (buffessid, 0x0, sizeof (buffessid));
                            memset (buffpwd, 0x0, sizeof (buffpwd));
                            GetWindowText(GetDlgItem(hWnd, IDC_MANWIFI_ESSID), buffessid, sizeof(buffessid));         
                            GetWindowText(GetDlgItem(hWnd, IDC_MANWIFI_KEY), buffpwd, sizeof(buffpwd));         
                            asprintf (&exec, "iwconfig wlan0 essid %s key %s", buffessid, buffpwd); 
                            system (exec);
                            free (exec);
                            ShowRollAnimate (TRUE);
                            sleep (6);
                            ShowRollAnimate (FALSE);
                            SendMessage (hWnd, MSG_CLOSE, 0, 0);
                         }
                         break;
                     case IDC_MANWIFI_CANCEL:
                         SendMessage (hWnd, MSG_CLOSE, 0, 0);
                         break;
                     default:
                         break;
                 }
             }
             return 0;

        case MSG_SYS_CLOSE:
             UnloadBitmap (&g_stWiFiManualBKG);
             UnloadBitmap (&g_stWiFiManualDone);
             UnloadBitmap (&g_stWiFiManualCancel);
             DestroyMainWindow (hWnd);
             DeleteCompatibleDC (g_hWiFiManualBKG);
             return 0;

    }
    return DefaultMainWinProc (hWnd, nMessage, wParam, lParam);
}

static void InitWifiManualInfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_NONE;
    pCreateInfo->dwExStyle = WS_EX_AUTOSECONDARYDC;
    pCreateInfo->spCaption = "";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor (0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = WiFiManualProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 240;
    pCreateInfo->by = 320;
    pCreateInfo->iBkColor = PIXEL_lightwhite;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

HWND CreateWiFiManualWindow (void)
{
    MAINWINCREATE stCreateInfo;
    InitWifiManualInfo (&stCreateInfo);

    LoadBitmap (HDC_SCREEN, &g_stWiFiManualBKG, WIFISETTING_RES"ManualWiFiBKG.png");
    LoadBitmap (HDC_SCREEN, &g_stWiFiManualDone, WIFISETTING_RES"ManualDone.png");
    LoadBitmap (HDC_SCREEN, &g_stWiFiManualCancel, WIFISETTING_RES"ManualCancel.png");
    g_hWiFiManualBKG = CreateCompatibleDC (HDC_SCREEN);
    FillBoxWithBitmap (g_hWiFiManualBKG, 0, 0, 240, 320, &g_stWiFiManualBKG);

    return CreateMainWindow (&stCreateInfo);
}
