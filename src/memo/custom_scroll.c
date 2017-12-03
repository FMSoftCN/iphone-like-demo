/*
** $Id: custom_scroll.c 224 2007-07-03 09:38:24Z houhuihua
**
** custom_scroll.c: minigui 3.0 demo's memo.
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
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

//#include "common.h"
#include "memo.h"

extern WINDOW_ELEMENT_RENDERER* current_rdr;

WINDOW_ELEMENT_RENDERER* get_window_renderer (HWND hWnd)
{
    const WINDOWINFO* wininfo = GetWindowInfo (hWnd);
    const WINDOW_ELEMENT_RENDERER* renderer = wininfo->we_rdr; 

    if (renderer)
        return (WINDOW_ELEMENT_RENDERER*)renderer;
    return NULL;
}

static int calc_scrollbarctrl_area(HWND hWnd, int sb_pos, PRECT prc)
{
    PSCROLLBARDATA pScData = (PSCROLLBARDATA) GetWindowAdditionalData2 (hWnd);
    GetClientRect (hWnd, prc);
    
    switch (sb_pos)
    {
        case HT_HSCROLL:
        case HT_VSCROLL:
                break;
        case HT_SB_LEFTARROW:
            {
                prc->right = prc->left + pScData->arrowLen;
                break;
            }
        case HT_SB_UPARROW:
            {
                prc->bottom = prc->top + pScData->arrowLen;
                break;
            }
        case HT_SB_RIGHTARROW:
            {
                prc->left = prc->right - pScData->arrowLen;
                break;
            }
        case HT_SB_DOWNARROW:
            {
                prc->top = prc->bottom - pScData->arrowLen;
                break;
            }
        case HT_SB_HTHUMB:
            {
                prc->left = pScData->barStart + pScData->arrowLen;
                prc->right = prc->left + pScData->barLen;
                break;
            }
        case HT_SB_VTHUMB:
            {
                prc->top = pScData->barStart + pScData->arrowLen;
                prc->bottom = prc->top + pScData->barLen;
                break;
            }
        default:
            return -1;
    }
    if(0 >= prc->right - prc->left || 0 >= prc->bottom - prc->top)
        return -1;
  
    return 0;
}

static int get_scroll_status (HWND hWnd, BOOL isVert)
{
    int sb_status; 
    const WINDOWINFO  *info;
    
    if (0 == strncasecmp(CTRL_SCROLLBAR, GetClassName(hWnd), strlen(CTRL_SCROLLBAR)))
    {
        sb_status = ((PSCROLLBARDATA)GetWindowAdditionalData2(hWnd))->status;
    }
    else
    {
        info = GetWindowInfo (hWnd);
        if(isVert)
            sb_status = info->vscroll.status; 
        else
            sb_status = info->hscroll.status; 
    }
   return sb_status; 
}

/* draw_scrollbar:
 *   This function draw the scrollbar of a window. 
 *
 * param hWnd : the handle of the window.
 * param hdc : the DC of the window.
 * param sb_pos : the pos need to draw.
 *
 * Author: wangjian<wangjian@minigui.org>
 * Date: 2007-11-22
 */
