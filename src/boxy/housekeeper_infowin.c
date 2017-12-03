/*
** $Id: infowin.c 221 2007-07-03 06:56:57Z xwyan $
**
** infowin.c: Info window module.
**
** Copyright (C) 2007 Feynman Software.
** All right reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
**
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "housekeeper.h"
#include <minigui/control.h>

#define INFOWIN_W 200 
//#define INFOWIN_H 124 
#define INFOWIN_H 150 

typedef struct tagBMPPARTS
{
    BITMAP* top_bmp;
    BITMAP* mid_bmp;
    BITMAP* btm_bmp;
    int     mid_nr;
}BMPPARTS;

typedef struct tagHELPMSGINFO {
    const char* title;
    const char* msg;
    const char* prompt_str;
    int nr_lines;

    int vis_lines;
    int start_line;

    RECT title_rc;
    RECT msg_rc;
    RECT prompt_rc;

    BMPPARTS* bmp_parts;
} HELPMSGINFO;


static int _infowin_proc (HWND hwnd, int message, WPARAM wparam, LPARAM lparam)
{
    
    HELPMSGINFO* info;
    DWORD old_text_color;

    info = (HELPMSGINFO*) GetWindowAdditionalData (hwnd);
    switch (message) {
        case MSG_INITDIALOG:
            {
                HWND h_focus = GetDlgDefPushButton (hwnd);
                if (h_focus)
                    SetFocus (h_focus);

                info = (HELPMSGINFO*)GetWindowAdditionalData(hwnd);
                SetWindowAdditionalData (hwnd, (DWORD)lparam);
                return 0;
            }

        case MSG_PAINT:
            {
                HDC hdc = BeginPaint (hwnd);
                RECT rc = info->msg_rc;

                rc.top -= info->start_line * GetSysFontHeight(SYSLOGFONT_WCHAR_DEF);
                SetBkMode (hdc, BM_TRANSPARENT);
                DrawText (hdc, info->msg, -1, &rc,
                        DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS);

                DrawText (hdc, info->title, -1, &(info->title_rc),
                        DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS);

                if (info->prompt_str)
                {
                    old_text_color = SetTextColor(hdc ,COLOR_blue);
                    DrawText (hdc, info->prompt_str, -1, &(info->prompt_rc),
                            DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS);
                    SetTextColor(hdc, old_text_color);
                }

                EndPaint (hwnd, hdc);
                return 0;
            }
        case MSG_KEYDOWN:
            {
                switch (wparam)
                {
                    case SCANCODE_CURSORBLOCKUP:
                        if (info->start_line > 0) 
                        {
                            info->start_line--;
                            InvalidateRect (hwnd, &info->msg_rc, TRUE);
                        }
                        return 0;
                    case SCANCODE_CURSORBLOCKDOWN:
                        if (info->start_line + info->vis_lines < info->nr_lines) 
                        {
                            info->start_line++;
                            InvalidateRect (hwnd, &info->msg_rc, TRUE);
                        }
                        return 0;

                    case SCANCODE_ESCAPE:
                        EndDialog (hwnd, IDCANCEL);
                        return 0;

                    case SCANCODE_ENTER:
                        EndDialog (hwnd, IDOK);
                        return 0;

                    case SCANCODE_CURSORBLOCKLEFT:
                        EndDialog (hwnd, IDYES);
                        return 0;

                    case SCANCODE_CURSORBLOCKRIGHT:
                        EndDialog (hwnd, IDNO);
                        return 0;

                }
            } 
            break;

            case MSG_ERASEBKGND:
            {
                int i;
                int y;
                HDC hdc = (HDC)wparam;
                const RECT* clip = (const RECT*) lparam;
                BOOL f_get_dc = FALSE;
                RECT rc_temp;

                if (hdc == 0) {
                    hdc = GetClientDC (hwnd);
                    f_get_dc = TRUE;
                }       

                if (clip) {
                    rc_temp = *clip;
                    ScreenToClient (hwnd, &rc_temp.left, &rc_temp.top);
                    ScreenToClient (hwnd, &rc_temp.right, &rc_temp.bottom);
                    IncludeClipRect (hdc, &rc_temp);
                }

                FillBoxWithBitmap (hdc, 0, 0, 0, 0, info->bmp_parts->top_bmp);
                y = info->bmp_parts->top_bmp->bmHeight;
                for (i=0; i<info->bmp_parts->mid_nr; i++)
                {
                    FillBoxWithBitmap(hdc, 0, y, 0, 0, info->bmp_parts->mid_bmp);
                    y += info->bmp_parts->mid_bmp->bmHeight;
                }
                FillBoxWithBitmap(hdc, 0, y, 0, 0, info->bmp_parts->btm_bmp);

                if (f_get_dc)
                    ReleaseDC (hdc);
                return 0;
            }

        default:
            break;
    }

    return DefaultDialogProc (hwnd, message, wparam, lparam);
}


#define MB_MARGIN    5
#define MB_TEXTW     200


static void _infowin_get_box_xy (HWND h_parent_wnd, DWORD dw_style, DLGTEMPLATE* infowin_data)
{
    RECT rc_temp;

    if (dw_style & MB_BASEDONPARENT) 
    {
        GetWindowRect (h_parent_wnd, &rc_temp);
    }
    else 
    {
        rc_temp = g_rcDesktop;
    }

    switch (dw_style & MB_ALIGNMASK) 
    {
        case MB_ALIGNCENTER:
            infowin_data->x = rc_temp.left + (RECTW(rc_temp) - infowin_data->w)/2;
            infowin_data->y = rc_temp.top + (RECTH(rc_temp) - infowin_data->h)/2;
            break;

        case MB_ALIGNTOPLEFT:
            infowin_data->x = rc_temp.left;
            infowin_data->y = rc_temp.top;
            break;

        case MB_ALIGNBTMLEFT:
            infowin_data->x = rc_temp.left;
            infowin_data->y = rc_temp.bottom - infowin_data->h;
            break;

        case MB_ALIGNTOPRIGHT:
            infowin_data->x = rc_temp.right - infowin_data->w;
            infowin_data->y = rc_temp.top;
            break;

        case MB_ALIGNBTMRIGHT:
            infowin_data->x = rc_temp.right - infowin_data->w;
            infowin_data->y = rc_temp.bottom - infowin_data->h;
            break;
    }

    if ((infowin_data->x + infowin_data->w) > g_rcDesktop.right) 
    {
        infowin_data->x = g_rcDesktop.right - infowin_data->w;
    }

    if ((infowin_data->y + infowin_data->h) > g_rcDesktop.bottom) 
    {
        infowin_data->y = g_rcDesktop.bottom - infowin_data->h;
    }
}

int pmp_show_info(HWND h_parent_wnd, const char* title, const char* msg, DWORD dwStyle)
{
    int dialog_ret;
    DLGTEMPLATE infowin_data = 
    {
        WS_VISIBLE, 
        WS_EX_NONE, 0, 0, 0, 0, "", 0, 0, 0, NULL, 0L
    };

    CTRLDATA ctrl_data [1];

    /*background bitmaps*/
    BMPPARTS bmp_parts;

    HELPMSGINFO msg_info;

    RECT rc_text;
    int sum_text_height;

    /*set title, msg and prompt string(yes, no)*/
    msg_info.msg = msg;
    msg_info.title = title;
    if(dwStyle & MB_YESNO)
    {
        msg_info.prompt_str = "Yes(left)    No(right)";
    }
    else
    {
        msg_info.prompt_str = NULL;
    }

    rc_text.left = 0;
    rc_text.right = INFOWIN_W;
    rc_text.top = 0;
    rc_text.bottom = INFOWIN_H;
    
    /*set title rect*/
    SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_WCHAR_DEF));
    DrawText (HDC_SCREEN, msg_info.title ? msg_info.title : "information", -1, &rc_text, 
                    DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS | DT_CALCRECT); 

    /*horizontal mediate*/
    msg_info.title_rc.left = (INFOWIN_W - RECTW(rc_text)) >> 1;
    msg_info.title_rc.right = msg_info.title_rc.left + RECTW(rc_text);
    /*vertical top (reserve margin)*/
    msg_info.title_rc.top = MB_MARGIN;
    msg_info.title_rc.bottom = msg_info.title_rc.top + RECTH(rc_text);
    
    /*set icon in info-window*/
    infowin_data.controlnr = 1;
    
    if (dwStyle & MB_ICONMASK) {
        int id_icon = -1;

        ctrl_data [0].class_name= "static";
        ctrl_data [0].dwStyle   = WS_VISIBLE | SS_ICON | WS_GROUP;
        ctrl_data [0].dwExStyle = WS_EX_TRANSPARENT;

        /*static box under title_rc, at left of dialogbox*/
        ctrl_data [0].x         = MB_MARGIN;
        ctrl_data [0].y         = MB_MARGIN + msg_info.title_rc.bottom;
        ctrl_data [0].w         = 32;
        ctrl_data [0].h         = 32;
       
        ctrl_data [0].id        = IDC_STATIC;
        ctrl_data [0].caption   = "";

        /*confirm which icon is putted on the static box*/
        switch (dwStyle & MB_ICONMASK) {
            case MB_ICONHAND:
                id_icon = IDI_HAND;
                break;
            case MB_ICONINFORMATION:
                id_icon = IDI_INFORMATION;
                break;
            case MB_ICONEXCLAMATION:
                id_icon = IDI_EXCLAMATION;
                break;
            case MB_ICONQUESTION:
                id_icon = IDI_QUESTION;
                break;
        }

        /*set icon at the static box base on id_icon*/
        if (id_icon != -1) {
            ctrl_data [0].dwAddData = GetLargeSystemIcon (id_icon);
        }
    }


    /*set prompt_rc of msg_info*/
    if (dwStyle & MB_YESNO)
    {
        rc_text.left = MB_MARGIN;
        rc_text.right = INFOWIN_W - MB_MARGIN;
        rc_text.top = 0;
        rc_text.bottom = INFOWIN_H - MB_MARGIN;

        SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_WCHAR_DEF));

        DrawText (HDC_SCREEN, msg_info.prompt_str, -1, &rc_text, 
                DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS | DT_CALCRECT);

        /*horizontal mediate*/
        msg_info.prompt_rc.left = (INFOWIN_W - RECTW(rc_text)) >> 1;
        msg_info.prompt_rc.right = msg_info.prompt_rc.left + RECTW(rc_text);
        /*vertical bottom (reserve margin)*/
        msg_info.prompt_rc.bottom = INFOWIN_H - (MB_MARGIN << 1);
        msg_info.prompt_rc.top = msg_info.prompt_rc.bottom - RECTH(rc_text);

    }
    else /*have not prompt*/
    {
        msg_info.prompt_rc.left = 0;
        msg_info.prompt_rc.right = 0;
        msg_info.prompt_rc.top = INFOWIN_H - MB_MARGIN;
        msg_info.prompt_rc.right = INFOWIN_H - MB_MARGIN;
    }

    /*set msg_rc of msg_info*/

    /*msg_rc is at right of icon*/
    rc_text.left = ctrl_data[0].w + (MB_MARGIN << 1);
    rc_text.right = INFOWIN_W - (MB_MARGIN << 1);
    
    /*msg_rc is under title_rc, and on the prompt_rc*/
    rc_text.top  = msg_info.title_rc.bottom + MB_MARGIN;
    rc_text.bottom = msg_info.prompt_rc.top - MB_MARGIN;

    SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_WCHAR_DEF));
    sum_text_height = DrawText (HDC_SCREEN, msg_info.msg, -1, &rc_text, 
            DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS | DT_CALCRECT);

    rc_text.bottom = msg_info.prompt_rc.top - MB_MARGIN;
    msg_info.msg_rc = rc_text;
    

    /*set sum line number, visible line number, and start line*/
    msg_info.nr_lines = sum_text_height / (GetSysFontHeight(SYSLOGFONT_WCHAR_DEF));
    msg_info.vis_lines = RECTH(rc_text)/(GetSysFontHeight(SYSLOGFONT_WCHAR_DEF));
    msg_info.start_line = 0;

    infowin_data.w = INFOWIN_W;
    infowin_data.h = INFOWIN_H;
    _infowin_get_box_xy (h_parent_wnd, dwStyle, &infowin_data);

    infowin_data.controls = ctrl_data;

    /*bmp_parts.top_bmp = pmp_image_res_acquire_bitmap("/res/menu/menu_captbg.gif");
    bmp_parts.mid_bmp = pmp_image_res_acquire_bitmap("/res/menu/menu_mid.gif");
    bmp_parts.btm_bmp = pmp_image_res_acquire_bitmap("/res/menu/menu_btm.gif");*/

    bmp_parts.top_bmp = (BITMAP *)RetrieveRes (housekeeper_pic[7]);
    bmp_parts.mid_bmp = (BITMAP *)RetrieveRes (housekeeper_pic[8]);
    bmp_parts.btm_bmp = (BITMAP *)RetrieveRes (housekeeper_pic[9]);

    bmp_parts.mid_nr = (INFOWIN_H - bmp_parts.top_bmp->bmHeight - bmp_parts.btm_bmp->bmHeight)
                    / bmp_parts.mid_bmp->bmHeight;

    msg_info.bmp_parts = &bmp_parts;
    
    infowin_data.dwAddData = (DWORD)&bmp_parts;
    dialog_ret =  DialogBoxIndirectParam (&infowin_data, h_parent_wnd, _infowin_proc, 
           (LPARAM)&msg_info);
    /*UnregisterRes (worm_menu_pic[1]);
    UnregisterRes (worm_menu_pic[2]);
    UnregisterRes (worm_menu_pic[3]);
    pmp_image_res_release_bitmap("/res/menu/menu_captbg.gif", bmp_parts.top_bmp);
    pmp_image_res_release_bitmap("/res/menu/menu_mid.gif", bmp_parts.mid_bmp);
    pmp_image_res_release_bitmap("/res/menu/menu_btm.gif", bmp_parts.btm_bmp);*/

    return dialog_ret;

}

