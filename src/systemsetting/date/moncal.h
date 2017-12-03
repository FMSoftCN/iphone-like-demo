/*
** $Id: monthcal.h 224 2007-07-03 09:38:24Z xwyan $
**
**monthcal.h: head file of monthcalendar.
**
** Copyright (C) 2007 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: Zhang Hua.
*/


#ifndef EXT_MONCAL_H
#define EXT_MONCAL_H

//#include "config.h"

#define CTRL_MONCALENDAR     ("moncalendar")

/** Structure of the system time */
#if 0
typedef struct _SYSTEMTIME
{
    /** Year number of the date */
    int year;
    /** Month number of the date */
    int month;
    /** Day number of the date */
    int day;
    /** Weekday number of the date */
    int weekday;
} SYSTEMTIME;

typedef SYSTEMTIME *PSYSTEMTIME;

#define MCM_GETCURDATE          0xF310

#endif
#define IPHONE_MAIN_HEIGHT      164
#define IPHONE_MAIN_WIDTH       234
#define CALE_BMP_BN TIME_RES"DateCalFont.png"

/*#ifdef  ENABLE_LANDSCAPE
#define CALE_BMP_BG "res/calendar_bg.png"
//Something about the rect of arrow
#define ARROWL_L        19             
#define ARROWL_T        7      
#define ARROWL_R        31      
#define ARROWL_B        19      

#define ARROWR_L        288      
#define ARROWR_T        7      
#define ARROWR_R        300      
#define ARROWR_B        19      

#define DAY_L       0
#define DAY_T       34
#define DAY_R       322
#define DAY_B       240

#define DAY_W       46
#define DAY_H       33

#define MONTH_L     100
#define MONTH_T     5
#define MONTH_R     160
#define MONTH_B     30

#define YEAR_L      160
#define YEAR_T      5
#define YEAR_R      240
#define YEAR_B      30

#else*/
/*The following is for portrait.*/

#include "../sysmain.h"
#define CALE_BMP_BG TIME_RES"DateCalBKG.png"

#define ARROWL_L        15             
#define ARROWL_T        2      
#define ARROWL_R        30      
#define ARROWL_B        20      

#define ARROWR_L        205//217      
#define ARROWR_T        2      
#define ARROWR_R        222//227      
#define ARROWR_B        20 //251      

#define DAY_L           1//5
#define DAY_T           42//48,height of bar
#define DAY_R           234//220 //240
#define DAY_B           164 //320

#define DAY_W           33//33
#define DAY_H           20//

#define MONTH_L         81
#define MONTH_T         5
#define MONTH_R         111//120
#define MONTH_B         35//16//38

#define YEAR_L          120
#define YEAR_T          13//13
#define YEAR_R          147//180
#define YEAR_B          22//16//38

int MonCalendarWindow(HWND hOwner);
BOOL RegisterMonCalendarControl ();
void MonCalendarControlCleanup ();
void UnregisterMonCalendarContorl ();

#endif /* EXT_MONTHCAL_H */

