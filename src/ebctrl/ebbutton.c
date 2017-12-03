/*    
** mginit: a widget of button for mgdesktop.
** Copyright (C) 2002~2007  Feynman Software
** Current maintainer: WangMinggang.

** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.

** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.

** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "ebbutton.h"

typedef struct _MGD_BUTTON_DATA
{
    DWORD     dwStyle;
    BITMAP*   pBitmap;
    int       nDrawLength;
    int       nType;
}MGD_BUTTON_DATA;
typedef MGD_BUTTON_DATA* MGD_BUTTON_DATA_PT;


static int MgdButtonProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC     hdc;
    RECT   rect;
    static int var;
    gal_pixel  ret;
    BITMAP *skin;
    
    DWORD Style;// = GetWindowStyle (hwnd);
    GetClientRect (hwnd, &rect);

    switch (message) 
    {
        case MSG_CREATE:
             {
                 MGD_BUTTON_DATA_PT pButtonData = NULL;
                 
                 pButtonData = (MGD_BUTTON_DATA_PT) malloc (sizeof(MGD_BUTTON_DATA));

                 if (pButtonData == NULL)
                 {
                    printf ("Create MGD_BUTTON_DATA error!\n");
                    return 1;
                 }

                 skin =(BITMAP *)GetWindowAdditionalData (hwnd);
                 Style = GetWindowStyle (hwnd);
                 if (skin && (Style & BMP_COLORKEY))
                 {
                     skin->bmType = BMP_TYPE_COLORKEY;
                     skin->bmColorKey = GetPixelInBitmap (skin, 0, 0);
                 }

                 pButtonData->pBitmap = skin;
                 pButtonData->dwStyle = GetWindowStyle (hwnd);
                 if (pButtonData->dwStyle & MGDBUTTON_ANTISTATE)
                 {
                   pButtonData->nType = 2;
                   pButtonData->nDrawLength = rect.right * (pButtonData->nType);
                 }
                 else
                 {
                   pButtonData->nType = GetWindowAdditionalData2 (hwnd);
                   pButtonData->nDrawLength = rect.right * (pButtonData->nType);
                 }
                 SetWindowAdditionalData2 (hwnd, (DWORD)pButtonData);
             }
             break;
        case BUTTON_UPDATEBMP:
             {
                 MGD_BUTTON_DATA_PT pData = (MGD_BUTTON_DATA_PT) GetWindowAdditionalData2 (hwnd);
                 pData->pBitmap = (BITMAP*)GetWindowAdditionalData(hwnd);
                 pData->nType = (int) wParam;
                 pData->nDrawLength = (int) lParam;
             }
             return 0;
        case MSG_LBUTTONDOWN:
             {
                 MGD_BUTTON_DATA_PT pData = (MGD_BUTTON_DATA_PT) GetWindowAdditionalData2 (hwnd);

                 skin = pData->pBitmap;

                 var = rect.right*2;
                 // SetWindowAdditionalData2 (hwnd, var);

                 /* if only one bitmap.*/
                 if (skin && (rect.right * 2 > skin->bmWidth)){
                   ret = GetPixelInBitmap (skin, 0, 0);
                 }
                 else if (skin)
                   ret = GetPixelInBitmap (skin, LOWORD (lParam)*(skin->bmHeight)/rect.bottom, 
                       HIWORD (lParam)*skin->bmWidth/(4*rect.right));
                 else
                   ret = 1;
