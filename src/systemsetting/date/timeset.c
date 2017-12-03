/*
 ** $ld: datetime.c 455 2008-07-09
 **
 **
 ** 
 ** datetime.c: Sample program for MiniGUI Programming Guide
 *
 ** Copyright (C) 2004 ~ 2009 Feyman Software
 *
 ** License: GPL
 */
/***************Date and Time***************/

#include "timeset.h"

/************** define idc ***************/
#define IDC_BTN_DT_OK           0x100
#define IDC_BTN_DT_CANCEL       0x101

#define IDC_STATIC_DT_TIMEZONE  0x201
#define IDC_STATIC_DT_TIME      0x202
#define IDC_STATIC_DT_CALENDAR  0x203

#define IDC_BUTTON_DT_TIMEZONE  0x204
#define IDC_BUTTON_DT_TIMEUP    0x205
#define IDC_BUTTON_DT_TIMEDOWN  0x206

#define IDC_EDIT_DT_HOUR        0x303
#define IDC_EDIT_DT_MIN         0x304
#define IDC_EDIT_DT_NULL        0x305

#define IDC_COMBOBOX_DT_TIME    0x407
#define IDC_SPINBOX_DT_TIME     0x408
#define IDC_CALENDAR_DT_DATE    0x409  

#define IDC_DT_TIMER            0x601

#define TYPE_HOUR           0x501
#define TYPE_MINUTE         0x502
#define TYPE_SECOND         0x503

#define DT_KEY_DOWN         0x602
#define DT_KEY_UP           0x603
#define DT_KEY_NONE         0x604

typedef struct _DateTime{
  char timezone[20];    //time zone;
  int minute;           //char minute[2];
  int hour;             //char hour[2];
  SYSTEMTIME date;      //date;
}DateTime;

static DateTime g_datetime;

//static PLOGFONT timefont,zone_font,dt_font;

static WNDPROC old_edit_proc, old_btn_proc;
static char editbuff[30];
static BITMAP dt_bg_bmp,dt_btnok_bmp,dt_btncancel_bmp;
static BITMAP time_up_bmp,time_down_bmp;
static int g_time_edit_type;
static HDC g_hdcBKG;
static int g_nHour;
static int g_nMin;
static BOOL g_bTimer;

typedef enum _EM_TIME_TYPE
{
    EM_TYPE_HOUR = 0,
    EM_TYPE_MIN,
    EM_TYPE_NULL
}EM_TIME_TYPE;

typedef enum _EM_TIME_STAUS
{
    EM_STAUS_NORMAL = 0,
    EM_STAUS_SELECT,
    EM_STAUS_ADD,
    EM_STAUS_DEL
}EM_TIME_STATUS;

static void InitDatetimeBitmap(void)
{
    LoadBitmap(HDC_SCREEN, &dt_btnok_bmp, SYSTEM_RES"SaveButton.png");
    LoadBitmap(HDC_SCREEN, &dt_btncancel_bmp, SYSTEM_RES"CancelButton.png");
    LoadBitmap(HDC_SCREEN, &time_up_bmp, TIME_RES"DateUpArrow.png");
    LoadBitmap(HDC_SCREEN, &time_down_bmp, TIME_RES"DateDownArrow.png");
}

