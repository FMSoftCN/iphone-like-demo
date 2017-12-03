#include "instruction.h"
#include "animate.h"

#include "iphone-animates.h"

static ANIMATE_SENCE switch_sence;
static TIME_LINE tl_image;
static TIME_LINE tl_icon_move;
static TIME_LINE tl_screenshot_scale;

#define IMAGE 0
#define END_X 1
#define END_Y 2
#define END_W 1
#define END_H 2
#define MAX_POOL 3

#define FRAME_NUM  5

static void init_timeline()
{
	//init frame timeline
	AppendTimeLineFrameInstruction(&tl_image, TRUE, IMAGE);
	AppendTimeLineBlankInstruction(&tl_image, FRAME_NUM-1);

	//init icon move timeline
	AppendTimeLineMoveInstruction(&tl_icon_move, TRUE, -1,-1, END_X, END_Y, FRAME_NUM);

	//init screenshot scale timeline
	AppendTimeLineScaleInstruction(&tl_screenshot_scale, TRUE, -1, -1, END_W, END_H, FRAME_NUM); 
}

static void init_switch_aniamtes()
{
}


BOOL DoDesktop2AppSwitch()
{
}

BOOL DoApp2DesktopSwitch()
{
}
