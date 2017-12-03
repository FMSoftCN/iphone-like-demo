/*
** $Id: calc.c 225 2007-07-03 10:02:36Z wangxuguang $
**
** calc.c:  Implement the GUI of normal and science calc.  
**
** Copyright (C) 2006 ~ 2007 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Wang Xuguang.
**
** Create date: 2006/11/17
*/

#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>

#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "calc_type.h"
#include "calc_size.h"
#include "calc_btn_code.h"
#include "common.h"
#include "../sharedbuff.h"

#define IDC_STATIC_DP	200
#define IDC_CHK_INV		202
#define IDC_CHK_HYP		203
#define IDC_BTN_BASE	204
#define IDC_BTN_ANG		205
//#define IDC_BTN_UNIT	206
//#define IDC_BTN_EXIT	207

#define CALC_COMPACT    1
#define CALC_SCIEN      2

#define CALC_ROW		5
#define CALC_COL        4
#define CALC_ROW_SC     7
#define CALC_COL_SC     6

char calc_strdisp[32];//FIXME ADD

static int      g_lastKeyId = 0;
static BOOL     g_selectMr = FALSE;
static BOOL     g_mButton = FALSE;
static BOOL     g_lButtonDown = FALSE;

static BITMAP   bmp_num;
static BITMAP   bmpBk;
static BITMAP   bmpBkClick;

static HDC      memDC;

RECT    calc_rcArrow;
static  int g_bHeight, g_bWidth, g_bBorderT, g_bBorderB, g_beginX, g_beginY, g_row, g_col;
static  int g_curRow, g_curCol;
static  BOOL g_curIsctrl;
//static HWND  hChkInv, hChkHyp, hBtnAng, hBtnBase, hBtnExit;

int         calc_pre_btnid = -1;
int         calc_Status = CALC_COMPACT;
Num_Base    calc_cur_base = BASE_DEC;
Angle_Type  calc_cur_angle = ANG_DEG;
BOOL        calc_hyp_mode = FALSE, calc_inv_mode = FALSE, calc_percent_mod = FALSE;

extern  int display_error;
extern  DIGIT_STACK digit_stack;
extern  FLOAT last_opnd;
extern  FLOAT dispnow;
int     calc_last_optr;

extern void InputProc (HWND hwnd, int btnid);
extern void InitCalculator (void);
extern void FreeStack (void);
extern int  longint_bin (char*, long, int);
extern BOOL digit_stack_empty (void);
extern void clear_digit_stack (void);
extern void clearzero (char*);


/*caption in buttions of simple calculator*/
static char *Caption[CALC_ROW][CALC_COL] = {
    {"m+", "m-", "mr/mc", "/"}, 
    {"7",  "8",  "9",     "x"}, 
    {"4",  "5",  "6",     "+"}, 
    {"1",  "2",  "3",     "-"}, 
    {"0",  ".",  "C",     "="} 
};
/*caption in buttons of scientific calculator*/
static char *Caption_Sc[CALC_ROW_SC][CALC_COL_SC] = {
    {"BS", "CE", "AC", "", "", ""}, 
    {"M+", "MS", "MR", "MC", "", ""}, 
    {"And", "Or", "Not", "Lsh", "Log", "Ln"},
    {"Sin", "Cos", "Tan", "/", "^/", "x^2"}, 
    {"D", "E", "F", "x", "%", "x^y"}, 
    {"A", "B", "C", "-", "1/x", "n!"}, 
    {"(", ")", "=", "+", "+/-", "Mod"} 
};
/*button IDs of simple calculator*/
static int BtId[CALC_ROW][CALC_COL] = {
    {CODE_MPLUS, CODE_MMINUS, CODE_MR, CODE_DIVIDE},
    {CODE_7,     CODE_8,      CODE_9,  CODE_MULTIPLY},
    {CODE_4,     CODE_5,      CODE_6,  CODE_ADD},
    {CODE_1,     CODE_2,      CODE_3,  CODE_SUBTRACT},
    {CODE_0,     CODE_POINT,  CODE_AC, CODE_EQ}    
};
/*button IDs of scientific calculator*/
static int BtId_Sc[CALC_ROW_SC][CALC_COL_SC] = {
    {CODE_BS, CODE_CE, CODE_AC, 0, 0, 0},
    {CODE_MPLUS, CODE_MS, CODE_MR, CODE_MC, 0, 0},
    {CODE_AND, CODE_OR, CODE_NOT, CODE_LSH, CODE_LOG, CODE_LN},
    {CODE_SIN, CODE_COS, CODE_TAN, CODE_DIVIDE, CODE_SQRT, CODE_SQ},
    {CODE_D, CODE_E, CODE_F, CODE_MULTIPLY, CODE_PERCENT, CODE_XY},
    {CODE_A, CODE_B, CODE_C, CODE_SUBTRACT, CODE_RECIP, CODE_FAC},
    {CODE_LEFTPAREN, CODE_RIGHTPAREN, CODE_EQ, CODE_ADD, CODE_PLUSMINUS, CODE_MOD}    
};

