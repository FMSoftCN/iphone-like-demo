/*
 * desktop special layout 
 *
 * 
 */

#include "common.h"
#include "layout.h"
#include "layouthlp.h"
#include "sence.h"
#include "dragmanager.h"
#include "phone_icon.h"
#include "icon_layout.h"
#include "animate/animate.h"
#include "iphone-animates.h"

#define MOUSE_LP_TO_XY(x, y, lParam)  \
do { \
    x = LOSWORD (lParam); \
    y = HISWORD (lParam); \
} while (0)

BOOL iconLayoutDelAnimation(GRID_LAYOUT* playout, int del_row, int del_col,int stop_at_row, int stop_at_col);

BOOL iconLayoutInsertAnimation(GRID_LAYOUT* playout, int row_add, int col_add, int space_row, int space_col,LAYOUT_CELL_OBJ * obj);

BOOL iconLayoutMoveAnimation(GRID_LAYOUT* playout, int row_add, int col_add, int del_row, int del_col, LAYOUT_CELL_OBJ *obj);

static GRID_LAYOUT* topGL;
static GRID_LAYOUT* upGL;
static GRID_LAYOUT* downGL;

extern HDC g_hMainDC;
extern g_animateflag;

static BOOL sway_all_objs (LAYOUT_CELL_OBJ* cur,GRID_LAYOUT* this,int row, int col,void* except_obj)
{
	
	icon_cell_t * ic = (icon_cell_t*) cur;

	if(ic->icon.a== NULL)
	{
		ic->icon.a = AppendMainAnimateIcons(ic->icon.x,
			ic->icon.y,
			ic->icon.normal_bmp.bmWidth,
			ic->icon.normal_bmp.bmHeight,
			&ic->icon.l_slant_bmp,
			ic == except_obj);
	}

    return TRUE;
}


BOOL clear_all_objs (LAYOUT_CELL_OBJ* cur,GRID_LAYOUT* this,int row, int col,void* except_obj)
{
	icon_cell_t * ic = (icon_cell_t*) cur;
	
	ic->icon.a = NULL;

	return TRUE;
}

static inline LAYOUT_CELL_OBJ* get_hitted_cobj (int x, int y)
{
    GRID_LAYOUT* hitted_layout;
    if (is_in_gridlayout(x, y, upGL))
        hitted_layout = upGL;
    else
        hitted_layout = downGL;
	
    return GridLayoutHit(hitted_layout, x, y);
}

///////////////////////////////////////////////
typedef struct _drag_info_t{
	GRID_LAYOUT* pgl;
	int col;
	int row;
}drag_info_t;

static inline BOOL drag_info_equal(drag_info_t* di1, drag_info_t* di2)
{
	return (di1->pgl==di2->pgl &&
		di1->row == di2->row &&
		di1->col == di2->col);
}

static BOOL get_drag_info_from_pos(GRID_LAYOUT* pglTop, int x, int y,drag_info_t* di)
{
	int row, col;
	di->pgl = NULL;
	di->col = di->row = -1;
	GRID_LAYOUT* pgl = (GRID_LAYOUT*)GridLayoutHit(pglTop, x, y);
	if(!pgl) return FALSE;
	
	if(GetRowColByPos(pgl,x,y,&row,&col))
	{
		di->pgl = pgl;
		// if current cell is null, forward and find the last one is not null
		while(GridLayoutGetCell(pgl, row, col)==NULL){
			col --;
			if(col < 0){
				if(--row < 0)
				{
					di->row = 0;
					di->col = 0;
					return TRUE;
				}
				col = pgl->col_cnt - 1;
			}
		}
		di->row = row;
		di->col = col;
		
		return TRUE;
	}
	return FALSE;
}

static void clean_drag_info(drag_info_t* di)
{
	di->pgl = NULL;
	di->col = di->row = -1;
}
////////////////////////////////////
#if 0
void printlayout(GRID_LAYOUT* pgl)
{
	int row,col;
	if(pgl == NULL)
	{
		printf("-------- null grid layout\n");
		return;
	}
	printf("------Grid Layout(%d,%d,%d,%d)\n",pgl->x,pgl->y,pgl->width,pgl->height);
	for(row=0;row<pgl->row_cnt;row++)
	{
		for(col=0;col<pgl->col_cnt;col++)
		{
			printf("<%d,%d>(%p)\t",row,col,pgl->cells[col][row].obj);
		}
		printf("\n");
	}
	printf("---------------------end grid layout----------\n");
}
#endif
static BOOL icons_active;
static LAYOUT_CELL_OBJ* draged_cobj;
static drag_info_t _old_drag_info;
enum _drag_state{
	dsReady = 0,
	dsDragAnimating,
	dsMouseFollowing,
	dsReleaseAnimating
};
static int _drag_state = dsReady;


