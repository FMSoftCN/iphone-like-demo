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


#ifndef EXT_MONTHCAL_H
#define EXT_MONTHCAL_H

//#ifdef  __cplusplus
//extern "C" {
//#endif

#include "config.h"

#define CTRL_MONTHCALENDAR     ("MonthCalendar")

/** Structure of the system time */
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

#define CALE_BMP_BN "res/calendar_num.png"

#ifdef  ENABLE_LANDSCAPE
#define CALE_BMP_BG "res/calendar_bg.png"
/*Something about the rect of arrow*/
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

#else
/*The following is for portrait.*/
#define CALE_BMP_BG "res/calendar_bg_pt.png"

#define ARROWL_L        15             
#define ARROWL_T        9      
#define ARROWL_R        24      
#define ARROWL_B        26      

#define ARROWR_L        217      
#define ARROWR_T        10      
#define ARROWR_R        227      
#define ARROWR_B        251      

#define DAY_L       1
#define DAY_T       48
#define DAY_R       240
#define DAY_B       320

#define DAY_W       35
#define DAY_H       45

#define MONTH_L     60
#define MONTH_T     13
#define MONTH_R     120
#define MONTH_B     38

#define YEAR_L      120
#define YEAR_T      13
#define YEAR_R      180
#define YEAR_B      38

#endif

//#ifdef  __cplusplus
//}
//#endif

#endif /* EXT_MONTHCAL_H */

