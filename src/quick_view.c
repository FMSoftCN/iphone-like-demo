

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "quick_view.h"

#define MAX_LEVEL 16

extern HWND g_hMainWnd;
extern HDC g_hMainDC;
static BITMAP init_bmp;
static BOOL show = FALSE;
static HDC memdc = 0;
static int level = 0;
static int ticket = 0;
static BOOL once = TRUE;
void Hide_QuickView(HDC hdc , BOOL draw_once)
{
    if (show) {
        RECT rc = {210, 0, 320, 240};
        IncludeClipRect(hdc, &rc);
        show = FALSE;
        once = draw_once;
    }
}

static void Step_QuickView(HDC hdc)
{
    if (!show && ticket > 10) {
        RECT rc = {210, 0, 320, 240};
        if (level < MAX_LEVEL) {
            SetMemDCAlpha (memdc, MEMDC_FLAG_SRCALPHA, level*16); 
            FillBoxWithBitmap (memdc, 0, 0, -1, -1, &init_bmp);
            IncludeClipRect(hdc, &rc);
            BitBlt(memdc, 0, 0, 110, 240, hdc, 210, 0 , 0);
            ExcludeClipRect(hdc, &rc);
            level++;
        } else {
            FillBoxWithBitmap (hdc, 210, 0, -1, -1, &init_bmp);
            ExcludeClipRect(hdc, &rc);
            show = TRUE;
            level = 0;
        }
        ticket = 0;
    }
    ticket++;
}


static void First_QuickView(HDC hdc)
{
    if (!show) {
        RECT rc = {210, 0, 320, 240};
        FillBoxWithBitmap (hdc, 210, 0, -1, -1, &init_bmp);
        ExcludeClipRect(hdc, &rc);
        show = TRUE;
    }
}

void Show_QuickView(HDC hdc)
{
    if (once)
        First_QuickView(hdc);
    else
        Step_QuickView(hdc);
}

void Init_QuickView()
{
    LoadBitmap (HDC_SCREEN, &init_bmp, "res/init_quickview.png");
    memdc = CreateCompatibleDCEx(HDC_SCREEN, 110, 240);
}

struct name_patch {
    char * name;
    char * path;
}pic_table[] ={
    {"email", "res/nk.png"},
    {"calender", "res/nd.png"},
    {"picture", "res/nl.png"},
    {"calculator", "res/nf.png"},
    {"watch", "res/nc.png"},
    {"weather", "res/nb.png"},
    {"note", "res/nh.png"},
    {"stock", "res/ng.png"},
    {"music", "res/ni.png"},
    {"phone", "res/nj.png"},
    {"sms", "res/na.png"},
    {"browser", "res/ne.png"},
};


static char * get_pic_form_name (const char * name)
{
    int i;
    for (i=0; i<TABLESIZE(pic_table); i++)
        if (strcasecmp(pic_table[i].name, name) == 0)
            return pic_table[i].path;
    return NULL;
}

void Switch_QuickView(const char * name , const char * command)
{
    char * path = NULL; 
    if (name) {
        path = get_pic_form_name(name);
        UnloadBitmap(&init_bmp);
        if (path)
            LoadBitmap (HDC_SCREEN, &init_bmp, path);
        else
            LoadBitmap (HDC_SCREEN, &init_bmp, "res/init_quickview.png");
        Hide_QuickView(g_hMainDC, FALSE);
    } else {
        UnloadBitmap(&init_bmp);
        LoadBitmap (HDC_SCREEN, &init_bmp, "res/init_quickview.png");
        Hide_QuickView(g_hMainDC, FALSE);
    }
}
