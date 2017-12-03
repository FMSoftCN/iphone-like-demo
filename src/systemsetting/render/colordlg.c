/*
 * colordlg.c
 *
 * color select dialog with non-regular window.
 *
 * wangjian<wangjian@minigui.org>
 *
 * 2008-8-15
 *
 */


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/fixedmath.h>

#include "colordlg.h"

#include "mask_pic.c"

#define MASK_PIC    mask_pic_data      //* the mask bitmap 169x169x8bits*/

static POINT ptA = TRI_PT1;
static POINT ptB = TRI_PT2;
static POINT ptC = TRI_PT3;

#define MAX3(a,b,c)  ((a)>(b)?((a)>(c)?(a):(c)):((b)>(c)?(b):(c)))
#define MIN3(a,b,c)  ((a)<(b)?((a)<(c)?(a):(c)):((b)<(c)?(b):(c)))

static void _RGB2HSV(int r, int g, int b, int *hout, int *sout, int *vout)
{
	int max;
	int min;
	int delta;
	int h, s, v;

	max = MAX3(r, g, b);
	min = MIN3(r, g, b);
	delta = max - min;
	
	v = (max<<16)/255 * 100 >> 16;
		
	if(delta == 0){
		h = 0;
		s = 0;
	}else {
		int del_r, del_g, del_b, d;
		s = (((delta<<16) / max) * 100 ) >> 16 ;

		max <<= 16;
		r <<= 16;
		g <<= 16;
		b <<= 16;
		d = delta << 16;
		
		del_r = ((max - r) / 6 + d / 2) / delta;
		del_g = ((max - g) / 6 + d / 2) / delta;
		del_b = ((max - b) / 6 + d / 2) / delta;

		if( r == max)
			h = ((del_b - del_g) ) * 360 >> 16;
		else if( g== max)
			h = ((del_r - del_b + (1<<16) / 3 ) ) * 360 >> 16;
		else if( b == max)
			h = ((del_g - del_r + (2<<16) / 3 ) ) * 360 >> 16;

		if(h < 0) 
			h += 360;
		else if(h > 360)
			h -= 360;
	}
	
	*hout = h;
	*sout = s;
	*vout = v;

}

void RGB2HSV (Uint8 r, Uint8 g, Uint8 b, 
                Uint16 *hout, Uint8 *sout, Uint8 *vout)
{
	_RGB2HSV(r, g, b, (int*)hout, (int *)sout,(int *) vout);
}

#define SetRGB(r, g, b) do { (*rout)=((r)*256>>16);\
    (*gout)=((g)*256)>>16; (*bout)=((b)*256)>>16; }while(0)

static void _HSV2RGB(int h, int s, int v, int *rout, int *gout, int *bout)
{
	if( s == 0){
		*rout = (((v*255)<<16 ) / 100 ) >> 16;
		*gout = *bout = *rout;
	}else {
		int i;
		int m,n;
		int hr;

		i = h / 60;
		hr = h % 60;

		if(!(i&1)) 
			hr = 60 - hr;

		m = ((v*(100-s))<<16) / 10000;
		n = ((v*(6000-s*hr)/60)<<16) / 10000;
		v = (v<<16)/100;

		switch(i){
			case 6:
			case 0:
				SetRGB(v, n, m); break;
			case 1: SetRGB(n, v, m); break;
			case 2: SetRGB(m, v, n); break;
			case 3: SetRGB(m, n, v); break;
			case 4: SetRGB(n, m, v); break;
			case 5: SetRGB(v, m, n); break;
		}
	}

}

void HSV2RGB (Uint16 hin, Uint8 sin, Uint8 vin, 
                Uint8 *rout, Uint8 *gout, Uint8 *bout)
{
	int r, g, b;
	_HSV2RGB(hin, sin, vin, &r, &g, &b);
	*rout = (Uint8)r;
	*gout = (Uint8)g;
	*bout = (Uint8)b;
}

static fixed calc_arc (int cx, int cy, int x, int y, int x0, int y0)
{
    fixed arc1, arc2;

    if (y == cy) {
        if (x > cx)
            arc1 = 0;
        else
            arc1 = itofix (64 * 2);
    } else if (x == cx){
        if (y > cy)
            arc1 = itofix (-64);
        else
            arc1 = itofix (64); 
    } else {
        arc1 = fixatan2 (itofix(y - cy), itofix(x - cx));   
    }

    if (y0 == cy) {
        if (x0 > cx)
            arc2 = 0;
        else
            arc2 = itofix (64 * 2); 
    } else if (x0 == cx){
        if (y0 > cy)
            arc2 = itofix (-64);
        else
            arc2 = itofix (64);
    } else { 
        arc2 = fixatan2 (itofix (y0 - cy), itofix (x0 - cx)); 
    }

    return fixsub (arc1, arc2);
}

