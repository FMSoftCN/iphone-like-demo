/*
 * layout help functions
 *
 */

#ifndef LAYOUT_HELP_H
#define LAYOUT_HELP_H

#include "layout.h"
/*
 * Load GridLayout form etc file
 * the section keys of GridLayout
 * [<name>]
 * type=gridlayout ; must be gridlayout
 * cols=<number>
 * rows=<number>
 * col<n>=<number>|<number>%  ; n must be 0~cols-1
 * row<n>=<number>|<number>%  ; n must be 0~rows-1
 * cell<row>-<col>=<section name> 
 */

GRID_LAYOUT* LoadGridLayoutFromEtcFile(const char* etcFile, const char* name, PGridLayoutProc glProc, void* user_data, 
	LAYOUT_CELL_OBJ* (*loadCell)(const char* etcFile, const char* sectName, void* data), void* data);


typedef struct window_layout{
	LAYOUT_CELL_INTF* intf;
	HWND hwnd;
}WND_LAYOUT;

WND_LAYOUT* CreateWndLayout(HWND hwnd);

#endif
