
#define VAL_DEBUG
#include "my_debug.h"
#include "layout.h"

extern BOOL g_bAnimateShow;

static void recalca_units(LAYOUT_UNIT* units, int n, int width)
{
	if(!units) return;

	int recent = width;
	int i;
	int avgs = 0;
	for(i=0;i<n;i++)
	{
		if(units[i].logic_size == 0)
		{
			avgs ++;
			continue;
		}

		if(units[i].type == lutPixel){
			units[i].current_size = units[i].logic_size;	
		}
		else{
			units[i].current_size = units[i].logic_size*width/100;
		}
		if(units[i].current_size > recent)
			units[i].current_size = recent;
		recent -= units[i].current_size;
	}

	if(recent <= 0)
		return;

	int avg_size = recent/avgs;
	for(i=0; i<n && avgs > 0; i++)
	{
		if(units[i].logic_size == 0)
		{
			units[i].current_size = avg_size;
			avgs --;
		}
	}
}

static int _grid_layout_dispatchMessage(GRID_LAYOUT* playout, int message, WPARAM wParam, LPARAM lParam)
{
	if(playout)
	{
		return playout->gridLayoutProc?
			(*playout->gridLayoutProc)(playout,message, wParam,lParam)
			:DefGridLayoutDispatchMessage(playout,message, wParam,lParam);
	}
	return 0;
}

static LAYOUT_CELL_INTF _grid_layout_interface={
	_grid_layout_dispatchMessage,
	DeleteGridLayout
};


GRID_LAYOUT* CreateGridLayout(int rows, int cols, PGridLayoutProc gridLayoutProc, void* user_data)
{

	if(rows <= 0 || cols<= 0)
		return NULL;

	GRID_LAYOUT* gl = (GRID_LAYOUT*)calloc(sizeof(GRID_LAYOUT),1);

	gl->intf = &_grid_layout_interface;	

	gl->gridLayoutProc = gridLayoutProc;

	gl->row_cnt = rows;
	gl->col_cnt = cols;

	gl->user_data = user_data;

	int col;
	gl->cells = (LAYOUT_CELL**)calloc(cols, sizeof(LAYOUT_CELL*));
	for(col = 0; col<cols; col++)
	{
		gl->cells[col] = (LAYOUT_CELL*)calloc(rows, sizeof(LAYOUT_CELL));
	}

	return gl;
}

static LAYOUT_UNIT* set_unit_size(LAYOUT_UNIT* units, int idx, int n, int width, int type, int logic_size)
{
	if(idx < 0 || idx >= n)
		return units;

	if(units == NULL)
	{
		units = (LAYOUT_UNIT*)calloc(n, sizeof(LAYOUT_UNIT));
	}
	
	units[idx].type = type==0?lutPixel:lutPercent;
	units[idx].logic_size = logic_size;

	recalca_units(units, n, width);
	return units;
}

void GridLayoutSetColWidth(GRID_LAYOUT* playout, int row, int type, int logic_size)
{
	if(playout==NULL || row>= playout->row_cnt)
		return;
	playout->row_height = set_unit_size(playout->row_height, row, 
		playout->row_cnt,	
		playout->height,
		type,
		logic_size);
}

void GridLayoutSetRowHeight(GRID_LAYOUT* playout, int col, int type, int logic_size)
{
	if(playout==NULL || col>= playout->col_cnt)
		return;
	playout->col_width = set_unit_size(playout->col_width, col, 
		playout->col_cnt,	
		playout->width,
		type,
		logic_size);
}

LAYOUT_CELL_OBJ *GridLayoutGetCell(GRID_LAYOUT* playout, int row, int col)
{
	if(!playout || (col <0 && col>=playout->col_cnt)
		|| (row < 0 && row>=playout->row_cnt))
		return NULL;
	
	return playout->cells[col][row].obj;
}

//set new obj into the cell, and return the old one
LAYOUT_CELL_OBJ* GridLayoutSetCell(GRID_LAYOUT* playout, int row, int col, LAYOUT_CELL_OBJ* new_obj)
{
	if(!playout || (col <0 && col>=playout->col_cnt)
		|| (row < 0 && row>=playout->row_cnt))
		return NULL;

	LAYOUT_CELL_OBJ* old = playout->cells[col][row].obj;
	playout->cells[col][row].obj = new_obj;
	return old;
}

static int get_idx_by_pos(LAYOUT_UNIT* units, int pos ,int n)
{
	int i;
	int w = 0;
	for(i=0;i<n && pos>w;i++) {
		w += units[i].current_size;
//		printf("pos=%d,w=%d,i=%d\n",pos,w,i);
	}
	return pos>w?-1:(i-1);
}

BOOL GetRowColByPos(GRID_LAYOUT* playout, int x, int y, int *row, int *col)
{
	if(playout->row_height) 
		*row = get_idx_by_pos(playout->row_height, y - playout->y, playout->row_cnt);
	else
		*row = (y-playout->y)/(playout->height/playout->row_cnt);

	if(playout->col_width)
		*col = get_idx_by_pos(playout->col_width, x - playout->x, playout->col_cnt);
	else
		*col = (x-playout->x)/(playout->width/playout->col_cnt);	
	return (*row>=0 && (*row)<playout->row_cnt 
			&& *col>=0 && (*col)<playout->col_cnt);
}


