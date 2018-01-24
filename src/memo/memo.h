/*
** $Id: memo.h 224 2007-07-03 09:38:24Z houhuihua$
**
** memo.h: head file of memo.
**
** Copyright (C) 2007 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: houhuihua.
*/


#ifndef EXT_MEMO_H
#define EXT_MEMO_H

#undef  _DEBUG
//#define _DEBUG

#ifdef  _DEBUG
    #define _MG_DEBUG(fmt...) fprintf (stderr, fmt)
#else
    #define _MG_DEBUG(fmt...)
#endif

#ifdef  _DEBUG
    #define DBGLOG(s)               do { if (1) { fprintf(stderr, s); } } while (0)
    #define DBGLOG1(s, t1)          do { if (1) { fprintf(stderr, s, t1); } } while (0)
    #define DBGLOG2(s, t1,t2)       do { if (1) { fprintf(stderr, s, t1,t2); } } while (0)
    #define DBGLOG3(s, t1,t2,t3)    do { if (1) { fprintf(stderr, s, t1,t2,t3); } } while (0)
    #define DBGLOG4(s, t1,t2,t3,t4) do { if (1) { fprintf(stderr, s, t1,t2,t3,t4); } } while (0)
#else
    #define DBGLOG(s)
    #define DBGLOG1(s, t1)
    #define DBGLOG2(s, t1, t2)
    #define DBGLOG3(s, t1, t2, t3)
    #define DBGLOG4(s, t1, t2, t3, t4)
#endif

#define LIST_TXT_COLOR     0x00374150
#define LIST_TXT_SELCOLOR  0x00000000
#define C_TITLE_COLOR      0x00ffffff
#define C_DATE_COLOR       0x0078C1FF

#define SCR_W         240
#define SCR_H         320
                      
#define MEMO_TITLE    50
#define LIST_LEFT     0
#define LIST_TOP      MEMO_TITLE 
#define LIST_W        SCR_W 
#define LIST_H        SCR_H-LIST_TOP 
#define EDIT_W        SCR_W 
#define EDIT_TOP      50 
#define EDIT_BTN_H    24 
#define EDIT_H        SCR_H-EDIT_TOP-EDIT_BTN_H
#define EDIT_L_MAR    22 
#define EDIT_L_VLINE  12 
#define EDIT_T_VLINE  30 
#define EDIT_SPACE_VLINE  3 

/* list memo button. */
#define MEMO_TITLE_X  60
#define MEMO_TITLE_Y  10 
#define MEMO_TITLE_W  120 
#define MEMO_TITLE_H  32 
#define EDIT_T_BTN_Y  6
#define MEMO_NEW_X    196 
#define MEMO_NEW_W    40 
#define MEMO_NEW_H    21 

#define EDIT_NOTES_Y  6
#define EDIT_NOTES_X  4 
#define EDIT_NOTES_W  42 
#define EDIT_NOTES_H  21 

/* edit memo button. */
#define EDIT_BTN_Y    290
#define EDIT_LAR_X    31 
#define EDIT_RAR_X    209 
#define EDIT_AR_W     28 
#define EDIT_AR_H     28 
#define EDIT_EMAIL_X  87 
#define EDIT_EMAIL_W  28 
#define EDIT_EMAIL_H  28 
#define EDIT_TRASH_X  150 
#define EDIT_TRASH_W  28 
#define EDIT_TRASH_H  28 

/* del dialog button. */
#define DELETE_W      240 
#define DELETE_H      105 
#define DELETE_X      0 
#define DELETE_Y      SCR_H-DELETE_H
#define DELETE_BTN_W  187 
#define DELETE_BTN_H  26 
#define DELETE_OK_X   26  
#define DELETE_OK_Y   27 
#define DELETE_C_X    26 
#define DELETE_C_Y    63 

/* eblist subiem width.*/
#define LIST_TITLE_W  175
#define LIST_DATE_W   50
#define LIST_ARROW_W  15 

/* memo edit button ID.*/
#define IDC_MEMOLIST  0x1000
#define IDC_MEMOEDIT  0x1001
#define IDC_MEMOLAR   0x1002
#define IDC_MEMOEMAIL 0x1003
#define IDC_MEMOTRASH 0x1004
#define IDC_MEMORAR   0x1005
#define IDC_MEMONOTES 0x1006
#define IDC_MEMONEW   0x1007
#define IDC_MEMOTITLE 0x1008
#define IDC_MEMODOWN  0x1009
#define IDC_MEMOOK    0x1010
#define IDC_MEMOCANCEL 0x1011
#define IDC_MEMOTODAY 0x1012
#define IDC_MEMODATE  0x1013

#define RES_PATH      "./res/memo/"

#define F_MEMO_E_SW   RES_PATH"memo_sw.png"
#define F_MEMO_L_SW   RES_PATH"memo_sw.png"
#define F_MEMO_BK     RES_PATH"memo_bk.png"
#define F_MEMO_NEW    RES_PATH"new.png"
#define F_MEMO_DOWN   RES_PATH"down.png"
#define F_LIST_TITLE  RES_PATH"list_title.png"
#define F_EDIT_TITLE  RES_PATH"edit_title.png"
#define F_LIST_BK     RES_PATH"list_bk.png"
#define F_EDIT_BK     RES_PATH"edit_bk.png"
#define F_LIST_ARROW  RES_PATH"gray_arrow.png"
#define F_EDIT_HLINE  RES_PATH"hline.png"
#define F_EDIT_VLINE  RES_PATH"vline.png"
#define F_EDIT_LARROW RES_PATH"l_arrow.png"
#define F_EDIT_RARROW RES_PATH"r_arrow.png"
#define F_EDIT_TRASH  RES_PATH"trash.png"
#define F_EDIT_EMAIL  RES_PATH"email.png"
#define F_EDIT_NOTES  RES_PATH"notes.png"
#define F_DEL_OK      RES_PATH"del_ok.png"
#define F_DEL_CANCEL  RES_PATH"del_cancel.png"
#define F_DEL_BK      RES_PATH"del_bk.png"
#define F_SCROLL_FG   RES_PATH"scroll.png"
#define F_SCROLL_BK   RES_PATH"scroll_bk.png"
#define F_LIST_2_EDIT RES_PATH"animate_list2edit.png"
#define F_EDIT_2_LIST RES_PATH"animate_edit2list.png"
#define F_DEL_DIALOG  RES_PATH"del_dialog.png"
#define F_EDIT_SNOP   RES_PATH"edit_snop.png"
#define F_LIST_SNOP   RES_PATH"list_snop.png"
#define F_DEL_SNOP    RES_PATH"del_snop.png"
#define F_DEL_ANIMATE_BK  RES_PATH"animate_del_bk.png"

void   draw_scrollbar(HWND hWnd, HDC hdc, int sb_pos);
WINDOW_ELEMENT_RENDERER* get_window_renderer(HWND hWnd);

typedef struct _MEMOITEM{
    char*   title;
    char*   date;
    char*   memo; 
    PBITMAP icon;
#if 0
    char    title[255];
    char    date[100];
    char*   memo; 
    PBITMAP icon;
#endif
}MEMOITEM;


#endif /* EXT_MEMO_H */

