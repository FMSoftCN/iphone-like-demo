/*
** $Id: memo.c 224 2007-07-03 09:38:24Z houhuihua$
**
** memo.c: minigui 3.0 demo's memo.
**
** Copyright (C) 2007 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: houhuihua.
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <minigui/common.h>
#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/ctrl/textedit.h>
#include <minigui/ctrl/edit.h>

#include "memo.h"
#include "ebcontrol.h"
#include "eblistview.h"
#include "ebbutton.h"
#include "softkeywindow.h"
#include "common_animates/common_animates.h"
#include "../sharedbuff.h"

#include "agg_flip.h"
#include "agg_scroll.h"

#define GOOD_ANIMATE
#define ANIMATE_FRAME 10

#define MSG_SHAREBUFFER_READY    5000

static void Edit2ListAnimate(HWND hDlg);
static void List2EditAnimate(HWND hDlg);
static void GetWindowBitmap(HWND hDlg, PBITMAP pbmp);
static void ShowDoubleBufferWin(HWND hWnd);
static void DeleteDlgAnimateOut(PBITMAP pbmp);
static void DeleteDlgAnimateIn(PBITMAP pbmp);
static void DrawDoubleBufferWindow(HWND hWnd);
static void PushPullBitmapAnimate(HDC hdc, const RECT *rt, 
        PBITMAP bmpPush, PBITMAP bmpPull, int frame_num, BOOL left_to_right);

static void LoadAllRes(char** bmp_file, int num);
static void GetScrollBkbmp(HWND hWnd, RECT* rc);
static void SetBoldFont(HWND hWnd, PLOGFONT* pfont);
static void GetDoubleBufferBitmap(HWND hWnd, PBITMAP pbmp);
static int DoubleBufferProc (HWND hWnd, HDC private_dc, HDC real_dc,
                const RECT* update_rc, const RECT* real_rc, const RECT* main_update_rc);

static PLOGFONT pedit_title_font;
static PLOGFONT plist_title_font;
static PLOGFONT plist_eblist_font;
static int g_row;
static int g_max_row;
static HWND hListWin;
static HWND hEditWin;
static BOOL g_bMainToSub;

#if 0
static char*  bmp_file[] = {
    F_MEMO_BK,   
    F_MEMO_E_SW,   
    F_MEMO_L_SW,   
    F_MEMO_NEW,
    F_MEMO_DOWN,
    F_LIST_ARROW, 
    F_EDIT_HLINE, 
    F_EDIT_VLINE, 
    F_EDIT_LARROW,
    F_EDIT_RARROW,
    F_EDIT_TRASH,
    F_EDIT_EMAIL, 
    F_EDIT_NOTES,
    F_DEL_OK,    
    F_DEL_CANCEL,
    F_DEL_BK,    
    F_DEL_DIALOG,  
    F_SCROLL_FG    
};
#else

#define MEMO_LIST_RES     1 
#define MEMO_EDIT_RES     2
#define MEMO_DELETE_RES   3

static BOOL list_res_load    = FALSE;
static BOOL edit_res_load    = FALSE;
static BOOL delete_res_load  = FALSE;

static char*  list_bmp_file[] = {
    F_MEMO_BK,   
    F_MEMO_NEW,
    F_MEMO_DOWN,
    F_LIST_ARROW,
    F_EDIT_HLINE, 
};

static char*  edit_bmp_file[] = {
    //F_EDIT_HLINE, 
    F_EDIT_VLINE, 
    F_EDIT_LARROW,
    F_EDIT_RARROW,
    F_EDIT_TRASH,
    F_EDIT_EMAIL, 
    F_EDIT_NOTES,
    F_SCROLL_FG    
};

static char*  delete_bmp_file[] = {
    F_DEL_OK,    
    F_DEL_CANCEL,
    F_DEL_BK,    
    F_DEL_DIALOG  
};

#endif

static char*  dc_bmp_file[] = {
    F_LIST_TITLE, 
    F_EDIT_TITLE, 
    F_LIST_BK,
    F_EDIT_BK,
    F_SCROLL_BK,
    F_DEL_ANIMATE_BK,
    F_LIST_SNOP,
    F_EDIT_SNOP,
    F_DEL_SNOP
};

WINDOW_ELEMENT_RENDERER* current_rdr;

#define ALL_BMP_NUM  35

static int     g_pos;
static BITMAP  bmp_all[ALL_BMP_NUM];

static WNDPROC OldEditProc;
static WNDPROC OldEbListProc;
static HWND    hListview;
static RECT    g_scroll_rc;
static HDC     g_bkdc;
static HDC     g_buffdc;
static BOOL    g_edit_return = FALSE;

static EBLVEXTDATA listitem_data =
{
    28,             //nItemHeight            
    0,              //nItemGap               
    0x7e1f,         //nSelectBKColor    
    0x0000,         //nSelectTextColor    
    0xffff,         //nItemBKColor       
    0x2146,         //nItemTextColor     
    0xffff,         //nMouseOverBkColor   
    0x2277,         //nMouseOverTextColor
    (DWORD)0,       //nItemBKImage 
    (DWORD)0        //nBKImage 
};

static DLGTEMPLATE DlgMemoDelete =
{
    WS_NONE,
    //WS_EX_NONE,
    WS_EX_AUTOSECONDARYDC | WS_EX_DLGHIDE,
    DELETE_X, DELETE_Y, DELETE_W, DELETE_H,
    "delete dlg",
    0, 0,
    2, NULL,
    0
};

static CTRLDATA CtrlMemoDelete[] =
{ 
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        DELETE_OK_X, DELETE_OK_Y, DELETE_BTN_W, DELETE_BTN_H,
        IDC_MEMOOK,
        0,
        0,
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        DELETE_C_X, DELETE_C_Y, DELETE_BTN_W, DELETE_BTN_H,
        IDC_MEMOCANCEL,
        0,
        0,
    }
};

static DLGTEMPLATE DlgMemoList =
{
    WS_NONE,
    //WS_EX_NONE,
    WS_EX_AUTOSECONDARYDC,
    0, 0, SCR_W, SCR_H,
    "memoList",
    0, 0,
    3, NULL,
    0
};

static CTRLDATA CtrlMemoList[] =
{ 
    {
        CTRL_EBLISTVIEW,
        WS_CHILD | WS_VISIBLE |
        ELVS_TYPE3STATE | ELVS_ITEMUSERCOLOR | ELVS_BKIMAGELEFTTOP | ELVS_BKBITMAP, 
        LIST_LEFT, LIST_TOP, LIST_W, LIST_H,
        IDC_MEMOLIST,
        0,
        (DWORD)&listitem_data,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE | SS_CENTER, 
        MEMO_TITLE_X, MEMO_TITLE_Y, MEMO_TITLE_W, MEMO_TITLE_H,
        IDC_MEMOTITLE,
        "MEMO TITLE",
        0,
        WS_EX_TRANSPARENT
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        MEMO_NEW_X, EDIT_T_BTN_Y, MEMO_NEW_W, MEMO_NEW_H,
        IDC_MEMONEW,
        0,
        0,
    }
};

static DLGTEMPLATE DlgMemoEdit =
{
    WS_NONE,
    WS_EX_AUTOSECONDARYDC,
    //WS_EX_NONE,
    0, 0, SCR_W, SCR_H,
    "memoEdit",
    0, 0,
    11, NULL,
    0
};

static MEMOITEM g_memo_list[20] = {
    {" About of MiniGUI.", "07-20", 
        "MiniGUI, is one of the world famous free software projects. MiniGUI aims to provide a lightweight graphics user interface (GUI) support system for real-time embedded systems. Since the first release under the GPL license at the beginning of 1999, MiniGUI has been widely used in handheld terminals (mobile phones and PDAs), set top boxes, industry control systems, industry instruments, portable media players, and so on."
        //"If a window is created using CreateWindowEx() with the WS_EX_TRANSPARENT style"},
    },
    {" The good architecture.", "07-21", 
        "The good architecture and optimized graphics interfaces of MiniGUI lead a very fast graphics output. Obviously, at the very start, MiniGUI was designed for realtime systems, taking into consideration the compactness, high performance, and high efficiency. MiniGUI has been widely used in many real cases, especially in industry production systems. MiniGUI plays an important role in these products or project"
        //"the original window was initially placed are not obscured and show through"},
    },
    {" GUI systems configurable.", "07-22", 
        "GUI systems are expected to be configurable in order to satisfy the different requirements from the embedded systems. Like Linux kernel, MiniGUI have many compilation configuration options, though which we can designate MiniGUI libraries to include and exclude some functions. In general, MiniGUI can be customized in the following aspects"
    },
        //"A good use of this style is for drawing tracking points on the top of another window"},
    {" Transparent style window.", "07-23", 
        "WS_EX_TRANSPARENT was designed to be used in very modal situations and the lifetime of a window with this style must be very short. A good use of this style is for drawing tracking points on the top of another window. For example, a dialog editor would use it to draw tracking points around the control that is being selected or moved"
        //"specific colors in a window can be made truly transparent by giving the window the WS_EX_LAYERED extended style and calling SetLayeredWindowAttributes"
    }, 
};

static CTRLDATA CtrlMemoEdit[] =
{ 
    {
        CTRL_MLEDIT,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOWRAP /* | TES_BASELINE */, 
        0, EDIT_TOP, EDIT_W, EDIT_H,
        IDC_MEMOEDIT,
        //"this is only a test, can you tell me your name?\n",
        "",
        0,
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        EDIT_NOTES_X, EDIT_T_BTN_Y, EDIT_NOTES_W, EDIT_NOTES_H,
        IDC_MEMONOTES,
        0,
        0,
    },
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE | SS_CENTER, 
        MEMO_TITLE_X, MEMO_TITLE_Y, MEMO_TITLE_W, MEMO_TITLE_H,
        IDC_MEMOTITLE,
        "Edit Title",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE | SS_LEFT, 
        25, 36, 40, 30,
        IDC_MEMOTODAY,
        "Today",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE | SS_CENTER, 
        120, 36, 110, 30,
        IDC_MEMODATE,
        "",
        0,
        WS_EX_TRANSPARENT
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        MEMO_NEW_X, EDIT_T_BTN_Y, MEMO_NEW_W, MEMO_NEW_H,
        IDC_MEMONEW,
        0,
        0,
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        MEMO_NEW_X, EDIT_T_BTN_Y, MEMO_NEW_W, MEMO_NEW_H,
        IDC_MEMODOWN,
        0,
        0,
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        EDIT_LAR_X, EDIT_BTN_Y, EDIT_AR_W, EDIT_AR_H,
        IDC_MEMOLAR,
        0,
        0,
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        EDIT_EMAIL_X, EDIT_BTN_Y, EDIT_EMAIL_W, EDIT_EMAIL_H,
        IDC_MEMOEMAIL,
        0,
        0,
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        EDIT_TRASH_X, EDIT_BTN_Y, EDIT_TRASH_W, EDIT_TRASH_H,
        IDC_MEMOTRASH,
        0,
        0,
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        EDIT_RAR_X, EDIT_BTN_Y, EDIT_AR_W, EDIT_AR_H,
        IDC_MEMORAR,
        0,
        0,
    }
};