static void on_end_released_draged_icon(){
	_drag_state = dsReady;
}

static int LeftLayoutProc(GRID_LAYOUT* playout, int message, WPARAM wParam, LPARAM lParam)
{
	int x;
    int y;
    GRID_LAYOUT* sub_layout;
    icon_cell_t* icon_cell;
    
	switch(message)
	{
	case MSG_DRAG_ACTIVE:
        if (!icons_active)
        {
			if(_drag_state != dsReady)
				return 0 ;

            MOUSE_LP_TO_XY (x, y, lParam);
            draged_cobj = get_hitted_cobj(x, y);
            if (draged_cobj)
            {
				//start the main aniamtion
                icons_active = TRUE;
                GridLayoutEnumAllCellObjs (upGL, sway_all_objs, draged_cobj);
                GridLayoutEnumAllCellObjs (downGL, sway_all_objs, draged_cobj);
				icon_cell = (icon_cell_t*)draged_cobj;
				if(icon_cell->icon.a)
				{
					SetDragedIcon(icon_cell->icon.a, x, y);
					_drag_state = dsMouseFollowing;
				}

				LanchMainAnimates();
                g_animateflag =TRUE;

				get_drag_info_from_pos(playout,x,y,&_old_drag_info);
				GridLayoutSetCaptureCell(topGL,LEFT_GL_ROW,LEFT_GL_COL);
            }
			else
				clean_drag_info(&_old_drag_info);
        }
        return;

    case MSG_KEYUP:
        if (wParam != SCANCODE_F4)
            break;
#ifdef ENABLE_CLOSE_CORNER
    case MSG_RBUTTONDOWN:
#endif
		if(icons_active && draged_cobj==NULL){
			TerminateMainAnimate();
            g_animateflag =FALSE;
       	 	icons_active = FALSE;
			_drag_state = dsReady;
       		draged_cobj = NULL;
			clean_drag_info(&_old_drag_info);
            GridLayoutEnumAllCellObjs (upGL, clear_all_objs, NULL);
            GridLayoutEnumAllCellObjs (downGL, clear_all_objs, NULL);
			GridLayoutDispatchMessage(playout, LAY_MSG_PAINT, (WPARAM)g_hMainDC, 0);
		}
        break;

    case MSG_MOUSEMOVE:
        if (icons_active /*&& wParam & KS_LEFTBUTTON*/ ) {
			if(!draged_cobj || _drag_state != dsMouseFollowing) return 0;
        	MOUSE_LP_TO_XY (x, y, lParam);
			drag_info_t  drag_info;
            icon_cell = (icon_cell_t*)draged_cobj;

			SetDragedIconPosition(x, y);	

			if(get_drag_info_from_pos(playout,x,y,&drag_info))
			{
				StartMoveIcons();
				if(!drag_info_equal(&drag_info,&_old_drag_info))
				{// a icon was move from one cell into another cell

					if(_old_drag_info.pgl != drag_info.pgl) // a icon move in diffent layout
					{
						//enter new layout
						if(drag_info.pgl!=NULL)
						{
							//insert successfully or not
							if(iconLayoutInsertAnimation(drag_info.pgl,
								drag_info.row,
								drag_info.col,
								-1,-1, draged_cobj))
							{

								//leave old layout
								if(_old_drag_info.pgl!=NULL)
								{
									GridLayoutSetCell(_old_drag_info.pgl,
										_old_drag_info.row,
										_old_drag_info.col,NULL);
									iconLayoutDelAnimation(_old_drag_info.pgl,
										_old_drag_info.row,
										_old_drag_info.col,
										-1,-1);
								}
								_old_drag_info = drag_info;
							}
						}
					}
					else // in the same layout
					{
						if(_old_drag_info.pgl)
						{
							GridLayoutSetCell(_old_drag_info.pgl,
								_old_drag_info.row,
								_old_drag_info.col,NULL);
						}
	
						if(drag_info.pgl!=NULL)
						{
							iconLayoutMoveAnimation(drag_info.pgl,
								drag_info.row,
								drag_info.col, //new pos to insert
								_old_drag_info.row,
								_old_drag_info.col,
								draged_cobj);
							_old_drag_info = drag_info;
						}
					}

				}
				RunMoveIcons();
			}

            return;
        }
        break;

	case MSG_LBUTTONDOWN:
        if (icons_active)
        {
			if(draged_cobj || _drag_state != dsReady)
				return 0;

            MOUSE_LP_TO_XY (x, y, lParam);
            draged_cobj = get_hitted_cobj(x, y);

            printf ("button down: %d, %d\n", x, y);

            if (draged_cobj)
            {
				get_drag_info_from_pos(playout,x,y,&_old_drag_info);
			 	PHONE_ICON* picon = &((icon_cell_t*)draged_cobj)->icon;
				SetDragedIcon(picon->a, x, y);

				GridLayoutSetCaptureCell(topGL,LEFT_GL_ROW,LEFT_GL_COL);
				_drag_state = dsMouseFollowing;
            }
			else {
				clean_drag_info(&_old_drag_info);
				_drag_state = dsReady;
			}
			return 0;
        }
		break;

	case MSG_LBUTTONUP:
        if (icons_active) {

			if(!draged_cobj || _drag_state != dsMouseFollowing) return 0;

			PHONE_ICON* picon = &((icon_cell_t*)draged_cobj)->icon;
			//get the nearest cell
			int row = 0, col = 0;
			GRID_LAYOUT *pgl = _old_drag_info.pgl;
			row = _old_drag_info.row;
			col = _old_drag_info.col;
			if(pgl) 
			{
				RECT rt;
				GridLayoutGetCellRect(pgl,row,col,&rt);
				picon->x = (rt.left+rt.right - picon->normal_bmp.bmWidth)/2;
				picon->y = (rt.top+rt.bottom-picon->normal_bmp.bmHeight)/2;
				_drag_state = dsReleaseAnimating;
				ReleaseDragedIcon(
					picon->x,
					picon->y,
					picon->normal_bmp.bmWidth,
					picon->normal_bmp.bmHeight,
					on_end_released_draged_icon);
				GridLayoutSetCell(pgl,row,col,draged_cobj);
			}
            draged_cobj = NULL;
			clean_drag_info(&_old_drag_info);
			GridLayoutReleaseCaptureCell(topGL);
            return 0;
        }
		break;
    case LAY_MSG_INFO:
/*        if (icons_active && draged_cobj) {
            icon_cell = (icon_cell_t*)draged_cobj;
#ifdef  ENABLE_LANDSCAPE 
            Switch_QuickView(icon_cell->icon.name, icon_cell->icon.command);
#endif
        }
  */      break;

    }

    return DefGridLayoutDispatchMessage(playout, message, wParam, lParam);
}