void draw_scrollbar (HWND hWnd, HDC hdc, int sb_pos)
{
    RECT rect;
    int sb_status = 0;
    int bn_status = 0;
    DWORD color_3d, fgc_3d, fgc_dis;
    gal_pixel old_brush_color;
    BOOL isCtrl = FALSE; /* if TRUE it is scrollbar control else not */
    const WINDOWINFO  *info = 
        (WINDOWINFO*)GetWindowInfo (hWnd);
        
    color_3d = GetWindowElementAttr(hWnd, WE_MAINC_THREED_BODY);
    fgc_3d = GetWindowElementAttr(hWnd, WE_FGC_THREED_BODY);
    fgc_dis = GetWindowElementAttr(hWnd, WE_FGC_DISABLED_ITEM);
    
    if (0 == strncasecmp(CTRL_SCROLLBAR, GetClassName(hWnd), strlen(CTRL_SCROLLBAR)))
    {
        isCtrl = TRUE;
    }
    
    if (isCtrl)
    {
        if (0 != calc_scrollbarctrl_area(hWnd, sb_pos, &rect))
            return;
    }
    else
    { 
           /** draw the rect between H and V */
           if(current_rdr->calc_we_area(hWnd, HT_HSCROLL, &rect) != -1)
           {
               if(info->dwStyle & WS_VSCROLL)
               {
                   if (info->dwExStyle & WS_EX_LEFTSCROLLBAR) 
                   {
                       rect.right = rect.left;
                       rect.left = rect.right - RECTH(rect);
                   }
                   else 
                   {
                       rect.left = rect.right;
                       rect.right = rect.left + RECTH(rect);
                   }
                   old_brush_color = SetBrushColor(hdc, 
                           RGBA2Pixel(hdc,GetRValue(color_3d),
                               GetGValue(color_3d), GetBValue(color_3d), 
                               GetAValue(color_3d)));
                   FillBox(hdc, rect.left, rect.top, RECTW(rect), RECTH(rect));
                   SetBrushColor(hdc, old_brush_color);
               }
           }

           if (sb_pos != 0)
           {
               if (0 != current_rdr->calc_we_area(hWnd, sb_pos, &rect)) 
                   return;
           }
           else
           {
               current_rdr->draw_scrollbar (hWnd, hdc, HT_HSCROLL);
               current_rdr->draw_scrollbar (hWnd, hdc, HT_VSCROLL);
               return;
           }
    }

    switch(sb_pos)
    {
        case HT_HSCROLL:       // paint the hscrollbar
            {
                if (!isCtrl || !(info->dwStyle & SBS_NOSHAFT))
                { 
                    static BITMAP scroll_bmp = {0};
                    static BOOL first_time = FALSE;
                    old_brush_color = SetBrushColor(hdc, 
                            RGBA2Pixel(hdc,GetRValue(color_3d),
                                GetGValue(color_3d), GetBValue(color_3d), 
                                GetAValue(color_3d)));
                    if (!first_time){
                        ClientToScreen(hWnd, &rect.left,  &rect.top);
                        ClientToScreen(hWnd, &rect.right, &rect.bottom);
                        GetBitmapFromDC(HDC_SCREEN, rect.left, rect.top, 
                                RECTW(rect), RECTH(rect), &scroll_bmp);
                        first_time = TRUE;
                    }

                    //FillBox(hdc, rect.left, rect.top, RECTW(rect), RECTH(rect));
                    //FillBoxWithBitmap(hdc, rect.left, rect.top, 
                    //        RECTW(rect), RECTH(rect), &scroll_bmp);
                    SetBrushColor(hdc, old_brush_color);
                }
                //draw_scrollbar(hWnd, hdc, HT_SB_LEFTARROW);
                //draw_scrollbar(hWnd, hdc, HT_SB_RIGHTARROW);
                current_rdr->draw_scrollbar(hWnd, hdc, HT_SB_HTHUMB);
                break;
            }

        case HT_VSCROLL:         // paint the vscrollbar
            {
                if (!isCtrl || !(info->dwStyle & SBS_NOSHAFT))
                { 
                    static BITMAP scroll_bmp = {0};
                    static BOOL first_time = FALSE;
                    old_brush_color = SetBrushColor(hdc, 
                            RGBA2Pixel(hdc,GetRValue(color_3d),
                            GetGValue(color_3d), GetBValue(color_3d), 
                            GetAValue(color_3d)));
                    if (!first_time){
                        ClientToScreen(hWnd, &rect.left,  &rect.top);
                        ClientToScreen(hWnd, &rect.right, &rect.bottom);
                        GetBitmapFromDC(HDC_SCREEN, rect.left, rect.top, 
                                RECTW(rect), RECTH(rect), &scroll_bmp);
                        //first_time = TRUE;
                    }
                    //FillBox(hdc, rect.left, rect.top, RECTW(rect), RECTH(rect));
                    ScreenToClient(hWnd, &rect.left,  &rect.top);
                    ScreenToClient(hWnd, &rect.right, &rect.bottom);
                    FillBoxWithBitmap(hdc, rect.left, rect.top, 
                            RECTW(rect), RECTH(rect), RetrieveRes(F_SCROLL_BK));
                    _MG_DEBUG("scroll:l=%d,t=%d,w=%d,h=%d\n", rect.left, rect.top, RECTW(rect), RECTH(rect));
                    SetBrushColor(hdc, old_brush_color);
                }
                //draw_scrollbar(hWnd, hdc, HT_SB_UPARROW);
                //draw_scrollbar(hWnd, hdc, HT_SB_DOWNARROW);
                current_rdr->draw_scrollbar(hWnd, hdc, HT_SB_VTHUMB);
                break;
            }

#if 0
        case HT_SB_LEFTARROW:
            {
                sb_status = get_scroll_status(hWnd, FALSE);
                if (sb_status & SBS_DISABLED_LTUP)
                    bn_status |= LFRDR_BTN_STATUS_DISABLED;
                else
                {
                    if(sb_status & SBS_PRESSED_LTUP)
                        bn_status |= LFRDR_BTN_STATUS_PRESSED;
                    else if(sb_status & SBS_HILITE_LTUP)
                        bn_status |= LFRDR_BTN_STATUS_HILITE;
                }

                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);
                
                if(sb_status & SBS_DISABLED_LTUP || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, LFRDR_ARROW_LEFT);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, LFRDR_ARROW_LEFT);
                break;
            }

        case HT_SB_RIGHTARROW:
            {
                sb_status = get_scroll_status(hWnd, FALSE);
                if (sb_status & SBS_DISABLED_BTDN) 
                    bn_status |= LFRDR_BTN_STATUS_DISABLED;
                else
                {
                    if(sb_status & SBS_PRESSED_BTDN)
                        bn_status |= LFRDR_BTN_STATUS_PRESSED;
                    else if(sb_status & SBS_HILITE_BTDN)
                        bn_status |= LFRDR_BTN_STATUS_HILITE;
                }
                
                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);

                if(sb_status & SBS_DISABLED_BTDN || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, LFRDR_ARROW_RIGHT);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, LFRDR_ARROW_RIGHT);
                break;
            }