static PBITMAP arrowlist[] = {
    0,
    0,
    0
};

static int GetMemoItemCount(void)
{
    int i = 0;
    int num = TABLESIZE(g_memo_list);
    for (i = 0; i < num; i++) {
        if (!g_memo_list[i].memo)
            break;
    }
    return i;
}

static HWND AddOneMemoListItem(HWND hList, int nItem, MEMOITEM* list)
{
    EBLVSUBITEM stListViewSubItem;
    EBLVITEM stListViewItem;
    char cServiceName[256];

    memset (&cServiceName, 0, sizeof(cServiceName));
    memset(&stListViewSubItem, 0, sizeof(EBLVSUBITEM));

    stListViewItem.nItem = nItem;
    SendMessage (hList, ELVM_ADDITEM, 0, (LPARAM) &stListViewItem);	
    stListViewSubItem.nItem = nItem;

    /* add mem title and date text cell.*/
    stListViewSubItem.mask = ELV_TEXT; 
    stListViewSubItem.subItem = 1;
    stListViewSubItem.pszText = (char *)list[nItem-1].title;
    stListViewSubItem.cchTextMax = strlen(list[nItem-1].title);
    stListViewSubItem.wordtype = NULL;
    stListViewSubItem.iImage = 0;
    stListViewSubItem.lparam = 0;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    stListViewSubItem.subItem = 2;
    stListViewSubItem.pszText = (char *)list[nItem-1].date;
    stListViewSubItem.cchTextMax = strlen(list[nItem-1].date);
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    /* add one icon cell.*/
    arrowlist[0] = (PBITMAP)RetrieveRes (F_LIST_ARROW);;
    arrowlist[1] = (PBITMAP)RetrieveRes (F_LIST_ARROW);;
    arrowlist[2] = (PBITMAP)RetrieveRes (F_LIST_ARROW);;

    stListViewSubItem.mask = ELV_BITMAP; 
    stListViewSubItem.subItem = 3;
    stListViewSubItem.pszText = NULL;
    stListViewSubItem.cchTextMax = 0;
    stListViewSubItem.iImage = (DWORD)arrowlist;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    return hList;
}

static int InitMemoList (HWND hWnd)
{
    HWND hTitle;
    int i = 0;
    EBLVCOLOUM stListViewColumn;
    //int itemCount = TABLESIZE(g_memo_list);
    int itemCount = GetMemoItemCount();

    //SetBoldFont(hWnd, &plist_eblist_font);
    /* add three column.*/
    stListViewColumn.nCols = 1;
    stListViewColumn.pszHeadText = "test";
    stListViewColumn.width = LIST_TITLE_W;
    SendMessage (hWnd, ELVM_ADDCOLUMN, 0, (LPARAM)&stListViewColumn);
    stListViewColumn.nCols = 2;
    stListViewColumn.width = LIST_DATE_W;
    SendMessage (hWnd, ELVM_ADDCOLUMN, 0, (LPARAM)&stListViewColumn);
    stListViewColumn.nCols = 3;
    stListViewColumn.width = LIST_ARROW_W;
    SendMessage (hWnd, ELVM_ADDCOLUMN, 0, (LPARAM)&stListViewColumn);

    /* add two line item.*/
    for (i = 1; i < (itemCount+1); i++) {
        AddOneMemoListItem(hWnd, i, g_memo_list);
    } 

    if (g_row)
        SendMessage (hWnd, ELVM_SETITEMSELECTED, 0, g_row);
    else
        SendMessage (hWnd, ELVM_SETITEMSELECTED, 0, 1);

    SetFocus(hWnd); 
    return 1;
}

void static GetScrollBkbmp(HWND hWnd, RECT* rc)
{
    HDC  memDC;
    HDC  hdc;
    hdc = GetClientDC (hWnd);
    memDC = CreateCompatibleDCEx (hdc, SCR_W, SCR_H);
    FillBoxWithBitmap (memDC, 0, 0, SCR_W, SCR_H, RetrieveRes(F_MEMO_BK));
    GetBitmapFromDC (memDC, rc->left, rc->top, RECTWP(rc), RECTHP(rc), 
            (PBITMAP)RetrieveRes(F_SCROLL_BK));
    DeleteCompatibleDC(memDC);
    ReleaseDC (hdc);
}

void static GetEditBkbmp(HWND hWnd, HWND hEdit)
{
    HDC  memDC;
    HDC  hdc;
    RECT rc;
    hdc = GetClientDC (hWnd);
    memDC = CreateCompatibleDCEx (hdc, SCR_W, SCR_H);
    FillBoxWithBitmap (memDC, 0, 0, SCR_W, SCR_H, RetrieveRes(F_MEMO_BK));
    GetWindowRect (hEdit, &rc);
    GetBitmapFromDC (memDC, rc.left, rc.top, RECTW(rc), RECTH(rc), 
            (PBITMAP)RetrieveRes(F_EDIT_BK));
    DeleteCompatibleDC(memDC);
    ReleaseDC (hdc);
}


static void lv_notify_process (HWND hwnd, int id, int code, DWORD addData)
{
	PEBLSTVWDATA pListdata = (PEBLSTVWDATA)addData;
    PEBITEMDATA  pCell = NULL;
    static int count = 0;
    if (pListdata && code == ELVN_SELCHANGE) {
        pCell = pListdata->pItemSelected;
        if (pCell->nRows <= 0) return;
        _MG_DEBUG("%d:click id is CONTROL_ID=%d, ROW=%d, COL=%d\n", count++, 
                id, pCell->nRows, pCell->nCols);
        g_row = pCell->nRows;
        SendMessage (hwnd, ELVM_SETITEMSELECTED, 0, g_row);
        GetBitmapFromDC (HDC_SCREEN, 0, 0, SCR_W, SCR_H, 
                (PBITMAP)RetrieveRes(F_LIST_SNOP));
#if 1
        //DestroyMainWindow (GetParent(hwnd));
        //MainWindowCleanup (GetParent(hwnd));
#else
        ShowWindow(hwnd, SW_HIDE);
#endif
        DlgMemoEdit.controls = CtrlMemoEdit;
        CreateMainWindowIndirectParam (&DlgMemoEdit,
                HWND_DESKTOP, MemoEditProc, 0);
    }
}

//R37 G41 B50
static void InitEbListview(HWND hWnd)
{
    SetWindowAdditionalData(hWnd, (DWORD)&listitem_data);
    SetWindowBkColor(hWnd, COLOR_lightwhite);
    SetNotificationCallback(hWnd, lv_notify_process);
    OldEbListProc = GetWindowCallbackProc(hWnd);
    SetWindowCallbackProc(hWnd, EbListControlProc);
}

void static GetListBkbmp(HWND hWnd)
{
    FillBoxWithBitmap (g_bkdc, 0, 0, SCR_W, SCR_H,   RetrieveRes(F_MEMO_BK));
    GetBitmapFromDC (g_bkdc, 0, LIST_TOP, LIST_W, LIST_H,   (PBITMAP)RetrieveRes(F_LIST_BK));
    GetBitmapFromDC (g_bkdc, 0, 0,        SCR_W,  LIST_TOP, (PBITMAP)RetrieveRes(F_LIST_TITLE));
    GetBitmapFromDC (g_bkdc, 0, EDIT_TOP, EDIT_W, EDIT_H,   (PBITMAP)RetrieveRes(F_EDIT_BK));
}

void static GetDelAnimatebmp(HWND hWnd)
{
    HDC hdc;
    HDC g_bkdc;
    hdc    = GetClientDC (hWnd);
    g_bkdc = CreateCompatibleDCEx(hdc, SCR_W, SCR_H);
    FillBoxWithBitmap(g_bkdc, 0, 0, SCR_W, SCR_H,   RetrieveRes(F_DEL_DIALOG));
    UnloadBitmap((PBITMAP)RetrieveRes(F_DEL_DIALOG));
    GetBitmapFromDC(g_bkdc, 0, DELETE_Y, DELETE_W, DELETE_H, (PBITMAP)RetrieveRes(F_DEL_DIALOG));
    DeleteCompatibleDC(g_bkdc);
    ReleaseDC (hdc);
}