//****************** bitmap font for the showing numbers *****************/

#define MAX_COL  30
#define _set_value(Var, Value)    if (Var) {*Var = Value;}

typedef struct _DIGITS_BMP_INFO 
{
    BITMAP*     bmp_chars;
    int         nr_label_chars;
    const char* label_chars;
} DIGITS_BMP_INFO;

/* get bmp char's width and height, and total col and row */
static void get_char_bmp_size (const DIGITS_BMP_INFO *info, int *w, int *h, 
                int *col, int *row)
{
    int _w, _h, _col, _row;

    /* get total number of col and row */
    if (info->nr_label_chars > MAX_COL) {
        _col =  MAX_COL;
        _row = (info->nr_label_chars - 1) / MAX_COL + 1;
    }
    else {
        _col = info->nr_label_chars;
        _row = 1;
    }
    
    _w = info->bmp_chars->bmWidth / _col;
    _h = info->bmp_chars->bmHeight/ _row;
    
    _set_value (w, _w);
    _set_value (h, _h);
    _set_value (col, _col);
    _set_value (row, _row);
}

static int get_char_bmp_pos (const DIGITS_BMP_INFO* info, char ch, 
        int* x, int* y, int* w, int* h)
{
    int i;
    int row, col;

    /* searches for the ch */
    for (i = 0 ; info->label_chars[i] != '\0' ; i++) {
        if (info->label_chars[i] == ch)
            break;
    }
    if (i >= info->nr_label_chars)    /* not found */
        return 0;

    get_char_bmp_size (info, w, h, &col, &row);

    *x = (i % col) * (*w);
    *y = (i / col) * (*h);

    return 1;
}

static void display_bmp_label (HDC hdc, const DIGITS_BMP_INFO *info, 
                int x, int y, const char* label)
{
    int _x, _y, _w, _h;
    const char *p;

    if (!label) 
        return;
    
    p = label;
    while (p && *p != '\0') {
        if (get_char_bmp_pos (info, *p, &_x, &_y, &_w, &_h)) {
            FillBoxWithBitmapPart (hdc, x, y, _w, _h, 0, 0, 
                    info->bmp_chars, _x, _y);
            x += _w;
        }
        p++;
    }
}

static DIGITS_BMP_INFO digits_bmp_info = {NULL, 13, "0123456789.E "};

static int display_led_digits (HDC hdc, int x, int y, 
        const char* digits, PBITMAP bmp)
{

    if (digits_bmp_info.bmp_chars == NULL) {
        digits_bmp_info.bmp_chars = bmp;
#if 0
        digits_bmp_info.bmp_chars->bmType = BMP_TYPE_COLORKEY;
        digits_bmp_info.bmp_chars->bmColorKey = 
                GetPixelInBitmap (digits_bmp_info.bmp_chars, 0, 0);
#endif
    }

    if (digits_bmp_info.bmp_chars == NULL)
        return -1;
    
    display_bmp_label (hdc, &digits_bmp_info, x, y, digits);
    return 0;
}


inline void calc_InvalidDiaplayRect(HWND hwnd) 
{
    RECT rc;
    
    rc.left     = CALC_DISPLAY_L;
    rc.right    = CALC_DISPLAY_R;
    rc.top      = CALC_DISPLAY_T;
    rc.bottom   = CALC_DISPLAY_B;

    InvalidateRect(hwnd, &rc, TRUE);
}

static int GetIdFromPos (int cur_row, int cur_col)
{
    if (calc_Status == CALC_COMPACT) 
        return BtId[cur_row][cur_col];
    else if (calc_Status == CALC_SCIEN)
        return BtId_Sc[cur_row][cur_col];
    else 
        return -1;
}