#endif

        case HT_SB_HTHUMB:
            { 
                sb_status = get_scroll_status(hWnd, FALSE);
                if(sb_status & SBS_PRESSED_THUMB)
                    bn_status |= LFRDR_BTN_STATUS_PRESSED;
                else if(sb_status & SBS_HILITE_THUMB)
                    bn_status |= LFRDR_BTN_STATUS_HILITE;
                 
                current_rdr->draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);
                break;
            }
#if 0
        case HT_SB_UPARROW:
            {
                sb_status = get_scroll_status(hWnd, TRUE);
                if (sb_status & SBS_DISABLED_LTUP)
                    bn_status |= LFRDR_BTN_STATUS_DISABLED;
                else
                {
                    if(sb_status & SBS_PRESSED_LTUP)
                        bn_status |= LFRDR_BTN_STATUS_PRESSED;
                    else if(sb_status & SBS_HILITE_LTUP)
                        bn_status |= LFRDR_BTN_STATUS_HILITE;
                }

                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);

                if(sb_status & SBS_DISABLED_LTUP || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, LFRDR_ARROW_UP);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, LFRDR_ARROW_UP);
                break;
            }

        case HT_SB_DOWNARROW:
            {
                sb_status = get_scroll_status(hWnd, TRUE);
                if (sb_status & SBS_DISABLED_BTDN)
                    bn_status |= LFRDR_BTN_STATUS_DISABLED;
                else
                {
                    if(sb_status & SBS_PRESSED_BTDN)
                        bn_status |= LFRDR_BTN_STATUS_PRESSED;
                    else if(sb_status & SBS_HILITE_BTDN)
                        bn_status |= LFRDR_BTN_STATUS_HILITE;
                }
                
                draw_3dbox(hdc, &rect, color_3d, 
                        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);

                if(sb_status & SBS_DISABLED_BTDN || sb_status & SBS_DISABLED)
                    draw_arrow(hWnd, hdc, &rect, fgc_dis, LFRDR_ARROW_DOWN);
                else
                    draw_arrow(hWnd, hdc, &rect, fgc_3d, LFRDR_ARROW_DOWN);
                break;
            }
#endif

        case HT_SB_VTHUMB: 
            {
                sb_status = get_scroll_status(hWnd, TRUE);
                if(sb_status & SBS_PRESSED_THUMB)
                    bn_status |= LFRDR_BTN_STATUS_PRESSED;
                else if(sb_status & SBS_HILITE_THUMB)
                    bn_status |= LFRDR_BTN_STATUS_HILITE;

                FillBoxWithBitmap(hdc, rect.left, rect.top, 
                        RECTW(rect), RECTH(rect), RetrieveRes(F_SCROLL_BK));
                FillBoxWithBitmap(hdc, rect.left, rect.top, 
                        RECTW(rect), RECTH(rect), RetrieveRes(F_SCROLL_FG));
                //current_rdr->draw_3dbox(hdc, &rect, color_3d, 
                //        bn_status | LFRDR_3DBOX_THICKFRAME | LFRDR_3DBOX_FILLED);
                break;
            } 
        default:
            return;
    }
}