/*
 * animation when del an icon
 *
 */

BOOL iconLayoutDelAnimation(GRID_LAYOUT* playout, int del_row, int del_col, int stop_at_row, int stop_at_col)
{
	int row,col;
	PHONE_ICON *picon;
	RECT rt;
	int prev_row, prev_col;
	
	if(!GridLayoutGetCellRect(playout,del_row,del_col,&rt))
		return FALSE;

	if(stop_at_row < 0 || stop_at_row>=playout->row_cnt) 
		stop_at_row = playout->row_cnt-1;
	if(stop_at_col < 0 || stop_at_col>=playout->col_cnt)
		stop_at_col = playout->col_cnt-1;
	
	prev_row = del_row;
	prev_col = del_col;

	col = del_col + 1;

	for(row=del_row; row<=stop_at_row; row++)
	{
		for(;(row!=stop_at_row && col<playout->col_cnt) || (row==stop_at_row && col<=stop_at_col);col++)
		{
			icon_cell_t * pic = (icon_cell_t*) playout->cells[col][row].obj;
			playout->cells[col][row].obj = NULL;
			if(pic)
			{
				GridLayoutGetCellRect(playout,prev_row,prev_col,&rt);
				picon = &pic->icon;
				picon->x = (rt.left+rt.right-picon->normal_bmp.bmWidth)/2;
				picon->y = (rt.top+rt.bottom-picon->normal_bmp.bmHeight)/2;
				if(picon->a)
					SetMoveIcon(picon->a, 
						picon->x,
						picon->y);
			}
			playout->cells[prev_col][prev_row].obj = pic;
			prev_col ++;
			if(prev_col >= playout->col_cnt)
			{
				prev_col = 0;
				prev_row ++;
			}
		}
		col = 0;
	}
	return TRUE;
}


/*
 * animation when insert an icon
 */

