
#include "screen_protect.h"
#include "../sysmain.h"
#define IDC_SAVE_BTN         200
#define IDC_CANCEL_BTN       201

#define RECT_HALF_WIDTH      10
#define RECT_HALF_HEIGHT     10


static BITMAP g_stBMSave;
static BITMAP g_stBMCancel;
static BITMAP g_stBMPBack;
static BITMAP g_stBMRotate;
static int g_nWaitTime = 10;
static int g_nSetZone = 3;
static HDC g_hdcBKG;
static POINT g_nPoint [7] = {{76, 241}, {81, 221}, {95, 205}, {116, 199}, {137, 205}, {151, 221}, {156, 241}};
static RECT g_stRect[7] = {
  {80 - RECT_HALF_WIDTH, 246 - RECT_HALF_HEIGHT, 80 + RECT_HALF_WIDTH, 246 + RECT_HALF_HEIGHT}, 
  {85 - RECT_HALF_WIDTH, 226 - RECT_HALF_HEIGHT, 85 + RECT_HALF_WIDTH, 226 + RECT_HALF_HEIGHT},
  {99 - RECT_HALF_WIDTH, 210 - RECT_HALF_HEIGHT, 99 + RECT_HALF_WIDTH, 210 + RECT_HALF_HEIGHT},
  {120 - RECT_HALF_WIDTH, 204 - RECT_HALF_HEIGHT, 120 + RECT_HALF_WIDTH, 204 + RECT_HALF_HEIGHT},
  {141 - RECT_HALF_WIDTH, 210 - RECT_HALF_HEIGHT, 141 + RECT_HALF_WIDTH, 210 + RECT_HALF_HEIGHT},
  {155 - RECT_HALF_WIDTH, 226 - RECT_HALF_HEIGHT, 155 + RECT_HALF_WIDTH, 226 + RECT_HALF_HEIGHT},
  {160 - RECT_HALF_WIDTH, 246 - RECT_HALF_HEIGHT, 160 + RECT_HALF_WIDTH, 246 + RECT_HALF_HEIGHT}, 
};
#if 0
static DLGTEMPLATE ScreenSaverDialog =
{
    WS_VISIBLE, 
    WS_EX_NONE,
    0, 0, 240, 320, 
    "Screen Saver",
    0, 0,
    0, NULL,
    0
};

static CTRLDATA ScreenSaverControl[]=
{
    {
        MGD_BUTTON, //CTRL_STATIC,
        WS_CHILD | WS_VISIBLE, 
        4, 6, 42, 21,
        IDC_SAVE_BTN,
        "SAVE",
        (DWORD)&g_stBMSave,
        0//WS_EX_TRANSPARENT
    },
    {
        MGD_BUTTON, //CTRL_STATIC,
        WS_CHILD | WS_VISIBLE,
        196, 6, 40, 21,
        IDC_CANCEL_BTN,
        "DELETE",
        (DWORD)&g_stBMCancel,
        0//WS_EX_TRANSPARENT
    },
};
#endif

