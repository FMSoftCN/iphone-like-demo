/* 
** $Id: helloworld.c,v 1.36 2007-08-30 01:20:10 xwyan Exp $
**
** Listing 2.1
**
** helloworld.c: Sample program for MiniGUI Programming Guide
**      The first MiniGUI application.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "softkeywindow.h"
#include "common.h"
#include "sence.h"
#include "layout.h"
#include "layouthlp.h"
#include "phone_icon.h"

#include "dragmanager.h"
#include "icon_layout.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


BOOL is_proc_active(pid_t pid)
{
	char szText[256];
	sprintf(szText,"/proc/%d/stat",pid);
	int fd = open(szText, O_RDONLY,S_IRUSR);
	if(fd <= 0)
		return FALSE;
	int len = read(fd,szText,sizeof(szText));
	close(fd);
	int i;
	for(i=0;i<sizeof(szText) && szText[i]!=')' ;i++);
	while(szText[i]<'A' || szText[i]>'Z') i++;
	return szText[i]=='R' || szText[i]=='S';
}

static pid_t g_pid = -1;
static char* argv[10];

static prase_command(const char* cmd)
{
	static char buffer[256];
	int i = 0;
	char* str = buffer;

	memset(argv, 0, sizeof(argv));
	if(!cmd) return;

	argv[i] = str;

	while(*cmd && str - buffer < 256 && i < 10) {

		if (*cmd == ' ') {
			*(str++) = '\0';
			cmd++;
			argv[++i] = str;
		}

		*(str++) = *(cmd++);
	}
	*str = 0;
}

pid_t exec_app (const char* file_name, const char* app_name)
{
	int i=0;
	prase_command(app_name);
#if 0
	while(i<= 9) {
		printf("argv[%d]=[%08x][%s]\n", i, argv[i],argv[i]);
		i++;
	}
#endif

    pid_t pid = 0;
    if ((pid = vfork ()) > 0) {
        fprintf (stderr, "new child, pid: %d.\n", pid);
		if (strcmp(argv[0], "./browser") == 0) {
			EnableSoftKeypad (FALSE);
		} else {
			EnableSoftKeypad (TRUE);
		}

    }
    else if (pid == 0) {
        execv (argv[0], argv);
	    prase_command ("./others");
        execv (argv[0], argv);
        perror ("execv");
        _exit (1);
    }
    else {
        perror ("vfork");
    }

    return pid;
}

extern BOOL g_bAppStarted;
extern void* g_pSharedBuffer;

void update_cur_switch_bitmap (void* bits);

static BOOL wait_for_app_start (void)
{
    MSG msg;
    unsigned int tick_count = GetTickCount ();

    ShowBusyWindow (TRUE);
    while (GetMessage (&msg, HWND_DESKTOP)) {
        if (g_bAppStarted) {
            update_cur_switch_bitmap (g_pSharedBuffer);
            break;
        }
        else if (GetTickCount () > (tick_count + 100*30))
            break;

        DispatchMessage (&msg);
    }

    ShowBusyWindow (FALSE);
    return g_bAppStarted;
}

static int iconCellDispatchMessage(void * this, int message, WPARAM wParam, LPARAM lParam)
{
    int x;
    int y;
    struct icon_cell_t* ic = this;
	switch(message)
	{
		case LAY_MSG_PAINT:
			{
				FillBoxWithBitmap((HDC)wParam, ic->icon.x, ic->icon.y, -1, -1, &ic->icon.normal_bmp);
			}
			break;
        case LAY_MSG_MOVETO:
            {
                PRECT prt = (PRECT)lParam;
        		ic->icon.x = (prt->left+prt->right - ic->icon.normal_bmp.bmWidth)/2;
		    	ic->icon.y = (prt->top+prt->bottom - ic->icon.normal_bmp.bmHeight)/2;
            }
            break;

        case MSG_LBUTTONUP: {
            int x, y;
            x = LOWORD (lParam);
            y = HIWORD (lParam);

            
            if (!ic->icon.command)
                break;
 
            if (is_hitted_icon (&ic->icon, x, y)) {
                if (ic->icon.pid <= 0 || !is_proc_active(ic->icon.pid)) {
                    g_bAppStarted = FALSE;
                    ic->icon.pid = exec_app (ic->icon.command,
							ic->icon.command);
                    SetAppSenceApp (&ic->icon);
                    wait_for_app_start ();
                }
                SetAppSenceApp (&ic->icon);
                SetCurrentSence (SENCE_APP);
            }

           break;
       }
    }
    return 0;
}

static void iconCellFree(void * this)
{
    struct icon_cell_t* ic = this;
	UnloadBitmap(&ic->icon.normal_bmp);
	UnloadBitmap(&ic->icon.l_slant_bmp);
	UnloadBitmap(&ic->icon.r_slant_bmp);
	if(ic->icon.command)
		free(ic->icon.command);
	if(ic->icon.name)
		free(ic->icon.name);
	free(ic);
}

static LAYOUT_CELL_INTF iconCellIntf={
	iconCellDispatchMessage,
	iconCellFree
};

//#define _USE_ICON_TEXT
#ifdef _USE_ICON_TEXT

static void get_bitmap(HDC hdc, const char* fileName, PBITMAP pbmp, const char* strName)
{
	BITMAP bmp;
	HDC hmdc;
	int i;
	Uint8* src, * dst;
	int text_height;

	if(LoadBitmapFromFile(hdc, &bmp, fileName)!= 0 )
		return;
	
	text_height = GetFontHeight(hdc);

	//copy the struct of BITMAP
	memcpy(pbmp, &bmp, sizeof(bmp));

	//extern the bitmap
	pbmp->bmHeight += text_height;						

	//alloc the bits for new bitmap
	pbmp->bmBits = (Uint8*)malloc(pbmp->bmHeight*pbmp->bmPitch);
	//copy bits from old bitmap
	src = bmp.bmBits;
	dst = pbmp->bmBits;
	for(i=0;i<bmp.bmHeight;i++){
		memcpy(dst, src, bmp.bmPitch);
		src += bmp.bmPitch;
		dst += bmp.bmPitch;
	}

	// very important, to void UnloadBitmap(&bmp)
	// to free the bmAlphaPixelFormat
	bmp.bmAlphaPixelFormat = NULL;

	//tet a dc, and draw text in it
	hmdc = CreateMemDCFromBitmap(hdc, pbmp);
	
	if(hmdc != HDC_INVALID)
	{
		RECT rt = {0, bmp.bmHeight, bmp.bmWidth, pbmp->bmHeight};
		SetBrushColor(hmdc, pbmp->bmColorKey);
		FillBox(hmdc, 0, bmp.bmHeight, bmp.bmWidth, text_height);
		//FillBox(hmdc, 0, 0, bmp.bmWidth, pbmp->bmHeight);
		//FillBoxWithBitmap(hmdc, 0, 0, 0,0, &bmp);
		SetBkMode(hmdc, BM_TRANSPARENT);
		DrawText(hmdc, strName, strlen(strName), &rt, DT_VCENTER|DT_CENTER);
		DeleteMemDC(hmdc);
	}

	UnloadBitmap(&bmp);
}

#define GET_BMP(key, pbmp, name) if(GetValueFromEtcFile(etcFile,sectName,key,szvalue, sizeof(szvalue)-1) == ETC_OK){ \
	get_bitmap(HDC_SCREEN, szvalue, pbmp, name); }

#else

#define GET_BMP(key, pbmp, name) if(GetValueFromEtcFile(etcFile,sectName,key,szvalue, sizeof(szvalue)-1) == ETC_OK){ \
	LoadBitmapFromFile(HDC_SCREEN, pbmp, szvalue); }
#endif

#define GET_NORMAL_BMP(key, pbmp) if(GetValueFromEtcFile(etcFile,sectName,key,szvalue, sizeof(szvalue)-1) == ETC_OK){ \
	LoadBitmapFromFile(HDC_SCREEN, pbmp, szvalue); }
#define GET_STR(key)  (GetValueFromEtcFile(etcFile,sectName,key,szvalue, sizeof(szvalue)-1)==ETC_OK?strdup(szvalue):NULL)
LAYOUT_CELL_OBJ* loadIcons(const char* etcFile,const char*sectName, void *data)
{
	struct icon_cell_t *ic;
	PHONE_ICON * picon;
	char szvalue[256];
	ic = (struct icon_cell_t*) calloc(sizeof(struct icon_cell_t),1);
	ic->intf = &iconCellIntf;
	
	picon = &ic->icon;

	picon->name = GET_STR("name");
	picon->command = GET_STR("command");

	GET_BMP("normal", &picon->normal_bmp, picon->name);
	GET_BMP("slant_left", &picon->l_slant_bmp,picon->name);
	GET_BMP("slant_right", &picon->r_slant_bmp,picon->name);
	//GET_NORMAL_BMP("switch_bmp", &picon->bmpSwitch);

	return (LAYOUT_CELL_OBJ*)ic;
}
#undef GET_BMP
#undef GET_STR