static void _x_line_by_s (HDC hdc, int x1, int x2, int y, 
        Uint16 h, Uint8 s1, Uint8 s2, Uint8 v)
{
    int _x, _del_x;
    Uint8 _s, _del_s;
    Uint8 r, g, b;
    gal_pixel pixel;

    _del_x = x2 -x1;
    if (_del_x == 0) return;

    _del_s = (s2 - s1);

    for (_x = x1, _s = s1; _x <= x2; _x++){
        _s = _del_s * (_x - x1) / _del_x;
        HSV2RGB (h, _s, v, &r, &g, &b);
        SetPixel(hdc, _x, y, RGB2Pixel(hdc, r, g, b));
    } 
}

static void _y_line_by_s (HDC hdc, int x, int y1, int y2, 
        Uint16 h, Uint8 s1, Uint8 s2, Uint8 v)
{
    int _y, _del_y;
    Uint8 _s, _del_s;
    Uint8 r, g, b;
    gal_pixel pixel;

    _del_y = y2 -y1;
    if (_del_y == 0) return;

    _del_s = (s2 - s1);

    for (_y = y1, _s = s1; _y <= y2; _y++){
        _s = _del_s * (_y - y1) / _del_y;
        HSV2RGB (h, _s, v, &r, &g, &b);
        SetPixel(hdc, x, _y, RGB2Pixel(hdc, r, g, b));
    } 
}

static int fill_h_triangle (HDC hdc, POINT ap, POINT bp1, POINT bp2, Uint16 h)
{ 
    int x1, x2, y1, y2;
    int xdelta, ydelta;
    int xinc, yinc;
    int rem;
    
    Uint8 s;
    Uint8 v;

    if(bp1.y != bp2.y && bp1.x != bp2.x) return -1;        

    x1 = ap.x; 
    y1 = ap.y; 
    x2 = bp1.x;
    y2 = bp1.y; 

    xdelta = x2 - x1;
    ydelta = y2 - y1;
    if (xdelta < 0) xdelta = -xdelta;
    if (ydelta < 0) ydelta = -ydelta;

    xinc = (x2 > x1) ? 1 : -1;
    yinc = (y2 > y1) ? 1 : -1;

    SetPixel (hdc, x1, y1, RGB2Pixel(hdc, 0, 0, 0));

    if (xdelta >= ydelta) 
    {
        rem = xdelta >> 1;
        while (x1 != x2) 
        {
            x1 += xinc;
            rem += ydelta;
            if (rem >= xdelta) 
            {
                rem -= xdelta;
                y1 += yinc;
            }
            v = (x1 - ap.x) * 100 / xdelta;
            if(bp1.y == bp2.y)
               _x_line_by_s (hdc, x1, x1+(ap.x-x1)*2, y1, h, 0, 100, v);
            else
               _y_line_by_s (hdc, x1, y1, y1+(ap.y-y1)*2, h, 0, 100, v);
        }
    } 
    else 
    {
        rem = ydelta >> 1;
        while (y1 != y2) 
        {
            y1 += yinc;
            rem += xdelta;
            if (rem >= ydelta) 
            {
                rem -= ydelta;
                x1 += xinc;
            }
            v = (y1 - ap.y) * 100 / ydelta;
            if(bp1.y == bp2.y)
               _x_line_by_s (hdc, x1, x1+(ap.x-x1)*2, y1, h, 0, 100, v);
            else
               _y_line_by_s (hdc, x1, y1, y1+(ap.y-y1)*2, h, 0, 100, v);
        }
    }
    
    return 0;
}


static void draw_triangle (HDC hdc, Uint16 h)
{
    fill_h_triangle (hdc, ptA, ptB, ptC, h);
}

