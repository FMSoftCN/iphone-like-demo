#ifndef __MLS_SFKBD_H__
#define __MLS_SFKBD_H__

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#define MLS_KBD_SHOWN		1
#define MLS_KBD_ACTIVE		2

int MLSKeyboardInit(RECT, RECT, int, int, int, SRVEVTHOOK);
int MLSKeyboardFini();
void MLSKeyboardShow(BOOL show);
void MLSKeyboardSetActive(BOOL active);
int MLSKeyboardGetState();
void MLSKeyboardSetState(int state);
void MLSKeyboardSetMonitorRect(RECT* rect);
#endif
