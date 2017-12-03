
#ifndef IPHONE_ANIMATE_H
#define IPHONE_ANIMATE_H


BOOL InitMainAnimates(const RECT *rt, void* userParam, void(*drawbkgnd)(HDC, const RECT*,void*) );

//////////////////////////////////////////////////
ANIMATE* AppendMainAnimateIcons(int x, int y, int w, int h, BITMAP* bmps, BOOL is_topmost);

BOOL LanchMainAnimates();

void SetDragedIcon(ANIMATE* a, int x, int y);

void ReleaseDragedIcon(int x, int y, int w, int h, void(*on_animate_end)(void));

void SetDragedIconPosition(int x, int y);

void StartMoveIcons();

void SetMoveIcon(ANIMATE*a, int x, int y);

void RunMoveIcons();

void CancelMoveIcons();

BOOL TerminateMainAnimate();


////////////////////////////////////////////////////
//switch bitmap application
//
BOOL BeginRadiationAndShrinkAnimates();

ANIMATE*  AppendRadiationIcon(int *bmp, int x, int y, int w, int h, void* param);

ANIMATE* AppendShrinkIcon(int *bmp, int x0, int y0, int w, int h, int x1, int y1, void* param);


ANIMATE* AppendScaleBitmap(int *bmp, int w0, int h0, int w1, int h1);

BOOL RunRadiationAndShrinkAnimates( void (*on_finished)(ANIMATE_SENCE* as));

#endif
