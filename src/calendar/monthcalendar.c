/*
** $Id: monthcalendar.c 224 2007-07-03 09:38:24Z xwyan $
**
** monthcalendar.c: A part of calendar module for Feynman PMP solution.
**
** Copyright (C) 2007 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: Zhang Hua .
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/mywindows.h>
#include <minigui/control.h>

//#include "common.h"

#include "monthcal.h"
#include "monthcalendar_impl.h"

static BITMAP bmp;
static BITMAP bmpNum;

static HDC memDC;

/*Cleanup the monthcalendar*/
static void mcMonthCalendarCleanUp(MONCALDDATA* mc_data)
{
    if (mc_data) 
        free( mc_data );
}

/*find out whether a year is a leap year*/
static BOOL IsLeapYear(int year)
{
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) return TRUE;
    else return FALSE;
}

/*get month length from year and month*/
static int GetMonLen(int year, int month)
{
    int mon_len;

    if (month < 1 || month > 12)
        return -1;
    if ( ( month <= 7 && month % 2 == 1 ) || ( month >= 8 && month % 2 == 0 ) )
        mon_len = 31;
    else if ( month == 2 ){
        if ( IsLeapYear ( year ) )
            mon_len = 29;
        else 
            mon_len = 28;
    } else
        mon_len = 30;
    return mon_len;
}

/*get previous month length from year and month*/
static int GetPMonLen(int year, int month)
{
    if ( month > 1 && month <= 12 )
        return GetMonLen ( year, month - 1 );
    else if ( month == 1 ) 
        return GetMonLen ( year - 1, 12 );
    else
        return -1;
}

/*get weekday from date*/
static int GetWeekDay(int year, int month, int day)
{
    struct tm nowday;

    if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31 || year > 2020)
        return -1;

    nowday.tm_sec = 0;
    nowday.tm_min = 0;
    nowday.tm_hour = 0;
    nowday.tm_mday = day;
    nowday.tm_mon = month-1;
    nowday.tm_year = year-1900;

    if ( __mg_mktime ( &nowday ) == -1 )
        return -1;
    else
        return nowday.tm_wday;    
}

/*get line and weekday from date according to weekday1*/
static void mcGetCurDayPos ( PMONCALDDATA mc_data, int* pline, int* pWeekDay )
{
    *pWeekDay = (mc_data->WeekDay1 + mc_data->cur_day - 1 ) % 7;
    *pline = ( mc_data->cur_day + 6 - *pWeekDay - 1 ) / 7;
}

/*get current item rect from mc_data*/
static void mcGetCurRect ( RECT* prcMDay, RECT* prcItem, PMONCALDDATA mc_data )
{
    prcItem->left = prcMDay->left + mc_data->cur_WeekDay*mc_data->item_w;    
    prcItem->top = prcMDay->top + mc_data->cur_line*mc_data->item_h;
    prcItem->right = prcItem->left + mc_data->item_w;
    prcItem->bottom = prcItem->top + mc_data->item_h;
}

/*textout in a rect center style*/
static void mcTextOutCenter ( HWND hWnd, HDC hdc, RECT* prcText, const char* pchText )
{

    if ( pchText ) {
        int bkMode;

        bkMode = SetBkMode ( hdc, BM_TRANSPARENT );
        SetBkColor ( hdc, GetWindowBkColor ( hWnd ) );
        DrawText ( hdc, pchText, -1, prcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP );
        SetBkMode ( hdc, bkMode );
    }
}

/*up the rect which is not highlight*/
static void mcUnHilightRect ( HWND hWnd, PMONCALDDATA mc_data, HDC hdc, RECT* prcItem, int day )
{
    int item_w, item_h;
    char daytext[3];

    item_w = prcItem->right - prcItem->left;
    item_h = prcItem->bottom - prcItem->top;
    SetBrushColor ( hdc, RGB2Pixel(HDC_SCREEN, 71, 156, 210));
    FillBox ( hdc, prcItem->left, prcItem->top, item_w, item_h );
    if ( day < 10 )
        sprintf ( daytext, " %d", day );
    else
        sprintf ( daytext, "%d", day );
    SetBkColor ( hdc, PIXEL_lightwhite );
    SetTextColor ( hdc, PIXEL_black );
    mcTextOutCenter ( hWnd, hdc, prcItem, daytext );
}