LAYOUT_CELL_OBJ* GridLayoutHit(GRID_LAYOUT* playout, int x, int y)
{
	int col,row;

	GetRowColByPos(playout, x, y, &row, &col);

	if(col>=0 && row>=0 && col<playout->col_cnt && row<playout->row_cnt)
		return playout->cells[col][row].obj;
	return NULL;
}

LAYOUT_CELL_OBJ* GridLayoutRemove(GRID_LAYOUT* playout, int row, int col)
{
	if(!playout || (col <0 && col>=playout->col_cnt)
		|| (row < 0 && row>=playout->row_cnt))
		return NULL;
	
	LAYOUT_CELL_OBJ* old = playout->cells[col][row].obj;
	playout->cells[col][row].obj = NULL;
	return old;
}

LAYOUT_CELL_OBJ* GridLayoutRemoveHit(GRID_LAYOUT* playout, int x, int y)
{
	int col,row;

	GetRowColByPos(playout, x, y, &row, &col);
	return GridLayoutRemove(playout, row, col);
}


void DeleteGridLayout(GRID_LAYOUT *playout)
{
	if(!playout)
		return;

	int col;
	for(col=0;col<playout->col_cnt;col++)
	{
		int row;
		for(row=0;row<playout->row_cnt;row++)
		{
			if(playout->cells[col][row].obj)
				lciFree(playout->cells[col][row].obj);
		}
		free(playout->cells[col]);
	}
	free(playout->cells);

	if(playout->row_height)
		free(playout->row_height);
	
	if(playout->col_width)
		free(playout->col_width);
	
	if(playout->bkground.type == lbtBmpBrush){
		UnloadBitmap(playout->bkground.data.tilBmp);
		free(playout->bkground.data.tilBmp);
	}
	free(playout);
}


void DrawBkground(HDC hdc, int x, int y, int w, int h, LAYOUT_BKGROUND *pbkg)
{
	switch(pbkg->type)
	{
	case lbtColor:
		{
			SetBrushType(hdc, BT_SOLID);
			register int rgb = pbkg->data.bkcolor;
			SetBrushColor(hdc, RGB2Pixel(hdc,RGBR(rgb),RGBG(rgb),RGBB(rgb)));
			FillBox(hdc,x,y,w,h);
		}
		break;

	case lbtBmpBrush:
		{
			SetBrushType(hdc, BT_TILED);
			SetBrushInfo(hdc,pbkg->data.tilBmp, NULL);
			FillBox(hdc,x,y,w,h);
			break;
		}
	case lbtCallback:
		{
			(*pbkg->data.callback.drawbk)(hdc,x,y,w,h,
				pbkg->data.callback.param);
		}
		break;
	}
}

static void _grid_layout_send_to_all_cells_msg(GRID_LAYOUT* playout, int message, WPARAM wParam, LPARAM lParam)
{
	if(!playout) return;
	int col;
	for(col=0;col<playout->col_cnt;col++)
	{
		int row;
		for(row=0;row<playout->row_cnt;row++)
		{
			LAYOUT_CELL * cell = &playout->cells[col][row];
			if(cell->obj)
				lciDispatchMessage(cell->obj,message,wParam,lParam);
		}
	}
}

static void _grid_layout_deal_cell_with_position(GRID_LAYOUT* playout,
	void (*deal_cell)(LAYOUT_CELL_OBJ* obj, PRECT prc, void *param),
	void *param)
{
	if(!playout || !deal_cell) return;
	
	int col=-1,row=-1;
	int avgcol,avgrow;
	RECT rect;
	
	rect.left = playout->x;
	rect.top = playout->y;

	if(!playout->col_width) avgcol = playout->width/playout->col_cnt;
	if(!playout->row_height) avgrow = playout->height/playout->row_cnt;


	for(col=0;col<playout->col_cnt;col++)
	{
		rect.right = rect.left+ (playout->col_width?playout->col_width[col].current_size:avgcol);	
		rect.top = playout->y;
		for(row=0;row<playout->row_cnt;row++)
		{
			rect.bottom = rect.top + (playout->row_height?playout->row_height[row].current_size:avgrow);

			LAYOUT_CELL* cell = &playout->cells[col][row];
			if(cell->obj)
				deal_cell(cell->obj,&rect,param);
			rect.top = rect.bottom;
		}
		rect.left = rect.right;
	}
}

static void _grid_layout_resize(LAYOUT_CELL_OBJ* obj, PRECT prt,void* param)
{
	lciDispatchMessage(obj,LAY_MSG_MOVETO,0,prt);
}

