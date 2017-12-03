
#include "instruction.h"
#include "animate.h"
#include "iphone-animates.h"

enum previewPoolType{
	IMAGE = 0,
	BEGIN_ALPHA,
	END_ALPHA,
	MAX_POOL
};

static ANIMATE_SENCE previewSence;
static ANIMATE pvOutAnimate, pvInAnimate;

static void do_nothing_draw_bkgnd(HDC hdc, const RECT* rt, void *param)
{
}

static void init_preview_animates()
{
	//init sence
	//set right rtPreview
	RECT rtPreview={0,0,0,0};
	
	InitAnimateSence(&previewSence, 250, -1, g_hMainDC, do_nothing_draw_bkgnd, NULL , &rtPrevView, NULL);

	//init timeline	
	AppendAnimateFrameInstruction(&pvOutAnimate, FRAME_time_line, TRUE, IMAGE);
	AppendAnimateBlankInstruction(&pvOutAnimate, FRAME_time_line, 4);
	AppendAniateAlphaInstruction(&pvOutAnimate, ALPHA_time_line, TRUE, BEGIN_ALPHA, END_ALPHA, 5);

	CloneAnimateTimeLine(pvInAnimate.time_lines[FRAME_time_line], &pvOutAnimate, FRAME_time_line);
	CloneAnimateTimeLine(pvInAnimate.time_lines[ALPHA_time_line], &pvOutAnimate, ALPHA_time_line);

	pvOutAnimate.instrct_data_pool = (int*)calloc(MAX_POOL, sizeof(int));
	pvOutAnimate.instrct_data_pool_len = MAX_POOL;

	pvInAnimate.instrct_data_pool = (int*)calloc(MAX_POOL, sizeof(int));
	pvInAnimate.instrct_data_pool_len = MAX_POOL;
}

BOOL DoPreviewAnimate(BITMAP* bmp_in, BITMAP* bmp_out)
{
	static int inited = 0;
	if(!inited){
		init_preview_animates();
		inited = 1;
	}

	pvInAnimate.instrct_data_pool[IMAGE] = bmp_in;
	pvInAnimate.instrct_data_pool[BEGIN_ALPHA] = 0;
	pvInAnimate.instrct_data_pool[END_ALPHA] = 255;

	pvOutAnimate.instrct_data_pool[IMAGE] = bmp_out;
	pvOutAnimate.instrct_data_pool[BEGIN_ALPHA] = 255;
	pvOutAnimate.instrct_data_pool[END_ALPHA] = 0;

	//start timer
	StartTimerAnimateSence(&previewSence);

	return TRUE;
}