BOOL iconLayoutInsertAnimation(GRID_LAYOUT* playout, int row_add, int col_add, int space_row, int space_col, LAYOUT_CELL_OBJ * obj)
{
	int row, col;
	int prev_row, prev_col;
	RECT rt;


	if(!playout || !obj)
		return FALSE;
	//find a place to insert
	if(space_row >= 0 && space_row < playout->row_cnt
		&& space_col >= 0 && space_col < playout->col_cnt)
	{
		col = space_col;
		row = space_row;
		goto FOUND;
	}

	col = col_add;
	for(row=row_add;row<playout->row_cnt;row++)
	{
		for(;col<playout->col_cnt;col++)
		{
			if(playout->cells[col][row].obj==NULL)
			{
				goto FOUND;
			}
		}
		col = 0;
	}
	return FALSE;

FOUND:
	
	GridLayoutGetCellRect(playout,row,col,&rt);
	int prev_x , prev_y;
	prev_x = (rt.left + rt.right)/2;
	prev_y = (rt.top + rt.bottom)/2;
	
	prev_row = row;
	prev_col = col;

	col --;
		
	for(;row>row_add;row--)
	{
		for(;col>=0;col--)
		{
			icon_cell_t* pic = (icon_cell_t*)playout->cells[col][row].obj;			
			if(pic){
				GridLayoutGetCellRect(playout,prev_row,prev_col,&rt);
				PHONE_ICON* picon = &pic->icon;
				picon->x = (rt.left+rt.right-picon->normal_bmp.bmWidth)/2;
				picon->y = (rt.top+rt.bottom-picon->normal_bmp.bmHeight)/2;
				if(picon->a)
					SetMoveIcon(picon->a,
						picon->x, 
						picon->y);

				playout->cells[col][row].obj = NULL;
			}
			playout->cells[prev_col][prev_row].obj = pic;
			prev_col --;
			if(prev_col<0)
			{
				prev_col = playout->col_cnt-1;
				prev_row --;
			}
		}
		col = playout->col_cnt-1;
	}
	
	if(col<0){
		col = playout->col_cnt - 1;
	}

	for(; col>=col_add; col--)
	{
		icon_cell_t* pic = (icon_cell_t*)playout->cells[col][row].obj;			
		if(pic){
			GridLayoutGetCellRect(playout,prev_row,prev_col,&rt);
			PHONE_ICON* picon = &pic->icon;
			picon->x = (rt.left+rt.right-picon->normal_bmp.bmWidth)/2;
			picon->y = (rt.top+rt.bottom-picon->normal_bmp.bmHeight)/2;
			if(picon->a)
				SetMoveIcon(picon->a,
					picon->x,
					picon->y);
			playout->cells[col][row].obj = NULL;
		}
		playout->cells[prev_col][prev_row].obj = pic;
		prev_col -- ;
		if(prev_col < 0)
		{
			prev_row --;
			prev_col = playout->col_cnt-1;
		}
	}

	return TRUE;
}

/*
 * icon do layout
 *
 */
BOOL iconLayoutMoveAnimation(GRID_LAYOUT* playout, int row_add, int col_add, int del_row, int del_col, LAYOUT_CELL_OBJ *obj)
{
	if(row_add == del_row && col_add == del_col)
	{
		GridLayoutSetCell(playout, row_add, col_add, obj);
	}
	else if(row_add>del_row 
		|| (row_add == del_row && col_add > del_col))
	{
		iconLayoutDelAnimation(playout, del_row, del_col,
			row_add, col_add);
		GridLayoutSetCell(playout,row_add,col_add, obj);
	}
	else{
		iconLayoutInsertAnimation(playout,row_add,col_add,del_row,del_col,obj);
	}

	return TRUE;
}


static int iconLayoutProc (GRID_LAYOUT* playout, int message, WPARAM wParam, LPARAM lParam)
{
    //static BOOL icons_active;
    //LAYOUT_CELL_OBJ* draged_cobj;
	
	//del me
	static LAYOUT_CELL_OBJ* obj_del;

    switch(message)
    {
	case LAY_MSG_PAINT:
		if(icons_active)
			return 0;
		break;
  	}

    return DefGridLayoutDispatchMessage(playout, message, wParam, lParam);
}


// the layout of app notification's proc
static int appNotifyProc(GRID_LAYOUT* playout, int message, WPARAM wParam, LPARAM lParam)
{
	return DefGridLayoutDispatchMessage(playout, message, wParam, lParam);
}


void InitDesktopSenceLayout(GRID_LAYOUT* layout)
{
	GRID_LAYOUT *gl;
	topGL = layout;
	//get and set application's layout
	gl = (GRID_LAYOUT*) GridLayoutGetCell(layout,RIGHT_GL_ROW,RIGHT_GL_COL);
	gl->gridLayoutProc = appNotifyProc;

	// get the left-layout
	GRID_LAYOUT* leftGL = (GRID_LAYOUT*)GridLayoutGetCell(layout,LEFT_GL_ROW,LEFT_GL_COL);
    leftGL->gridLayoutProc = LeftLayoutProc;
	upGL = (GRID_LAYOUT*) GridLayoutGetCell(leftGL,UP_ICON_ROW,UP_ICON_COL);
	upGL->gridLayoutProc = iconLayoutProc;
	downGL = (GRID_LAYOUT*) GridLayoutGetCell(leftGL,DOWN_ICON_ROW,DOWN_ICON_COL);
	downGL->gridLayoutProc = iconLayoutProc;
}

