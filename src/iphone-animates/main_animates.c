

#include <stdio.h>
#include <stdlib.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "animate/p-code.h"
#include "animate/animate.h"
#include "iphone-animates.h"

#include "layout.h"
#include "phone_icon.h"
#include "icon_layout.h"
#include "dragmanager.h"

#define SCALE_SIZE 125

#define DRAG_TIMELINE 1
#define MOVE_ICONS_TIMELINE 2

//////////////////////////////////////////

static ANIMATE_SENCE  mainSence;
static PCODE_METHOD* pm_swingAll;
static PCODE_METHOD* pm_releaseDragIcon;
//static PCODE_METHOD* pm_startMoveIcons;
static TIME_LINE* tl_moveicons = NULL;
static HDC hdc_mem = HDC_INVALID;

extern HDC g_hMainDC;
static HDC hdc_buffer = HDC_INVALID;

static void on_end_draw_one_frame(ANIMATE_SENCE *as)
{
	if(as != NULL)
	{
//		printf("on_end_draw_one_frame:%d,%d\n",RECTW(as->rtArea), RECTH(as->rtArea));
		BitBlt(as->hdc, 0, 0, RECTW(as->rtArea),RECTH(as->rtArea), HDC_SCREEN, 0, 0, 0); 
	}
}

//called by p-code method, when swing icons
static unsigned int change_objs_swing(int *param, int param_count)
{
	static _idx = 0;
	ANIMATE_SENCE * as = (ANIMATE_SENCE*)param[0];
	ANIMATE* a = as->normal;
	ANIMATE* h = a;

	while(a){
		BITMAP *bmps = (BITMAP*)a->param;
        if(a->img == (bmps + _idx%2))
            _idx ++;

		a->img = bmps + _idx%2;

		_idx ++;

		_idx &= 0xFFFF;

		a = a->next;
		if(a == h)
			break;
	}
	return 0;
}

//called by p-code method, when releaseDragedIcons' animate finished
static void (*p_on_end_release_draged_icon)(void);
static unsigned int clear_topmost(int *param, int param_count)
{
	RemoveAnimateFromTopMost(&mainSence, mainSence.topmost);
	if(p_on_end_release_draged_icon)
		(*p_on_end_release_draged_icon)();
	p_on_end_release_draged_icon = NULL;
	return 0;
}

static PCODE_NATIVE_METHOD mainAnimateNativeCallback[] = {
	{ change_objs_swing, 1},
	{ clear_topmost, 0 }
};

static PCODE_NATIVE_METHOD_TABLE mainAnimatesNativeTable={
	mainAnimateNativeCallback,
	sizeof(mainAnimateNativeCallback)/sizeof(PCODE_NATIVE_METHOD)
};

static void drawIcon(HDC hdc, ANIMATE* a)
{
	//printf("drawIcon: %p,%d,%d,%d,%d,%d,img:%p\n",a,GetAnimateX(a), GetAnimateY(a), GetAnimateW(a), GetAnimateH(a), GetAnimateA(a),a->img);
	if(GetAnimateW(a) == 0 || GetAnimateH(a) == 0)
		return;

	if(GetAnimateA(a) >= 250){
//		printf("-- %s:%d\n",__FUNCTION__,__LINE__);
		FillBoxWithBitmap(hdc, GetAnimateX(a), GetAnimateY(a), GetAnimateW(a), GetAnimateH(a), a->img);
	}
	else{
		//
		static int _w = 0;
		static int _h = 0;
		static void* _img = NULL;
		
		//printf("--%s:%d: %p,%d,%d,%d,%d,%d,img:%p\n",__FUNCTION__,__LINE__,a,GetAnimateX(a), GetAnimateY(a), GetAnimateW(a), GetAnimateH(a), GetAnimateA(a),a->img);

		gal_pixel bkcolor = RGB2Pixel(hdc_mem,3,3,3);
		if(_w != GetAnimateW(a) || _h != GetAnimateH(a) || _img != a->img)
		{
			_w = GetAnimateW(a);
			_h = GetAnimateH(a);
			_img = a->img;
			
			SetBrushColor(hdc_mem, bkcolor);
			SetBrushType(hdc_mem, BT_SOLID);
			FillBox(hdc_mem, 0, 0, _w, _h);
			FillBoxWithBitmap(hdc_mem, 0, 0, _w, _h, (PBITMAP)_img);
		}
		SetMemDCAlpha(hdc_mem, MEMDC_FLAG_SRCALPHA, GetAnimateA(a));
		SetMemDCColorKey(hdc_mem, MEMDC_FLAG_SRCCOLORKEY, bkcolor);
		BitBlt(hdc_mem, 0, 0, _w, _h, hdc, GetAnimateX(a),GetAnimateY(a), 0 );
	}
}

