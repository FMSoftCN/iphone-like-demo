#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include <minigui/common.h>
#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "common.h"
#include "picture.h"
#include "../sharedbuff.h"
#include "common_animates/common_animates.h"
#include <agg_coverflow.h>

#define SCR_W         240
#define SCR_H         320

static NODE_LIST g_nl;
static MOUSE_OP g_mouse;

static char g_fileName[512];

//static BITMAP bmpBk;
static HDC memDC;

/* for rollpage animate. */
static HDC g_DownDC;
static HDC g_UpDC;

static int g_nTestFlame = 0;
static int g_maxframe = 0;
static BOOL scroll_dir = TRUE;
static BITMAP g_stBitmap;

static void DrawAnimateDefault (HDC hdc, ANIMATE* pAnimate)
{
    static int step = 1;
    if (GetAnimateW (pAnimate) != 0 && GetAnimateH (pAnimate) != 0) 
    {
        if (scroll_dir)
            g_nTestFlame += step;
        else
            g_nTestFlame --;

        unsigned int tick_count = GetTickCount ();
        if (g_nTestFlame > 5) {
            step = 1;
            ScrollPageFlame (g_nTestFlame, &g_stBitmap, HDC_SCREEN);
        }
        else {
            step = 1;
            ScrollPageFlame (g_nTestFlame, &g_stBitmap, HDC_SCREEN);
        }
        //printf ("g_nTestFlame %d, start:%d,end=%d\n", g_nTestFlame, tick_count, GetTickCount());
    }
}

static void DrawBKGDefault (HDC hdc, const RECT* pRect, void* pParam)
{
    //do nothing
}

static void DrawEndFrameDefault (ANIMATE_SENCE* pAnimateSence)
{
    if (pAnimateSence != NULL)
    {
    }
}

void ScrollPictureAnimate(HDC hdc, BOOL dir, int frame_num)
{
    int w, h;
    BITMAP bmp;
    const RECT rt = {0, 0, SCR_W, SCR_H};

    w = RECTW(rt);
    h = RECTH(rt);

    PUSH_PULL_OBJ objs[] ={
        {&bmp, 0, 0, 0, 0},
    };

    ANIMATE_OPS stAnimateOps = {
        DrawAnimateDefault,
        DrawBKGDefault,
        NULL,
        NULL,
        //DrawEndFrameDefault
        NULL 
    };

    if (dir) {
        scroll_dir   = TRUE;
        g_nTestFlame = 0;
    }          
    else {     
        scroll_dir   = FALSE;
        g_nTestFlame = frame_num;
        g_maxframe   = g_nTestFlame;
    }

    SetInterval (150);
    RunPushPullAnimate(hdc, &rt, objs, 1, &stAnimateOps, frame_num, NULL);
}

static BMP_NODE *create_bmp_node()
{
    BMP_NODE *node = malloc(sizeof(BMP_NODE));
    if (node) {
        memset((void*)node, 0, sizeof(BMP_NODE));
        return node;
    } 
    return NULL;
}

static void init_global_node_list()
{
    memset(&g_nl, 0, sizeof(NODE_LIST));
    g_nl.isViewList = TRUE;
    g_nl.scale = 100;
}

static void load_bitmap_from_directory()
{
    DIR * dir;
    struct dirent * ptr;
    int index = 0;
    int totalRow = 0;

    BMP_NODE * last = NULL;
    dir = opendir(VIEW_PATH);
    if (!dir)
        return;
    while((ptr = readdir(dir))!=NULL)
    {
        if (index > MAX_PICTURES)
            return;
        memset(&g_fileName, 0, 512);
        strcpy(g_fileName, VIEW_PATH);
        strcat(g_fileName, ptr->d_name);
        BMP_NODE *bn = create_bmp_node();
        if (bn) {
            if (LoadBitmap (HDC_SCREEN, &(bn->icon), g_fileName)==0) {

#ifndef _TARGET_IPAQ
                RegisterCoverFlowRes (g_fileName);
#endif
                bn->row = index/PIC_MAIN_COL;
                bn->col = index%PIC_MAIN_COL;
                bn->index = index;
                bn->prev = last;

                if (!g_nl.beginNode) {
                    g_nl.beginNode = bn;
                    g_nl.curNode = g_nl.beginNode;
                }
                if(last)
                    last->next = bn;
                last = bn;
                index ++;
            } else
                free(bn);
        }
    }
    closedir(dir);

    totalRow = (index%PIC_MAIN_COL)?(index/PIC_MAIN_COL +1):(index/PIC_MAIN_COL);
    g_nl.maxRowOffset = (totalRow > PIC_MAIN_ROW)?(totalRow-PIC_MAIN_ROW):0;
}