/*draw change day*/
static void mcDrawDay ( HWND hWnd, HDC hdc, PMONCALDDATA mc_data, RECT* prcMDay, 
        int newday )
{
    RECT rcPItemDay, rcItemDay;

    mcGetCurRect (prcMDay, &rcPItemDay, mc_data);
    //mcUnHilightRect (hWnd, mc_data, hdc, &rcPItemDay, mc_data->cur_day);    

    mc_data->cur_day = newday;
    mcGetCurDayPos ( mc_data, &mc_data->cur_line, &mc_data->cur_WeekDay ); 
    mcGetCurRect ( prcMDay, &rcItemDay, mc_data );
}

// draw month day area
static void mcDrawMonthDay ( HWND hWnd, HDC hdc, RECT* prcMDay, PMONCALDDATA mc_data )
{
    int i, WeekDayPM = 0, LineIndex = 0;
    int mdaypm = 0, MonLenPM, mdaynm;
    int iWeekDay = 0;
    char chMonthDay[3];
    RECT rcMonthDay;
    //SetBkColor ( hdc, PIXEL_lightwhite ); 
    SetTextColor ( hdc, PIXEL_black );
    for ( i = 1; i <= mc_data->monlen; i++ ) {
        if ( i < 10 ) 
            sprintf ( chMonthDay, " %d", i );
        else
            sprintf ( chMonthDay, "%d", i );
        iWeekDay = ( mc_data->WeekDay1 + i - 1 ) % 7;
        rcMonthDay.left = prcMDay->left + iWeekDay*mc_data->item_w;
        rcMonthDay.right = rcMonthDay.left + mc_data->item_w;
        LineIndex = ( mc_data->WeekDay1 + i - 1 ) / 7;
        rcMonthDay.top = prcMDay->top + mc_data->item_h*LineIndex;
        rcMonthDay.bottom = rcMonthDay.top + mc_data->item_h;
        if ( i == mc_data->cur_day ) {
            SetBrushColor(hdc, RGB2Pixel(hdc, 130, 176, 249));
            FillBox(hdc, rcMonthDay.left, rcMonthDay.top, DAY_W, DAY_H);
        }
        else if (i == mc_data->sys_day && mc_data->cur_month == mc_data->sys_month &&
               mc_data->cur_year == mc_data->sys_year ){
            SetPenColor ( hdc, PIXEL_red );
            Rectangle ( hdc, rcMonthDay.left, rcMonthDay.top,
                    rcMonthDay.left+mc_data->item_w-1 ,
                    rcMonthDay.top+mc_data->item_h-1 );
        }
        else {
            //SetBkColor ( hdc, PIXEL_lightwhite );
            SetTextColor ( hdc, PIXEL_black );
        }

        if ( mc_data->customed_day & ( 1 << i ) ) 
            SetTextColor ( hdc, mc_data->customed_day_color[i] );
        mcTextOutCenter ( hWnd, hdc, &rcMonthDay, chMonthDay );
    }

    SetTextColor ( hdc, PIXEL_lightwhite );

    LineIndex += ( iWeekDay + 1 ) / 7;
    iWeekDay = ( iWeekDay + 1 ) % 7;
    mdaynm = 1;
    while ( LineIndex <= 5 ) {
        if ( mdaynm < 10 ) 
            sprintf ( chMonthDay, " %d", mdaynm );
        else
            sprintf ( chMonthDay, "%d", mdaynm );
        rcMonthDay.left = prcMDay->left + iWeekDay*mc_data->item_w;
        rcMonthDay.right = rcMonthDay.left + mc_data->item_w;
        rcMonthDay.top = prcMDay->top + mc_data->item_h*LineIndex;
        rcMonthDay.bottom = rcMonthDay.top + mc_data->item_h;
        mcTextOutCenter ( hWnd, hdc, &rcMonthDay, chMonthDay );
        mdaynm++;
        iWeekDay++;
        if ( iWeekDay == 7 ) {
            iWeekDay = 0;
            LineIndex++;
        }
    }

    WeekDayPM = mc_data->WeekDay1 - 1;
    if ( WeekDayPM >= 0 ) {
        rcMonthDay.top = prcMDay->top;
        rcMonthDay.bottom = rcMonthDay.top + mc_data->item_h;
        rcMonthDay.left = prcMDay->left + WeekDayPM*mc_data->item_w;
        rcMonthDay.right = rcMonthDay.left + mc_data->item_w;
        MonLenPM = GetPMonLen ( mc_data->cur_year, mc_data->cur_month );
        mdaypm = MonLenPM;
    }
    while ( WeekDayPM >= 0 ) {
        sprintf ( chMonthDay, "%d", mdaypm );
        mcTextOutCenter ( hWnd, hdc, &rcMonthDay, chMonthDay );
        OffsetRect( &rcMonthDay, -mc_data->item_w, 0 );
        mdaypm--;
        WeekDayPM--;
    }
}

