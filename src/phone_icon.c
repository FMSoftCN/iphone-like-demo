#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "phone_icon.h"

BOOL is_hitted_icon (PHONE_ICON* ph_icon, int x, int y)
{
	return  (x >= ph_icon->x && x <= (ph_icon->x + ph_icon->normal_bmp.bmWidth)) 
	&& (y >= ph_icon->y && y <= (ph_icon->y + ph_icon->normal_bmp.bmHeight));
}


