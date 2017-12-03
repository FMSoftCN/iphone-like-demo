#ifndef SYSMAIN_H
#define SYSMAIN_H
#include "./screen_protect/screen_protect.h"
#include "./render/render.h"
#include "./date/timeset.h"
#include "./localnetwork/netcardset.h"
#include "./wifisetting/wifiset.h"
#include "./wifisetting/ipset.h"
#include <ebbutton.h>
#include <eblistview.h>
#include <ebcontrol.h>
#include <wifiapi.h>
HWND g_hSysMain;
RECT g_rcSysSetting;
HDC  g_hDoubleBuffer;
BITMAP g_SubMenuBitmap[10];

//#ifdef _TARGET_IPAQ
#define GOOD_WIFI
//#endif

#define MSG_CLOSE_APP           2000
#define MSG_RENDER_COLOR        2001
#define MSG_SYS_CLOSE           2002
#define MSG_WIFINET_CANCEL      2003

#define   PIC_PATH    "./res/systemsetting/"

#define SYSTEM_RES              PIC_PATH
#define ANIMATE_RES             SYSTEM_RES"animate/" 
#define TIME_RES                SYSTEM_RES"date/" 
#define SCREEN_SAVER_RES        SYSTEM_RES"screen_protect/"
#define LOCALNETWORK_RES        SYSTEM_RES"localnetwork/"
#define WIFISETTING_RES         SYSTEM_RES"wifisetting/"

void GUIAPI UpdateAll (HWND hWnd, BOOL fErase);
int DoubleBufferProc (HWND hWnd, HDC private_dc, HDC real_dc, const RECT*update_rc, const RECT* real_rc);
int CloseSYSIMEMethod (BOOL open);
int GetSearchAPNum (void);
#endif