static void mcDrawYear( HWND hWnd, HDC hdc, PMONCALDDATA mc_data )
{
    int y1 = mc_data->cur_year/1000;
    int y2 = (mc_data->cur_year-y1*1000)/100;
    int y3 = (mc_data->cur_year-y1*1000- y2*100)/10;
    int y4 = mc_data->cur_year%10;
    int bmpWidth = 12;
    int bmpHeight = 16;
    int bmpDis = 4 + bmpWidth;
    int bmpBeginX = 32;
    int bmpBeginY = 192;

    FillBoxWithBitmapPart(hdc, mc_data->yRect.left, mc_data->yRect.top, 
            bmpWidth, bmpHeight, 0, 0, &bmpNum, bmpBeginX+y1*bmpDis, bmpBeginY);
    FillBoxWithBitmapPart(hdc, mc_data->yRect.left+bmpWidth, mc_data->yRect.top, 
            bmpWidth, bmpHeight, 0, 0, &bmpNum, bmpBeginX+y2*bmpDis, bmpBeginY);
    FillBoxWithBitmapPart(hdc, mc_data->yRect.left+bmpWidth*2, mc_data->yRect.top, 
            bmpWidth, bmpHeight, 0, 0, &bmpNum, bmpBeginX+y3*bmpDis, bmpBeginY);
    FillBoxWithBitmapPart(hdc, mc_data->yRect.left+bmpWidth*3, mc_data->yRect.top, 
            bmpWidth, bmpHeight, 0, 0, &bmpNum, bmpBeginX+y4*bmpDis, bmpBeginY);
}
 
static void mcDrawMonth( HWND hWnd, HDC hdc, PMONCALDDATA mc_data )
{
    int m = mc_data->cur_month-1;
    int index = m%6;
    int bx[6]={32, 68, 104, 140, 180, 216};
    int by;
    if (m < 6)
        by = 128;
    else
        by = 160;
    FillBoxWithBitmapPart(hdc, mc_data->mRect.left, mc_data->mRect.top, 
            36, 20, 0, 0, &bmpNum, bx[index], by);
}

/*draw the whole calendar*/
static void mcDrawCalendar ( HWND hWnd, HDC hdc, PMONCALDDATA mc_data )
{
    mcDrawMonth( hWnd, hdc, mc_data );
    mcDrawYear( hWnd, hdc, mc_data );
    /*draw month day text*/
    mcDrawMonthDay ( hWnd, hdc, &mc_data->dRect, mc_data );
}

/*initialize mc_data*/
static void mcInitMonthCalendarData ( HWND hWnd, MONCALDDATA* mc_data )
{
    time_t nowtime;
    struct tm *pnt;

    __mg_time( &nowtime );
    pnt = (struct tm * )__mg_localtime ( &nowtime );
    mc_data->sys_year = mc_data->cur_year = pnt->tm_year + 1900;
    mc_data->sys_month = mc_data->cur_month = pnt->tm_mon + 1;
    mc_data->sys_day = mc_data->cur_day = pnt->tm_mday;

    mc_data->item_w = (DAY_R - DAY_L)/7;
    mc_data->item_h = (DAY_B - DAY_T)/6;

    mc_data->WeekDay1 = GetWeekDay(mc_data->cur_year, mc_data->cur_month, 1);

    mcGetCurDayPos (mc_data, &mc_data->cur_line, &mc_data->cur_WeekDay);
    
    mc_data->sys_WeekDay = mc_data->cur_WeekDay;    
    mc_data->monlen = GetMonLen ( mc_data->cur_year, mc_data->cur_month );

    mc_data->state = 0;
    mc_data->customed_day = 0;

    SetRect(&mc_data->dRect, DAY_L, DAY_T, DAY_R, DAY_B);
    SetRect(&mc_data->mRect, MONTH_L, MONTH_T, MONTH_R, MONTH_B);
    SetRect(&mc_data->yRect, YEAR_L, YEAR_T, YEAR_R, YEAR_B);
    SetRect(&mc_data->lRect, ARROWL_L, ARROWL_T, ARROWL_R, ARROWL_B);
    SetRect(&mc_data->rRect, ARROWR_L, ARROWR_T, ARROWR_R, ARROWR_B);
}