BOOL InitMainAnimates(const RECT* rt, void* userParam, void(*drawbkgnd)(HDC, const RECT*, void*))
{
	SetNativeCallback(&mainAnimatesNativeTable);

	if(!LoadAnimateFile("animates.pcc")){
		fprintf(stderr,"annimates.pcc is not exists of not right\n");
		return FALSE;
	}
	
	//printf("rt: %d,%d,%d,%d\n", rt->left, rt->top, rt->right, rt->bottom);
	pm_swingAll = GetTimeLineMethod("swingAll");
	pm_releaseDragIcon = GetTimeLineMethod("releaseDragIcon");
	//pm_startMoveIcons = GetTimeLineMethod("startMoveIcons");
	
//	printf("init main animates:%p\n",drawbkgnd);

	return InitAnimateSence(&mainSence, 100, -1, g_hMainDC, drawIcon, drawbkgnd, NULL,rt,userParam);
}

ANIMATE* AppendMainAnimateIcons(int x, int y,int w, int h, BITMAP* bmps, BOOL is_topmost)
{
	ANIMATE* a;
	int i;

	a = (ANIMATE*)calloc(1, sizeof(ANIMATE));

	SetAnimateX(a,x);
	SetAnimateY(a,y);
	SetAnimateW(a,w);
	SetAnimateH(a,h);
	SetAnimateA(a,255);
	a->param = bmps;
	a->img = bmps;
	
	InsertAnimate(&mainSence, a, is_topmost);

	if(hdc_mem == HDC_INVALID)
	{
		hdc_mem = CreateCompatibleDCEx(g_hMainDC, w*SCALE_SIZE/100, h*SCALE_SIZE/100);
	}

	return a;
}

BOOL LanchMainAnimates()
{
	int args[1];
	TIME_LINE* tl ;

	if(mainSence.state == assRunning || mainSence.state == assPaused)
		return TRUE;
	
	//printf("mainSence.rtArea:%d,%d,%d,%d\n",mainSence.rtArea.left,mainSence.rtArea.top, mainSence.rtArea.right, mainSence.rtArea.bottom);
	if(hdc_buffer == HDC_INVALID){
		hdc_buffer = CreateCompatibleDCEx(g_hMainDC, RECTW(mainSence.rtArea), RECTH(mainSence.rtArea));
		mainSence.hdc = hdc_buffer;
	}

	mainSence.end_draw_one_frame = on_end_draw_one_frame;
	mainSence.state = assReady;
	tl = CreateTimeLine(-1);
	args[0] = (int)&mainSence;
	StartTimeLine(&mainSence, tl, pm_swingAll,8,args);
	StartThreadAnimateSence(&mainSence);
	return TRUE;
}

void SetDragedIcon(ANIMATE* a, int x, int y)
{
	int w, h;
	BOOL is_get = TRUE;

	TIME_LINE* tl;

	AS_LOCK(&mainSence);

	tl = GetTimeLineByID(&mainSence,DRAG_TIMELINE);
	if(tl == NULL){
		is_get = FALSE;
		tl = CreateTimeLine(DRAG_TIMELINE);
	}

	w = GetAnimateW(a)*SCALE_SIZE/100;
	h = GetAnimateH(a)*SCALE_SIZE/100;
	x -= (w/2);
	y -= (h/2);

	TLMoveTo(tl, a, x, y,3);
	TLScaleTo(tl,a, w, h,3);
	TLAlphaTo(tl, a, 128,3);
	a->img = ((BITMAP*)a->param)+2;
	TLRun(tl,3);

	MoveAnimateToTopMost(&mainSence, a);
	AS_UNLOCK(&mainSence);
	if(!is_get)
		StartTimeLine(&mainSence, tl, NULL, 0, NULL);
}

void ReleaseDragedIcon(int x, int y, int w, int h, void (*on_end_animate)(void))
{
	int args[5];
	TIME_LINE* tl; 

	if(mainSence.topmost == NULL)
		return;

	args[0] = (int)mainSence.topmost;
	args[1] = x;
	args[2] = y;
	args[3] = w;
	args[4] = h;

	p_on_end_release_draged_icon = on_end_animate;
	
	AS_LOCK(&mainSence);
	tl = GetTimeLineByID(&mainSence, DRAG_TIMELINE);
	if(tl == NULL){
		tl = CreateTimeLine(DRAG_TIMELINE);
		AS_UNLOCK(&mainSence);
		StartTimeLine(&mainSence,tl,pm_releaseDragIcon,32,args);
	}
	else{
		SetTimeLineMethod(tl, pm_releaseDragIcon, 32, args);
		AS_UNLOCK(&mainSence);
	}	
}