static int GetIDCtrlFromPos(int cur_row, int cur_col)
{
	if (calc_Status == CALC_COMPACT)
		return 0;

	if (calc_Status == CALC_SCIEN)
	{
		switch (cur_row)
		{
			case 0:
			{
				switch(cur_col)
				{
			//		case 3:
			//			return IDC_BTN_EXIT;
					case 4:
						return IDC_CHK_INV;
					case 5:
						return IDC_CHK_HYP;
					default:
						return 0;
				}
			}
			case 1:
			{
				switch(cur_col)
				{
					case 4:
						return IDC_BTN_BASE;
					case 5:
						return IDC_BTN_ANG;
					default:
						return 0;
				}
			}

			default:
			{
				return 0;
			}
		}
	}

    return 0;
}

/*坐标》按钮上的字*/
static int GetCaptFromPos (int cur_row, int cur_col, char *caption)
{
    if (!caption)
        return -1;
    if (cur_row >= g_row || cur_col >= g_col) 
        return -1;

    if (calc_Status == CALC_COMPACT)
        strcpy (caption, Caption[cur_row][cur_col]);
    else if (calc_Status == CALC_SCIEN)
        strcpy (caption, Caption_Sc[cur_row][cur_col]);
    else 
        return -1;

    return 0;
}

static BOOL change_tab(lParam)
{
    int selectX = LOSWORD (lParam);
    int selectY = HISWORD (lParam);

    if (selectX > calc_rcArrow.left-3 && selectX < calc_rcArrow.right+3 
            && selectY > calc_rcArrow.top-3 && selectY < calc_rcArrow.bottom+3)
        return TRUE;

    return FALSE;
}

static BOOL validate_select(LPARAM lParam)
{
    int selectX = LOSWORD (lParam);
    int selectY = HISWORD (lParam);

    if (selectX < g_beginX || selectY < g_beginY)
        return FALSE;
    
    int offY = (selectY - g_beginY) % g_bHeight;
    int offX = (selectX - g_beginX) % g_bWidth;
    
    if (offX < g_bBorderT || offX > (g_bWidth - g_bBorderT))
        return FALSE;
    if (offY < g_bBorderT || offY > (g_bHeight - g_bBorderB))
        return FALSE;

    int selectRow = (selectY - g_beginY) / g_bHeight;
    int selectCol = (selectX - g_beginX) / g_bWidth;

    if (selectRow >= g_row || selectCol >= g_col)
        return FALSE;

    int selId = GetIdFromPos (selectRow, selectCol);
    int selCtrlId = GetIDCtrlFromPos (selectRow, selectCol);

    if (!selId && !selCtrlId)
        return FALSE;

    if (selCtrlId == IDC_CHK_INV|| selCtrlId == IDC_CHK_HYP
            || selCtrlId == IDC_BTN_BASE|| selCtrlId == IDC_BTN_ANG )
        return FALSE;
 	   
    return TRUE;
}

static void mouse_select(LPARAM lParam)
{
    int selectX = LOSWORD (lParam);
    int selectY = HISWORD (lParam);

    g_curRow = (selectY - g_beginY) / g_bHeight;
    g_curCol = (selectX - g_beginX) / g_bWidth;

	/*certain it is control or not*/
	if (GetIDCtrlFromPos(g_curRow, g_curCol) > 0)
	{
		g_curIsctrl = TRUE;
	}
	else
	{
		g_curIsctrl = FALSE;
	}

}
#if 0
static void move_up(void)
{
	/*up, while is not butten and is not control*/
	do
	{
		g_curRow = (g_curRow - 1 + g_row) % g_row;
	}
	while(GetIdFromPos(g_curRow, g_curCol)==0 && 
			GetIDCtrlFromPos(g_curRow, g_curCol)==0);

	/*certain it is control or not*/
	if (GetIDCtrlFromPos(g_curRow, g_curCol) > 0)
	{
		g_curIsctrl = TRUE;
	}
	else
	{
		g_curIsctrl = FALSE;
	}

}


static void move_down(void)
{
	/*down, while is not butten and is not control*/
	do
	{
		g_curRow = (g_curRow + 1) % g_row;
	}
	while(GetIdFromPos(g_curRow, g_curCol)==0 && 
			GetIDCtrlFromPos(g_curRow, g_curCol)==0);

	/*certain it is control or not*/
	if (GetIDCtrlFromPos(g_curRow, g_curCol) > 0)
	{
		g_curIsctrl = TRUE;
	}
	else
	{
		g_curIsctrl = FALSE;
	}

}


