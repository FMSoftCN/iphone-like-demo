/*
 * common.h define the common information for iphone-like demo
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include "config.h"

#define CFGFILE  "iphone-like-demo.etc"

#define TOP_LAYOUT "top-layout"

#define DOWN_ICON_ROW 3
#define DOWN_ICON_COL 1
#define UP_ICON_ROW 1
#define UP_ICON_COL 1


//#ifdef  ENABLE_LANDSCAPE 
#define LEFT_GL_ROW 0
#define LEFT_GL_COL 0
#define RIGHT_GL_ROW 0
#define RIGHT_GL_COL 1
#define IPHONE_MAIN_WIDTH   240 
#define IPHONE_MAIN_HEIGHT  320
/*#else
#define LEFT_GL_ROW 0
#define LEFT_GL_COL 1
#define RIGHT_GL_ROW 0
#define RIGHT_GL_COL 2
#define IPHONE_MAIN_WIDTH    240
#define IPHONE_MAIN_HEIGHT   320
#endif
*/

#if defined(__GNUC__)
#ifdef _DEBUG_MSG
#   define _MY_PRINTF(fmt...) fprintf (stderr, fmt)
#else
#   define _MY_PRINTF(fmt...)
#endif
#else /* __GNUC__ */
static inline void _MY_PRINTF(const char* fmt, ...)
{
#ifdef _DEBUG_MSG
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
#endif
}
#endif /* __GNUC__ */

#endif