static int EbListControlProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) {
        case MSG_CREATE:
            break;
        case MSG_VSCROLL:
            if (wParam == SB_THUMBPOSITION) {
                _MG_DEBUG("list thumb pos=%d\n", lParam);
            }
            break;
        case MSG_PAINT:
            {
                int   i = 0;
                int   item_count;
                int   nItemHeight = listitem_data.nItemHeight;
                int   h = nItemHeight;
                RECT  rc;

                /* old paint will not paint to screen after EndPaint. */
                hdc = GetSecondaryDC (GetParent(hWnd));
                SetSecondaryDC(GetParent(hWnd), hdc, DoubleBufferProc);
                OldEbListProc(hWnd, message, wParam, lParam);
                SetSecondaryDC(GetParent(hWnd), hdc, NULL);

                hdc = BeginPaint(hWnd);
                GetWindowRect(hWnd, &rc);
                item_count = RECTH(rc) / nItemHeight;
                for (i = 0; i < item_count; i++) {
                    FillBoxWithBitmap(hdc, 0, h, RECTW(rc), 1, 
                            RetrieveRes(F_EDIT_HLINE));
                    h += nItemHeight;
                }
                EndPaint(hWnd, hdc);
                return 0;
            }
    }
    return OldEbListProc(hWnd, message, wParam, lParam);
}


static void SetStaticCenter(HWND hCtrl)
{
    int  size = 0;
    RECT rc;
    GetWindowRect(hCtrl, &rc);
    size = (GetWindowFont(hCtrl))->size;
    MoveWindow(hCtrl, rc.left, (MEMO_TITLE_H-size)/2, RECTW(rc), RECTH(rc), TRUE);
}

static void SetBoldFont(HWND hWnd, PLOGFONT* pfont)
{
    if (*pfont && ((*pfont)->style & FS_WEIGHT_BOLD)) {
        SetWindowFont(hWnd, *pfont);
        return;
    }
    *pfont = CreateLogFontIndirect (GetWindowFont(hWnd));
    (*pfont)->style |= FS_WEIGHT_BOLD;
    SetWindowFont(hWnd, *pfont);
}

static int MemoListProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam )
{
    HDC  hdc;
    HWND hCtrl;
    RECT rc;
    int  size;

    switch ( message ) {
        case MSG_CREATE:
            if (!list_res_load) {
                list_res_load = TRUE; 
                LoadAllRes(list_bmp_file, TABLESIZE(list_bmp_file));
            }
            listitem_data.nBKImage = (DWORD)RetrieveRes(F_LIST_BK);
            listitem_data.nSelectBKColor = RGB2Pixel(HDC_SCREEN, 120, 193, 255);
            listitem_data.nItemTextColor = RGB2Pixel(HDC_SCREEN, 37, 41, 50);
            current_rdr = get_window_renderer (hDlg);
            current_rdr->draw_scrollbar = draw_scrollbar;
            GetListBkbmp (hDlg);
            return 0;
        case MSG_INITDIALOG:
            hCtrl = GetDlgItem(hDlg, IDC_MEMOLIST);
            InitEbListview(hCtrl);
            InitMemoList(hCtrl);
            hCtrl = GetDlgItem(hDlg, IDC_MEMONEW);
            SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_MEMO_NEW));
            SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
            hCtrl = GetDlgItem(hDlg, IDC_MEMOTITLE);
            SetWindowElementAttr (hCtrl, WE_FGC_WINDOW, C_TITLE_COLOR);
            SetBoldFont(hCtrl, &plist_title_font);
            SetStaticCenter(hCtrl);
#if 0
            if (g_edit_return) {
                _MG_DEBUG("start edit2listanimate.\n");
                Edit2ListAnimate(hDlg);
                g_edit_return  = FALSE;
            }
#endif
            return 0;
        case MSG_LBUTTONDOWN:
            break;
        case BUTTON_KEYUP:
            switch(wParam){
                case IDC_MEMONEW:
                    g_row = -1; /* new memoedit. */
                    GetBitmapFromDC (HDC_SCREEN, 0, 0, SCR_W, SCR_H, 
                            (PBITMAP)RetrieveRes(F_LIST_SNOP));
                    DlgMemoEdit.controls = CtrlMemoEdit;
                    CreateMainWindowIndirectParam (&DlgMemoEdit,
                            HWND_DESKTOP, MemoEditProc, 0);
                    break;
            }
        case MSG_SHAREBUFFER_READY:
            g_bMainToSub = TRUE;
            return 0;
        case MSG_ERASEBKGND:
            {
                RECT  rc;
                BOOL  fGetDC = FALSE;
                RECT* clip = (RECT*) lParam;

                if (g_bMainToSub) {
                    g_bMainToSub = FALSE;
                    return 0;
                }
                hdc = (HDC)wParam;
                if (!hdc){
                    //hdc    = GetClientDC(hDlg);
                    hdc    = GetSecondaryClientDC(hDlg);
                    fGetDC = TRUE;
                }
                if (clip) {
                    rc = *clip;
                    ScreenToClient(hDlg, &rc.left, &rc.top);
                    ScreenToClient(hDlg, &rc.right, &rc.bottom);
                    IncludeClipRect(hdc, &rc);
                }

                FillBoxWithBitmap(hdc, 0, 0, SCR_W, MEMO_TITLE, RetrieveRes(F_LIST_TITLE));
                if (fGetDC){
                    //ReleaseDC(hdc);
                    ReleaseSecondaryDC(hDlg, hdc);
                }
            }
            return 0;
        case MSG_CLOSE:
            DestroyMainWindow (hDlg);
            return 0;
        default:
            break;
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

typedef struct ZorderOpInfo
{
    int     id_op;

    int     idx_znode;
    DWORD   flags;
    HWND    hwnd;
    HWND    main_win;
    RECT    rc; 
    RECT    rcA;
    char    caption[41]; 
} ZORDEROPINFO;


int ClientSetActiveWindow (HWND hWnd)
{
    int ret;
    REQUEST req;
    ZORDEROPINFO info;
    void* pWnd = (void*) hWnd;
    int* pValue;
    
    info.id_op = 7;
    pWnd = pWnd + 196;
    pValue = (int*)pWnd;
    info.idx_znode = *pValue;

    req.id = 0x000D;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (int)) < 0)
        return -1;
    return 0;
}

int ClientShow (HWND hWnd)
{
    int ret;
    REQUEST req;
    ZORDEROPINFO info;
    void* pWnd = (void*) hWnd;
    int* pValue;
    
    info.id_op = 4;
    pWnd = pWnd + 196;
    pValue = (int*)pWnd;
    info.idx_znode = *pValue;

    req.id = 0x000D;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (int)) < 0)
        return -1;
    return 0;
}

int Client2TopMost (HWND hWnd)
{
    int ret;
    REQUEST req;
    ZORDEROPINFO info;
    void* pWnd = (void*) hWnd;
    int* pValue;
    
    info.id_op = 3;
    pWnd = pWnd + 196;
    pValue = (int*)pWnd;
    info.idx_znode = *pValue;

    req.id = 0x000D;
    req.data = &info;
    req.len_data = sizeof (ZORDEROPINFO);

    if (ClientRequest (&req, &ret, sizeof (int)) < 0)
        return -1;
    return 0;
}

void WinMoveToUp (HWND hWnd)
{
    Client2TopMost (hWnd);
    //IncludeWindowStyle (hWnd, WS_VISIBLE);
    ClientShow (hWnd);
    ClientSetActiveWindow (hWnd);
}

static int MemoDeleteProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam )
{
    HDC  hdc;
    HWND hCtrl;

    switch ( message ) {
        case MSG_CREATE:
            return 0;
        case MSG_INITDIALOG:
            hCtrl = GetDlgItem (hDlg, IDC_MEMOOK);
            SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_DEL_OK));
            SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
            hCtrl = GetDlgItem (hDlg, IDC_MEMOCANCEL);
            SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_DEL_CANCEL));
            SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);

            GetDoubleBufferBitmap(hDlg, (PBITMAP)RetrieveRes(F_DEL_SNOP));
            DeleteDlgAnimateIn((PBITMAP)RetrieveRes(F_DEL_DIALOG));
            WinMoveToUp (hDlg);
            IncludeWindowStyle (hDlg, WS_VISIBLE);
            hCtrl = GetActiveWindow();
            //ExcludeWindowExStyle(hDlg, WS_EX_DLGHIDE);
            return 0;
        case BUTTON_KEYUP:
            switch (wParam) {
                case IDC_MEMOOK:
                case IDC_MEMOCANCEL:
                    EndDialog (hDlg, wParam);
                    //DeleteDlgAnimateOut((PBITMAP)RetrieveRes(F_DEL_SNOP));
                    DeleteDlgAnimateOut((PBITMAP)RetrieveRes(F_DEL_DIALOG));
                    break;
            }
            break;
        case MSG_ERASEBKGND:
            {
                RECT  rc;
                BOOL  fGetDC = FALSE;
                RECT* clip = (RECT*) lParam;

                hdc = (HDC)wParam;
                if (!hdc){
                    //hdc    = GetClientDC(hDlg);
                    hdc    = GetSecondaryClientDC(hDlg);
                    fGetDC = TRUE;
                }
                FillBoxWithBitmapPart (hdc, 0, 0, DELETE_W, DELETE_H,
                        SCR_W, SCR_H, RetrieveRes(F_DEL_BK), 0, SCR_H-DELETE_H);
                if (fGetDC){
                    //ReleaseDC(hdc);
                    ReleaseSecondaryDC(hDlg, hdc);
                }
            }
            return 0;
        case MSG_CLOSE:
            EndDialog (hDlg, IDCANCEL);
            return 0;
        default:
            break;
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}


void static GetEditCtrlBkbmp(HWND hWnd, HWND hEdit)
{
    HDC memDC;
    HDC hdc;
    RECT rc;
    hdc   = GetClientDC (hWnd);
    memDC = CreateCompatibleDCEx (hdc, SCR_W, SCR_H);
    FillBoxWithBitmap (memDC, 0, 0, SCR_W, SCR_H, RetrieveRes(F_LIST_BK));
    GetWindowRect (hEdit, &rc);
    GetBitmapFromDC (memDC, rc.left, rc.top, RECTW(rc), RECTH(rc), 
             (PBITMAP)RetrieveRes(F_EDIT_BK));
    DeleteCompatibleDC(memDC);
    ReleaseDC (hdc);
}

