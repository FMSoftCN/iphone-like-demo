#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "colordlg.h"
#include "../sysmain.h" 

typedef enum _EM_STYLE_TYPE
{
    EM_TYPE_CLASSIC = 1,
    EM_TYPE_FLAT,
    EM_TYPE_SKIN,
    EM_TYPE_NONE
}EM_STYLE_TYPE;

HWND CreateRenderWindow (void);

void SetWindowStyleType (EM_STYLE_TYPE emType);

#endif
