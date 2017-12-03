/*
 * snence.c the implement of sence
 *
 */

#include <sys/types.h>
#include <signal.h>

#include "common.h"
#include "sence.h"
#include "layout.h"
#include "layouthlp.h"
#include "phone_icon.h"
#include "icon_layout.h"

static int switchkey=SCANCODE_F4;

typedef struct _DESKTOP_SENCE{
	SENCE sence;
	GRID_LAYOUT* layout;
}DESKTOP_SENCE;

// an appsence for application
typedef struct _APP_SENCE{
	SENCE sence;
    PHONE_ICON* picon;
}APP_SENCE;

extern BOOL g_bAnimateShow;

int desktopSenceProc(DESKTOP_SENCE *dsence, int message, WPARAM wParam, LPARAM lParam);

void desktopSwitchOut(DESKTOP_SENCE* _this);
void desktopSwitchIn(DESKTOP_SENCE* _this);


int appSenceProc(DESKTOP_SENCE* asence, int message, WPARAM wParam, LPARAM lParam);

void appSwitchOut(APP_SENCE *_this);
void appSwitchIn(APP_SENCE *_this);

static DESKTOP_SENCE desktopSence={
	{ SENCE_DESKTOP, 
  	  0,
	  desktopSenceProc,
	  desktopSwitchOut,
	  desktopSwitchIn
	},
	NULL
};

static APP_SENCE appSence={
	{ SENCE_APP,
	  0,
	  appSenceProc,
	  appSwitchOut,
	  appSwitchIn
	},
    NULL    
};

SENCE* g_curSence = &desktopSence;

///////////////////////////////////////////////
//
SENCE * GetSenceById(int id)
{
	if(id == SENCE_DESKTOP)
		return (SENCE*)&desktopSence;
	else if(id == SENCE_APP)
		return (SENCE*)&appSence;
	return NULL;
}

/////////////////////////////////////////////////////
// external functions
extern LAYOUT_CELL_OBJ* loadIcons(const char* cfgFile, const char* key,void* userdata);

static void desktopSenceClearBkground(HDC hdc, PRECT prt, void *userdata)
{
    GRID_LAYOUT* pgl = (GRID_LAYOUT*)userdata;
    if(pgl)
    {
        DrawBkground(hdc,pgl->x,pgl->y,pgl->width,pgl->height,
                   &pgl->bkground);
		//lciDispatchMessage(pgl, LAY_MSG_PAINT, hdc,0);
    }
}

extern void InitDesktopSenceLayout(GRID_LAYOUT* layout);
BOOL InitSence()
{
	//init desktop sennce
	desktopSence.layout = LoadGridLayoutFromEtcFile(CFGFILE, TOP_LAYOUT, NULL, NULL, loadIcons, NULL);
	InitDesktopSenceLayout(desktopSence.layout);
	lciDispatchMessage(desktopSence.layout,LAY_MSG_MOVETO, 0, &g_rcScr);

	//init aniamtes
	GRID_LAYOUT* leftGL = (GRID_LAYOUT*)GridLayoutGetCell(desktopSence.layout,LEFT_GL_ROW,LEFT_GL_COL);
	RECT rt = { leftGL->x, leftGL->y, leftGL->x + leftGL->width, leftGL->y + leftGL->height};
	InitMainAnimates(&rt, (void*)desktopSence.layout, desktopSenceClearBkground);	

	// get switch code
	if(GetIntValueFromEtcFile(CFGFILE,"system","switchkey",&switchkey)!=ETC_OK || switchkey == 0)
		switchkey = SCANCODE_F4;
    
	return TRUE;
}

//this function cause the switch of sceen
void SetCurrentSence(int newId)
{
	SENCE * s =  GetSenceById(newId);
	if(s !=g_curSence)
	{
		SenceSwitchOut(g_curSence);
		SenceSwitchIn(s);

		g_curSence = s;
	}
}