void SetDragedIconPosition(int x, int y)
{
	ANIMATE* a = mainSence.topmost;
	if(a == NULL)
		return;
	x -= (GetAnimateW(a)/2);
	y -= (GetAnimateH(a)/2);
	AS_LOCK(&mainSence);
	SetAnimateX(a,x);
	SetAnimateY(a,y);
	AS_UNLOCK(&mainSence);
}

void StartMoveIcons()
{
	tl_moveicons = GetTimeLineByID(&mainSence,MOVE_ICONS_TIMELINE);
	if(tl_moveicons == NULL){
		tl_moveicons = CreateTimeLine(MOVE_ICONS_TIMELINE);
	}
}

void SetMoveIcon(ANIMATE*a, int x, int y)
{
	if(tl_moveicons == NULL)
		return;

	if(a == NULL)
		return;

	AS_LOCK(&mainSence);
	TLMoveTo(tl_moveicons, a, x, y, 5);
	AS_UNLOCK(&mainSence);
}

void CancelMoveIcons()
{
	if(tl_moveicons)
		TLStopAnimateStep(tl_moveicons, NULL, tlstMove|tlstScale|tlstAlpha,FALSE);
}

void RunMoveIcons()
{
	if(tl_moveicons == NULL)
		return;

/*	if(tl_moveicons->env == NULL){
		int argv[1] = {&tl_moveicons};
		StartTimeLine(&mainSence, tl_moveicons,pm_startMoveIcons, 8, argv);
	}
	else{
		AS_LOCK(&mainSence);
		//restart the method
		ResetPCodeExecute(tl_moveicons->env);
		AS_UNLOCK(&mainSence);
	}
	*/
	
	AS_LOCK(&mainSence);
	TLRun(tl_moveicons, 5);
	AS_UNLOCK(&mainSence);
	StartTimeLine(&mainSence, tl_moveicons, NULL, 0, NULL);
}

BOOL TerminateMainAnimate()
{
	DeinitAnimateSence(&mainSence,NULL);
	if(hdc_mem != HDC_INVALID){
		DeleteMemDC(hdc_mem);
		hdc_mem = HDC_INVALID;
	}

	DeleteMemDC(hdc_buffer);
	hdc_buffer = HDC_INVALID;
}

///////////////////////////////////////////////////////////////////////////////////
//animates for icon radiation and shrink
#if 0
#define RAD_TIME 5
/*(x2, y2)--(x0, y0) is times of (x1, y1)--(x0, y0)
 * times is 16.16 fixed decimal*/
static void inline get_rad_target (int x0, int y0, int x1, int y1, 
        int* x2, int* y2)
{
	printf("---x0=%d,y0=%d,x1=%d,y1=%d\n",x0,y0,x1,y1);
    *x2 = x0 + ((x1 - x0) * RAD_TIME);
    *y2 = y0 + ((y1 - y0) * RAD_TIME);
}
#endif

#define DISTANCE(a,b)   ((a)>(b)?((a)-(b)):((b)-(a)))
static void get_rad_target(const RECT *rt, int x, int y, int w, int h, int *xt, int *yt)
{
	int xo = ( rt->left + rt->right ) / 2;
	int yo = ( rt->top + rt->bottom ) / 2;
	int left = rt->left - w/2;
	int top = rt->top - h/2;
	int right = rt->right + w/2;
	int bottom = rt->bottom + h/2;
	int nearst;
	
	x += (w/2);
	y += (h/2);

	// 找到和x,y最近的rt的边, 和(x0,y0)-(x,y)的交点
	// 为了方便计算, 我们把x,y取对象中点,rt扩展w,h长度
	
	//找到x,y最接近的边
	{
		int d[4]; //d1 is :(left, top)-(right,top), clockwise
		d[0] = DISTANCE(y,top);
		d[1] = DISTANCE(x,right);
		d[2] = DISTANCE(y,bottom);
		d[3] = DISTANCE(x,left);
		int i;
		nearst = 0;
		for(i=1; i<4; i++){
			if(d[nearst] > d[i]){
				nearst = i;
			}
		}
//		printf("nearst = %d, %d,%d,%d,%d,(%d,%d,%d,%d)\n",nearst, d[0],d[1],d[2],d[3], left, top, right, bottom);
	}
	
	switch(nearst)
	{
	case 0: // left,top - right,top
		*yt = top;
		if(y!=yo)
			*xt = (x-xo)*(*yt-yo)/(y-yo) + xo ;
		break;
	case 1: // right, top - right, bottom
		*xt = right;
		if(x!=xo)
			*yt = (y-yo)*(*xt-xo)/(x-xo) + yo;
		break;
	case 2: // right,bottom - left, bottom
		*yt = bottom;
		if(y!=yo)
			*xt = (x-xo)*(*yt-yo)/(y-yo) + xo ;
		break;
	case 3: // left,bottom - left, top
		*xt = left;
		if(x!=xo)
			*yt = (y-yo)*(*xt-xo)/(x-xo) + yo;
		break;
	}

	*xt -= (w/2);
	*yt -= (h/2);

}