static void move_left(void)
{
	/*left, while is not butten and is not control*/
	do
	{
		g_curCol = (g_curCol - 1 + g_col) % g_col;
	}
	while(GetIdFromPos(g_curRow, g_curCol)==0 && 
			GetIDCtrlFromPos(g_curRow, g_curCol)==0);

	/*certain it is control or not*/
	if (GetIDCtrlFromPos(g_curRow, g_curCol) > 0)
	{
		g_curIsctrl = TRUE;
	}
	else
	{
		g_curIsctrl = FALSE;
	}

}


static void move_right(void)
{
	/*up, while is not butten and is not control*/
	do
	{
		g_curCol = (g_curCol + 1) % g_col;
	}
	while(GetIdFromPos(g_curRow, g_curCol)==0 && 
			GetIDCtrlFromPos(g_curRow, g_curCol)==0);

	/*certain it is control or not*/
	if (GetIDCtrlFromPos(g_curRow, g_curCol) > 0)
	{
		g_curIsctrl = TRUE;
	}
	else
	{
		g_curIsctrl = FALSE;
	}

}

#endif

static int GetRectByPos (RECT *prcBtn, int cur_row, int cur_col)
{
    if (cur_row >= g_row || cur_col >= g_col) return -1;
    if (calc_Status == CALC_COMPACT) 
	{
        if (strcmp (Caption[cur_row][cur_col], "") == 0) return -1;
    }
    else if (calc_Status == CALC_SCIEN)
    {
        if (strcmp (Caption_Sc[cur_row][cur_col], "") == 0) return -1;
    }
#ifdef ENABLE_LANDSCAPE 
    prcBtn->top = g_beginY + g_bHeight * cur_row + g_bBorderT;
    prcBtn->left = g_beginX + g_bWidth * cur_col + g_bBorderT;

    prcBtn->bottom = prcBtn->top + g_bHeight - g_bBorderT - g_bBorderB;
    prcBtn->right = prcBtn->left + g_bWidth  - g_bBorderT - g_bBorderB;
#else
    prcBtn->top = g_beginY + g_bHeight * cur_row;
    prcBtn->left = g_beginX + g_bWidth * cur_col;

    prcBtn->bottom = prcBtn->top + g_bHeight;
    prcBtn->right = prcBtn->left + g_bWidth;
#endif
    return 0;
}

/*decide layout of calculatora*/
int DecideSize (void)
{
	/*simpule calculator*/
	if (calc_Status == CALC_COMPACT) {
        g_bHeight   = CALC_B_H;
        g_bWidth    = CALC_B_W;
        g_bBorderT  = CALC_B_BORDERT;
        g_bBorderB  = CALC_B_BORDERB;
        g_beginX    = CALC_BEGIN_X;
        g_beginY    = CALC_BEGIN_Y;
        g_row       = CALC_ROW;
        g_col       = CALC_COL;
    }
	/*scientific calculator*/
    else {
        g_bHeight   = B_H_SC;
        g_bWidth    = B_W_SC;
        g_bBorderT  = CALC_B_BORDERT;
        g_bBorderB  = CALC_B_BORDERB;
        g_beginX    = BEGIN_X_SC;
        g_beginY    = BEGIN_Y_SC;
        g_row       = CALC_ROW_SC;
        g_col       = CALC_COL_SC;
    }

    return 0;
}

/* 
 * TextOutCenter: Output string in the center of a rectangle.
 * Params       : hdc - the handle of  device context
 *                preText - the rectangle where the string will be outputed
 * Return       : void
 */ 
static void TextOutCenter (HDC hdc, RECT* prcText, char* pchText)
{
    int bkMode;
	/*output the string*/
    bkMode = SetBkMode (hdc, BM_TRANSPARENT);
    //if (pchText) TextOut (hdc, x, y, pchText);
    if (pchText) DrawText (hdc, pchText, -1, prcText, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_NOCLIP);
    SetBkMode (hdc, bkMode);
}
#if 0
/*
 * DrawButton: draw the button. 
 *			  called when the button is pressed and loosen
 * Params	: hdc - handle of device context
 *			  prcBtn - the point of button rectangle
 *			  status - the button is pressed or loosen
 * return	: void
 */
