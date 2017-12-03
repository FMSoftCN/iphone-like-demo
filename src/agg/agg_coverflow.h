#ifndef AGG_COVERFLOW_INCLUDE
#define AGG_COVERFLOW_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <minigui/common.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

BOOL InitCoverFlow (char** pBitmapFile, int nCount, int nWidth, int nHeight);
BOOL CoverFlowMoveFromRightToLeftSingle (HDC hDC);
BOOL CoverFlowMoveFromLeftToRightSingle (HDC hDC);
int ReturnCurCoverFlowIndex (void);
BOOL ShowCoverFlow (HDC hDC);
BOOL RegisterCoverFlowRes (char* pCoverFlowImg);
BOOL StartUpCoverFlow (int nWidth, int nHeight);
BOOL InitCoverFlowByRegisterRes (void);

#ifdef __cplusplus
}
#endif

#endif