static void tePaint(HWND hWnd, HDC hdc, RECT *rcDraw)
{
    RECT rect;
    RECT clientrect;
    RECT *rc = rcDraw;
    RECT margin_rc;
    int h, indent = 0;
    int nLineHeight = SendMessage(hWnd, EM_GETLINEHEIGHT, 0, 0);

    SendMessage(hWnd, EM_GETMARGINS, 0, (LPARAM)&margin_rc);

    h = nLineHeight - 1;
    GetWindowRect(hWnd, &rect);
    GetClientRect(hWnd, &clientrect);
    SetPenColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_WINDOW));

    /* draw vline. */
    FillBoxWithBitmap(hdc, EDIT_L_VLINE, 0, 1, RECTH(rect), 
            RetrieveRes(F_EDIT_VLINE));
    FillBoxWithBitmap(hdc, EDIT_L_VLINE+EDIT_SPACE_VLINE, 0, 1, RECTH(rect), 
            RetrieveRes(F_EDIT_VLINE));

    /* draw hline. */
    dumpRegion(hdc);
    while (h < RECTHP(rc)) {
#if 1
        FillBoxWithBitmap(hdc, 0, rc->top + h, RECTW(rect), 1, 
                RetrieveRes(F_EDIT_HLINE));
#else
        MoveTo(hdc, 0, rc->top + h);
        LineTo(hdc, RECTW(rect), rc->top + h);
#endif
        h += nLineHeight;
    }
}

#include <minigui/ctrl/ctrlhelper.h>

typedef struct _scrdata
{
    unsigned short hScrollVal;
    unsigned short vScrollVal;
    unsigned short hScrollPageVal;
    unsigned short vScrollPageVal;

    /* scroll offset */
    int nContX;
    int nContY;
    /* content range */
    int nContWidth;
    int nContHeight;
    /* visible content range */
    unsigned short visibleWidth;
    unsigned short visibleHeight;

    /* content margins */
    unsigned short leftMargin;
    unsigned short topMargin;
    unsigned short rightMargin;
    unsigned short bottomMargin;

    HWND hSV;

    /* scrollbar mode */
    SBPolicyType sbPolicy;

    void (*move_content) (HWND, struct _scrdata *, BOOL);

} SCRDATA;
typedef SCRDATA *PSCRDATA;

static inline void scrolled_window_to_visible (PSCRDATA pscrdata, int *x, int *y)
{
    if (x)
        *x -= pscrdata->leftMargin;
    if (y)
        *y -= pscrdata->topMargin;
}

static inline void scrolled_visible_to_window (PSCRDATA pscrdata, int *x, int *y)
{
    if (x)
        *x += pscrdata->leftMargin;
    if (y)
        *y += pscrdata->topMargin;
}

static inline void scrolled_get_content_rect (PSCRDATA pscrdata, RECT *rc)
{
    if (rc) {
        rc->left = -pscrdata->nContX;
        rc->top = -pscrdata->nContY;
        rc->right = rc->left + pscrdata->nContWidth;
        rc->bottom = rc->top + pscrdata->nContHeight;
    }
}

static inline void scrolled_get_visible_rect (PSCRDATA pscrdata, RECT *rcVis)
{
    if (rcVis) {
        rcVis->left = pscrdata->leftMargin;
        rcVis->top = pscrdata->topMargin;
        rcVis->right = pscrdata->leftMargin + pscrdata->visibleWidth;
        rcVis->bottom = pscrdata->topMargin + pscrdata->visibleHeight;
    }
}

static void GetScrollDrawRect(HDC hdc, PSCRDATA p, PRECT rcDraw, PRECT rcVis)
{
    //RECT rcDraw, rcVis;
    //scrolled_get_visible_rect (p, rcVis);
    //ClipRectIntersect (hdc, rcVis);
    scrolled_get_content_rect  (p, rcDraw);
    scrolled_visible_to_window (p, &rcDraw->left,  &rcDraw->top);
    scrolled_visible_to_window (p, &rcDraw->right, &rcDraw->bottom);
}

#define _REGION_DEBUG
#ifdef _REGION_DEBUG
static void dumpRegion (HDC hdc)
{
#if _DEBUG
    CLIPRGN* region;
    CLIPRGN* lcrgn;
    CLIPRECT *cliprect;

    if (!hdc) return;

    region = (CLIPRGN*) ((BYTE*)hdc + 392); // pdc->ecrgn.
    lcrgn  = (CLIPRGN*) ((BYTE*)hdc + 360); // pdc->lcrgn.

    if (!(cliprect = region->head)) {
        _MG_DEBUG("region: %p is a null region.\n", region);
    }
    else {
        static int count = 0;
        _MG_DEBUG("************%d*****************.\n", count++);
        _MG_DEBUG("start of region: %p.\n", region);
        _MG_DEBUG("head of region: %p.\n", region->head);
        _MG_DEBUG("tail of region: %p.\n", region->tail);
        _MG_DEBUG("Bound of region: (%d, %d, %d, %d)\n", 
                        region->rcBound.left,
                        region->rcBound.top,
                        region->rcBound.right,
                        region->rcBound.bottom);
        while (cliprect) {
            _MG_DEBUG("cliprect %p: (%d, %d, %d, %d)\n", cliprect, 
                            cliprect->rc.left, cliprect->rc.top,
                            cliprect->rc.right,
                            cliprect->rc.bottom);

            cliprect = cliprect->next;
        }

        region = lcrgn;
         _MG_DEBUG("start of lcrgn: %p.\n", region);
         _MG_DEBUG("head of lcrgn: %p.\n", region->head);
         _MG_DEBUG("tail of lcrgn: %p.\n", region->tail);
         _MG_DEBUG("Bound of lcrgn: (%d, %d, %d, %d)\n", 
                        region->rcBound.left,
                        region->rcBound.top,
                        region->rcBound.right,
                        region->rcBound.bottom);
        while (cliprect) {
            _MG_DEBUG("cliprect %p: (%d, %d, %d, %d)\n", cliprect, 
                            cliprect->rc.left, cliprect->rc.top,
                            cliprect->rc.right,
                            cliprect->rc.bottom);

            cliprect = cliprect->next;
        }
        _MG_DEBUG("end of lcrgn: %p.\n", region);
        _MG_DEBUG("*****************************.\n");
    }
#endif
}
#endif

static void EditDrawPaper(HWND hWnd, HDC hdc)
{
    RECT rcDraw;
    PSCRDATA p = NULL;
    p = (PSCRDATA) GetWindowAdditionalData2 (hWnd);
    GetScrollDrawRect(hdc, p, &rcDraw, NULL);
    tePaint(hWnd, hdc, &rcDraw);
}

static void PaintTextEdit(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rcDraw, rcVis;
    PSCRDATA p = NULL;
    hdc = GetClientDC(hWnd);
    OldEditProc(hWnd, message, wParam, lParam);
    p = (PSCRDATA) GetWindowAdditionalData2 (hWnd);
    GetScrollDrawRect(hdc, p, &rcDraw, &rcVis);
    tePaint(hWnd, hdc, &rcDraw);
    ReleaseDC(hdc);
}

static void IncludeMarginScroll(HWND hWnd, HDC hdc, BOOL clip)
{
    RECT rc;
    RECT margin_rc;
    RECT left_rc;
    //RECT g_scroll_rc;
    SendMessage(hWnd, EM_GETMARGINS, 0, (LPARAM)&margin_rc);
    printf("margin_rc:l=%d,t=%d,r=%d,b=%d\n",
            margin_rc.left, margin_rc.top, margin_rc.right, margin_rc.bottom);
    GetWindowRect(hWnd, &rc);
#if 0
    if (!clip) {
        ScreenToClient(hWnd, &rc.left, &rc.top);
        ScreenToClient(hWnd, &rc.right, &rc.bottom);
        SelectClipRect(hdc, &rc);
    }
#endif
    left_rc.left   = 0;
    left_rc.right  = margin_rc.left;
    left_rc.top    = margin_rc.top;
    left_rc.bottom = RECTH(rc);

    if (!current_rdr->calc_we_area(hWnd, HT_VSCROLL, &g_scroll_rc)) {
        static int first = FALSE;
        g_scroll_rc.left -= margin_rc.right;
        g_scroll_rc.top  += margin_rc.top;
        IncludeClipRect(hdc, &g_scroll_rc);
        _MG_DEBUG("scroll_clip:l=%d,t=%d,r=%d,b=%d\n", g_scroll_rc.left, 
                g_scroll_rc.top, g_scroll_rc.right, g_scroll_rc.bottom);
        if (!first) {
            //RetrieveRes(F_SCROLL_BK));
            ClientToScreen(hWnd, &g_scroll_rc.left,  &g_scroll_rc.top);
            ClientToScreen(hWnd, &g_scroll_rc.right, &g_scroll_rc.bottom);
            GetScrollBkbmp(hWnd, &g_scroll_rc);
            //GetBitmapFromDC (g_bkdc, g_scroll_rc.left, g_scroll_rc.top, 
            //        RECTW(g_scroll_rc), RECTH(g_scroll_rc), 
            //        (PBITMAP)RetrieveRes(F_SCROLL_BK));
            first = TRUE;
        }
    }
    _MG_DEBUG("left_clip:l=%d,t=%d,r=%d,b=%d\n", left_rc.left, 
            left_rc.top, left_rc.right, left_rc.bottom);
    IncludeClipRect(hdc, &left_rc);
}

static WNDPROC OldEditNotesProc;
static int EditNotesProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) {
        case MSG_ERASEBKGND:
            break;
    }
    return OldEditNotesProc(hWnd, message, wParam, lParam);
}