static void _cb_circle (void *context, int x1, int x2, int y) 
{
    Uint16 _h;
    fixed arc;
    Uint8 r, g, b;
    PCCON pcon = (PCCON)context;
    
    SetPenWidth (pcon->hdc, 2);
    arc = calc_arc (CIRCLE_C_X, CIRCLE_C_Y, H_POS_X, H_POS_Y, x1, y);
    _h = fixtoi(arc) * 90 / 64 + pcon->arc;
    while (_h > 360){
        _h -= 360;
    }
/*    
    while (_h < 0){
        _h += 360;
    }
*/
    HSV2RGB (_h, 99, 99, &r, &g, &b);
    SetPenColor (pcon->hdc, RGB2Pixel (pcon->hdc, r, g, b));
    LineEx (pcon->hdc, pcon->cx, pcon->cy, x1, y);

    arc = calc_arc (CIRCLE_C_X, CIRCLE_C_Y, H_POS_X, H_POS_Y, x2, y);
    _h = fixtoi(arc) * 90 / 64 + pcon->arc;
    while (_h > 360){
        _h -= 360;
    }
/*    
    while (_h < 0){
        _h += 360;
    }
*/
    HSV2RGB (_h, 99, 99, &r, &g, &b);
    SetPenColor (pcon->hdc, RGB2Pixel (pcon->hdc, r, g, b));
    LineEx (pcon->hdc, pcon->cx, pcon->cy, x2, y);
}

static void draw_cirque (HDC hdc, int h_offset)
{
    CCON context;
    
    context.hdc = hdc;
    context.cx  = CIRCLE_C_X;
    context.cy  = CIRCLE_C_Y;
    context.arc = h_offset;
    
    CircleGenerator ((void *)(&context), CIRCLE_C_X, CIRCLE_C_Y, 
            CIRCLE_C_RW+2, _cb_circle);
}