/*Set the curday*/
static void mcSetCurDay ( HWND hWnd, PMONCALDDATA mc_data, int newday )
{
    HDC hdc;
    hdc = GetClientDC ( hWnd );
    mcDrawDay ( hWnd, hdc, mc_data, &mc_data->dRect, newday );
    ReleaseDC ( hdc );
}

/*Shift the year and the month*/
static void mcShiftYM ( HWND hWnd, PMONCALDDATA mc_data, BOOL bMonth, BOOL bRight )
{

    if ( bMonth && bRight ) {
        if ( mc_data->cur_month != 12 ) {
            mc_data->cur_month++;
            InvalidateRect ( hWnd, &mc_data->mRect, TRUE );
            InvalidateRect ( hWnd, &mc_data->dRect, TRUE );
        } else {
            if ( mc_data->cur_year >= 2020 )
                return;
            mc_data->cur_month = 1;
            mc_data->cur_year++;
            InvalidateRect ( hWnd, &mc_data->yRect, TRUE );
            InvalidateRect ( hWnd, &mc_data->mRect, TRUE );
            InvalidateRect ( hWnd, &mc_data->dRect, TRUE );
        }
    } else if ( bMonth ) {
        if ( mc_data->cur_month != 1 ) {
            mc_data->cur_month--;
            InvalidateRect ( hWnd, &mc_data->mRect, TRUE );
            InvalidateRect ( hWnd, &mc_data->dRect, TRUE );
        } else {
            if ( mc_data->cur_year <= 1970 )
                return;
            mc_data->cur_month = 12;
            mc_data->cur_year--;
            InvalidateRect ( hWnd, &mc_data->yRect, TRUE );
            InvalidateRect ( hWnd, &mc_data->mRect, TRUE );
            InvalidateRect ( hWnd, &mc_data->dRect, TRUE );
        }
    } else if (bRight) {
        if ( mc_data->cur_year >= 2020 )
            return;
        mc_data->cur_year++;
        InvalidateRect ( hWnd, &mc_data->yRect, TRUE );
        InvalidateRect ( hWnd, &mc_data->dRect, TRUE );
    } else {
        if ( mc_data->cur_year <= 1970 )
            return;
        mc_data->cur_year--;
        InvalidateRect ( hWnd, &mc_data->yRect, TRUE );
        InvalidateRect ( hWnd, &mc_data->dRect, TRUE );
    }

    mc_data->monlen = GetMonLen ( mc_data->cur_year, mc_data->cur_month );
    while ( mc_data->cur_day > mc_data->monlen ) 
        mc_data->cur_day--;

    mc_data->WeekDay1 = GetWeekDay ( mc_data->cur_year, mc_data->cur_month, 1 );
    mcGetCurDayPos ( mc_data, &mc_data->cur_line, &mc_data->cur_WeekDay );    
}

/*When left button up what  will be done*/
static void mcMouseOperations ( HWND hWnd, PMONCALDDATA mc_data, LPARAM lParam )
{
    int day = -1;
    int x = LOSWORD (lParam);
    int y = HISWORD (lParam);

    RECT rcPItemDay, rcItemDay;
    PMONCALDDATA pmcci;

    if (x > mc_data->dRect.left && x < mc_data->dRect.right && y > mc_data->dRect.top && y < mc_data->dRect.bottom) {
        int offWeek = ((x-mc_data->dRect.left)/mc_data->item_w)>6? 6:(x-mc_data->dRect.left)/mc_data->item_w;
        int offLine = ((y-mc_data->dRect.top)/mc_data->item_h)>5? 5:(y-mc_data->dRect.top)/mc_data->item_h;
        int offsetDay = (offLine - mc_data->cur_line)*7 +( offWeek - mc_data->cur_WeekDay);

        day = mc_data->cur_day + offsetDay;
    } else if (x > mc_data->lRect.left && x < mc_data->lRect.right && y > mc_data->lRect.top && y < mc_data->lRect.bottom) {
        mcShiftYM ( hWnd, mc_data, TRUE, FALSE );
        return;
    } else if (x > mc_data->rRect.left && x < mc_data->rRect.right && y > mc_data->rRect.top && y < mc_data->rRect.bottom) {
        mcShiftYM ( hWnd, mc_data, TRUE, TRUE );
        return;
    } else
        return;

    if ( day > mc_data->monlen ) {
        mc_data->cur_day = day - mc_data->monlen;
        mcShiftYM ( hWnd, mc_data, TRUE, TRUE );
    }
    else if ( day < 1 ) {
        mcShiftYM ( hWnd, mc_data, TRUE, FALSE );
        mcSetCurDay ( hWnd, mc_data, mc_data->monlen + day );
    }
    else {
        mcGetCurRect ( &mc_data->dRect, &rcPItemDay, mc_data );

        pmcci = ( PMONCALDDATA )mc_data;
        pmcci->cur_day = day;
        mcGetCurDayPos ( pmcci, &pmcci->cur_line, &pmcci->cur_WeekDay );    
        mcGetCurRect ( &mc_data->dRect, &rcItemDay, pmcci );
        mcSetCurDay ( hWnd, mc_data, day );
        InvalidateRect ( hWnd, &rcPItemDay, TRUE );
        InvalidateRect ( hWnd, &rcItemDay, TRUE );
    }
}