static void  DrawButton (HDC hdc, RECT *prcBtn, int status)
{
    int w = 2;
    SetPenColor (hdc , PIXEL_black);    

    MoveTo (hdc, prcBtn->left + w, prcBtn->top);
    LineTo (hdc, prcBtn->right - w, prcBtn->top);
    LineTo (hdc, prcBtn->right, prcBtn->top + w);
    LineTo (hdc, prcBtn->right, prcBtn->bottom - w);
    LineTo (hdc, prcBtn->right - w, prcBtn->bottom);
    LineTo (hdc, prcBtn->left + w, prcBtn->bottom);
    LineTo (hdc, prcBtn->left, prcBtn->bottom - w);
    LineTo (hdc, prcBtn->left, prcBtn->top + w);
    LineTo (hdc, prcBtn->left + w, prcBtn->top);

    if (status == 1) { //pressed
        MoveTo (hdc, prcBtn->left + 1, prcBtn->bottom - w);
        LineTo (hdc, prcBtn->left + 1, prcBtn->top + w);
        LineTo (hdc, prcBtn->left + w, prcBtn->top + 1);
        LineTo (hdc, prcBtn->right - w + 1, prcBtn->top + 1);
    }
    else if (status == 0) { //loosen
       MoveTo (hdc, prcBtn->right+1, prcBtn->top + w + 1);
        LineTo (hdc, prcBtn->right+1, prcBtn->bottom - w);
        LineTo (hdc, prcBtn->right - w, prcBtn->bottom + 1);
        LineTo (hdc, prcBtn->left + w + 1, prcBtn->bottom + 1);
    }
}
#endif
static int GetCaptFromPos (int cur_row, int cur_col, char *caption);
#if 0
static void HiliteButton (HDC hdc, int cur_row, int cur_col)
{
    RECT rcDown, rcText;
    char caption[10];
	int top =  g_beginY + g_bHeight * cur_row + g_bBorder;
	int left = g_beginX + g_bWidth * cur_col + g_bBorder;
	int bottom = top + g_bHeight - 2*g_bBorder;
	int right = left + g_bWidth - 2*g_bBorder;

    SetRect (&rcDown, left, top, right+1, bottom+1);
    SetRect (&rcText, left+1, top+1, right+1, bottom+1);

    SetBrushColor (hdc, RGB2Pixel(hdc, 71, 156, 210/*0xd3, 0xc6, 0xef*/));
    FillBox (hdc, left, top, (right-left+1), (bottom-top+1));

    DrawButton (hdc, &rcDown, -1);

    GetCaptFromPos (cur_row, cur_col, caption);
    TextOutCenter (hdc, &rcText, caption);
}

static void select_cur_color(HWND hwnd, HDC hdc, 
				gal_pixel btn_color, gal_pixel ctrl_color)
{
    char caption[10];
	RECT btn_rect;

	if (g_curIsctrl == FALSE)
	{
		GetRectByPos(&btn_rect, g_curRow, g_curCol);

		/*fill box with color*/
		SetBrushColor(hdc, btn_color);
		FillBox(hdc, btn_rect.left, btn_rect.top,
			   	(btn_rect.right - btn_rect.left), 
				(btn_rect.bottom - btn_rect.top));

		DrawButton (hdc, &btn_rect, 0);

		GetCaptFromPos (g_curRow, g_curCol, caption);
	    TextOutCenter (hdc, &btn_rect, caption);
	}
	else
	{
		HWND hctrl = GetDlgItem(hwnd, 
						GetIDCtrlFromPos(g_curRow, g_curCol));
		SetWindowElementColorEx(hctrl, FGC_BUTTON_NORMAL, ctrl_color);
		UpdateWindow(hctrl, TRUE);
	}
}
#endif
static void select_cur(HWND hwnd, HDC hdc)
{
    RECT btn_rect;
    GetRectByPos(&btn_rect, g_curRow, g_curCol);
    FillBoxWithBitmapPart(hdc, btn_rect.left, btn_rect.top, RECTW(btn_rect), RECTH(btn_rect), 
            0, 0, &bmpBkClick, btn_rect.left, btn_rect.top);
    InvalidateRect (hwnd, &btn_rect, FALSE);
}

static void unselect_cur(HWND hwnd, HDC hdc)
{
    RECT btn_rect;
    GetRectByPos(&btn_rect, g_curRow, g_curCol);
    FillBoxWithBitmapPart(hdc, btn_rect.left, btn_rect.top, RECTW(btn_rect), RECTH(btn_rect), 
            0, 0, &bmpBk, btn_rect.left, btn_rect.top);
    InvalidateRect (hwnd, &btn_rect, FALSE);
}