static BMP_NODE *find_node_by_index(int index)
{
    BMP_NODE *node;
    for(node = g_nl.beginNode; node; node = node->next) {
        if(index == node->index) {
            return node;
        }
    }
    return NULL;
}

static void draw_bitmap_in_view_list_mode(HDC hdc)
{
    int box_x, box_y, box_w, box_h;
    BMP_NODE *node = (BMP_NODE *)find_node_by_index (g_nl.rowOffset*PIC_MAIN_COL);
    while(node) {
        if (node->row > (PIC_MAIN_ROW + g_nl.rowOffset))
            break;
        
        box_x = node->col * (IPHONE_MAIN_WIDTH/PIC_MAIN_COL) + MAIN_BORDER;
        box_y = (node->row-g_nl.rowOffset) * (IPHONE_MAIN_HEIGHT/PIC_MAIN_ROW) + MAIN_BORDER;
        box_w = IPHONE_MAIN_WIDTH/PIC_MAIN_COL - 2*MAIN_BORDER;
        box_h = IPHONE_MAIN_HEIGHT/PIC_MAIN_ROW - 2*MAIN_BORDER;
        
        FillBoxWithBitmap (hdc, box_x, box_y, box_w, box_h, &(node->icon));
        node = node->next;
    }
}

static void draw_bitmap_in_single_mode(HDC hdc)
{
    int box_w = g_nl.curNode->icon.bmWidth;
    int box_h = g_nl.curNode->icon.bmHeight;
    
    if (!g_nl.boxX && !g_nl.boxY) {
        g_nl.boxX = (IPHONE_MAIN_WIDTH-box_w)/2;
        g_nl.boxY = (IPHONE_MAIN_HEIGHT-box_h)/2;
    }
    
    g_nl.boxX += g_nl.boxOffX;
    g_nl.boxY += g_nl.boxOffY;

    //g_nl.boxX = ((g_nl.boxX+box_w) < 0) ? (g_nl.boxX+box_w) : g_nl.boxX;
    g_nl.boxX = ((g_nl.boxX+box_w) < 0) ?  0 : g_nl.boxX;
    g_nl.boxX = (g_nl.boxX > IPHONE_MAIN_WIDTH) ? IPHONE_MAIN_WIDTH : g_nl.boxX;
    //g_nl.boxY = ((g_nl.boxY+box_h) < 0) ? (g_nl.boxY+box_h) : g_nl.boxY;
    g_nl.boxY = ((g_nl.boxY+box_h) < 0) ? 0 : g_nl.boxY;
    g_nl.boxY = (g_nl.boxY > IPHONE_MAIN_HEIGHT) ? IPHONE_MAIN_HEIGHT : g_nl.boxY;

#if 1
    if (g_nl.scale == 100)
    {
        BITMAP bkbmp;
        RECT rc = g_rcScr;
        rc.left   = g_nl.boxX;
        rc.top    = g_nl.boxY;
        rc.right  = rc.left + (int)(box_w*g_nl.scale/100);
        rc.bottom = rc.top  + (int)(box_h*g_nl.scale/100);
        printf("rc:l=%d,t=%d,r=%d,b=%d\n", rc.left, rc.top, rc.right, rc.bottom);
        StartAlphaAnimate (50, 1000, memDC, 
                &(g_nl.curNode->icon), &rc, 0, 255);
    }
    else{
        FillBoxWithBitmap(hdc, g_nl.boxX, g_nl.boxY, 
                (int)(box_w*g_nl.scale/100), (int)(box_h*g_nl.scale/100), &(g_nl.curNode->icon));
    }
#else
    FillBoxWithBitmap(hdc, g_nl.boxX, g_nl.boxY, 
            (int)(box_w*g_nl.scale/100), (int)(box_h*g_nl.scale/100), &(g_nl.curNode->icon));
#endif
}

static void draw_bitmap(HDC hdc)
{
    if(g_nl.isViewList) 
        draw_bitmap_in_view_list_mode(hdc);
    else
        draw_bitmap_in_single_mode(hdc);
}