#if 1 
                 if (pData->dwStyle & MGDBUTTON_ANTISTATE)
                 {
                   pData->nType = 0;
                   pData->nDrawLength = 0; 
                 }
                 else
                 {
#endif
                   pData->nType = 2;
                   pData->nDrawLength = rect.right * 2; 

                 }

                 //InvalidateRect (hwnd, NULL, FALSE);
                 SendMessage (hwnd, MSG_PAINT, 0, 0);
                 PostMessage (GetParent (hwnd), BUTTON_KEYDOWN, GetDlgCtrlID (hwnd), 0);
             }
             return 0;
        case MSG_MOUSEMOVEIN:
             {
             MGD_BUTTON_DATA_PT pData = (MGD_BUTTON_DATA_PT) GetWindowAdditionalData2 (hwnd);
             if (!(pData->dwStyle & MGDBUTTON_2STATE))
             {
                 if (wParam)
                 {
                     if (pData->dwStyle & IRREGULAR)
                         break;
                     pData->nType = 1;
                     pData->nDrawLength = rect.right;
                 }
                 else
                 {
                     if (pData->dwStyle & IRREGULAR)
                         break;
                     pData->nType = 0;
                     pData->nDrawLength = 0;
                 }
                 pData->nType = 1;
                 //SetWindowAdditionalData2 (hwnd, var);
                 UpdateWindow (hwnd, FALSE);
             }
             }
             PostMessage (GetParent (hwnd), BUTTON_TIP, hwnd, wParam);
             break;
        case BUTTON_NORMAL:
             {
             MGD_BUTTON_DATA_PT pData = (MGD_BUTTON_DATA_PT) GetWindowAdditionalData2 (hwnd);
#if 0             
             pData->nType = 0;
             pData->nDrawLength = 0;
             //SetWindowAdditionalData2 (hwnd, var);
#endif

              if (pData->dwStyle & MGDBUTTON_ANTISTATE)
              {
                 pData->nType = 2;
                 pData->nDrawLength = rect.right * 2; 
              }
              else
              {
                 pData->nType = 0;
                 pData->nDrawLength = 0; 
              }
              UpdateWindow (hwnd, TRUE);
             }
             return 0;
        case BUTTON_ANTISTATE:
             {
                MGD_BUTTON_DATA_PT pData = (MGD_BUTTON_DATA_PT) GetWindowAdditionalData2 (hwnd);
                if (pData->dwStyle & MGDBUTTON_ANTISTATE) 
                   pData->dwStyle = pData->dwStyle & (~MGDBUTTON_ANTISTATE);
                else
                   pData->dwStyle = pData->dwStyle | MGDBUTTON_ANTISTATE;
             }
             return 0;
        case MSG_LBUTTONUP:
             {
                 MGD_BUTTON_DATA_PT pData = (MGD_BUTTON_DATA_PT) GetWindowAdditionalData2 (hwnd);
                 if (pData->dwStyle & IRREGULAR)
                 {
                     SendMessage (GetParent (hwnd), BUTTON_KEYUP, GetDlgCtrlID (hwnd), hwnd);
                     break;
                 }

                 if (pData->dwStyle & MGDBUTTON_2STATE)
                 {
                   if (pData->dwStyle & MGDBUTTON_ANTISTATE)
                   {
                     pData->nType = 2;
                     pData->nDrawLength = rect.right * 2; 
                   }
                   else
                   {
                     pData->nType = 0;
                     pData->nDrawLength = 0; 
                   }
                 }
                 else
                 {
                     pData->nType = 1;
                     pData->nDrawLength = rect.right;
                 }
                 //var =rect.right;
                 //SetWindowAdditionalData2 (hwnd, var);
                 UpdateWindow (hwnd, FALSE);
             }
             PostMessage (GetParent (hwnd), BUTTON_KEYUP, GetDlgCtrlID (hwnd), 0);
             break;
        case MSG_DESTROY:
             {
             //UnloadBitmap (skin);
                 MGD_BUTTON_DATA_PT pData = (MGD_BUTTON_DATA_PT) GetWindowAdditionalData2 (hwnd);
                 free (pData);
             }
             break;
        case MSG_PAINT:
             {
               MGD_BUTTON_DATA_PT pData = (MGD_BUTTON_DATA_PT) GetWindowAdditionalData2 (hwnd);
               hdc = BeginPaint (hwnd);
               var = pData->nDrawLength;
               skin = pData->pBitmap;
#if 0
               if ((pData->dwStyle & MGDBUTTON_MASK) == MGDBUTTON_ANTISTATE)
               {
                   if (pData->nType == ) 
               }
#endif
               if (skin){
                 /* if only one bitmap.*/
                 if (rect.right * 2 > skin->bmWidth){
                   FillBoxWithBitmap (hdc, 0, 0, rect.right, rect.bottom, skin);
                 }
                 else
                   FillBoxWithBitmapPart (hdc, 0, 0, rect.right, rect.bottom, rect.right*4, rect.bottom, skin, var, 0);
               }
               else
                 DefaultControlProc (hwnd, message, wParam, lParam);

               EndPaint (hwnd,hdc);
             }
            return 0;
    }

    return DefaultControlProc (hwnd, message, wParam, lParam);
}

 BOOL RegisterMgdButton (void)
{
    WNDCLASS MyClass;

    MyClass.spClassName = MGD_BUTTON;
    MyClass.dwStyle     = WS_NONE;
    MyClass.dwExStyle   = WS_EX_NONE|WS_EX_TRANSPARENT;
    MyClass.hCursor     = GetSystemCursor (IDC_ARROW);
    MyClass.iBkColor    = COLOR_lightwhite ;
    MyClass.WinProc     = MgdButtonProc;
    return RegisterWindowClass (&MyClass);
}

 void UnregisterMgdButton (void)
{
    UnregisterWindowClass (MGD_BUTTON);
}