// show the value of last_opnd
void show_var (HWND hwnd)        
{
//FIXME DEL    char calc_strdisp[50];

    if (display_error) {
        strcpy(calc_strdisp, "E"); //FIXME ADD
        calc_InvalidDiaplayRect(hwnd);
        return; 
    }

    if (calc_cur_base == BASE_DEC) {
        char outformat[20];
        FLOAT abs_last_opnd = FABS (last_opnd);
        //if (abs_last_opnd >= 1e12 || (abs_last_opnd > 0 && abs_last_opnd < 1e-12)){
        if (abs_last_opnd >= 1e9 || (abs_last_opnd > 0 && abs_last_opnd < 1e-9)){
            sprintf (outformat, "%%.%dE", EFF_NUM - 1);
        } else if (abs_last_opnd < 1){
            sprintf (outformat, "%%.%df", EFF_NUM);
        } else {
            char buff[EFF_NUM + 5];
            sprintf (buff, "%d", (int) abs_last_opnd);  // to do 
            sprintf (outformat, "%%.%df", EFF_NUM - strlen (buff));
        }
        sprintf (calc_strdisp, outformat, last_opnd);
        clearzero (calc_strdisp);
    }
    else { //to do
        FLOAT i_opnd;

        MODF (last_opnd, &i_opnd);
        if (i_opnd < LONG_MIN || i_opnd > ULONG_MAX) {
            fprintf (stderr, "show_var: %f outside the ulong scope\n", i_opnd);
            display_error = 1;
        }
        else if (i_opnd > LONG_MAX) {
            last_opnd = (long) (i_opnd + LONG_MIN - LONG_MAX - 1);
        }
        else {
            last_opnd = (long) i_opnd;
        }
        
        if (calc_cur_base == BASE_BIN) {
            if (longint_bin (calc_strdisp, last_opnd, 17) == -1) {
                display_error = 1;
            }
        }
        else if (calc_cur_base == BASE_OCT) {
            sprintf (calc_strdisp, "%lo", (long)last_opnd);    
        }
        else if (calc_cur_base == BASE_HEX) {
            sprintf (calc_strdisp, "%lX", (long)last_opnd);    
        }
    }

    if (display_error)
    {
        strcpy(calc_strdisp, "E");
    }
    
    calc_InvalidDiaplayRect(hwnd);
    dispnow = last_opnd;
}