int desktopSenceProc(DESKTOP_SENCE *dsence, int message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
    case MSG_CREATE:
        break;
	case MSG_PAINT:
		lciDispatchMessage(dsence->layout,LAY_MSG_ERASEBKGND,g_hMainDC,lParam);	
		lciDispatchMessage(dsence->layout,LAY_MSG_PAINT, g_hMainDC,lParam);
		break;
	case MSG_ERASEBKGND:
		//lciDispatchMessage(dsence->layout,LAY_MSG_ERASEBKGND,g_hMainDC,lParam);	
		//break;
        return 0;
    case MSG_KEYUP:
#ifdef ENABLE_CLOSE_CORNER
    case MSG_RBUTTONDOWN:
#endif
		lciDispatchMessage(GridLayoutGetCell(dsence->layout,LEFT_GL_ROW,LEFT_GL_COL),message,wParam, lParam);
       break;
    case LAY_MSG_INFO:
		lciDispatchMessage(GridLayoutGetCell(dsence->layout,LEFT_GL_ROW,LEFT_GL_COL),message,wParam, lParam);
        break;
	default:
		return lciDispatchMessage(dsence->layout, message,wParam, lParam);	
	}
	return 0;
}

//////////////////////////////////////////////////////
//
extern BOOL clear_all_objs (LAYOUT_CELL_OBJ* cur,GRID_LAYOUT* this,int row, int col,void* except_obj);

static BOOL radiation_all_objs(LAYOUT_CELL_OBJ* cur,GRID_LAYOUT* this, int row, int col, void * param)
{
	icon_cell_t* ic = (icon_cell_t*)cur;

	if(ic->icon.a == NULL){
		ic->icon.a = AppendRadiationIcon(&ic->icon.normal_bmp,
			ic->icon.x,
			ic->icon.y,
			ic->icon.normal_bmp.bmWidth,
			ic->icon.normal_bmp.bmHeight,
			&ic->icon);
	}

}

static BOOL shrink_all_objs(LAYOUT_CELL_OBJ* cur,GRID_LAYOUT* this, int row, int col, void * param)
{
	icon_cell_t* ic = (icon_cell_t*) cur;	

	if(ic->icon.a == NULL){
		RECT rt;
		if(!GridLayoutGetCellRect(this, row, col,&rt))
			return FALSE;
		ic->icon.a = AppendShrinkIcon(&ic->icon.normal_bmp,
			ic->icon.x,
			ic->icon.y,
			ic->icon.normal_bmp.bmWidth,
			ic->icon.normal_bmp.bmHeight,
			(rt.left+rt.right-ic->icon.normal_bmp.bmWidth)/2,
			(rt.top+rt.bottom-ic->icon.normal_bmp.bmHeight)/2,
			&ic->icon
			);
	}
	return TRUE;
}

static PBITMAP get_cur_switch_bitmap()
{
	if(appSence.picon == NULL)
		return NULL;
	return &appSence.picon->bmpSwitch;			
}

void update_cur_switch_bitmap (void* bits)
{
	if (appSence.picon == NULL)
		return ;

    appSence.picon->bmpSwitch.bmType = BMP_TYPE_NORMAL;
    appSence.picon->bmpSwitch.bmBitsPerPixel = GetGDCapability (HDC_SCREEN, GDCAP_DEPTH);
    appSence.picon->bmpSwitch.bmBytesPerPixel = GetGDCapability (HDC_SCREEN, GDCAP_BPP);
    appSence.picon->bmpSwitch.bmAlpha = 0;
    appSence.picon->bmpSwitch.bmColorKey = 0;
    appSence.picon->bmpSwitch.bmWidth = RECTW (g_rcScr);
    appSence.picon->bmpSwitch.bmHeight = RECTH (g_rcScr);
    appSence.picon->bmpSwitch.bmPitch = GetGDCapability (HDC_SCREEN, GDCAP_PITCH);
    appSence.picon->bmpSwitch.bmBits = bits;
    //appSence.picon->bmpSwitch.bmAlphaPixelFormat = NULL;
}

static void reset_icons_pos(ANIMATE_SENCE* as)
{
	ANIMATE* a, *h;
	a = as->normal;
	h = a;

	while(a){
		PHONE_ICON* picon = (PHONE_ICON*)a->param;
		if(picon){
			picon->x = GetAnimateX(a);
			picon->y = GetAnimateY(a);
		}
		a = a->next;
		if(a == h)
			break;
	}
}