static void ChangeTheHourAndMin (HWND hWnd, EM_TIME_TYPE emTimeType, EM_TIME_STATUS emTimeStatus)
{
    HDC hdc;


    switch (emTimeType)
    {
        case EM_TYPE_HOUR:
            {
                RECT rcHour = {106, 97, 126, 117};
                char cHour[3]; 

                //InvalidateRect (hWnd, &rcHour, TRUE);
                hdc = GetClientDC (hWnd);
                BitBlt (g_hdcBKG, rcHour.left, rcHour.top, 20, 20, hdc, rcHour.left, rcHour.top, 0);
                SetBkMode (hdc, BM_TRANSPARENT);
                switch (emTimeStatus)
                {
                    case EM_STAUS_NORMAL:
                        {
                            sprintf (cHour, "%02d", g_nHour);
                            SetTextColor (hdc, PIXEL_black);
                            TabbedTextOut (hdc, 106, 97, cHour);
                        }
                        break;
                    case EM_STAUS_SELECT:
                        {
                            sprintf (cHour, "%02d", g_nHour);
                            //SetTextColor (hdc, PIXEL_blue);
                            SetTextColor (hdc, RGB2Pixel (hdc, 42, 118, 204));
                            TabbedTextOut (hdc, 106, 97, cHour);
                        }
                        break;
                    case EM_STAUS_ADD:
                        {
                            if (g_nHour == 23)
                            {
                                g_nHour = 0;
                            }
                            else
                            {
                                g_nHour ++;
                            }
                            sprintf (cHour, "%02d", g_nHour);
                            //SetTextColor (hdc, PIXEL_blue);
                            SetTextColor (hdc, RGB2Pixel (hdc, 42, 118, 204));
                            TabbedTextOut (hdc, 106, 97, cHour);
                        }
                        break;
                    case EM_STAUS_DEL:
                        {
                            if (g_nHour == 0)
                            {
                                g_nHour = 23;
                            }
                            else
                            {
                                g_nHour --;
                            }
                            sprintf (cHour, "%02d", g_nHour);
                            //SetTextColor (hdc, PIXEL_blue);
                            SetTextColor (hdc, RGB2Pixel (hdc, 42, 118, 204));
                            TabbedTextOut (hdc, 106, 97, cHour);
                        }
                        break;
                }
                ReleaseDC (hdc);
            }
            break;
        case EM_TYPE_MIN:
            {
                RECT rcMin = {142, 97, 162, 117};
                char cMin[3]; 

                hdc = GetClientDC (hWnd);
                BitBlt (g_hdcBKG, rcMin.left, rcMin.top, 20, 20, hdc, rcMin.left, rcMin.top, 0);
                SetBkMode (hdc, BM_TRANSPARENT);
                switch (emTimeStatus)
                {
                    case EM_STAUS_NORMAL:
                        {
                            sprintf (cMin, "%02d", g_nMin);
                            SetTextColor (hdc, PIXEL_black);
                            TabbedTextOut (hdc, 142, 97, cMin);
                        }
                        break;
                    case EM_STAUS_SELECT:
                        {
                            sprintf (cMin, "%02d", g_nMin);
                            //SetTextColor (hdc, PIXEL_blue);
                            SetTextColor (hdc, RGB2Pixel (hdc, 42, 118, 204));
                            TabbedTextOut (hdc, 142, 97, cMin);
                        }
                        break;
                    case EM_STAUS_ADD:
                        {
                            if (g_nMin == 59)
                            {
                                g_nMin = 0;
                            }
                            else
                            {
                                g_nMin ++;
                            }
                            sprintf (cMin, "%02d", g_nMin);
                            //SetTextColor (hdc, PIXEL_blue);
                            SetTextColor (hdc, RGB2Pixel (hdc, 42, 118, 204));
                            TabbedTextOut (hdc, 142, 97, cMin);
                        }
                        break;
                    case EM_STAUS_DEL:
                        {
                            if (g_nMin == 0)
                            {
                                g_nMin = 59;
                            }
                            else
                            {
                                g_nMin --;
                            }
                            sprintf (cMin, "%02d", g_nMin);
                            //SetTextColor (hdc, PIXEL_blue);
                            SetTextColor (hdc, RGB2Pixel (hdc, 42, 118, 204));
                            TabbedTextOut (hdc, 142, 97, cMin);
                        }
                        break;
                }
                ReleaseDC (hdc);
            }
            break;
    }
}


