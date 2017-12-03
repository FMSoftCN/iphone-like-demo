/*
** $Id: menu.c 223 2007-07-03 09:37:59Z xwyan $
**
** menu.c: Menu for Feynman PMP solution.
**
** Copyright (C) 2007 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: liupeng .
*/

#include <string.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "worm.h"

#define PMP_MENU_CAPTION_H          12
#define PMP_MENU_TOP_MARGIN         (10+PMP_MENU_CAPTION_H)
#define PMP_MENU_BTM_MARGIN         10
#define PMP_MENU_LEFT_MARGIN        10
#define PMP_MENU_RIGHT_MARGIN       10

#define PMP_MENU_CAPTIONBG_H        20

#define PMP_MENU_WIDTH     200
#define PMP_MENU_LEFT_X    ((MAINWINDOW_W - PMP_MENU_WIDTH)/2)
#define PMP_MENU_LEFT_Y    50

#define MENU_BK_TOP_NAME       "/res/menu/menu_top.gif"
#define MENU_BK_CAPTBG_NAME    "/res/menu/menu_captbg.gif"
#define MENU_BK_MID_NAME       "/res/menu/menu_mid.gif"
#define MENU_BK_BTM_NAME       "/res/menu/menu_btm.gif" 

#define PMP_MENU_BUFFER_LEN    255
#define BITMAPBK
typedef struct _MENU_INFO {
    int cur_idx;
    int nr_items;
    PMP_MENU_ITEM *items;
    void *ctx;

#ifdef BITMAPBK
    BITMAP** bmp_bks;
#endif

} MENU_INFO;

#define PMP_MENU_SPACE_BTW_ITEMS    0

static void get_menu_item_rc (HWND hwnd, int idx, RECT* rc)
{
    int height = GetSysCharHeight () + PMP_MENU_SPACE_BTW_ITEMS;

    rc->left = PMP_MENU_LEFT_MARGIN;
    rc->right = PMP_MENU_WIDTH - PMP_MENU_RIGHT_MARGIN;
    rc->top = height * idx + PMP_MENU_TOP_MARGIN;
    rc->bottom = rc->top + height;
}

static int proc_menu_item_status (HWND hwnd, MENU_INFO* info, int status)
{
    int idx;
    RECT rc;

    switch (status) {
        case PMP_MI_ST_UPDATED:
            get_menu_item_rc (hwnd, info->cur_idx, &rc);
            InvalidateRect (hwnd, &rc, TRUE);
            break;

        case PMP_MI_ST_UPDATED_OTHERS:
            idx = 0;
            while (info->items [idx].id) {
                if (info->items [idx].dirty) {
                    get_menu_item_rc (hwnd, idx, &rc);
                    InvalidateRect (hwnd, &rc, TRUE);
                    info->items [idx].dirty = FALSE;
                }
                idx ++;
            }
            break;

        case PMP_MI_ST_SELECTED:
            return info->items [info->cur_idx].id;

        case PMP_MI_ST_NONE:
        default:
            /* do nothing */
            break;
    }

    return 0;
}