// show the digit stack---show input
void show_digit_stack(HWND hwnd)
{
    char strzero[50], *pstrzero;
    int *ptmp;

    if (display_error) {
        strcpy(calc_strdisp, "E");
        calc_InvalidDiaplayRect(hwnd);
        return;
    }
    if (digit_stack_empty ()) {
        dispnow = 0;
        strcpy(calc_strdisp, "0");
        calc_InvalidDiaplayRect(hwnd);
        return;
    }
    if (digit_stack.eNum != 0) {
        show_var (hwnd);
        return;    
    }    

    pstrzero = strzero;    
    for(ptmp = digit_stack.base; ptmp <= digit_stack.top - 1; 
                    ptmp++, pstrzero++) {
        if (ptmp == digit_stack.base && *ptmp == DIGIT_POINT) {
            *pstrzero = '0';
            pstrzero++;
        }
        if (*ptmp >= 0 && *ptmp <= 9) *pstrzero = *ptmp + 48;
        if (*ptmp >= 10 && *ptmp <= 15) *pstrzero = *ptmp + 55;
        else if (*ptmp == DIGIT_POINT) *pstrzero = '.';
    }
    *pstrzero = '\0';
    strcpy(calc_strdisp, strzero);
    calc_InvalidDiaplayRect(hwnd);
    dispnow = last_opnd;
}
#if 0
static void set_button_selected(HWND hWnd, int id, BOOL isSelect)
{
	RECT btn_rect;
    int col, row;

    HDC hdc = GetClientDC (hWnd);
    switch (id){
        case CODE_MR:
            row = 0;
            col = 2;
            break;
        default:
            break;
    }
    GetRectByPos(&btn_rect, row, col);

    if (isSelect) {
        SetPenColor (hdc, RGB2Pixel(hdc, 0, 200, 200));
        //SetPenColor (hdc, PIXEL_green);
#ifdef ENABLE_LANDSCAPE
        MoveTo (hdc, btn_rect.left +3, btn_rect.top);
        LineTo (hdc, btn_rect.right-3, btn_rect.top);
        LineTo (hdc, btn_rect.right-1, btn_rect.top+3);
        LineTo (hdc, btn_rect.right-1, btn_rect.bottom-2);
        LineTo (hdc, btn_rect.right-3, btn_rect.bottom);
        LineTo (hdc, btn_rect.left+3, btn_rect.bottom);
        LineTo (hdc, btn_rect.left, btn_rect.bottom-3);
        LineTo (hdc, btn_rect.left,btn_rect.top+3);
        LineTo (hdc, btn_rect.left +3, btn_rect.top);
#else
        MoveTo (hdc, btn_rect.left+8, btn_rect.top+9);
        LineTo (hdc, btn_rect.right-6, btn_rect.top+9);
        LineTo (hdc, btn_rect.right-4, btn_rect.top+12);
        LineTo (hdc, btn_rect.right-4, btn_rect.bottom-7);
        LineTo (hdc, btn_rect.right-6, btn_rect.bottom-7);
        LineTo (hdc, btn_rect.left+8, btn_rect.bottom-7);
        LineTo (hdc, btn_rect.left+5, btn_rect.bottom-10);
        LineTo (hdc, btn_rect.left+5,btn_rect.top+12);
        LineTo (hdc, btn_rect.left+8, btn_rect.top+12);
#endif
    } else {
#ifdef ENABLE_LANDSCAPE
        FillBoxWithBitmapPart(hdc, btn_rect.left - g_bBorderT, btn_rect.top - g_bBorderT, 
                RECTW(btn_rect) + 2*g_bBorderT , RECTH(btn_rect) + g_bBorderT + g_bBorderB, 
                0, 0, &bmpBk, btn_rect.left - g_bBorderT, btn_rect.top - g_bBorderT);
#else
        FillBoxWithBitmapPart(hdc, btn_rect.left, btn_rect.top, 
                RECTW(btn_rect), RECTH(btn_rect), 
                0, 0, &bmpBk, btn_rect.left, btn_rect.top);
#endif
    }
    ReleaseDC (hdc);
}
#endif
static void deal_with_special_button(int id, HWND hWnd)
{
    if (id == CODE_MR && g_lastKeyId == CODE_MR)
        id = CODE_MC;
    g_lastKeyId = id;

    switch (id) {
        case CODE_MPLUS:
        case CODE_MMINUS:
            g_mButton = TRUE;
            g_selectMr = TRUE;
            break;
        case CODE_MR:
            if (g_mButton) 
                g_selectMr = TRUE;
            break;
        case CODE_MC:
            g_mButton = FALSE;
            g_selectMr = FALSE;
            break;
        case CODE_AC:
            g_selectMr = FALSE;
            break;
    }
}