static int EditControlProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) {
        case MSG_CREATE:
            break;
        case MSG_VSCROLL:
            if (wParam == SB_THUMBPOSITION) {
                _MG_DEBUG("thumb pos=%d\n", lParam);
            }
            break;
        case MSG_PAINT:
            _MG_DEBUG("EditControlProc MSG_PAINT:\n");
            break;
        case MSG_NCPAINT:
            {
                BOOL  fGetDC = FALSE;
                RECT  rc;
                HDC   hdc; 
                HDC   mem_dc;
#if 1
#if 1
                hdc = GetClientDC(hWnd);
                GetWindowRect(hWnd, &rc);
                mem_dc = CreateCompatibleDCEx (hdc, RECTW(rc), RECTH(rc));
                IncludeMarginScroll(hWnd, hdc, FALSE);
                FillBoxWithBitmap(mem_dc, 0, 0, RECTW(rc), RECTH(rc), RetrieveRes(F_EDIT_BK));
                EditDrawPaper(hWnd, mem_dc);
                BitBlt(mem_dc, 0, 0, RECTW(rc), RECTH(rc), hdc, 0, 0, 0); 
                DeleteCompatibleDC(mem_dc);
                ReleaseDC(hdc);
#else
                hdc = GetSecondaryDC(hWnd);
                //hdc = GetDC(hWnd);
                GetWindowRect(hWnd, &rc);
                if (lParam) {
                    ClipRectIntersect (hdc, (RECT*)lParam);
                }
                IncludeMarginScroll(hWnd, hdc, FALSE);
                FillBoxWithBitmap(hdc, 0, 0, RECTW(rc), RECTH(rc), RetrieveRes(F_EDIT_BK));
                EditDrawPaper(hWnd, hdc);
                ReleaseSecondaryDC(hWnd, hdc);
                //ReleaseDC(hdc);
#endif
#else
                RECT client_rc;
                GetClientRect(hWnd, &client_rc);
                hdc = GetSubDC(GetSencondaryDC(GetParent(hWnd)), client_rc.left, 
                        client_rc.right, RECTW(client_rc), RECTH(client_rc));
                GetWindowRect(hWnd, &rc);
                IncludeMarginScroll(hWnd, hdc, FALSE);
                FillBoxWithBitmap(hdc, 0, 0, RECTW(rc), RECTH(rc), RetrieveRes(F_EDIT_BK));
                EditDrawPaper(hWnd, hdc);
#endif
            }
            break;
        case MSG_ERASEBKGND:
            {
                RECT  rc;
                BOOL  fGetDC = FALSE;
                RECT* clip = (RECT*) lParam;
                hdc = (HDC)wParam;
                if (!hdc){
                    //hdc    = GetClientDC(hWnd);
                    hdc    = GetSecondaryClientDC(hWnd);
                    fGetDC = TRUE;
                }
                if (clip) {
                    rc = *clip;

                    /* houhh 20081204, if is control, the rc's coordinate is 
                     * relattive to control.*/
                    //ScreenToClient(hWnd, &rc.left, &rc.top);
                    //ScreenToClient(hWnd, &rc.right, &rc.bottom);
                    
                    IncludeClipRect(hdc, &rc);
                    _MG_DEBUG("clip:l=%d,t=%d,r=%d,b=%d\n", clip->left, 
                            clip->top, clip->right, clip->bottom);
                }

                GetWindowRect(hWnd, &rc);
                FillBoxWithBitmap(hdc, 0, 0, RECTW(rc), RECTH(rc), RetrieveRes(F_EDIT_BK));
                EditDrawPaper(hWnd, hdc);

                if (fGetDC){
                    //ReleaseDC(hdc);
                    ReleaseSecondaryDC(hWnd, hdc);
                }
                PostMessage(hWnd, MSG_NCPAINT, 0, 0);
                return 0;
            }
    }
    return OldEditProc(hWnd, message, wParam, lParam);
}

static void EditCtrlInit(HWND hWnd)
{
    RECT margin_rc;
    HWND hEdit;
#if 0
    PLOGFONT editfont = CreateLogFont ("vbf", "Courier", "ISO8859-1", 
            FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, 
            FONT_SETWIDTH_NORMAL, FONT_SPACING_CHARCELL, 
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
            13, 0);
#endif
    hEdit = GetDlgItem (hWnd, IDC_MEMOEDIT);
    OldEditProc = GetWindowCallbackProc(hEdit);
    SendMessage(hEdit, EM_GETMARGINS, 0, (LPARAM)&margin_rc);
    margin_rc.left = EDIT_L_MAR;
    SendMessage(hEdit, EM_SETMARGINS, 0, (LPARAM)&margin_rc);
    if (g_row > 0) {
        _MG_DEBUG("g_row=%d\n", g_row);
        SetWindowText(hEdit, g_memo_list[g_row-1].memo);
    }
    SetWindowCallbackProc (hEdit, EditControlProc);
    //hEdit = GetDlgItem (hWnd, IDC_MEMONOTES);
    hEdit = GetDlgItem (hWnd, IDC_MEMONEW);
    OldEditNotesProc = GetWindowCallbackProc(hEdit);
    SetWindowCallbackProc (hEdit, EditNotesProc);
    
}

static int DoubleBufferProc (HWND hWnd, HDC private_dc, HDC real_dc,
                const RECT* update_rc, const RECT* real_rc, const RECT* main_update_rc)
{
    return 0;
}

void GUIAPI UpdateAll (HWND hWnd, BOOL fErase)
{
    MSG Msg;
    RECT rect;

#if 0
    if (fErase)
        SendAsyncMessage (hWnd, MSG_CHANGESIZE, 0, 0);

    SendAsyncMessage (hWnd, MSG_NCPAINT, 0, 0);
    if (fErase)
        InvalidateRect (hWnd, NULL, TRUE);
    else
        InvalidateRect (hWnd, NULL, FALSE);

    GetClientRect(hWnd, &rect);
    SendMessage (hWnd, MSG_PAINT, 0, (LPARAM)&rect);
#else
    UpdateWindow(hWnd, fErase);
#endif

    while (PeekMessageEx (&Msg, hWnd, MSG_PAINT, MSG_PAINT+1, 
                FALSE, PM_REMOVE)){
        TranslateMessage (&Msg);
        DispatchMessage (&Msg);
    }
}


static void GetDoubleBufferBitmap(HWND hWnd, PBITMAP pbmp)
{
    HDC hdc;
    int w = 0, h = 0;
    hdc = GetSecondaryDC (hWnd);
    SetSecondaryDC(hWnd, hdc, DoubleBufferProc);
    //ShowWindow(hWnd, SW_SHOWNORMAL); 
    UpdateAll (hWnd, TRUE);
    memset(pbmp, 0, sizeof(BITMAP));
    w = GetGDCapability(hdc, GDCAP_HPIXEL);
    h = GetGDCapability(hdc, GDCAP_VPIXEL);
    GetBitmapFromDC (hdc, 0, 0, w, h, pbmp);
    SetSecondaryDC(hWnd, hdc, NULL);
    //BitBlt(hdc, 0, 0, SCR_W, SCR_H, HDC_SCREEN, 0, 0, 0); 
}

static void ShowDoubleBufferWin(HWND hWnd)
{
    HDC hdc;
    int w = 0, h = 0;
    hdc = GetSecondaryDC (hWnd);
    SetSecondaryDC(hWnd, hdc, DoubleBufferProc);
    ShowWindow (hWnd, SW_SHOWNORMAL);
    SetSecondaryDC(hWnd, hdc, NULL);
}

static void DrawDoubleBufferWindow(HWND hWnd)
{
    HDC hdc;
    hdc = GetSecondaryDC (hWnd);
    SetSecondaryDC(hWnd, hdc, DoubleBufferProc);
    UpdateAll (hWnd, TRUE);
    SetSecondaryDC(hWnd, hdc, NULL);
}

#define ROLL_ANIMATE
#ifdef  ROLL_ANIMATE
static int g_nTestFlame = 0;
static BOOL scroll_dir = TRUE;
static BITMAP g_stBitmap;

/* for rollpage animate. */
static HDC g_DownDC;
static HDC g_UpDC;

static void DrawAnimateDefault (HDC hdc, ANIMATE* pAnimate)
{

    if (GetAnimateW (pAnimate) != 0 && GetAnimateH (pAnimate) != 0) 
    {
        if (scroll_dir)
            g_nTestFlame ++;
        else
            g_nTestFlame --;

        ScrollPageFlame (g_nTestFlame, &g_stBitmap, HDC_SCREEN);
    }
}

static void DrawBKGDefault (HDC hdc, const RECT* pRect, void* pParam)
{
    //do nothing
}

static void DrawEndFrameDefault (ANIMATE_SENCE* pAnimateSence)
{
    if (pAnimateSence != NULL)
    {
    }
}

#ifdef _MGIAL_IPAQ_H3600
void iPAQRollPage(BOOL dir)
{
    int* p_idx = NULL;
    int  idx_num = 0;
    int  idx[6] = {1, 2, 3, 5, 8, 10};
    int  idx_prev[6] = {10, 8, 5, 3, 2, 0};
    int  i = 0;

    if (dir) {
        p_idx = idx;
        idx_num = TABLESIZE(idx);
    }          
    else {     
        p_idx = idx_prev;
        idx_num = TABLESIZE(idx_prev);
    }

    while (TRUE){
        if ( i >= idx_num)
            break;
        ScrollPageFlame (p_idx[i], &g_stBitmap, HDC_SCREEN);
        i++;
    }
}
#endif

