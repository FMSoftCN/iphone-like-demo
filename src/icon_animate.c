/*
 * animate obj for icon
 *
 *
 */

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "phone_icon.h"
#include "timer.h"

typedef struct _icon_animate_t{
	ANIMATE_INTF *intf;
	PHONE_ICON *icon;
}icon_animate_t;

static BOOL ia_is_end(icon_animate_t* ia)
{
    int movement = ia->icon->movement;
    if (movement & ICON_MOVE ||
        movement & ICON_SWAY ||
        movement & ICON_ZOOMOUT ||
        movement & ICON_ZOOMIN)
	{
        return FALSE;
	}
    return TRUE;
}

static void ia_show_cur_frame(icon_animate_t* ia, HDC hdc)
{
	draw_icon(hdc, ia->icon);
}

static void ia_next_frame(icon_animate_t* ia)
{
	icon_next_frame(ia->icon);
}

static BOOL ia_equal(icon_animate_t* ia, PHONE_ICON* icon)
{
	return ia->icon == icon;
}

static ANIMATE_INTF icon_animate_intf = {
	ia_is_end,
	ia_equal,
	ia_show_cur_frame,
	ia_next_frame,
	free
};

extern animate_obj_link* _animate_priorities[4];
extern ANIMATE_BKCLEAN* _animate_bkclean;
ANIMATE_OBJ* SearchIconAnimate (PHONE_ICON* icon)
{
	int i;
	for(i=0; i<sizeof(_animate_priorities)/sizeof(animate_obj_link*); i++)
	{
		
		animate_obj_link* lk = (animate_obj_link*)_animate_priorities[i];
		while(lk)
		{
			if(((icon_animate_t*)(&lk->obj))->icon == icon)
                return lk->obj;
            lk = lk->next;
		}
	}

	return NULL;
}

ANIMATE_OBJ* CreateIconAnimate(PHONE_ICON *icon)
{
	icon_animate_t * ia = (icon_animate_t*) malloc(sizeof(icon_animate_t));
	ia->intf = &icon_animate_intf;
	ia->icon = icon;
	return (ANIMATE_OBJ*)ia;
}

///////////////////////////////////////////////////////////////
//animation for switch bitmap
//

extern HWND g_hMainWnd;

extern BOOL is_proc_active(pid_t pid);

static BOOL bs_is_end(icon_animate_t* ia)
{
    if (ia->icon->bs_type == BMP_ZOOMIN) {
        
        if ((ia->icon->bs_cur_w > ia->icon->bs_scale_w) || 
                (ia->icon->bs_cur_h > ia->icon->bs_scale_h)) {
           return TRUE;
        }
    }
    else if (ia->icon->bs_type == BMP_ZOOMOUT)
	{
        if ((ia->icon->bs_cur_w < ia->icon->bs_scale_w) || 
                (ia->icon->bs_cur_h < ia->icon->bs_scale_h)) {
            //ShowWindow (g_hMainWnd, SW_SHOW);
            return TRUE;
 
        }
	}
     return FALSE;
}

static void bs_show_cur_frame(icon_animate_t* ia, HDC hdc)
{
/*	printf("expand size=%d,%d,%d,%d\n",
		ia->icon->bs_x>>16, ia->icon->bs_y>>16,
        ia->icon->bs_cur_w>>16, ia->icon->bs_cur_h>>16);

  */
	if((ia->icon->bs_cur_w>>16) <= 0 
		|| (ia->icon->bs_cur_h>>16) <= 0)
		return;
	FillBoxWithBitmap(hdc, ia->icon->bs_x>>16, ia->icon->bs_y>>16,
        ia->icon->bs_cur_w>>16, ia->icon->bs_cur_h>>16, 
        ia->icon->bs_type==BMP_ZOOMIN?&ia->icon->bmpSwitch:&ia->icon->bmpSwitchOut);
}

static void bs_next_frame(icon_animate_t* ia)
{
    if (ia->icon->bs_type == BMP_ZOOMIN) {
        ia->icon->bs_cur_w += ia->icon->bs_step_w;
        ia->icon->bs_cur_h += ia->icon->bs_step_h;
        ia->icon->bs_x -= (ia->icon->bs_step_w>>1);
        ia->icon->bs_y -= (ia->icon->bs_step_h>>1);
    }
    else if (ia->icon->bs_type == BMP_ZOOMOUT) {
        ia->icon->bs_cur_w -= ia->icon->bs_step_w;
        ia->icon->bs_cur_h -= ia->icon->bs_step_h;
        ia->icon->bs_x += (ia->icon->bs_step_w>>1);
        ia->icon->bs_y += (ia->icon->bs_step_h>>1);
    }
}

static ANIMATE_INTF bmp_switch_animate_intf={
	bs_is_end,
	ia_equal,
	bs_show_cur_frame,
	bs_next_frame,
	free
};

// the switch animation must slower then the icons animation
#define SWITCH_FRAME 10

ANIMATE_OBJ* CreateBmpSwitchAnimate(PHONE_ICON *icon, int scale_w, int scale_h, int cur_w, int cur_h, int type)
{
	icon_animate_t * ia = (icon_animate_t*) malloc(sizeof(icon_animate_t));

	ia->intf = &bmp_switch_animate_intf;
	ia->icon = icon;

    ia->icon->bs_scale_w = scale_w<<16;
    ia->icon->bs_scale_h = scale_h<<16;
    ia->icon->bs_type = type; 

    if (type == BMP_ZOOMIN) {
        ia->icon->bs_step_w = ((scale_w - cur_w)<<16)/SWITCH_FRAME;
        ia->icon->bs_step_h = ((scale_h - cur_h)<<16)/SWITCH_FRAME;
        ia->icon->bs_x = (g_rcScr.right>>1)<<16;
        ia->icon->bs_y = (g_rcScr.bottom>>1)<<16;
    }
    else if (type == BMP_ZOOMOUT) {
		GetBitmapFromDC(HDC_SCREEN,0,0,g_rcScr.right,g_rcScr.bottom, &ia->icon->bmpSwitchOut);	
        ia->icon->bs_step_w = ((cur_w - scale_w)<<16)/SWITCH_FRAME;
        ia->icon->bs_step_h = ((cur_h - scale_h)<<16)/SWITCH_FRAME;
		ia->icon->bs_x = 0;
        ia->icon->bs_y = 0;
     }

	if(cur_w == 0 && cur_h == 0)
	{
		bs_next_frame(ia);
	}
	else {
	    ia->icon->bs_cur_w = cur_w<<16;
   		ia->icon->bs_cur_h = cur_h<<16;
	}

	return ia;
}