static int ScreenSaverProc (HWND hWnd, int nMessage, WPARAM wParam, LPARAM lParam)
{

    switch (nMessage)
    {
      case MSG_CREATE:
      {
          CreateWindowEx (MGD_BUTTON, 
                          "SAVE",
                          WS_CHILD | WS_VISIBLE,
                          0,
                          IDC_SAVE_BTN,
                          196, 6, 40, 21,
                          hWnd, (DWORD)&g_stBMSave);
          
          CreateWindowEx (MGD_BUTTON, 
                          "CANCEL",
                          WS_CHILD | WS_VISIBLE,
                          0,
                          IDC_CANCEL_BTN,
                          4, 6, 42, 21,
                          hWnd, (DWORD)&g_stBMCancel);
      }
      break;
      case MSG_PAINT:
      {
          HDC hdc;
          char cWaitTime[32];

          memset (cWaitTime, 0x0, sizeof (cWaitTime));

          hdc = BeginPaint (hWnd);
          SetBkMode (hdc, BM_TRANSPARENT);
          SetTextColor (hdc, 0x528A);
          SelectFont (hdc, g_pLogFont);
          sprintf (cWaitTime, "%d min", g_nWaitTime);
          TabbedTextOut (hdc, 140, 65, cWaitTime);
          FillBoxWithBitmap (hdc, g_nPoint[g_nSetZone].x, g_nPoint[g_nSetZone].y, 9, 10, &g_stBMRotate);
          EndPaint (hWnd, hdc);
      }
      return 0;
      case MSG_LBUTTONUP:
      {
          int x = LOSWORD (lParam);
          int y = HISWORD (lParam);
          int i;
          int nCurr = g_nSetZone;

          if (x <= g_nPoint[g_nSetZone].x)
          {
             for (i = 0; i < g_nSetZone; i++) 
             {
                 if (PtInRect (&g_stRect[i], x, y))
                     g_nSetZone = i;
             }
          }
          else
          {
             for (i = g_nSetZone + 1; i < 7; i++)
             {
                 if (PtInRect (&g_stRect[i], x, y))
                     g_nSetZone = i;
             }
          }

          if (nCurr != g_nSetZone)
          {
              HDC hdc;
              RECT rcTime;
              rcTime.left = 140;
              rcTime.top = 65;
              rcTime.right = 200;
              rcTime.bottom = 85;
              
              InvalidateRect (hWnd, &g_stRect[nCurr], TRUE);
              InvalidateRect (hWnd, &g_stRect[g_nSetZone], FALSE);
              InvalidateRect (hWnd, &rcTime, TRUE);
              g_nWaitTime = 7 + g_nSetZone;
          }


      }
      return 0;
      case BUTTON_KEYDOWN:
        switch (wParam)
        {
        case IDC_SAVE_BTN:
        case IDC_CANCEL_BTN:
            SendMessage (g_hSysMain, MSG_CLOSE_APP, 0, 5);
            break;
        }
        break;
      break;
      
      case MSG_ERASEBKGND:
      {
          HDC hdc = (HDC)wParam;
          const RECT* clip = (const RECT*) lParam;
          BOOL fGetDC = FALSE;
          RECT rcTemp;

          if (hdc == 0){
              //hdc = GetClientDC (hWnd);
              hdc = GetSecondaryClientDC(hWnd);
              fGetDC = TRUE;
          }
          if (clip){
              rcTemp = *clip;
              ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
              ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
              IncludeClipRect (hdc, &rcTemp);
          }

          BitBlt (g_hdcBKG, 0, 0, 240, 320, hdc, 0, 0, 0);

          if (fGetDC) {
              //ReleaseDC (hdc);
              ReleaseSecondaryDC(hWnd, hdc);
          }
          return 0;

      }
      case MSG_CLOSE:
      case MSG_SYS_CLOSE:
          DeleteCompatibleDC (g_hdcBKG);
          DestroyAllControls ( hWnd );
          DestroyMainWindow ( hWnd );
      return 0;
      
      default:
      break;
    }
    return DefaultMainWinProc(hWnd, nMessage, wParam, lParam);
    //return DefaultDialogProc (hWnd, nMessage, wParam, lParam);
}

HWND CreateScreenSaver (void) 
{

    MAINWINCREATE CreateInfo;

    LoadBitmap (HDC_SCREEN, &g_stBMPBack, SCREEN_SAVER_RES"ScreenSaver_BKG.png");
    LoadBitmap (HDC_SCREEN, &g_stBMRotate, SCREEN_SAVER_RES"RedButton.png");
    LoadBitmap (HDC_SCREEN, &g_stBMSave, SYSTEM_RES"SaveButton.png");
    LoadBitmap (HDC_SCREEN, &g_stBMCancel, SYSTEM_RES"CancelButton.png");
    g_hdcBKG = CreateCompatibleDC (HDC_SCREEN);
    FillBoxWithBitmap (g_hdcBKG , 0, 0, 240, 320, &g_stBMPBack);

    CreateInfo.dwStyle = WS_NONE;
    CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC;
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = ScreenSaverProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 240;
    CreateInfo.by = 320;
    CreateInfo.iBkColor = PIXEL_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    return CreateMainWindow (&CreateInfo);
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif



