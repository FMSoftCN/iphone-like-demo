/*    
** mginit:mgd_button control program.
** Copyright (C) 2002~2007  Feynman Software
** Current maintainer: WangMiniggang.

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

#ifndef MGD_BUTTON_H
#define MGD_BUTTON_H  

#define MGD_BUTTON      "MGD_BUTTON"

#define BUTTON_KEYDOWN         MSG_USER+4
#define BUTTON_KEYUP           MSG_USER+5
#define BUTTON_NORMAL          MSG_USER+6
#define BUTTON_TIP             MSG_USER+12
#define MSG_SET_MGD_TYPE       MSG_USER+7
#define BUTTON_ANTISTATE       MSG_USER+8
#define BUTTON_UPDATEBMP       MSG_USER+9

#define IRREGULAR              0x00000010
#define MGDBUTTON_2STATE       0x00000001
#define MGDBUTTON_3STATE       0x00000002
#define MGDBUTTON_ANTISTATE    0x00000004 
#define BMP_COLORKEY           0x00000008
#define MGDBUTTON_MASK         0xffffffff

extern BOOL  RegisterMgdButton  (void);
extern void  UnregisterMgdButton(void);
#endif