static BOOL PtInCirle (int x, int y , int sx, int sy, int r1, int r2)
{
    int r = (x - sx) * (x - sx) + (y - sy) * (y - sy);
    
    if (r > r1 * r1 && r < r2 * r2) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static BOOL PtInTri (int x, int y, POINT pt1, POINT pt2, POINT pt3)
{
    double fa, fb, fc;
    
    if (pt1.x != pt2.x && pt1.y != pt2.y) {
        fa = (((y-pt1.y)*(pt2.x-pt1.x))-((x-pt1.x)*(pt2.y-pt1.y)))
            *(((pt3.y-pt1.y)*(pt2.x-pt1.x))-((pt3.x-pt1.x)*(pt2.y-pt1.y)));
    } else if (pt1.x == pt2.x && pt1.y != pt2.y){
        fa = (x - pt1.x) * (pt3.x - pt1.x);
    } else if (pt1.x != pt2.x && pt1.y == pt2.y){
        fa = (y - pt1.y) * (pt3.y - pt1.y);
    }

    if (pt3.x != pt2.x && pt3.y != pt2.y) {
        fb = (((y-pt2.y)*(pt3.x-pt2.x))-((x-pt2.x)*(pt3.y-pt2.y)))
            *(((pt1.y-pt2.y)*(pt3.x-pt2.x))-((pt1.x-pt2.x)*(pt3.y-pt2.y)));
    }else if (pt3.x == pt2.x && pt3.y != pt2.y) {
        fb = (x - pt2.x) * (pt1.x - pt2.x);
    }else if (pt3.x != pt2.x && pt3.y == pt2.y) {
        fb = (y - pt2.y) * (pt1.y - pt2.y);
    }

    if (pt3.x != pt1.x && pt3.y != pt1.y) {
        fc = (((y-pt3.y)*(pt1.x-pt3.x))-((x-pt3.x)*(pt1.y-pt3.y)))
            *(((pt2.y-pt3.y)*(pt1.x-pt3.x))-((pt2.x-pt3.x)*(pt1.y-pt3.y)));
    }else if (pt3.x == pt1.x && pt3.y != pt1.y) {
        fc = (x - pt3.x) * (pt2.x - pt3.x);
    }else if (pt3.x != pt1.x && pt3.y == pt1.y) {
        fc = (y - pt3.y) * (pt2.y - pt3.y);
    }
    
    if (fa < 0 || fb < 0 || fc < 0)
        return FALSE;
    else
        return TRUE;
}

static int ColorSelProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC     hdc;
    static HDC      mem_dc;
    static POINT    color_pos;
    static Uint16   sel_h;
    static MYBITMAP mask;

    switch (message) {
        case MSG_INITDIALOG:
            {
                int i;
                RGB pal[CL_PAL_LEN];

                sel_h       = 0;
                color_pos.x = CIRCLE_C_X;
                color_pos.y = CIRCLE_C_Y;

                hdc = GetDC (hWnd);
                mem_dc = CreateCompatibleDC (hdc);
                ReleaseDC (hdc);

                LoadMyBitmapFromMem (&mask, pal, 
                        MASK_PIC, sizeof (MASK_PIC), "bmp");
                for (i = 0; i < CL_PAL_LEN; ++i) {
                    if(pal[i].r == CL_MASK_R && pal[i].g == CL_MASK_G 
                            &&  pal[i].b == CL_MASK_B) {
                        mask.transparent = i;
                        break;
                    }
                }
                if (!SetWindowMask (hWnd, &mask)){
                    return -1;
                }

                draw_cirque (mem_dc, sel_h);
                draw_triangle (mem_dc, sel_h);

                SetWindowAdditionalData (hWnd, lParam);

                break;
            }
        case MSG_LBUTTONUP:
            {
                gal_pixel pixel;
                int x = LOSWORD (lParam);
                int y = HISWORD (lParam);

                COLORRET * color = (COLORRET *)GetWindowAdditionalData (hWnd);

                if (PtInTri (x, y, ptA, ptB, ptC)) {
                    pixel = GetPixel (mem_dc, color_pos.x, color_pos.y);

                    Pixel2RGB (mem_dc, pixel, &color->r, 
                            &color->g, &color->b);
                    RGB2HSV ( color->r, color->g,color-> b, 
                            &color->h, &color->s, &color->v);
                    /*
                       printf ("write the color :\nr--%d, g--%d, b-- %d,\n 
                       h--%d, s--%d, v--%d \n",
                       color->r, color->g, color->b, 
                       color->h, color->s, color->v);
                       */
                    EndDialog (hWnd, IDOK);
                }  
                break;
            }
        case MSG_LBUTTONDOWN:
            {
                fixed arc;
                int x = LOSWORD (lParam);
                int y = HISWORD (lParam);

                if (PtInCirle (x, y, CIRCLE_C_X, CIRCLE_C_Y, 
                            CIRCLE_C_RN, CIRCLE_C_RW)) {
                    arc = calc_arc (CIRCLE_C_X, CIRCLE_C_Y, 
                            H_POS_X, H_POS_Y, x, y);
                    sel_h += fixtoi (arc) * 90 / 64;
                    while (sel_h > 360){
                        sel_h -=360;
                    }
                    /*
                    while (sel_h < 0){
                        sel_h += 360;
                    }
                    */
                    
                    draw_cirque (mem_dc, sel_h);
                    draw_triangle (mem_dc, sel_h);

                    InvalidateRect (hWnd, NULL, FALSE);
                } 
                else if (PtInTri (x, y, ptA, ptB, ptC)) {
                    color_pos.x = x;
                    color_pos.y = y;
                    
                    draw_triangle (mem_dc, sel_h);

                    InvalidateRect (hWnd, NULL, FALSE);
                }                 
                break;
            }
        case MSG_PAINT:
            {
                hdc = BeginPaint (hWnd);

                BitBlt (mem_dc, 0, 0, 0, 0, hdc, 0, 0, 0);

                SetPenWidth (hdc, 3);
                SetPenColor (hdc, PIXEL_lightwhite);
                LineEx (hdc, ptC.x+1, ptC.y+1, 
                        2*H_POS_X - ptC.x, 2*H_POS_Y - ptC.y);

                SetPenWidth (hdc, 1);
                Circle (hdc, color_pos.x, color_pos.y, 2);

                EndPaint (hWnd, hdc);
                return 0;
            }

        case MSG_CLOSE:
            {
                EndDialog (hWnd, IDOK);
                break;
            }
        case MSG_DESTROY:
            {
                DeleteMemDC (mem_dc);
                UnloadMyBitmap (&mask);
                //DestroyMainWindow (hWnd);
                //PostQuitMessage (hWnd);
                return 0;
            }
    }

    return DefaultDialogProc (hWnd, message, wParam, lParam);
}

static DLGTEMPLATE ColorSelDlgDate =
{
    WS_NONE,
    WS_EX_NONE,
    0, 120, 200, 200,
    "Color Select Dialog",
    0, 0,
    0,
	NULL,
    0
};


void SelectColor (HWND hosting, COLORRET *color)
{
    DialogBoxIndirectParam (&ColorSelDlgDate, hosting, 
            ColorSelProc, (DWORD)color);
}