static void init_month_calendar_info(HWND hWnd, PMONCALDDATA data )
{
    data = (MONCALDDATA *)malloc(sizeof(MONCALDDATA));
    if (data) {
        SetWindowAdditionalData2(hWnd, (DWORD)data);
        mcInitMonthCalendarData ( hWnd, data );
    }
}

/*The proc of the monthcalendar window*/

static int MonthCalendarCtrlProc ( HWND hWnd, int message, WPARAM wParam, LPARAM lParam )
{
    HDC hdc;
    PMONCALDDATA mc_data = NULL;

    if (message != MSG_CREATE)
        mc_data = (PMONCALDDATA)GetWindowAdditionalData2(hWnd);

    switch ( message ) {
        case MSG_CREATE:
#ifdef MEMDC_32
            memDC = CreateMemDC (IPHONE_MAIN_WIDTH, CAL_MAIN_HEIGHT, 32,
                    MEMDC_FLAG_HWSURFACE | MEMDC_FLAG_SRCALPHA,
                    0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#else
            hdc = GetClientDC(hWnd);
            memDC = CreateCompatibleDCEx (hdc, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT);
            ReleaseDC(hdc);
#endif
            if (LoadBitmap (HDC_SCREEN, &bmp, CALE_BMP_BG))
                return -1;
            if (LoadBitmap (HDC_SCREEN, &bmpNum, CALE_BMP_BN))
                return -1;

            init_month_calendar_info(hWnd, mc_data);
            UpdateWindow(hWnd, TRUE);
            return 0;

        case MCM_GETCURDATE:
            {
                PSYSTEMTIME pcurdate = NULL;

                pcurdate = ( PSYSTEMTIME ) lParam;
                pcurdate->year = mc_data->cur_year;
                pcurdate->month = mc_data->cur_month;
                pcurdate->day = mc_data->cur_day;
                pcurdate->weekday = mc_data->cur_WeekDay;
                return 0;
            }

        case MSG_LBUTTONDOWN:
            mcMouseOperations ( hWnd, mc_data, lParam );
            break;
        case MSG_PAINT:
            {
                hdc = GetClientDC(hWnd);
                mcDrawCalendar ( hWnd, memDC, mc_data );
                BitBlt (memDC, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, hdc, 0, 0, 0);
                ReleaseDC(hdc);
            }
            break;
        case MSG_ERASEBKGND:
            FillBoxWithBitmap(memDC, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, &bmp);
            return 0;
        case MSG_CLOSE:
            DeleteMemDC (memDC);
            UnloadBitmap (&bmp);
            UnloadBitmap (&bmpNum);
            mcMonthCalendarCleanUp(mc_data );
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
        default:
            break;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void _calendar_init_create_info ( PMAINWINCREATE pCreateInfo )
{
    pCreateInfo->dwStyle = WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "Calendar";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor ( 0 );
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = MonthCalendarCtrlProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = IPHONE_MAIN_WIDTH;
    pCreateInfo->by = IPHONE_MAIN_HEIGHT;
    pCreateInfo->iBkColor = COLOR_lightgray;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "Calendar" , 0 , 0);
#endif

    _calendar_init_create_info ( &CreateInfo );

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
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