static void Int2Str(int num, char *str)
{
    char i,j,tmpnum,tmpstr[10];
    char string[10];
    

    i = 0;
   tmpnum = num;
    while(tmpnum != 0)
    {
        tmpstr[i++] = tmpnum % 10;    
        tmpnum = tmpnum / 10;
    }

    j=0;
    while (i-- > 0)
    {
        string[j++] = '0' + tmpstr[i];
    }
    string[j] = '\n';
    memcpy (str, string, strlen(string));
}
static int SetDateTime(void)
{
    struct tm nowday;
    int year, month, day;


    year = g_datetime.date.year;
    month = g_datetime.date.month;
    day = g_datetime.date.day;

    if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31 || year > 2020)
        return -1;
   
    g_datetime.hour = g_nHour;
    g_datetime.minute = g_nMin;

    nowday.tm_sec = 0;
    nowday.tm_min = g_datetime.minute;
    nowday.tm_hour = g_datetime.hour;
    nowday.tm_mday = g_datetime.date.day;
    nowday.tm_mon = g_datetime.date.month;
    nowday.tm_year = g_datetime.date.year;
    
    if ( __mg_mktime ( &nowday ) == -1 )
        return -1;
    else
        return nowday.tm_wday;
}
static void InitDatetime(void)
{
    time_t nowtime;
    struct tm *pnt;
    SYSTEMTIME date;

   // char buff[10] = {0};
    __mg_time (&nowtime);
    pnt = (struct tm *)__mg_localtime (&nowtime);
    g_datetime.hour = pnt->tm_hour;
    g_datetime.minute = pnt->tm_min;
  

}
static int DatetimeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HWND hwnd_time,houreditwnd, mineditwnd, hwnd_moncalendar;
    SIZE size;
    SYSTEMTIME date;
    static int key_state = DT_KEY_NONE;
     
    switch(message){
    case MSG_CREATE:
    {   
        g_nHour = g_datetime.hour;
        g_nMin = g_datetime.minute;

        CreateWindow ("MGD_BUTTON", "",
                WS_CHILD | WS_VISIBLE,
                IDC_BTN_DT_OK,
                196, 6, 40, 21,
                hWnd, 
                (int)(&dt_btnok_bmp));
        CreateWindow ("MGD_BUTTON","",
                WS_CHILD | WS_VISIBLE,
                IDC_BTN_DT_CANCEL,
                4, 6, 42, 21,
                hWnd, 
                (int)(&dt_btncancel_bmp));
        CreateWindow("MGD_BUTTON", "",
                    WS_CHILD | WS_VISIBLE,
                    IDC_BUTTON_DT_TIMEUP,
                    213, 92, 12, 9,                  
                    hWnd, (int)(&time_up_bmp));
        CreateWindow("MGD_BUTTON", "",
                    WS_CHILD | WS_VISIBLE,
                    IDC_BUTTON_DT_TIMEDOWN,
                    213, 109, 12, 9,                  
                    hWnd, (int)(&time_down_bmp));
        g_time_edit_type = IDC_EDIT_DT_NULL;
        hwnd_moncalendar = CreateWindow("moncalendar", "", 
                      WS_CHILD | WS_VISIBLE | MCS_NOTIFY | MCS_ENG_L,
                      IDC_CALENDAR_DT_DATE,
                      3,154,234,164,
                      hWnd, (LPARAM)&date);
        SendMessage(hwnd_moncalendar, MCM_SETCURDATE, 0, (LPARAM)(&g_datetime.date));

       return 0;
    }
    case MSG_PAINT:
        {
            HDC hdc;
            LOGFONT logFont;
            char hour[3],min[3];

            sprintf (hour, "%02d", g_datetime.hour);
            sprintf (min, "%02d", g_datetime.minute);
            
            hdc = BeginPaint(hWnd);     
            SetBkMode (hdc,BM_TRANSPARENT);
            TabbedTextOut (hdc, 106, 97, hour);
            TextOut (hdc, 130, 96, ":");
            TabbedTextOut (hdc, 142, 97, min);
            EndPaint (hWnd, hdc);
            return 0;
        }
   case MSG_TIMER:
        if(DT_KEY_UP == key_state)
        {
            if (g_time_edit_type == IDC_EDIT_DT_HOUR)
            {
                ChangeTheHourAndMin (hWnd, EM_TYPE_HOUR, EM_STAUS_ADD);
            }
            else if (g_time_edit_type == IDC_EDIT_DT_MIN)
            {
                ChangeTheHourAndMin (hWnd, EM_TYPE_MIN, EM_STAUS_ADD);
            }
        }
        else if (DT_KEY_DOWN == key_state)
        {
            if (g_time_edit_type == IDC_EDIT_DT_HOUR)
            {
                ChangeTheHourAndMin (hWnd, EM_TYPE_HOUR, EM_STAUS_DEL);
            }
            else if (g_time_edit_type == IDC_EDIT_DT_MIN)
            {
                ChangeTheHourAndMin (hWnd, EM_TYPE_MIN, EM_STAUS_DEL);
            }
        }
        break;
    case MSG_LBUTTONDOWN:
        {
            RECT rcHour = {106 - 10, 97 - 5, 126, 117 + 5};
            RECT rcMin = {142, 97 - 5, 162 + 10, 117 + 5};
            RECT rcUp = {213, 92, 225, 101};
            RECT rcDown = {213, 109, 225, 118};
            int x = LOSWORD (lParam);
            int y = HISWORD (lParam);
            char cTime[3];
          

            if (PtInRect (&rcHour, x, y))
            {
                if (g_time_edit_type == IDC_EDIT_DT_MIN)
                    ChangeTheHourAndMin (hWnd, EM_TYPE_MIN, EM_STAUS_NORMAL);
                g_time_edit_type = IDC_EDIT_DT_HOUR;
                ChangeTheHourAndMin (hWnd, EM_TYPE_HOUR, EM_STAUS_SELECT);
                return 0;

            }
            else if (PtInRect (&rcMin, x, y)) 
            {
                if (g_time_edit_type == IDC_EDIT_DT_HOUR)
                    ChangeTheHourAndMin (hWnd, EM_TYPE_HOUR, EM_STAUS_NORMAL);

                g_time_edit_type = IDC_EDIT_DT_MIN;
                ChangeTheHourAndMin (hWnd, EM_TYPE_MIN, EM_STAUS_SELECT);
                return 0;
            }
            else
            {
                if (PtInRect (&rcUp, x, y) || PtInRect (&rcDown, x, y))
                {
                    break;
                }
                else
                {
                    if (g_time_edit_type == IDC_EDIT_DT_MIN)
                        ChangeTheHourAndMin (hWnd, EM_TYPE_MIN, EM_STAUS_NORMAL);
                    if (g_time_edit_type == IDC_EDIT_DT_HOUR)
                        ChangeTheHourAndMin (hWnd, EM_TYPE_HOUR, EM_STAUS_NORMAL);
                    g_time_edit_type = IDC_EDIT_DT_NULL; 
                }
            }
        }
        break;
    case MSG_COMMAND:
    case BUTTON_KEYDOWN:
      {
          int id_btn;
        
          id_btn = LOWORD(wParam);
          switch(id_btn)
          {        
             case IDC_BUTTON_DT_TIMEUP:
              {    
                  key_state = DT_KEY_UP;
                  g_bTimer = TRUE;
                  SetTimer (hWnd, IDC_DT_TIMER, 15);
                  break;
              }
              case IDC_BUTTON_DT_TIMEDOWN:
              {
                  key_state = DT_KEY_DOWN;
                  g_bTimer = TRUE;
                  SetTimer (hWnd, IDC_DT_TIMER, 15);
                  break;                  
              }
              case IDC_BTN_DT_OK:
              {
                  SYSTEMTIME curdate;

                  g_datetime.hour = g_nHour;
                  g_datetime.minute = g_nMin;
                  SendMessage(GetDlgItem(hWnd, IDC_CALENDAR_DT_DATE), MCM_GETCURDATE,0,(LPARAM)&curdate);
                  memcpy(&g_datetime.date, &curdate, sizeof(SYSTEMTIME));
                  SetDateTime();
                  SendMessage(hWnd, MSG_CLOSE, 0, 0);
                  break;
              }
              case IDC_BTN_DT_CANCEL:
              g_nHour = g_datetime.hour;
              g_nMin = g_datetime.minute;
              SendMessage(hWnd, MSG_CLOSE, 0, 0);
              break;
          }
          return 0; 
      }
    case MSG_LBUTTONUP:
    case BUTTON_KEYUP:
      if (g_bTimer)
      {
          KillTimer(hWnd, IDC_DT_TIMER);
          g_bTimer = FALSE;
      }
      SendMessage (hWnd, MSG_TIMER, 0, 0);
      key_state = DT_KEY_NONE;

      break;
     case MSG_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            const RECT* clip = (const RECT*) lParam;
            BOOL fGetDC = FALSE;
            RECT rcTemp;

            if (hdc == 0){
                //hdc = BeginPaint (hWnd);//GetClientDC (hWnd);
                hdc = GetSecondaryClientDC(hWnd);
                fGetDC = TRUE;
            }
            if (clip){
                rcTemp = *clip;
                ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
                ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
                IncludeClipRect (hdc, &rcTemp);
            }
            BitBlt (g_hdcBKG, 0, 0, 0, 0, hdc, 0, 0, 0 );
            if (fGetDC){
                //EndPaint (hWnd, hdc);//ReleaseDC (hdc);
                ReleaseSecondaryDC(hWnd, hdc);
            }
            return 0;
        }
     case MSG_CLOSE:
      SendMessage (g_hSysMain, MSG_CLOSE_APP, 0, 4);
      return 0;
     case MSG_SYS_CLOSE:
        DeleteCompatibleDC (g_hdcBKG); 
        UnloadBitmap(&dt_bg_bmp);
        UnloadBitmap(&dt_btncancel_bmp);
        UnloadBitmap(&time_up_bmp);
        UnloadBitmap(&time_down_bmp);
        UnloadBitmap(&dt_btnok_bmp);
        DestroyMainWindow (hWnd);
        return 0;
     }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}