static TIME_LINE* tl_radiation_shrink = NULL;
BOOL BeginRadiationAndShrinkAnimates()
{
//	printf("main animates:%p\n",mainSence.draw_bkgnd);
	if(tl_radiation_shrink != NULL)
		return FALSE;
	tl_radiation_shrink = CreateTimeLine(-1);
	return TRUE;
}

ANIMATE*  AppendRadiationIcon(int *bmp, int x, int y, int w, int h, void *param)
{
	int x1, y1;
	ANIMATE* a = (ANIMATE*) calloc(1, sizeof(ANIMATE));
	
	SetAnimateX(a, x);
	SetAnimateY(a, y);
	SetAnimateW(a, w);
	SetAnimateH(a, h);
	SetAnimateA(a, 255);
	a->img = bmp;
	a->param = param;

	get_rad_target(
		&mainSence.rtArea,
		x,
		y,
		w,
		h,
		&x1,
		&y1);
//	printf("obj:x=%d, y=%d, x1=%d, y1=%d\n",x,y,x1,y1);
	
	TLMoveTo(tl_radiation_shrink, a, x1, y1, 5);
	
	InsertAnimate(&mainSence, a, FALSE);

	return a;

}

ANIMATE* AppendShrinkIcon(int *bmp, int x0, int y0, int w, int h, int x1, int y1, void* param)
{
	ANIMATE* a = (ANIMATE*) calloc(1, sizeof(ANIMATE));

	SetAnimateX(a, x0);
	SetAnimateY(a, y0);
	SetAnimateW(a, w);
	SetAnimateH(a, h);
	SetAnimateA(a, 255);

	a->img = bmp;
	a->param = param;

	TLMoveTo(tl_radiation_shrink, a, x1, y1, 5);

	InsertAnimate(&mainSence, a, FALSE);
	return a;
	
}

ANIMATE* AppendScaleBitmap(int *bmp, int w0, int h0, int w1, int h1)
{
#define GetX(w) ((mainSence.rtArea.left + mainSence.rtArea.right - (w)) /2)
#define GetY(h) ((mainSence.rtArea.top + mainSence.rtArea.bottom - (h)) /2)

	ANIMATE* a = (ANIMATE*) calloc(1, sizeof(ANIMATE));

	SetAnimateX(a, GetX(w0));
	SetAnimateY(a, GetY(h0));
	SetAnimateW(a, w0);
	SetAnimateH(a, h0);
	SetAnimateA(a, 255);

	a->img = bmp;
	
	TLMoveTo(tl_radiation_shrink, a, GetX(w1), GetY(h1), 5);
	TLScaleTo(tl_radiation_shrink, a, w1, h1, 5);

	InsertAnimate(&mainSence, a, FALSE);

#undef GetX
#undef GetY
	return a;
}

BOOL RunRadiationAndShrinkAnimates( void (*on_finished)(ANIMATE_SENCE* as))
{
	if(tl_radiation_shrink == NULL)
		return FALSE;

	if(hdc_buffer == HDC_INVALID){
		hdc_buffer = CreateCompatibleDCEx(g_hMainDC, RECTW(mainSence.rtArea), RECTH(mainSence.rtArea));
		mainSence.hdc = hdc_buffer;
	}

	TLRun(tl_radiation_shrink, 5);

	StartTimeLine(&mainSence,tl_radiation_shrink, NULL, 0, NULL);

	mainSence.on_finished = on_finished;
	mainSence.end_draw_one_frame = on_end_draw_one_frame;
	mainSence.state = assReady;

	StartAnimateSence(&mainSence);	

	tl_radiation_shrink = NULL;
	mainSence.on_finished = NULL;
	mainSence.hdc = HDC_SCREEN;

	DeleteMemDC(hdc_buffer);
	hdc_buffer = HDC_INVALID;
}

