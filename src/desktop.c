
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "desktop.h"

#define THIS(context)  DESKTOP_CONTEXT* this = (DESKTOP_CONTEXT*)(context)

static DESKTOP_CONTEXT* _this;

static void * dsk_init(void)
{
    if (!_this)
        _this = (DESKTOP_CONTEXT*) calloc(1, sizeof(DESKTOP_CONTEXT));
	return _this;
}

static void dsk_deinit(void *context)
{
	free(context);
}

static void dsk_paint_desktop(void * context, HDC dc_desktop, const RECT* inv_rc)
{
	THIS(context);

	if(inv_rc == NULL)
		inv_rc = &g_rcScr;

	if(this && this->bmp_bkgnd)
	{
		FillBoxWithBitmap(dc_desktop, inv_rc->left, inv_rc->top, RECTWP(inv_rc), RECTHP(inv_rc), this->bmp_bkgnd);
	}
	else {
		SetBrushColor(dc_desktop, 
			GetWindowElementPixel (HWND_DESKTOP, WE_BGC_DESKTOP));
		FillBox(dc_desktop,inv_rc->left, inv_rc->top, RECTWP(inv_rc), RECTHP(inv_rc));
	}
}

static void dsk_keyboard_handler(void* context, int message, 
	WPARAM wParam, LPARAM lParam)
{
}

static void dsk_mouse_handler(void* context, int message,
	WPARAM wParam, LPARAM lParam)
{
}

static void dsk_customize_desktop_menu (void* context, HMENU hmnu, int start_pos)
{
}

static void this_desktop_menucmd_handler (void* context, int id)
{
}

DESKTOPOPS mg_dsk_ops = {
	dsk_init,
	dsk_deinit,
	dsk_paint_desktop,
	dsk_keyboard_handler,
	dsk_mouse_handler,
	dsk_customize_desktop_menu,
	this_desktop_menucmd_handler
};
//SetCustomDesktopOperationSet

void SetDesktopBkgnd(PBITMAP bmpbk)
{
	if(_this){
		_this->bmp_bkgnd = bmpbk;
	}
}