static int MenuProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC     hdc;
    char    buf[PMP_MENU_BUFFER_LEN];    
    RECT    rc;
    MENU_INFO* info = (MENU_INFO*)GetWindowAdditionalData (hWnd);

    switch (message)
    {
        case MSG_INITDIALOG:
            return 1;

        case MSG_KEYDOWN: {
            switch (wParam) {
                case SCANCODE_CURSORBLOCKUP:
                    if (info->cur_idx > 0) {
                        get_menu_item_rc (hWnd, info->cur_idx, &rc);
                        InvalidateRect (hWnd, &rc, TRUE);

                        info->cur_idx --;
                        get_menu_item_rc (hWnd, info->cur_idx, &rc);
                        InvalidateRect (hWnd, &rc, TRUE);
                    }
                    break;
                
                case SCANCODE_CURSORBLOCKDOWN:
                    if (info->cur_idx < info->nr_items - 1) {
                        get_menu_item_rc (hWnd, info->cur_idx, &rc);
                        InvalidateRect (hWnd, &rc, TRUE);

                        info->cur_idx ++;
                        get_menu_item_rc (hWnd, info->cur_idx, &rc);
                        InvalidateRect (hWnd, &rc, TRUE);
                    }
                    break;

                case SCANCODE_CURSORBLOCKLEFT:
                    proc_menu_item_status (hWnd, info, 
                            info->items [info->cur_idx].cb_menu_item (info->ctx, 
                                    info->items [info->cur_idx].id, PMP_MI_OP_PREV, buf));
                    break;

                case SCANCODE_CURSORBLOCKRIGHT:
                    proc_menu_item_status (hWnd, info, 
                            info->items [info->cur_idx].cb_menu_item (info->ctx, 
                                    info->items [info->cur_idx].id, PMP_MI_OP_NEXT, buf));
                    break;

                case SCANCODE_ENTER:
                    if (proc_menu_item_status (hWnd, info, 
                            info->items [info->cur_idx].cb_menu_item (info->ctx, 
                                    info->items [info->cur_idx].id, PMP_MI_OP_ENTER, buf))) {
                        EndDialog (hWnd, info->items [info->cur_idx].id);
                    }
                    break;
                    
                case SCANCODE_ESCAPE:
                    proc_menu_item_status (hWnd, info, 
                            info->items [info->cur_idx].cb_menu_item (info->ctx, 
                                    info->items [info->cur_idx].id, PMP_MI_OP_CANCEL, buf));
                    EndDialog (hWnd, 0);
                    break;
            }
            break;
        }

        case MSG_PAINT: {
            int idx = 0;
            RECT rc;
            const char *str_after_tab;

            hdc = BeginPaint(hWnd);
            SetBkMode(hdc, BM_TRANSPARENT);

            while (info->items [idx].id) {

                info->items [idx].cb_menu_item (info->ctx, info->items [idx].id, 
                    PMP_MI_OP_DEF, buf);
                info->items [idx].dirty = FALSE;
                
                get_menu_item_rc (hWnd, idx, &rc);

                if (idx == info->cur_idx) { //draw selected bar
                    SetBrushColor (hdc, PIXEL_blue);
                    FillBox (hdc,  rc.left, rc.top, RECTW (rc), RECTH (rc));
                    SetTextColor (hdc, RGB2Pixel(hdc, 255, 255, 255) );
                }
                else
                    SetTextColor (hdc, RGB2Pixel(hdc, 0, 0, 0) );


                str_after_tab = strchr (buf, '\t');
                if (str_after_tab == NULL)
                    DrawText (hdc, buf, -1, &rc, DT_LEFT);
                else {
                    DrawText (hdc, buf, str_after_tab - buf, &rc, DT_LEFT);
                    DrawText (hdc, str_after_tab + 1, -1, &rc, DT_RIGHT);                   
                }

                idx++;
            }
            EndPaint(hWnd, hdc);
            
            return 0;
        }               

        case MSG_ERASEBKGND: {
            RECT rc;
            const RECT* clip = (const RECT*) lParam;
            BOOL fGetDC = FALSE;
            RECT rcTemp;

#ifndef BITMAPBK
            MG_RWops *data = NULL;
#endif

            int y = PMP_MENU_CAPTIONBG_H;
            hdc = (HDC)wParam;
    
            hdc = (HDC)wParam;            
            if (hdc == 0) {
                hdc = GetClientDC (hWnd);
                fGetDC = TRUE;
            }

            if (clip) {
                rcTemp = *clip;
                ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
                ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
                IncludeClipRect (hdc, &rcTemp);
            }

            GetClientRect (hWnd, &rc);
#ifdef BITMAPBK
            FillBoxWithBitmap(hdc, 0, 0, 0, 0, info->bmp_bks[0]);
            FillBoxWithBitmap(hdc, 0, 2, 0, 0, info->bmp_bks[1]);

            while (y < rc.bottom-5)
            {
                FillBoxWithBitmap(hdc, 0, y, 0, 0, info->bmp_bks[2]);
                y += 5;
            }
            FillBoxWithBitmap(hdc, 0, rc.bottom-5, 0, 0, info->bmp_bks[3]);
#else
            
            /*data =  pmp_image_res_acquire_ds (MENU_BK_TOP_NAME);
            PaintImageEx (hdc, 0, 0, data, pmp_get_file_suffix (MENU_BK_TOP_NAME));
            pmp_image_res_release_ds (MENU_BK_TOP_NAME, data);

            data =  pmp_image_res_acquire_ds (MENU_BK_CAPTBG_NAME);
            PaintImageEx (hdc, 0, 2, data, pmp_get_file_suffix (MENU_BK_CAPTBG_NAME));
            pmp_image_res_release_ds (MENU_BK_CAPTBG_NAME, data);            

            while (y<(rc.bottom-5)) {
                data =  pmp_image_res_acquire_ds (MENU_BK_MID_NAME);                                            
                PaintImageEx (hdc, 0, y, data, pmp_get_file_suffix (MENU_BK_MID_NAME));
                y = y + 5;
                pmp_image_res_release_ds (MENU_BK_MID_NAME, data);                                                 
            }  
 
            data =  pmp_image_res_acquire_ds (MENU_BK_BTM_NAME);
            PaintImageEx (hdc, 0, rc.bottom-5, data, 
                pmp_get_file_suffix (MENU_BK_BTM_NAME));
            pmp_image_res_release_ds (MENU_BK_BTM_NAME, data);*/

#endif
            rc.left = 0;
            rc.right = PMP_MENU_WIDTH;
            rc.top = 3;
            rc.bottom = rc.right + GetSysCharHeight();
            
            SetBkMode(hdc, BM_TRANSPARENT);            
            DrawText (hdc, GetWindowCaption (hWnd), -1, &rc, DT_CENTER);
            
            if (fGetDC)
                ReleaseDC (hdc);
            
            return 0;
        }
    }

    return DefaultDialogProc(hWnd, message, wParam, lParam);
}