static free_bitmap()
{
    BMP_NODE *node = g_nl.beginNode;
    g_nl.count = 0;
    while(node) {
        UnloadBitmap(&(node->icon));
        g_nl.beginNode = node->next;
        free(node);
        node=g_nl.beginNode;
    }
}

static void change_view_mode(HWND hWnd, LPARAM lParam)
{
    if (g_nl.isViewList) {
        int col = LOSWORD (lParam)/(IPHONE_MAIN_WIDTH/PIC_MAIN_COL);
        int row = HISWORD (lParam)/(IPHONE_MAIN_HEIGHT/PIC_MAIN_ROW);
#ifndef _TARGET_IPAQ
        int index = ReturnCurCoverFlowIndex ();//(row+g_nl.rowOffset) * PIC_MAIN_COL + col;
#else
        int index = (row+g_nl.rowOffset) * PIC_MAIN_COL + col;
#endif
        BMP_NODE *node = find_node_by_index(index);
        if (node) {
            g_nl.isViewList = FALSE;
            g_nl.curNode = node;
            g_nl.boxX = 0;
            g_nl.boxY = 0;
            g_nl.boxOffX = 0;
            g_nl.boxOffY = 0;
            g_nl.scale = 100;
        }
    } else {
#if 1
        {
            BITMAP bkbmp;
            BITMAP fgbmp;
            RECT rc = g_rcScr;
            int box_w = g_nl.curNode->icon.bmWidth;
            int box_h = g_nl.curNode->icon.bmHeight;

            rc.left   = g_nl.boxX;
            rc.top    = g_nl.boxY;
            rc.right  = rc.left + (int)(box_w*g_nl.scale/100);
            rc.bottom = rc.top  + (int)(box_h*g_nl.scale/100);

            memset(&fgbmp, 0, sizeof(BITMAP));
            BitBlt (HDC_SCREEN, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, memDC, 0, 0, 0);
            GetBitmapFromDC (memDC, rc.left, rc.top, RECTW(rc), RECTH(rc), 
                    &fgbmp);
            printf("rc:l=%d,t=%d,r=%d,b=%d\n", rc.left, rc.top, rc.right, rc.bottom);
            StartAlphaAnimate (50, 1000, memDC, &(fgbmp), &rc, 255, 0);
        }
#endif
        g_nl.isViewList = TRUE;
        g_nl.curNode = g_nl.beginNode;
        g_nl.rowOffset = 0;
    }
    InvalidateRect(hWnd, NULL, TRUE);
}