void ScrollMemoAnimate(HDC hdc, BOOL dir, int frame_num);
void RollPage(HDC upDC, HDC downDC, BOOL dir, int frame_num)
{
    BITMAP srcbmp;

    if (dir)
        InitScrollPage(upDC, downDC, SCR_W, SCR_H, frame_num);
    else
        InitScrollPage(downDC, upDC, SCR_W, SCR_H, frame_num);

    SetScrollPageRender (TRUE, 0x6e6e6e7f, 0x2d415fcc);
#ifdef _MGIAL_IPAQ_H3600
    iPAQRollPage(dir);
#else
    ScrollMemoAnimate(HDC_SCREEN, dir, frame_num);
#endif
}

void ScrollMemoAnimate(HDC hdc, BOOL dir, int frame_num)
{
    int w, h;
    BITMAP bmp;
    const RECT rt = {0, 0, SCR_W, SCR_H};

    w = RECTW(rt);
    h = RECTH(rt);

    PUSH_PULL_OBJ objs[] ={
        {&bmp, 0, 0, 0, 0},
    };

    ANIMATE_OPS stAnimateOps = {
        DrawAnimateDefault,
        DrawBKGDefault,
        NULL,
        NULL,
        DrawEndFrameDefault
    };

    if (dir) {
        scroll_dir   = TRUE;
        g_nTestFlame = 0;
    }          
    else {     
        scroll_dir   = FALSE;
        g_nTestFlame = frame_num;
        //g_maxframe   = g_nTestFlame;
    }

    SetInterval (150);
    RunPushPullAnimate(hdc, &rt, objs, 1, &stAnimateOps, frame_num, NULL);
}

#endif

static GotoNextMemo(HWND hWnd, BOOL dir)
{
    HWND hCtrl;

    if (dir){
        if (g_row < GetMemoItemCount()) {
            g_row++;
            hCtrl = GetDlgItem (hWnd, IDC_MEMOLAR);
            EnableWindow(hCtrl, TRUE);
        }
        else {
            return ;
            hCtrl = GetDlgItem (hWnd, IDC_MEMORAR);
            EnableWindow(hCtrl, FALSE);
        }
    }
    else {
        if (g_row > 0) {
            g_row--;
            hCtrl = GetDlgItem (hWnd, IDC_MEMORAR);
            EnableWindow(hCtrl, TRUE);
        }
        else {
            return ;
            hCtrl = GetDlgItem (hWnd, IDC_MEMOLAR);
            EnableWindow(hCtrl, FALSE);
        }
    }

    if (g_row > 0) {
        hCtrl = GetDlgItem (hWnd, IDC_MEMOEDIT);
        SetWindowText(hCtrl, g_memo_list[g_row-1].memo);
    }


#ifdef ROLL_ANIMATE

    if (scroll_dir)
        g_nTestFlame = 0;
    else
        g_nTestFlame = 10;

    scroll_dir = dir;

    DrawDoubleBufferWindow(hWnd);
    
    if (!g_UpDC) {
        g_UpDC   = CreateCompatibleDC(HDC_SCREEN);
    }

    g_DownDC = GetSecondaryDC(hWnd);
    BitBlt (HDC_SCREEN, 0, 0, SCR_W, SCR_H, g_UpDC, 0, 0, 0);

    RollPage(g_UpDC, g_DownDC, dir, 10);

    //InitScrollPage(HDC_SCREEN, GetSecondaryDC(hWnd), 240, 320, 20);
    //ScrollMemoAnimate(HDC_SCREEN, 20);

#endif
}

static void AnimateNewToDown(HWND hWnd)
{
    HWND hCtrl;
    hCtrl = GetDlgItem (hWnd, IDC_MEMONEW);
    ShowWindow(hCtrl, SW_HIDE);
    hCtrl = GetDlgItem (hWnd, IDC_MEMODOWN);
    ShowWindow(hCtrl, SW_SHOWNORMAL);
}

static void AnimateDownToNew(HWND hWnd)
{
    HWND hCtrl;
    hCtrl = GetDlgItem (hWnd, IDC_MEMONEW);
    ShowWindow(hCtrl, SW_SHOWNORMAL);
    hCtrl = GetDlgItem (hWnd, IDC_MEMODOWN);
    ShowWindow(hCtrl, SW_HIDE);
}

static void SetEbButtonData(HWND hWnd)
{
    HWND hCtrl;
    PBITMAP pbmp;
    hCtrl = GetDlgItem (hWnd, IDC_MEMONEW);
    SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_MEMO_NEW));
    SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
#if 0
    pbmp = (PBITMAP)RetrieveRes(F_MEMO_NEW);
    pbmp->bmType |= BMP_TYPE_ALPHACHANNEL;
    pbmp->bmAlpha = 0x40;
#endif
    hCtrl = GetDlgItem (hWnd, IDC_MEMODOWN);
    SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_MEMO_DOWN));
    SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
    pbmp = (PBITMAP)RetrieveRes(F_MEMO_NEW);
    hCtrl = GetDlgItem (hWnd, IDC_MEMOTRASH);
    SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_EDIT_TRASH));
    SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
    pbmp = (PBITMAP)RetrieveRes(F_MEMO_NEW);
    hCtrl = GetDlgItem (hWnd, IDC_MEMONOTES);
    SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
    pbmp = (PBITMAP)RetrieveRes(F_MEMO_NEW);
    SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_EDIT_NOTES));
    SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
    pbmp = (PBITMAP)RetrieveRes(F_MEMO_NEW);
    hCtrl = GetDlgItem (hWnd, IDC_MEMOEMAIL);
    SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_EDIT_EMAIL));
    SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
    pbmp = (PBITMAP)RetrieveRes(F_MEMO_NEW);
    hCtrl = GetDlgItem (hWnd, IDC_MEMOLAR);
    SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_EDIT_LARROW));
    SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
    pbmp = (PBITMAP)RetrieveRes(F_MEMO_NEW);
    hCtrl = GetDlgItem (hWnd, IDC_MEMORAR);
    SetWindowAdditionalData(hCtrl, (DWORD)RetrieveRes(F_EDIT_RARROW));
    SendMessage(hCtrl, BUTTON_UPDATEBMP, 0, 0);
    pbmp = (PBITMAP)RetrieveRes(F_MEMO_NEW);
}

#define REQID_OPENIMEWND        0x000F
#define REQID_SETIMESTAT        0x0010
#define REQID_GETIMESTAT        0x0011

#include "../sfkbd_client.h"

#define SFKBD_REQID    (MAX_SYS_REQID + 9)
static int CloseIMEMethod(HWND hWnd, BOOL open)
{
#if 0
    int ret;
    REQUEST req;
    req.id       = 0x000F;
    req.data     = &open;
    req.len_data = sizeof (BOOL);
    ClientRequest(&req, NULL, 0);
#else
    MSG Msg;
    unsigned int tick_count = GetTickCount ();
    SFKBDShow (open);
    /* if have IME animate.*/
#if 0
    while (GetMessage (&Msg, HWND_DESKTOP)) {
        if (!SFKBDIsShown())
            break;
        if (GetTickCount () > (tick_count + 100))
            break;
        DispatchMessage (&Msg);
    }
    while (PeekMessageEx (&Msg, hWnd, MSG_PAINT, MSG_PAINT+1, 
                FALSE, PM_REMOVE)){
        DispatchMessage (&Msg);
    }
#endif
#endif

}

static void RestoreEbbutonBk(HWND hWnd, int id)
{
    RECT rc;
    HWND hCtrl = GetDlgItem(hWnd, id);
    GetWindowRect(hCtrl, &rc);
    InvalidateRect(hWnd, &rc, TRUE);
}

static char g_buf[255];
static char* month_str[] = {
    "Jan", 
    "Feb",
    "Mar",
    "Apr",
    "May",
    "June",
    "July",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};
static void SetDateTime(HWND hWnd)
{
    //time_t nowtime;
    //struct tm *pnt;
    HWND hCtrl;
    int year, month, mday;

    time_t t;
    struct tm * tm;

    time (&t);
    tm = localtime (&t);

    //__mg_time( &nowtime );
    //pnt = (struct tm * )__mg_localtime (nowtime);
    year  = tm->tm_year + 1900;
    month = tm->tm_mon + 1;
    mday  = tm->tm_mday + 1;
                    
    hCtrl = GetDlgItem(hWnd, IDC_MEMODATE);
    sprintf(g_buf, "%s %d %02d:%02d", month_str[month], mday, tm->tm_hour, tm->tm_min);
    SetWindowText(hCtrl, g_buf);
	SetWindowElementAttr (hCtrl, WE_FGC_WINDOW, C_DATE_COLOR);
    hCtrl = GetDlgItem(hWnd, IDC_MEMOTODAY);
	SetWindowElementAttr (hCtrl, WE_FGC_WINDOW, C_DATE_COLOR);
}

static void def_draw_bkgnd(HDC hdc, const RECT* rtbk, void *param)
{
    FillBoxWithBitmap(hdc, 0, DELETE_Y, DELETE_W, DELETE_H,
            RetrieveRes(F_DEL_ANIMATE_BK));
}

static void def_draw_del_animate(HDC hdc, ANIMATE*ani)
{
    if(GetAnimateW(ani) != 0 && GetAnimateH(ani)!=0)
    {
#if 1
        FillBoxWithBitmapPart(hdc, GetAnimateX(ani), GetAnimateY(ani),
                GetAnimateW(ani), GetAnimateH(ani),
                SCR_W, SCR_H, (PBITMAP)ani->img,
                0, SCR_H-DELETE_H);
#else
		FillBoxWithBitmap(hdc, 
			GetAnimateX(ani), GetAnimateY(ani),
			GetAnimateW(ani), GetAnimateH(ani),
			(PBITMAP)ani->img);
#endif
    }
    
}


static void on_del_end_draw_one_frame(ANIMATE_SENCE *as)
{
    if(as != NULL) {
        //SetMemDCAlpha(as->hdc, MEMDC_FLAG_SRCALPHA, 30);
        BitBlt(as->hdc, 0, SCR_H-DELETE_H, RECTW(as->rtArea),
                RECTH(as->rtArea), HDC_SCREEN, 0, SCR_H-DELETE_H, 0); 
    }
}

