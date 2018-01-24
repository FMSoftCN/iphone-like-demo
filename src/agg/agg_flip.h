
#ifndef AGG_FLIP_INCLUDE
#define AGG_FLIP_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <minigui/common.h>

BOOL LoadFrontBitmapFromDC(HWND hwnd);
BOOL LoadRearBitmapFromDC(HWND hwnd);
BOOL InitFlipAnimation(void);
void Agg_Flip(HDC hdc, int interval);

#ifdef __cplusplus
}
#endif

#endif