void desktopSwitchOut(DESKTOP_SENCE* _this)
{
    GRID_LAYOUT *glup,*gldown;

    printf ("desktop switch out.\n");

    BeginRadiationAndShrinkAnimates();

    /* set animation of layout cell[0][0] */
    GRID_LAYOUT* leftGL = (GRID_LAYOUT*)GridLayoutGetCell(_this->layout,LEFT_GL_ROW,LEFT_GL_COL);
    glup = (GRID_LAYOUT*) GridLayoutGetCell(leftGL,UP_ICON_ROW,UP_ICON_COL);
    GridLayoutEnumAllCellObjs (glup, radiation_all_objs, NULL); 

    /* set animation of layout cell[1][0] */
    gldown = (GRID_LAYOUT*) GridLayoutGetCell(leftGL,DOWN_ICON_ROW,DOWN_ICON_COL);
    GridLayoutEnumAllCellObjs (gldown, radiation_all_objs, NULL); 

    AppendScaleBitmap(get_cur_switch_bitmap(),
            0,0,
            RECTW(g_rcScr), RECTH(g_rcScr));
    RunRadiationAndShrinkAnimates(reset_icons_pos);

    GridLayoutEnumAllCellObjs(glup, clear_all_objs, NULL);
    GridLayoutEnumAllCellObjs(gldown, clear_all_objs, NULL);

    //set the desktop's bkgnd is scale bitmap
    SetDesktopBkgnd(get_cur_switch_bitmap());
    /* Hide desktop window */
    printf ("start hide ...\n");
    ShowWindow (g_hMainWnd, SW_HIDE);
    printf("---end \n");
}

void desktopSwitchIn(DESKTOP_SENCE* _this)
{
	static BITMAP bmpOut;	
	GRID_LAYOUT *glup , *gldown;

	BeginRadiationAndShrinkAnimates();

    /* set animation of layout cell[0][0] */
	GRID_LAYOUT* leftGL = (GRID_LAYOUT*)GridLayoutGetCell(_this->layout,LEFT_GL_ROW,LEFT_GL_COL);
	glup = (GRID_LAYOUT*) GridLayoutGetCell(leftGL,UP_ICON_ROW,UP_ICON_COL);
    GridLayoutEnumAllCellObjs (glup, shrink_all_objs, NULL); 

    /* set animation of layout cell[1][0] */
	gldown = (GRID_LAYOUT*) GridLayoutGetCell(leftGL,DOWN_ICON_ROW,DOWN_ICON_COL);
    GridLayoutEnumAllCellObjs (gldown, shrink_all_objs, NULL); 

	GetBitmapFromDC(HDC_SCREEN, 
		0, 0, 
		g_rcScr.right, g_rcScr.bottom,
		&bmpOut);
	AppendScaleBitmap(&bmpOut,
		RECTW(g_rcScr), RECTH(g_rcScr),
		0, 0);

	/* Show desktop window */
        //printf ("Start to show window ...\n");
        g_bAnimateShow = TRUE;

        ShowBusyWindow (FALSE); 
        ShowWindow (g_hMainWnd, SW_SHOW);

	SetDesktopBkgnd(NULL);

	RunRadiationAndShrinkAnimates(reset_icons_pos);

	GridLayoutEnumAllCellObjs(glup, clear_all_objs, NULL);
	GridLayoutEnumAllCellObjs(gldown, clear_all_objs, NULL);
    
}

////////////////////////////////////////////////////////////////////////////

int appSenceProc(DESKTOP_SENCE* asence, int message, WPARAM wParam, LPARAM lParam)
{
    if (message == MSG_RBUTTONDOWN || 
            (message == MSG_KEYUP && wParam ==  SCANCODE_F4)) {
        SetCurrentSence(SENCE_DESKTOP);
    }
    return 0;
}

void appSwitchOut(APP_SENCE *_this)
{
	kill(appSence.picon->pid,SIGKILL);
	appSence.picon->pid = 0;
}

void appSwitchIn(APP_SENCE *_this)
{
}

void SetAppSenceApp(PHONE_ICON* icon)
{
     appSence.picon = icon;
}