static void drag_view_in_view_list_mode(HWND hWnd, LPARAM lParam)
{
    int offset = (g_mouse.originalY-HISWORD (lParam))/(IPHONE_MAIN_HEIGHT/PIC_MAIN_ROW);
    if (offset) {
        g_nl.rowOffset += offset;
        if(g_nl.rowOffset < 0)
            g_nl.rowOffset = 0;
        else if (g_nl.rowOffset > g_nl.maxRowOffset) 
            g_nl.rowOffset = g_nl.maxRowOffset;

        g_mouse.originalX = LOSWORD (lParam);
        g_mouse.originalY = HISWORD (lParam);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

static void drag_view_in_single_view_mode(HWND hWnd, LPARAM lParam)
{
    int offx = LOSWORD (lParam) - g_mouse.originalX;
    int offy = HISWORD (lParam) - g_mouse.originalY;
    
    g_nl.boxOffX = offx;
    g_nl.boxOffY = offy;
    g_mouse.originalX = LOSWORD (lParam);
    g_mouse.originalY = HISWORD (lParam);
    
    InvalidateRect(hWnd, NULL, TRUE);
}

static void zoom_view_in_single_view_mode(HWND hWnd, LPARAM lParam)
{
    int factor;
    int off = LOSWORD (lParam) - g_mouse.originalX;
   
    if (off > 0) {
        factor = off*10;
        if (factor > g_nl.scale) {
            g_nl.scale = factor;
            g_mouse.originalX = LOSWORD (lParam);
            g_mouse.originalY = HISWORD (lParam);
            InvalidateRect(hWnd, NULL, TRUE);
        }
    } else if(off < 0) {
        factor = (int)(400/ABS(off));
        if ( factor < g_nl.scale) {
            g_nl.scale = factor;
            g_mouse.originalX = LOSWORD (lParam);
            g_mouse.originalY = HISWORD (lParam);
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }
}

static void drag_view (HWND hWnd, LPARAM lParam)
{
    if (g_nl.isViewList) {
        drag_view_in_view_list_mode (hWnd, lParam);
    } else {
        if (GetTickCount() - g_mouse.time > 20) {
            zoom_view_in_single_view_mode (hWnd, lParam);
        }else {
            drag_view_in_single_view_mode (hWnd, lParam);
            g_mouse.time = GetTickCount();
        }
    }
}

static void deal_with_mouse_operation(HWND hWnd, int message, LPARAM lParam)
{
    switch (message) {
        case MSG_LBUTTONDOWN:
            {
                g_mouse.downX = g_mouse.originalX = LOSWORD (lParam);
                g_mouse.downY = g_mouse.originalY = HISWORD (lParam);
                g_mouse.time = GetTickCount();
            }
            break;
        case MSG_MOUSEMOVE:
            {
                if (g_mouse.originalX || g_mouse.originalY) {
                    int offX = ABS(g_mouse.originalX - LOSWORD (lParam));
                    int offY = ABS(g_mouse.originalY - HISWORD (lParam));
                    if (offX > 5 || offY > 5)
                        drag_view (hWnd, lParam);
                }
            }
            break;
        case MSG_LBUTTONUP: 
            {
                if (ABS(g_mouse.downX - g_mouse.originalX) < 2 
                        && ABS(g_mouse.downY - g_mouse.originalY) < 2)
                    change_view_mode(hWnd, lParam);
                memset(&g_mouse, 0, sizeof(MOUSE_OP));
            }
            break;
    }
}

#ifdef _MGIAL_IPAQ_H3600
void iPAQRollPage(BOOL dir)
{
    int* p_idx = NULL;
    int  idx_num = 0;
    int  idx[6] = {1, 2, 3, 5, 8, 10};
    int  idx_prev[6] = {10, 8, 5, 3, 2, 0};
    int  i = 0;

    if (dir) {
        p_idx = idx;
        idx_num = TABLESIZE(idx);
    }          
    else {     
        p_idx = idx_prev;
        idx_num = TABLESIZE(idx_prev);
    }

    while (TRUE){
        if ( i >= idx_num)
            break;
        printf("i=%d, p_idx=%d, idx_num=%d\n", i, p_idx[i], idx_num);
        ScrollPageFlame (p_idx[i], &g_stBitmap, HDC_SCREEN);
        i++;
    }
}
#endif


void RollPage(BOOL dir, int frame_num)
{
    BITMAP srcbmp;

    if (dir){
        if (g_nl.curNode->next) {
            g_nl.curNode = g_nl.curNode->next;
        }
        else {
            g_nl.curNode = g_nl.beginNode;
        }
    }
    else {
        if (g_nl.curNode->prev) {
            g_nl.curNode = g_nl.curNode->prev;
        }
        else {
            struct _BMP_NODE* pNode = g_nl.curNode->next;
            static struct _BMP_NODE* endNode = NULL;
            if (!endNode) {
                while (pNode){
                    endNode = pNode;
                    pNode   = pNode->next;
                }
            }

            if (endNode)
                g_nl.curNode = endNode;
            else
                g_nl.curNode = g_nl.beginNode;
        }
    }


#if 0
    memset(&srcbmp, 0, sizeof(BITMAP));
    GetBitmapFromDC (HDC_SCREEN, 0, 0, SCR_W, SCR_H, &srcbmp);

    if (dir)
        InitRollPage(&srcbmp, &g_nl.curNode->icon, SCR_W, SCR_H, frame_num);
    else
        InitRollPage(&g_nl.curNode->icon, &srcbmp, SCR_W, SCR_H, frame_num);
#else
    if (!g_DownDC) {
        g_DownDC = CreateCompatibleDC(HDC_SCREEN);
        g_UpDC   = CreateCompatibleDC(HDC_SCREEN);
    }

    FillBoxWithBitmap(g_DownDC, 0, 0, SCR_W, SCR_H, &g_nl.curNode->icon);
    BitBlt (HDC_SCREEN, 0, 0, SCR_W, SCR_H, g_UpDC, 0, 0, 0);

    //BitBlt (g_DownDC, 0, 0, SCR_W, SCR_H, HDC_SCREEN, 0, 0, 0);
    //while(1);
    if (dir)
        InitScrollPage(g_UpDC, g_DownDC, SCR_W, SCR_H, frame_num);
    else
        InitScrollPage(g_DownDC, g_UpDC, SCR_W, SCR_H, frame_num);
#endif

#ifdef _MGIAL_IPAQ_H3600
    iPAQRollPage(dir);
#else
    ScrollPictureAnimate(HDC_SCREEN, dir, frame_num);
#endif
}
static char* g_pBitmapName [] = {};
static BOOL g_bCoverFlow = TRUE;

static int PicWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) {
        case MSG_CREATE:
            
            hdc = GetClientDC(hWnd);
            memDC = CreateCompatibleDCEx (hdc, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT);
            //LoadBitmap (memDC, &bmpBk, PIC_BMP_BG);
            ReleaseDC (hdc);

            //InitCoverFlow (g_pBitmapName, 9, 240, 320);
            StartUpCoverFlow (IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT);
            init_global_node_list();
            load_bitmap_from_directory();
            memset(&g_mouse, 0, sizeof(MOUSE_OP));
            InitCoverFlowByRegisterRes () ;
            //InitCoverFlowBySingle (IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT);
            UpdateWindow(hWnd, TRUE);
            return 0;

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            draw_bitmap(memDC);
            //BitBlt (memDC, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, hdc, 0, 0, 0);
#ifndef _TARGET_IPAQ
            if (g_nl.isViewList)
            {
                ShowCoverFlow (memDC);
                BitBlt (memDC, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, hdc, 0, 0, 0);
            }
            else
            {
                BitBlt (memDC, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, hdc, 0, 0, 0);
            }
#else
            BitBlt (memDC, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, hdc, 0, 0, 0);
#endif
            EndPaint (hWnd, hdc);
            return 0;
        case MSG_LBUTTONDOWN:
        case MSG_MOUSEMOVE:
        case MSG_LBUTTONUP:
            {
                //CoverFlowMoveFromRightToLeftSingle (HDC_SCREEN);
                deal_with_mouse_operation(hWnd, message, lParam);
            }
            break;
        case MSG_KEYDOWN:
            if (g_nl.isViewList)
            {
#ifndef _TARGET_IPAQ
                switch( wParam ){
                    case SCANCODE_CURSORBLOCKLEFT:
                        CoverFlowMoveFromRightToLeftSingle (HDC_SCREEN);
                        break;
                    case SCANCODE_CURSORBLOCKRIGHT:
                        CoverFlowMoveFromLeftToRightSingle (HDC_SCREEN);
                        break;
                    default:
                        break;
                }
#endif
                return 0;
            }
            switch( wParam ){
                case SCANCODE_CURSORBLOCKUP:
                    RollPage(FALSE, 10);
                    break;
                case SCANCODE_CURSORBLOCKDOWN:
                    RollPage(TRUE, 10);
                    break;
                default:
                    break;
            }
            break;
        case MSG_LBUTTONDBLCLK: 
        case MSG_RBUTTONDOWN: 
            RollPage(TRUE, 20);
            memset(&g_mouse, 0, sizeof(MOUSE_OP));
            break;

        case MSG_ERASEBKGND:
            {
                const RECT *inv_rect = (const RECT*)lParam;
#if 0
                if (inv_rect) 
                    FillBoxWithBitmapPart(memDC, inv_rect->left, inv_rect->top, 
                            inv_rect->right, inv_rect->bottom, 0, 0, &bmpBk, 
                            inv_rect->left, inv_rect->top);
                else
                    FillBoxWithBitmap(memDC, 0, 0, IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT, &bmpBk);
#else
#if 1 
                gal_pixel old = SetBrushColor(memDC, PIXEL_black);
                if (inv_rect)
                    FillBox(memDC, inv_rect->left, inv_rect->top, 
                                inv_rect->right, inv_rect->bottom);
                else
                    FillBox(memDC, 0, 0,IPHONE_MAIN_WIDTH, IPHONE_MAIN_HEIGHT);

                SetBrushColor(memDC, old);
#endif
#endif
            }
            return 0;
        case MSG_CLOSE:
            free_bitmap();
            DeleteMemDC (memDC);
            if (g_DownDC) DeleteCompatibleDC (g_DownDC);
            //UnloadBitmap(&bmpBk);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "picture" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_NONE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Picture View";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = PicWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = PIXEL_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

#if 0
    ShowWindow (hMainWnd, SW_SHOWNORMAL);
#else
    ShowWindowUsingShareBuffer (hMainWnd);
#endif

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