void enter_key(HWND hwnd)
{
    int keyId = GetIdFromPos(g_curRow, g_curCol);
    /*drawed button*/
    if (keyId > 0)
    {
        deal_with_special_button(keyId, hwnd);
        //HiliteButton (hdc, g_curRow, g_curCol);
        InputProc(hwnd, keyId);
    }
    /*control button*/
    else
	{
		int idc = GetIDCtrlFromPos(g_curRow, g_curCol);
		int hbtn = GetDlgItem(hwnd, idc);
		if (idc == IDC_BTN_BASE || idc == IDC_BTN_ANG) 
		//		idc == IDC_BTN_EXIT)
		{
			SendMessage(hbtn, BM_CLICK, 0, 0);
		}
		else
		{
			if (SendMessage(hbtn, BM_GETCHECK, 0, 0) == BST_CHECKED)
			{
				SendMessage(hbtn, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			else
			{
				SendMessage(hbtn, BM_SETCHECK, BST_CHECKED, 0);
			}
				
		}

	}
}
// ------------------------------------------------------------------------

static BOOL calcOnCreate (HWND hWnd)
{

    strcpy(calc_strdisp, "0");
    calc_InvalidDiaplayRect(hWnd);
    InitCalculator ();

    return TRUE;
}

static BOOL calcOnDestroy (HWND hWnd)
{
    FreeStack ();

    return TRUE;
}

void trans_status(HWND hWnd)
{
	switch (calc_Status) 
	{
        case CALC_COMPACT:
            SetWindowCaption (hWnd, ("Scientific"));
#if 0
            ShowWindow (hChkInv, SW_SHOW);
            ShowWindow (hChkHyp, SW_SHOW);
            ShowWindow (hBtnBase, SW_SHOW);
            ShowWindow (hBtnAng, SW_SHOW);
			ShowWindow (hBtnExit, SW_SHOW);
#endif
            calc_Status = CALC_SCIEN;
            break;
        case CALC_SCIEN:
            SetWindowCaption (hWnd, ("Simple"));
#if 0
            ShowWindow (hChkInv, SW_HIDE);
            ShowWindow (hChkHyp, SW_HIDE);
            ShowWindow (hBtnBase, SW_HIDE);
            ShowWindow (hBtnAng, SW_HIDE);
			ShowWindow (hBtnExit, SW_HIDE);
#endif
            calc_Status = CALC_COMPACT;
            break;
    }
    DecideSize ();
    UpdateWindow (hWnd, TRUE);
}

static int CalcWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) 
    {
        case MSG_CREATE:
            {            
                calcOnCreate(hWnd); 

                hdc = GetClientDC(hWnd);
                memDC = CreateCompatibleDCEx (hdc, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT);
                ReleaseDC(hdc);

                if (LoadBitmap (HDC_SCREEN, &bmp_num, CALC_BMP_BN))
                    return -1;
                if (LoadBitmap (HDC_SCREEN, &bmpBk, CALC_BMP_BG))
                    return -1;
                if (LoadBitmap (HDC_SCREEN, &bmpBkClick, CALC_BMP_BC))
                    return -1;

                SetWindowAdditionalData2 (hWnd, 0);
                g_curRow = 0;
                g_curCol = 0;
                UpdateWindow (hWnd, TRUE);
                break;
            }
        case MSG_LBUTTONDOWN:
            {
                if (validate_select(lParam)) {
                    g_lButtonDown = TRUE;
                    mouse_select(lParam);
                    select_cur(hWnd, memDC);	
                }
                break;
            }
        case MSG_LBUTTONUP:
            {
                if (g_lButtonDown) {
                    unselect_cur(hWnd, memDC);
                    enter_key(hWnd);
                  //  set_button_selected(hWnd, CODE_MR, g_selectMr);
                }
                g_lButtonDown = FALSE;
                break;
            }
        case MSG_PAINT:
            {
                int num_len;

                if (strcmp(calc_strdisp, "inf") == 0
                        ||strcmp(calc_strdisp, "INF") ==0)
                {
                    strcpy(calc_strdisp, "E");
                }

                num_len = strlen(calc_strdisp);
                if (num_len > 15) return 0;

                memmove (calc_strdisp+(15-num_len), calc_strdisp, num_len+1);
                memset (calc_strdisp, ' ',  15-num_len);
                
                hdc = BeginPaint(hWnd);
                display_led_digits (memDC, CALC_DISPLAY_L, CALC_DISPLAY_T, calc_strdisp, &bmp_num);
                BitBlt (memDC, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, hdc, 0, 0, 0);
                EndPaint (hWnd, hdc);

                return 0;
            }
        case MSG_ERASEBKGND:
            {
                FillBoxWithBitmap(memDC, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, &bmpBk);
                return 0;
            }
        case MSG_CLOSE:
            {
                UnloadBitmap (&bmp_num);
                UnloadBitmap (&bmpBk);
                UnloadBitmap (&bmpBkClick);
                DeleteMemDC (memDC);
                DestroyAllControls ( hWnd );
                DestroyMainWindow ( hWnd );
                PostQuitMessage (hWnd);
                return 0;
            }
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void _calculator_init_create_info ( PMAINWINCREATE pCreateInfo )
{
    pCreateInfo->dwStyle    = WS_NONE;
    pCreateInfo->dwExStyle  = WS_EX_NONE;
    pCreateInfo->spCaption  = "Calculator";
    pCreateInfo->hMenu      = 0;
    pCreateInfo->hCursor    = GetSystemCursor ( 0 );
    pCreateInfo->hIcon      = 0;
    pCreateInfo->MainWindowProc = CalcWinProc;
    pCreateInfo->lx         = 0;
    pCreateInfo->ty         = 0;
    pCreateInfo->rx         = IPHONE_MAIN_WIDTH;
    pCreateInfo->by         = IPHONE_MAIN_HEIGHT;
    pCreateInfo->iBkColor   = COLOR_lightgray;
    pCreateInfo->dwAddData  = 0;
    pCreateInfo->hHosting   = HWND_DESKTOP;
}


int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "Calculator" , 0 , 0);
#endif

    _calculator_init_create_info ( &CreateInfo );

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

#if 0
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
#else
    ShowWindowUsingShareBuffer (hMainWnd);
#endif

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