static void InitDatetimeCreateInfo(PMAINWINCREATE CreateInfo)
{
    CreateInfo->dwStyle = WS_NONE;
    CreateInfo->dwExStyle = WS_EX_AUTOSECONDARYDC;//WS_EX_NONE;
    CreateInfo->spCaption = "";
    CreateInfo->hMenu = 0;
    CreateInfo->hCursor = GetSystemCursor(IDC_ARROW);
    CreateInfo->hIcon = 0;
    CreateInfo->MainWindowProc = DatetimeWinProc;
    CreateInfo->lx = 0;
    CreateInfo->ty = 0;

    CreateInfo->rx = 240;
    CreateInfo->by = 320;
    CreateInfo->iBkColor = PIXEL_lightwhite;
    CreateInfo->dwAddData = 0;
    CreateInfo->hHosting =HWND_DESKTOP;

}


HWND CreateDateWindow (void)
{
    MAINWINCREATE CreateInfo;
    InitDatetimeCreateInfo(&CreateInfo);
    InitDatetimeBitmap();       
    LoadBitmap(HDC_SCREEN, &dt_bg_bmp, TIME_RES"DateBKG.png");
    g_hdcBKG = CreateCompatibleDC (HDC_SCREEN);
    FillBoxWithBitmap (g_hdcBKG , 0, 0, 240, 320, &dt_bg_bmp);
    InitDatetime();
    return CreateMainWindow (&CreateInfo);
}