int DefGridLayoutDispatchMessage(GRID_LAYOUT* playout, int message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case LAY_MSG_PAINT:
		_grid_layout_send_to_all_cells_msg(playout,message,wParam,lParam);
		break;
	case LAY_MSG_ERASEBKGND:
	//paint the grids;
        //printf ("start the LAY_MSG_ERASEBKGND...\n");
        if (g_bAnimateShow)
        {
            g_bAnimateShow = FALSE;
            return 0;
        }
		DrawBkground((HDC)wParam,playout->x, playout->y,
			playout->width, playout->height,
			&playout->bkground);
		_grid_layout_send_to_all_cells_msg(playout,message,wParam,lParam);
		break;
	case LAY_MSG_MOVETO:
		{
			RECT* prc = (RECT*)lParam;
			playout->x = prc->left;
			playout->y = prc->top;
			playout->width = prc->right - prc->left;
			playout->height = prc->bottom - prc->top;
			if(playout->col_width)
				recalca_units(playout->col_width, playout->col_cnt, playout->width);
			if(playout->row_height)
				recalca_units(playout->row_height, playout->row_cnt,playout->height);
			_grid_layout_deal_cell_with_position(playout,_grid_layout_resize,NULL);
		}
		break;
	case MSG_FIRSTMOUSEMSG ...  MSG_NCMOUSEOFF:
	case LAY_MSG_MOUSE_BEGIN ... LAY_MSG_MOUSE_END:
		{
			if(playout->captured_obj){
				return lciDispatchMessage(playout->captured_obj,message,wParam,lParam);
			}
			int x = LOSWORD (lParam);
			int y = HISWORD (lParam);
			LAYOUT_CELL_OBJ* obj = GridLayoutHit(playout, x, y);
			if(message == MSG_MOUSEMOVE) {
				if(obj != playout->cur_mouse_on)
				{
					if(playout->cur_mouse_on) 
					{
						lciDispatchMessage(playout->cur_mouse_on,LAY_MSG_MOUSE_MOVEOUT,wParam, 0);
					}
					if(obj)
					{
						lciDispatchMessage(obj,LAY_MSG_MOUSE_MOVEIN,wParam,0);
					}
						playout->cur_mouse_on = obj;
				}
			}
			if(obj)
			{
				lciDispatchMessage(obj, message, wParam,lParam);
			}
		}
		break;
	}
	return 0;
}

void GridLayoutEnumAllCellObjs(GRID_LAYOUT* playout, BOOL (*deal_obj)(LAYOUT_CELL_OBJ*,GRID_LAYOUT* this,int row, int col,void *),void * user_data)
{
	if(!playout || !deal_obj)
		return;
	
	int row,col;
	for(row=0;row<playout->row_cnt;row++)
	{
		for(col=0;col<playout->col_cnt;col++)
		{
			if(playout->cells[col][row].obj)
			{
				if(!(*deal_obj)(playout->cells[col][row].obj,playout,row, col, user_data))
					return;
			}
		}
	}
}

BOOL inline is_in_gridlayout (int x, int y, GRID_LAYOUT* layout)
{
    int in_x = x - layout->x;
    int in_y = y - layout->y;
    return (in_x >= 0 && in_x < layout->width 
            && in_y >= 0 && in_y < layout->height);
}


static int getPosFromUnit(LAYOUT_UNIT *unit, int idx, int n)
{
	int i;
	int pos = 0;
	for(i=0; i<idx;i++)
	{
		pos += unit[i].current_size;	
	}
	return pos;
}

BOOL GridLayoutGetCellRect(GRID_LAYOUT* playout, int row, int col, PRECT prt)
{
	if((row<0 || row>=playout->row_cnt)
		|| (col<0 || col>=playout->col_cnt))
		return FALSE;

	if(playout->col_width)
	{
		prt->left = playout->x + getPosFromUnit(playout->col_width, col, playout->col_cnt);
		prt->right = prt->left + playout->col_width[col].current_size;
	}
	else
	{
		int avg = playout->width / playout->col_cnt;
		prt->left = playout->x + avg* col;
		prt->right = prt->left + avg;
	}
		
	if(playout->row_height)
	{
		prt->top = playout->y + getPosFromUnit(playout->row_height,row, playout->row_cnt);
		prt->bottom = prt->top + playout->row_height[row].current_size;
	}
	else
	{
		int avg = playout->height / playout->row_cnt;
		prt->top = playout->y + avg * row;
		prt->bottom = prt->top + avg;
	}
	return TRUE;
}

BOOL GetHittedGridCenterPos (GRID_LAYOUT* layout, int x, int y, int* grid_x, int* grid_y)
{
    int row;
    int col;
    RECT rc;

    if (!is_in_gridlayout (x, y, layout))
        return FALSE;

    GetRowColByPos (layout, x, y, &row, &col);
    GridLayoutGetCellRect(layout, row, col, &rc);
    TEST_VAL (rc.left, %d);
    TEST_VAL (rc.right, %d);
    TEST_VAL (rc.top, %d);
    TEST_VAL (rc.bottom, %d);
    *grid_x = (rc.left + rc.right) >> 1;
    *grid_y = (rc.top + rc.bottom) >> 1;
    return TRUE;
}