static void on_del_finished(ANIMATE_SENCE *as)
{
	if(as != NULL)
	{
        def_draw_bkgnd(HDC_SCREEN, &as->rtArea, NULL);
	}
}

static void DeleteDlgAnimateIn(PBITMAP pbmp)
{
    RECT rt = {0, SCR_H-DELETE_H, SCR_W, SCR_H};
    PUSH_PULL_OBJ objs[2] ={
        {pbmp, 0, SCR_H, 0, SCR_H-DELETE_H},
    };
    ANIMATE_OPS ops = {
        def_draw_del_animate,
        def_draw_bkgnd,
        //on_del_finished,
        NULL,
        NULL,
        on_del_end_draw_one_frame
    };

    SetInterval (50);
    RunPushPullAnimate(g_buffdc, &rt, objs, 1, &ops, 5, NULL);
}

static void DeleteDlgAnimateOut(PBITMAP pbmp)
{
    RECT rt = {0, SCR_H-DELETE_H, SCR_W, SCR_H};
    //PBITMAP pbmp = (PBITMAP)RetrieveRes(F_DEL_DIALOG);
    PUSH_PULL_OBJ objs[2] ={
        {pbmp, 0, SCR_H-DELETE_H, 0, SCR_H},
    };
    ANIMATE_OPS ops = {
        def_draw_del_animate,
        def_draw_bkgnd,
        on_del_finished,
        NULL,
        on_del_end_draw_one_frame
    };

    SetInterval (50);
    RunPushPullAnimate(g_buffdc, &rt, objs, 1, &ops, ANIMATE_FRAME, NULL);
}

static void def_nothing_bkgnd(HDC hdc, const RECT* rtbk, void *param)
{
    /* do nothing. */
}

static void def_draw_animate(HDC hdc, ANIMATE*ani)
{
    if(GetAnimateW(ani) != 0 && GetAnimateH(ani)!=0)
    {
		FillBoxWithBitmap(hdc, 
			GetAnimateX(ani), GetAnimateY(ani),
			GetAnimateW(ani), GetAnimateH(ani),
			(PBITMAP)ani->img);
    }
    
}


static void on_end_draw_one_frame(ANIMATE_SENCE *as)
{
	if(as != NULL)
	{
		BitBlt(as->hdc, 0, 0, RECTW(as->rtArea),RECTH(as->rtArea), HDC_SCREEN, 0, 0, 0); 
	}
}

static void PushPullBitmapAnimate(HDC hdc, const RECT *rt, 
        PBITMAP bmpPush, PBITMAP bmpPull, int frame_num, BOOL left_to_right)
{
	int w,h;
	w = RECTWP(rt);
	h = RECTHP(rt);
	PUSH_PULL_OBJ objs[2] ={
		{bmpPush,left_to_right?-w:w, 0,0,0},
		{bmpPull,0, 0, left_to_right?w:-w, 0}
    };
    ANIMATE_OPS ops = {
        def_draw_animate,
        def_nothing_bkgnd,
        NULL,
        NULL,
        on_end_draw_one_frame
    };

    SetInterval (50);
	RunPushPullAnimate(hdc, rt, objs, 2,  &ops, frame_num, NULL);
}

static void List2EditAnimate(HWND hDlg)
{
    UnloadBitmap((PBITMAP)RetrieveRes(F_EDIT_SNOP));
    //GetDoubleBufferBitmap(hDlg, (PBITMAP)RetrieveRes(F_EDIT_SNOP));
    GetWindowBitmap(hDlg, (PBITMAP)RetrieveRes(F_EDIT_SNOP));
    PushPullBitmapAnimate(g_buffdc, &g_rcScr, (PBITMAP)RetrieveRes(F_EDIT_SNOP), 
            (PBITMAP)RetrieveRes(F_LIST_SNOP), ANIMATE_FRAME, FALSE);
}

#define SURFACE(dc)    ((BYTE*)dc+2*sizeof(short)+2*sizeof(int))
#define PIXEL(surface) ((BYTE*)surface+4*sizeof(int)+2*sizeof(char*))

static int PaintAllControl(HWND hDlg)
{
    HWND hCtrl = 0;
    ShowWindow(hDlg, SW_SHOWNORMAL); 
    InvalidateRect(hDlg, NULL, TRUE);
    SendMessage(hDlg, MSG_PAINT, 0, 0);
    do {
        hCtrl = GetNextChild(hDlg, hCtrl);
        if (IsWindowVisible(hCtrl))
            SendMessage(hCtrl, MSG_PAINT, 0, 0);
        _MG_DEBUG("%d\n", hCtrl);
    }while(hCtrl && hCtrl != HWND_INVALID);
}

static void Edit2ListAnimate(HWND hDlg)
{
    UnloadBitmap((PBITMAP)RetrieveRes(F_LIST_SNOP));
    //GetDoubleBufferBitmap(hDlg, (PBITMAP)RetrieveRes(F_LIST_SNOP));
    GetWindowBitmap(hDlg, (PBITMAP)RetrieveRes(F_LIST_SNOP));
    PushPullBitmapAnimate(g_buffdc, &g_rcScr, (PBITMAP)RetrieveRes(F_LIST_SNOP), 
            (PBITMAP)RetrieveRes(F_EDIT_SNOP), ANIMATE_FRAME, TRUE);
}

static void GetWindowBitmap(HWND hDlg, PBITMAP pbmp)
{
    HDC hdc;
    HDC mem_dc;
    RECT  rc;
    char* pixel = NULL;
    char* mem_pixel = NULL;
    int   pixel_addr = 0;

    GetWindowRect(hDlg, &rc);
    hdc    = GetClientDC (hDlg);
    pixel  = PIXEL(*(int*)SURFACE(hdc));
    mem_dc = CreateCompatibleDCEx (hdc, SCR_W, SCR_H);
    SetMemDCAlpha(mem_dc, 0, 0);
    mem_pixel  = PIXEL(*(int*)SURFACE(mem_dc));
    pixel_addr = *((int*)pixel);
    *((int*)pixel) = *(int*)mem_pixel;
    ShowCursor(FALSE);
    PaintAllControl(hDlg);
    GetBitmapFromDC (mem_dc, rc.left, rc.top, RECTW(rc), RECTH(rc), 
            pbmp);
    ShowCursor(TRUE);
    *((int*)pixel) = (int)pixel_addr;

    FillBoxWithBitmap(hdc, rc.left, rc.top,  
            RECTW(rc), RECTH(rc), pbmp);

    //BitBlt(hdc, 0, 0, SCR_W, SCR_H, HDC_SCREEN, 0, 0, 0); 
    //while(1);
    ReleaseDC(hdc);
    DeleteCompatibleDC(mem_dc);
}