static int get_menu_item_count (PMP_MENU_ITEM *m)
{
    int n = 0;
    
    while (m->id>0) {
        n++;
        m->dirty = FALSE;
        m++;
    }        
    return n;
}


int pmp_show_menu (HWND parent, const char* title, PMP_MENU_ITEM *items, 
            void *context)
{
#ifdef BITMAPBK
    MENU_INFO info = {0, get_menu_item_count (items), items, context, NULL};
#else
    MENU_INFO info = {0, get_menu_item_count (items), items, context};
#endif

    DLGTEMPLATE _menu_win = {
        WS_VISIBLE,
        WS_EX_NONE,
        PMP_MENU_LEFT_X, PMP_MENU_LEFT_Y,
        PMP_MENU_WIDTH, 170,
        title,  /* caption */
        0, /* icon */
        0, /* menu */
        0, /* number of controls */
        NULL, 
        (DWORD)&info,
    };

    int dlg_rtn, i, j;

    _menu_win.h = GetSysCharHeight()*info.nr_items + 
                PMP_MENU_TOP_MARGIN + PMP_MENU_BTM_MARGIN;
    _menu_win.y = (g_rcScr.bottom - _menu_win.h)/2;

#ifdef BITMAPBK
    BITMAP* bmp_bks[4];
    bmp_bks[0] = (BITMAP *)RetrieveRes (worm_pic[6]);
    bmp_bks[1] = (BITMAP *)RetrieveRes (worm_pic[7]);
    bmp_bks[2] = (BITMAP *)RetrieveRes (worm_pic[8]);
    bmp_bks[3] = (BITMAP *)RetrieveRes (worm_pic[9]);

#if 0
    bmp_bks[0] = pmp_image_res_acquire_bitmap (MENU_BK_TOP_NAME);
    bmp_bks[1] = pmp_image_res_acquire_bitmap (MENU_BK_CAPTBG_NAME);
    bmp_bks[2] = pmp_image_res_acquire_bitmap (MENU_BK_MID_NAME);
    bmp_bks[3] = pmp_image_res_acquire_bitmap (MENU_BK_BTM_NAME);
#endif
    info.bmp_bks = bmp_bks;
#endif

    dlg_rtn = DialogBoxIndirectParam (&_menu_win, parent, MenuProc, (LPARAM)0);

#ifdef BITMAPBK
#if 0
    pmp_image_res_release_bitmap (MENU_BK_TOP_NAME, bmp_bks[0]);
    pmp_image_res_release_bitmap (MENU_BK_CAPTBG_NAME, bmp_bks[1]);
    pmp_image_res_release_bitmap (MENU_BK_MID_NAME, bmp_bks[2]);
    pmp_image_res_release_bitmap (MENU_BK_BTM_NAME, bmp_bks[3]);
#endif
#endif

    return dlg_rtn;
}

