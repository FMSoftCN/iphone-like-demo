
#ifndef IPSET_H
#define IPSET_H  

#include <stdlib.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "./../sysmain.h"

/* define the three ipset type: hcdp,static */

#define TYPE_DHCP       0x800
#define TYPE_STATIC     0x801
#define TYPE_BOOTP      0x802
#define TYPE_NONE       0x803

HWND CreateIpsetWindow(void);

#endif
