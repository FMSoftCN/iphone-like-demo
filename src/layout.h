/*
 *  the grid layout
 *
 *  author dongjunjie 04/16
 */

#ifndef GRID_LAYOUT_H
#define GRID_LAYOUT_H

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>


typedef struct _layout_unit{
	unsigned int type:1;
	unsigned int logic_size:15;
	unsigned int current_size:16;
}LAYOUT_UNIT;
enum layout_unit_type{
	lutPixel = 0,
	lutPercent
};

typedef struct _layout_cell_interface{
	int (*dispatchMessage)(void *_this, int , WPARAM, LPARAM);
	void (*free)(void* _this);
}LAYOUT_CELL_INTF;

typedef struct _layout_cell_object{
	LAYOUT_CELL_INTF* intf;
}LAYOUT_CELL_OBJ;

#define LAY_MSG_BEGIN  MSG_USER+1000

//move to the cell object to a new RECT. wParam=0, LPARAM= prect
#define LAY_MSG_MOVETO LAY_MSG_BEGIN + 1

//paint layout and every cell wParam=hDC,
#define LAY_MSG_PAINT  MSG_PAINT
// wParam = HDC
#define LAY_MSG_ERASEBKGND MSG_ERASEBKGND

#define LAY_MSG_MOUSE_BEGIN  LAY_MSG_BEGIN + 100
#define LAY_MSG_MOUSE_END  LAY_MSG_MOUSE_BEGIN + 100
//All Message Begin and End LAY_MSG_MOUSE must have the 
//format like:
//int x_pos = LOSWORD(lParam)
//int y_pos = LOSWORD(lParam)

//wParam same as MOUSEMOVE
//lParam = 0
#define LAY_MSG_MOUSE_MOVEIN LAY_MSG_BEGIN + 2
//wParam same as MOUSEMOVE
//lParam = 0
#define LAY_MSG_MOUSE_MOVEOUT LAY_MSG_BEGIN + 3

#define LAY_MSG_INFO LAY_MSG_BEGIN + 4


#define LAYCELL_FUNC(_this,func)   ((LAYOUT_CELL_INTF*) ( (LAYOUT_CELL_OBJ*)(_this)->intf))->func
#define lciDispatchMessage(_this, message, wParam, lParam) \
	(*LAYCELL_FUNC(_this,dispatchMessage))(_this,message,(WPARAM)wParam,(LPARAM)lParam)
#define lciFree(_this) if(LAYCELL_FUNC(_this,free))(*LAYCELL_FUNC(_this,free))(_this)

typedef struct _layout_cell{
	LAYOUT_CELL_OBJ *obj;
}LAYOUT_CELL;

#define LAYCELL_ATTR_VISIBLE  0x01


typedef struct _layout_bkground{
	int type;
	union{
		gal_pixel bkcolor;
		PBITMAP tilBmp;
		struct {
			void (*drawbk)(HDC,int,int,int,int,void *param);
			void *param;
		}callback;
	}data;
}LAYOUT_BKGROUND;
enum layout_bkground_type{
	lbtNull=0,
	lbtColor,
	lbtBmpBrush,
	lbtCallback
};
#define RGBR(rgb)  (((rgb)&0xFF0000)>>16)
#define RGBG(rgb)  (((rgb)&0xFF00)>>8)
#define RGBB(rgb)  ((rgb)&0xFF)

typedef struct _grid_layout{
	LAYOUT_CELL_INTF *intf;
	// the size of grid layout
	int x, y, width, height;

	int row_cnt; // the total rows of grid 
	LAYOUT_UNIT * row_height; // if it is null, mean avarage every rows

	int col_cnt; // the total cols of grid
	LAYOUT_UNIT * col_width; // if it is null, mean avarage every cols
	
	LAYOUT_CELL ** cells;

	LAYOUT_BKGROUND bkground;

	int (*gridLayoutProc)(struct _grid_layout*, int, WPARAM, LPARAM);

	void *user_data;

	LAYOUT_CELL_OBJ* cur_mouse_on;

	LAYOUT_CELL_OBJ* captured_obj;

}GRID_LAYOUT;

typedef int (*PGridLayoutProc)(GRID_LAYOUT *playout, int message,
	WPARAM wParam, LPARAM lParam);

//create a new layout
GRID_LAYOUT* CreateGridLayout(int rows, int cols, PGridLayoutProc gridLayoutProc, void* user_data);

//set the width of col or height of row
void GridLayoutSetColWidth(GRID_LAYOUT* playout, int row, int type, int logic_size);

void GridLayoutSetRowHeight(GRID_LAYOUT *playout, int col, int type, int logic_size);

LAYOUT_CELL_OBJ * GridLayoutGetCell(GRID_LAYOUT *playout, int row, int col);

//set new obj into the cell, and return the old one
LAYOUT_CELL_OBJ* GridLayoutSetCell(GRID_LAYOUT* playout, int row, int col, LAYOUT_CELL_OBJ* new_obj);

LAYOUT_CELL_OBJ* GridLayoutHit(GRID_LAYOUT* playout, int x, int y);

#define GridLayoutDispatchMessage(playout,message,wParam,lParam)  lciDispatchMessage(playout,message,wParam,lParam)

void DeleteGridLayout(GRID_LAYOUT *playout);


void DrawBkground(HDC hdc, int x, int y, int w, int h, LAYOUT_BKGROUND *pbkg);

void GridLayoutDrawBkground(GRID_LAYOUT* playout, HDC hdc, int x, int y, int w, int h);

int DefGridLayoutDispatchMessage(GRID_LAYOUT* playout, int message, WPARAM wParam, LPARAM lParam);

BOOL is_in_gridlayout (int x, int y, GRID_LAYOUT* layout);

#define GridLayoutProc(playout)  ((GRID_LAYOUT*)(playout)->gridLayoutProc)

#define GridLayoutUserData(playout)  ((GRID_LAYOUT*)(playout)->user_data)

// if deal_obj return FALSE, exit the function
//
void GridLayoutEnumAllCellObjs(GRID_LAYOUT* playout, BOOL (*deal_obj)(LAYOUT_CELL_OBJ*,GRID_LAYOUT* this,int row, int col,void *),void * user_data);

BOOL GridLayoutGetCellRect(GRID_LAYOUT* layout, int row, int col, PRECT prt);

BOOL GetRowColByPos(GRID_LAYOUT* playout, int x, int y, int *row, int *col);

LAYOUT_CELL_OBJ* GridLayoutRemove(GRID_LAYOUT* playout, int row, int col);

LAYOUT_CELL_OBJ* GridLayoutRemoveHit(GRID_LAYOUT* playout, int x, int y);

BOOL GetHittedGridCenterPos (GRID_LAYOUT* layout, int x, int y, int* grid_x, int* grid_y);

#define GridLayoutSetCaptureCell(playout,row,col) \
	(((GRID_LAYOUT*)(playout))->captured_obj = GridLayoutGetCell((GRID_LAYOUT*)(playout),(row),(col)))

#define GridLayoutReleaseCaptureCell(playout) \
	(((GRID_LAYOUT*)(playout))->captured_obj = NULL)

#endif