static int MemoEditProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    HDC  hdc;
    RECT rc;
    HWND hCtrl;
    HWND hEdit;

    switch (message) {
        case MSG_CREATE:
            if (!edit_res_load) {
                edit_res_load = TRUE; 
                LoadAllRes(edit_bmp_file, TABLESIZE(edit_bmp_file));
                LoadAllRes(delete_bmp_file, TABLESIZE(delete_bmp_file));
            }
            break;
        case MSG_INITDIALOG:
            EditCtrlInit(hDlg);
            SetEbButtonData(hDlg);
            SetDateTime(hDlg);
            hCtrl = GetDlgItem(hDlg, IDC_MEMODOWN);
            ShowWindow(hCtrl, SW_HIDE);
            hCtrl = GetDlgItem(hDlg, IDC_MEMONEW);
            ShowWindow(hCtrl, SW_SHOWNORMAL);
            hCtrl = GetDlgItem(hDlg, IDC_MEMOTITLE);
            SetWindowElementAttr (hCtrl, WE_FGC_WINDOW, C_TITLE_COLOR);
            SetBoldFont(hCtrl, &pedit_title_font);
            SetStaticCenter(hCtrl);
            List2EditAnimate(hDlg);
            return 0;
        case MSG_VSCROLL:
            if (wParam == SB_THUMBPOSITION) {
                _MG_DEBUG("thumb pos=%d\n", lParam);
            }
            break;
        case MSG_COMMAND:
            {
                switch (LOWORD(wParam)) {
                    case IDC_MEMOEDIT:
                        if (HIWORD(wParam) == EN_KILLFOCUS) {
                            //AnimateDownToNew(hDlg);
                        }
                        else if (HIWORD(wParam) == EN_SETFOCUS) {
                            //AnimateNewToDown(hDlg);
                        }
                        break;
                }
            }
            break;

        case BUTTON_KEYDOWN:
            break;
        case BUTTON_KEYUP:
            switch(wParam){
                case IDC_MEMONEW:
                    hEdit = GetDlgItem(hDlg, IDC_MEMOEDIT);
                    SetWindowText(hEdit, "");
                    SendMessage(hEdit, MSG_KILLFOCUS, 0, 0);
                    AnimateNewToDown(hDlg);
                    break;
                case IDC_MEMODOWN:
                    CloseIMEMethod(hDlg, FALSE);
                    AnimateDownToNew(hDlg);
                    break;
                case IDC_MEMONOTES:
                    {
                        CloseIMEMethod(hDlg, FALSE);
                        g_edit_return = TRUE;
                        GetBitmapFromDC (HDC_SCREEN, 0, 0, SCR_W, SCR_H, 
                                (PBITMAP)RetrieveRes(F_EDIT_SNOP));
#if 1
                        Edit2ListAnimate(hListWin);
                        DestroyMainWindow (hDlg);
                        MainWindowCleanup (hDlg);
                        WinMoveToUp(hListWin);
#else
                        DestroyMainWindow (hDlg);
                        MainWindowCleanup (hDlg);
                        DlgMemoList.controls = CtrlMemoList;
                        CreateMainWindowIndirectParam (&DlgMemoList,
                                HWND_DESKTOP, MemoListProc, 0);
#endif
                    }
                    break;
                case IDC_MEMOLAR:
                    RestoreEbbutonBk(hDlg, IDC_MEMOLAR);
                    GotoNextMemo(hDlg, FALSE);
                    break;
                case IDC_MEMORAR:
                    RestoreEbbutonBk(hDlg, IDC_MEMORAR);
                    GotoNextMemo(hDlg, TRUE);
                    break;
                case IDC_MEMOEMAIL:
                    RestoreEbbutonBk(hDlg, IDC_MEMOEMAIL);
                    break;
                case IDC_MEMOTRASH:
                    RestoreEbbutonBk(hDlg, IDC_MEMOTRASH);
                    GetBitmapFromDC (HDC_SCREEN, 0, DELETE_Y, DELETE_W, DELETE_H, 
                            (PBITMAP)RetrieveRes(F_DEL_ANIMATE_BK));
                    //DeleteDlgAnimateIn((PBITMAP)RetrieveRes(F_DEL_DIALOG));
                    DlgMemoDelete.controls = CtrlMemoDelete;
#if 0
                    CreateMainWindowIndirectParam (&DlgMemoDelete,
                            HWND_DESKTOP, MemoDeleteProc, 0);
#else
                    DialogBoxIndirectParam (&DlgMemoDelete, hDlg, MemoDeleteProc, 0L);
#endif
                    break;
            }
            break;
        case MSG_ERASEBKGND:
            {
                HDC hdc = (HDC)wParam;
                const RECT* clip = (const RECT*) lParam;
                BOOL fGetDC = FALSE;
                RECT rcTemp;
                if (hdc == 0) {
                    //hdc = GetClientDC (hDlg);
                    hdc    = GetSecondaryClientDC(hDlg);
                    fGetDC = TRUE;
                }    

                if (clip) {
                    rcTemp = *clip;
                    ScreenToClient (hDlg, &rcTemp.left, &rcTemp.top);
                    ScreenToClient (hDlg, &rcTemp.right, &rcTemp.bottom);
                    IncludeClipRect (hdc, &rcTemp);
                    //SelectClipRect (hdc, &rcTemp);
                }
                GetClientRect (hDlg, &rcTemp);

                hEdit = GetDlgItem (hDlg, IDC_MEMOEDIT);
                GetWindowRect(hEdit, &rc);
                //ExcludeClipRect(hdc, &rc);            

                FillBoxWithBitmap(hdc, 0, 0,  
                        RECTW(rcTemp), RECTH(rcTemp), RetrieveRes(F_MEMO_BK));

                /* draw hline. */
                FillBoxWithBitmap(hdc, 0, MEMO_TITLE_H+2, RECTW(rcTemp), 1, 
                        RetrieveRes(F_EDIT_HLINE));
                FillBoxWithBitmap(hdc, 0, MEMO_TITLE_H+4, RECTW(rcTemp), 1, 
                        RetrieveRes(F_EDIT_HLINE));
                /* draw vline. */
                FillBoxWithBitmap(hdc, EDIT_L_VLINE,   EDIT_T_VLINE, 1, RECTH(rcTemp), 
                        RetrieveRes(F_EDIT_VLINE));
                FillBoxWithBitmap(hdc, EDIT_L_VLINE+3, EDIT_T_VLINE, 1, RECTH(rcTemp), 
                        RetrieveRes(F_EDIT_VLINE));

                if (fGetDC) {
                    //ReleaseDC (hdc);
                    ReleaseSecondaryDC(hDlg, hdc);
                }
                return 0;
            }
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static void _init_create_info ( PMAINWINCREATE pCreateInfo )
{
    pCreateInfo->dwStyle   = WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "memo";
    pCreateInfo->hMenu   = 0;
    pCreateInfo->hCursor = GetSystemCursor ( 0 );
    pCreateInfo->hIcon   = 0;
    pCreateInfo->MainWindowProc = MemoListProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = SCR_W;
    pCreateInfo->by = SCR_H;
    pCreateInfo->iBkColor  = COLOR_lightgray;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting  = HWND_DESKTOP;
}

#define REGISTER_BMP(file, p) if (LoadBitmap(HDC_SCREEN, p, file)) { \
        _MG_DEBUG("load %s error!\n", file); \
    } \
    RegisterResFromBitmap(file, p);

#define UNREGISTER_BMP(file, p) UnloadBitmap(p); \
    UnregisterRes (file);

#if 0
static int GetResPos(char* file)
{
    int pos = 0;
    int num = TABLESIZE(bmp_file);

    for (pos = 0; pos < num; pos++) {
        if ( !strcmp(bmp_file[pos], file)) {
            return pos;
        }
    }
    return -1;
}

static void LoadAllRes(int type)
{
    int pos = 0;
    int i = 0;
    int num = TABLESIZE(bmp_file);
    PBITMAP pbmp = NULL;

    for (pos = 0; pos < num; pos++) {
        REGISTER_BMP(bmp_file[pos], &bmp_all[pos]);
    }

    num = TABLESIZE(dc_bmp_file);
    for (i = 0; i < num; i++) {
        RegisterResFromBitmap(dc_bmp_file[i], &bmp_all[i+pos]);
    }

    pbmp = (PBITMAP)RetrieveRes (F_LIST_ARROW);
    arrowlist[0] = pbmp;
    arrowlist[1] = pbmp;
    arrowlist[2] = pbmp;

}

static void UnLoadAllRes(void)
{
    int  i = 0;
    int  pos = 0;
    int  num = TABLESIZE(bmp_file);

    for (pos = 0; pos < num; pos++) {
        UNREGISTER_BMP(bmp_file[pos], &bmp_all[pos]);
    }

    num = TABLESIZE(dc_bmp_file);
    for (i = 0; i < num; i++) {
        UnregisterRes(dc_bmp_file[i]);
    }
}

#else

static void LoadAllRes(char** bmp_file, int num)
{
    int pos = 0;
    int i = 0;
    //int num = TABLESIZE(bmp_file);

    for (pos = 0; pos < num; pos++) {
        for (i = 0; i < ALL_BMP_NUM; i++) {
           if (!bmp_all[i].bmBitsPerPixel) {
               break;
           }
        }
        if (i < ALL_BMP_NUM) {
            REGISTER_BMP(bmp_file[pos], &bmp_all[i]);
        }
        else {
           DBGLOG1("load bmp file %d failed\n", bmp_file[pos]); 
        }
    }
}

static void LoadAllDCRes(void)
{
    int pos = 0;
    int i   = 0;
    int num = 0;

    num = TABLESIZE(dc_bmp_file);
    for (pos = 0; pos < num; pos++) {
        for (i = 0; i < ALL_BMP_NUM; i++) {
           if (!bmp_all[i].bmBitsPerPixel) {
               break;
           }
        }
        if (i < ALL_BMP_NUM) {
            RegisterResFromBitmap(dc_bmp_file[pos], &bmp_all[i]);
            bmp_all[i].bmBitsPerPixel = 16;
        }
        else {
           DBGLOG1("load bmp file %d failed\n", dc_bmp_file[pos]); 
        }
    }
}

#endif

static void UnLoadAllRes(void)
{
    int  pos = 0;
    int  num = TABLESIZE(list_bmp_file);

    for (pos = 0; pos < num; pos++) {
        UNREGISTER_BMP(list_bmp_file[pos], (PBITMAP)RetrieveRes(list_bmp_file[pos]));
    }

    num = TABLESIZE(edit_bmp_file);
    for (pos = 0; pos < num; pos++) {
        UNREGISTER_BMP(edit_bmp_file[pos], (PBITMAP)RetrieveRes(edit_bmp_file[pos]));
    }

    num = TABLESIZE(delete_bmp_file);
    for (pos = 0; pos < num; pos++) {
        UNREGISTER_BMP(delete_bmp_file[pos], (PBITMAP)RetrieveRes(delete_bmp_file[pos]));
    }

    num = TABLESIZE(dc_bmp_file);
    for (pos = 0; pos < num; pos++) {
        UNREGISTER_BMP(dc_bmp_file[pos], (PBITMAP)RetrieveRes(dc_bmp_file[pos]));
        //UnregisterRes(dc_bmp_file[i]);
    }
}


int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "memo" , 0 , 0);
#endif
    {
        char res_path[255];
        _MG_DEBUG("current path: %s,%s\n", getcwd(res_path, 255), argv[0]);
    }
	SetInterval(50);
    
    _init_create_info ( &CreateInfo );

    /* load resource and register ebctrl.*/
    g_buffdc = CreateCompatibleDCEx(HDC_SCREEN, SCR_W, SCR_H);
    g_bkdc = CreateCompatibleDCEx (HDC_SCREEN, SCR_W, SCR_H);
    LoadAllDCRes();
    RegisterMgdButton();
    RegisterEBListViewControl();

    listitem_data.nBKImage = (DWORD)RetrieveRes(F_LIST_BK);

    DlgMemoList.controls   = CtrlMemoList;
    hMainWnd = CreateMainWindowIndirectParam (&DlgMemoList,
            HWND_DESKTOP, MemoListProc, 0);

    hListWin = hMainWnd;

    ShowWindowUsingShareBuffer (hMainWnd);

    while (GetMessage (&Msg, hMainWnd)) {
        TranslateMessage (&Msg);
        DispatchMessage (&Msg);
    }
    MainWindowCleanup (hMainWnd);

    /* unload resource and unregister ex_ctrl.*/
    DestroyLogFont(plist_title_font);
    DestroyLogFont(pedit_title_font);
    DestroyLogFont(plist_eblist_font);
    EBListViewControlCleanup();
    UnregisterMgdButton();
    UnLoadAllRes();
    DeleteCompatibleDC(g_bkdc);
    DeleteCompatibleDC(g_buffdc);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif
