/*
 ** $Id:$
 **
 ** watch.c: A watch screen saver program.
 **
 ** Copyright(C) 2007 Feynman Software, all rights reserved.
 **
 ** Use of this source package is subject to specific license terms
 ** from Beijing Feynman Software Technology Co., Ltd.
 **
 ** URL: http://www.minigui.com
 **
 */


#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#include <minigui/common.h>
#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define _ID_TIMER 100
#define TIMER_INV 100 // 1s

#define MAINWINDOW_W g_rcScr.right  //480
#define MAINWINDOW_H g_rcScr.bottom  //272
 
#define WIDTH_DIGIT     10
#define HEIGHT_DIGIT    15
#define WIDTH_TIME      80
#define X_TIME       ((MAINWINDOW_W - WIDTH_TIME)/2)
#define Y_TIME       ((MAINWINDOW_H - HEIGHT_DIGIT)/2)
#define NR_DIGIT_GLYPHS       10
#define NR_COLON_GLYPHS       1

#define DIGIT_IMAGE_FILE    "./res/digits.gif"
#define COLON_IMAGE_FILE    "./res/colon.gif"

static int minute = 0, second = 0, hour = 0;
static int time_left = 0, time_top = 0;

//static BITMAP back_bmp;
static HDC mem_dc;

static BITMAP digit_bmp;
static BITMAP colon_bmp;
static DEVFONT *dev_font;
static LOGFONT *logfont;

static int random_in_range(int low, int high)
{
    return low + random() % (high-low+1);
}


static char *get_time_text( void )
{
    static char buf[10];
    time_t timep;
    struct tm *p;

    time (&timep);
    p = localtime (&timep);
    hour = p->tm_hour; 
    minute = p->tm_min; 
    second = p->tm_sec; 

    sprintf( buf, "%02d:%02d:%02d", hour, minute, second );
    buf [8] = '\0';
    return buf;
}

static int load_image (void)
{
    if (LoadBitmapFromFile (HDC_SCREEN, &digit_bmp, DIGIT_IMAGE_FILE)) {
        fprintf (stderr, "Can't load image: %s\n", DIGIT_IMAGE_FILE);
        return -1;
    }

    if (LoadBitmapFromFile (HDC_SCREEN, &colon_bmp, COLON_IMAGE_FILE)) {
        fprintf (stderr, "Can't load image: %s\n", COLON_IMAGE_FILE);
        return -1;
    }
/* 
    if (LoadBitmap (HDC_SCREEN, &back_bmp,  BKG_IMAGE_FILE )) {
        fprintf (stderr, "Can't load image: %s\n", BKG_IMAGE_FILE);
        return -1;
    }
*/    
    return 0;
}

static int create_bmp_font (void)
{
    dev_font = CreateBMPDevFont ("bmp-led-rrncnn-10-15-ISO8859-1",
                   &digit_bmp, "0", NR_DIGIT_GLYPHS, WIDTH_DIGIT);
    if (!dev_font) {
        fprintf (stderr, "Fail to create bitmap font.\n");
        return -1;
    }

    AddGlyphsToBMPFont (dev_font, &colon_bmp, ":", NR_COLON_GLYPHS, WIDTH_DIGIT);

    logfont = CreateLogFont (FONT_TYPE_NAME_BITMAP_BMP, "led",
                  "ISO8859-1",
                  FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN,
                  FONT_SETWIDTH_NORMAL, FONT_SPACING_CHARCELL,
                  FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                  HEIGHT_DIGIT, 0);
    if (!logfont) {
        fprintf (stderr, "Fail to create logic font.\n");
        return -1;
    }

    SelectFont (mem_dc, logfont);
        
    return 0;

}


static void create_mem_dc (HWND hWnd)
{
    HDC hdc;
    
    hdc = GetDC(hWnd);
    mem_dc = CreateCompatibleDCEx(hdc, MAINWINDOW_W, MAINWINDOW_H);            
    ReleaseDC(hdc);    
}



static LRESULT WatchProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{

    switch( message ) {
        case MSG_CREATE:
            create_mem_dc (hWnd);

            if (load_image ())
                return -1; 
            if(create_bmp_font ())
                return -1;

            time_left = X_TIME;
            time_top = Y_TIME;

            srand(time(NULL));
            SetTimer( hWnd, _ID_TIMER, TIMER_INV);

            break;


        case MSG_PAINT: {

            HDC hdc = BeginPaint (hWnd);
            SetBrushColor (mem_dc, COLOR_black);
            FillBox (mem_dc, 0, 0, MAINWINDOW_W, MAINWINDOW_H);
            TextOut (mem_dc, time_left, time_top, get_time_text ());
/*
            pmp_display_led_digits (mem_dc, time_left, time_top, get_time_text()); */
            BitBlt (mem_dc, 0, 0, MAINWINDOW_W, MAINWINDOW_H, hdc, 0, 0, 0); 
            EndPaint (hWnd, hdc);
            break;
        }

        case MSG_TIMER: {
            int x_low, x_high;
            int y_low, y_high;

            x_low = 0 - time_left;
            x_high = MAINWINDOW_W - WIDTH_TIME - time_left;
            y_low = 0 - time_top;
            y_high = MAINWINDOW_H - HEIGHT_DIGIT - time_top;

            time_left += random_in_range(x_low, x_high);
            time_top += random_in_range(y_low, y_high);


/* 
                x = random_in_range(-(X_TIME), +(X_TIME));
                y = random_in_range(-(Y_TIME), +(Y_TIME));

                if ((time_left + x) < 0 )
                    time_left = 0;
                else if ((time_left + x) > MAINWINDOW_W)
                    time_left = MAINWINDOW_W - WIDTH_TIME;
                else 
                    time_left = x;

                if ((time_top + y) < 0)
                    time_top = 0;
                else if ((time_top + y) > MAINWINDOW_H)
                    time_top = MAINWINDOW_H - WIDTH_DIGIT;
                else
                    time_top = y;
*/
                //InvalidateRect( hWnd, &number_rect[i], FALSE );
            InvalidateRect( hWnd, NULL, FALSE );
            break;
        }
        case MSG_DESTROY:
            DestroyAllControls( hWnd );
            return 0;

        case MSG_CLOSE:
            KillTimer (hWnd, _ID_TIMER);
            //UnloadBitmap (&back_bmp);
            UnloadBitmap (&digit_bmp);
            UnloadBitmap (&colon_bmp);
            DestroyBMPFont (dev_font);
            DestroyLogFont (logfont);
            DeleteCompatibleDC (mem_dc);
            DestroyMainWindow (hWnd);
            MainWindowCleanup (hWnd);
            return 0;

    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void _stopwatch_init_create_info( PMAINWINCREATE pCreateInfo )
{
    pCreateInfo->dwStyle = WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "Watch";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = WatchProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = MAINWINDOW_W;
    pCreateInfo->by = MAINWINDOW_H;
    pCreateInfo->iBkColor = COLOR_black;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "screen_saver" , 0 , 0);
#endif

    _stopwatch_init_create_info( &CreateInfo );
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}



