#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include<unistd.h>
#include <pthread.h>
//#include <ctime.h>
#include <sys/time.h>
#include <unistd.h>

//extern "C" {
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
//}

#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_u.h"
#include "agg_renderer_scanline.h"
#include "agg_path_storage.h"
#include "agg_conv_transform.h"
#include "agg_trans_affine.h"
#include "agg_trans_bilinear.h"
#include "agg_trans_perspective.h"
#include "agg_span_allocator.h"
#include "agg_span_interpolator_linear.h"
#include "agg_span_interpolator_trans.h"
#include "agg_span_subdiv_adaptor.h"
#include "agg_pixfmt_rgba.h"
#include "agg_image_accessors.h"
#include "agg_span_image_filter_rgba.h"
//#include "ctrl/agg_rbox_ctrl.h"
#include "agg_platform_support.h"
#include "interactive_polygon.h"
#include "agg_span_image_filter_rgb.h"
#include "agg_flip.h"

enum flip_y_e { flip_y = true };

agg::rasterizer_scanline_aa<> g_rasterizer;
agg::scanline_u8  g_scanline;
double            g_x1 = 0;
double            g_y1 = 0;
double            g_x2 = 0;
double            g_y2 = 0;

int cur_image=0;
int cur_type = 0;

//#define PIC_W 240
//#define PIC_H 320

class the_application : public agg::platform_support
{
public:
    typedef agg::pixfmt_bgra32                             pixfmt;
    typedef pixfmt::color_type                             color_type;
    typedef agg::renderer_base<pixfmt>                     renderer_base;
    typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;

    typedef agg::pixfmt_bgra32_pre         pixfmt_pre;
    typedef agg::renderer_base<pixfmt_pre> renderer_base_pre;

    agg::interactive_polygon   m_quad;
    //agg::rbox_ctrl<agg::rgba8> m_trans_type;

    the_application(agg::pix_format_e format, bool flip_y) :
        agg::platform_support(format, flip_y),
        m_quad(4, 0.5)
        //m_trans_type(230, 5.0, 230+170.0, 70.0, !flip_y)
        //m_trans_type(420, 5.0, 420+170.0, 70.0, !flip_y)
    {
        //m_trans_type.add_item("Affine Parallelogram");
        //m_trans_type.add_item("Bilinear");
        //m_trans_type.add_item("Perspective");
        //m_trans_type.cur_item(0);
        //add_ctrl(m_trans_type);
    }


    virtual void on_init()
    {
        double d = 0.0;
        g_x1 = d;
        g_y1 = d;
#if 0
        g_x2 = rbuf_img(0).width() - d;
        g_y2 = rbuf_img(0).height() - d;
#else
        g_x2 = g_rcScr.right - d;
        g_y2 = g_rcScr.bottom - d;
#endif
        //printf("on_init:g_x1=%f, g_y1=%f, g_x2=%f, g_y2=%f\n", g_x1, g_y1, g_x2, g_y2);
        const int start=0;
        m_quad.xn(0) = start;
        m_quad.yn(0) = start;
#if 0
        m_quad.xn(1) = width()  - start;
        m_quad.yn(1) = start;
        m_quad.xn(2) = width()  - start;
        m_quad.yn(2) = height() - start;
        m_quad.xn(3) = start;
        m_quad.yn(3) = height() - start;
#else
        m_quad.xn(1) = start+g_rcScr.right;
        m_quad.yn(1) = start;
        m_quad.xn(2) = start+g_rcScr.right;
        m_quad.yn(2) = start+g_rcScr.bottom;
        m_quad.xn(3) = start;
        m_quad.yn(3) = start+g_rcScr.bottom;
#endif
    }

    virtual void on_draw()
    {
        pixfmt            pixf(rbuf_window());
        pixfmt_pre        pixf_pre(rbuf_window());
        renderer_base     rb(pixf);
        renderer_base_pre rb_pre(pixf_pre);

        renderer_solid r(rb);
        rb.clear(agg::rgba(0, 0, 0));
        //printf("cur_item=%d\n", m_trans_type.cur_item());
        //if(m_trans_type.cur_item() == 0)
        if(cur_type == 0)
        {
            // For the affine parallelogram transformations we
            // calculate the 4-th (implicit) point of the parallelogram
            m_quad.xn(3) = m_quad.xn(0) + (m_quad.xn(2) - m_quad.xn(1));
            m_quad.yn(3) = m_quad.yn(0) + (m_quad.yn(2) - m_quad.yn(1));
        }

        //--------------------------
        // Render the "quad" tool and controls
#if 0
        g_rasterizer.add_path(m_quad);
        agg::render_scanlines_aa_solid(g_rasterizer, g_scanline, rb, 
                                       agg::rgba(0, 0.3, 0.5, 0.6));
#endif
        // Prepare the polygon to rasterize. Here we need to fill
        // the destination (transformed) polygon.
        g_rasterizer.clip_box(0, 0, width(), height());
        g_rasterizer.reset();
        g_rasterizer.move_to_d(m_quad.xn(0), m_quad.yn(0));
        g_rasterizer.line_to_d(m_quad.xn(1), m_quad.yn(1));
        g_rasterizer.line_to_d(m_quad.xn(2), m_quad.yn(2));
        g_rasterizer.line_to_d(m_quad.xn(3), m_quad.yn(3));

        agg::span_allocator<color_type> sa;
        agg::image_filter_bilinear filter_kernel;
        agg::image_filter_lut filter(filter_kernel, false);

        pixfmt pixf_img(rbuf_img(cur_image));

        //typedef agg::image_accessor_wrap<pixfmt, 
        //                                 agg::wrap_mode_reflect,
        //                                 agg::wrap_mode_reflect> img_accessor_type;
        //img_accessor_type ia(pixf_img);
#if 0
        typedef agg::image_accessor_clip<pixfmt> img_accessor_type;
        img_accessor_type ia(pixf_img, agg::rgba(1,1,1));
#else
        typedef agg::image_accessor_clone<pixfmt> img_accessor_type;
        img_accessor_type ia(pixf_img);
#endif
        start_timer();
        //switch(m_trans_type.cur_item())
        switch(cur_type)
        {
            case 0:
            {
                // Note that we consruct an affine matrix that transforms
                // a parallelogram to a rectangle, i.e., it's inverted.
                // It's actually the same as:
                // tr(g_x1, g_y1, g_x2, g_y2, m_triangle.polygon());
                // tr.invert();
                agg::trans_affine tr(m_quad.polygon(), g_x1, g_y1, g_x2, g_y2);
#if 0
                printf("polygon:(%f,%f),(%f,%f),(%f,%f),(%f,%f)\n",
                                    m_quad.xn(0), m_quad.yn(0),
                                    m_quad.xn(1), m_quad.yn(1),
                                    m_quad.xn(2), m_quad.yn(2),
                                    m_quad.xn(3), m_quad.yn(3));

                //printf("polygon:g_x1=%f, g_y1=%f, g_x2=%f, g_y2=%f\n",g_x1, g_y1, g_x2, g_y2);
                //printf("tr:sx=%f, shy=%f, shx=%f, sy=%f, tx=%f, ty=%f\n\n\n", 
                //                        tr.sx, tr.shy, tr.shx, tr.sy, tr.tx, tr.ty);
#endif
                // Also note that we can use the linear interpolator instead of 
                // arbitrary span_interpolator_trans. It works much faster, 
                // but the transformations must be linear and parellel.
                typedef agg::span_interpolator_linear<agg::trans_affine> interpolator_type;
                interpolator_type interpolator(tr);
                typedef agg::span_image_filter_rgba_nn<img_accessor_type,
                                                       interpolator_type> span_gen_type;
                span_gen_type sg(ia, interpolator);
                //printf("test string\n");
                agg::render_scanlines_aa(g_rasterizer, g_scanline, rb_pre, sa, sg);
                break;
            }

            case 1:
            {
                agg::trans_bilinear tr(m_quad.polygon(), g_x1, g_y1, g_x2, g_y2);
                if(tr.is_valid())
                {
                    typedef agg::span_interpolator_linear<agg::trans_bilinear> interpolator_type;
                    interpolator_type interpolator(tr);

                    typedef agg::span_image_filter_rgba_2x2<img_accessor_type,
                                                            interpolator_type> span_gen_type;
                    span_gen_type sg(ia, interpolator, filter);
                    agg::render_scanlines_aa(g_rasterizer, g_scanline, rb_pre, sa, sg);
                }
                break;
            }

            case 2:
            {
                agg::trans_perspective tr(m_quad.polygon(), g_x1, g_y1, g_x2, g_y2);
                if(tr.is_valid())
                {
                    // Subdivision and linear interpolation (faster, but less accurate)
                    //-----------------------
                    //typedef agg::span_interpolator_linear<agg::trans_perspective> interpolator_type;
                    //typedef agg::span_subdiv_adaptor<interpolator_type> subdiv_adaptor_type;
                    //interpolator_type interpolator(tr);
                    //subdiv_adaptor_type subdiv_adaptor(interpolator);
                    //
                    //typedef agg::span_image_filter_rgba_2x2<img_accessor_type,
                    //                                        subdiv_adaptor_type> span_gen_type;
                    //span_gen_type sg(ia, subdiv_adaptor, filter);
                    //-----------------------

                    // Direct calculations of the coordinates
                    //-----------------------
                    typedef agg::span_interpolator_trans<agg::trans_perspective> interpolator_type;
                    interpolator_type interpolator(tr);
                    typedef agg::span_image_filter_rgba_2x2<img_accessor_type,
                                                            interpolator_type> span_gen_type;
                    span_gen_type sg(ia, interpolator, filter);
                    //-----------------------

                    agg::render_scanlines_aa(g_rasterizer, g_scanline, rb_pre, sa, sg);
                }
                break;
            }
        }
        double tm = elapsed_time();

#if 0
        char buf[128]; 
        agg::gsv_text t;
        t.size(10.0);

        agg::conv_stroke<agg::gsv_text> pt(t);
        pt.width(1.5);

        sprintf(buf, "%3.2f ms", tm);
        t.start_point(10.0, 10.0);
        t.text(buf);

        g_rasterizer.add_path(pt);
#endif

#if 0
        agg::render_scanlines_aa_solid(g_rasterizer, g_scanline, rb, 
                                       agg::rgba(0,0,0));
        //--------------------------
        agg::render_ctrl(g_rasterizer, g_scanline, rb, m_trans_type);
#endif
    }
};

the_application app(agg::pix_format_bgra32, flip_y);

void GUIAPI UpdateAll (HWND hWnd, BOOL fErase)
{
    MSG Msg;
    UpdateWindow(hWnd, TRUE);
    while (PeekMessageEx (&Msg, hWnd, MSG_PAINT, MSG_PAINT+1,
                            FALSE, PM_REMOVE))
    {
        TranslateMessage (&Msg);
        DispatchMessage (&Msg);
    }
}

int NullProc (HWND hWnd, HDC private_dc, HDC real_dc, 
const RECT*update_rc, const RECT* real_rc,const RECT* main_update_rc)
{
        return 0;
}

BOOL InitFlip(int w, int h)
{
    if(!app.init(w, h, agg::window_resize))
    {
        printf("app init failed\n");
        return false;
    }
    return true;
}

BOOL LoadBitmapFromDC(HWND hwnd, int idx)
{
    HDC hdc = GetSecondaryDC(hwnd);
    SetSecondaryDC(hwnd, hdc, NullProc);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateAll(hwnd, true);
    if(!app.load_img_from_dc(idx, hdc)) 
    {
        printf("load_img_from_dc failed\n");
        SetSecondaryDC(hwnd, hdc, NULL);
        return false;
    }
    SetSecondaryDC(hwnd, hdc, NULL);
    return true;
}

BOOL LoadFrontBitmapFromDC(HWND hwnd)
{
    return LoadBitmapFromDC(hwnd, 0);
}

BOOL LoadRearBitmapFromDC(HWND hwnd)
{
#if 0
    char * file="gyy.jpg";
    return app.load_img(1, file);
#else
    return LoadBitmapFromDC(hwnd, 1);
#endif
}

typedef struct _flip_data
{
    bool   finit;
    double ra;
    double rb;
    double polygon_h;
    double start_x;
    double origin_x, origin_y;
    int    step;
}flip_data;

flip_data flip={0};
const int g_frame_num=10;/*the number of frame in 1 second*/

//const double pi=3.1415926535897932384626;

BOOL InitFlipAnimation()
{
    if (flip.finit)
    {
        memset(&flip, 0, sizeof(flip_data));
    }

    InitFlip(g_rcScr.right, g_rcScr.bottom);
    app.on_init();

    cur_image = 0;

    flip.ra = (app.m_quad.xn(1)-app.m_quad.xn(0))/2;
    flip.rb = flip.ra/3;
    flip.polygon_h = app.m_quad.yn(3) - app.m_quad.yn(0);
    flip.start_x = app.m_quad.xn(0);
    flip.origin_x = (int)(flip.start_x + flip.ra);
    flip.origin_y = (int)app.m_quad.yn(0);
    flip.step = (int)(2*flip.ra/g_frame_num); 
    flip.finit = true;
#if 0
    printf("r=%f, h=%f, start_x=%f, x=%f, y=%f, step=%d\n",
           flip.r, flip.polygon_h, flip.start_x, flip.origin_x, flip.origin_y, flip.step);
#endif
    return true;
}

bool DrawFlipAnimation(HDC hdc)
{
    static bool badd=true;
    static double cur_x = flip.start_x;
    double cur_y;

    if (badd)
    {
        if (cur_x < flip.start_x+flip.ra)
            cur_x += flip.step;
        else
        {
            badd = false;
            /*change picture*/
            if (cur_x == flip.start_x+flip.ra)
            {
                cur_image = 1;
                return true;
            }
        }
    }
    else
    {
        if (cur_x > flip.start_x)
            cur_x -= flip.step;
        else
        {
            badd = true;
            cur_x = 0; 
            return false;
        }
    }
    cur_y = flip.origin_y + flip.rb * 
                sqrt( fabs( 1- 
                          ((cur_x - flip.origin_x)*
                           (cur_x - flip.origin_x)) /
                           (flip.ra*flip.ra) 
                          ) 
                    ); 
    if (badd)
    {
        //printf("cur_x=%f, cur_y=%f\n", cur_x, cur_y);
        app.m_quad.xn(0) = cur_x;
        app.m_quad.yn(0) = cur_y;

        app.m_quad.xn(1) = 2*flip.origin_x - cur_x;
        app.m_quad.yn(1) = 2*flip.origin_y - cur_y;

        app.m_quad.xn(2) = app.m_quad.xn(1);
        app.m_quad.yn(2) = app.m_quad.yn(1)+flip.polygon_h;

        app.m_quad.xn(3) = cur_x;
        app.m_quad.yn(3) = cur_y+flip.polygon_h;
    }
    else
    {
         //printf("cur_x=%f, cur_y=%f\n", cur_x, cur_y);
        app.m_quad.xn(0) = cur_x;
        app.m_quad.yn(0) = 2*flip.origin_y - cur_y;

        app.m_quad.xn(1) = 2*flip.origin_x - cur_x;
        app.m_quad.yn(1) = cur_y;

        app.m_quad.xn(2) = app.m_quad.xn(1);
        app.m_quad.yn(2) = app.m_quad.yn(1)+flip.polygon_h;

        app.m_quad.xn(3) = cur_x;
        app.m_quad.yn(3) = cur_y+flip.polygon_h;
    }

#if 0
    printf("(%f, %f), (%f, %f), (%f, %f), (%f, %f)\n",
                app.m_quad.xn(0), app.m_quad.yn(0),app.m_quad.xn(1), app.m_quad.yn(1),
                app.m_quad.xn(2), app.m_quad.yn(2),app.m_quad.xn(3), app.m_quad.yn(3));
#endif
    app.on_draw();

    HDC hdc_agg  = app.rbuf_dc();
    BitBlt(hdc_agg, 0, 0, 0, 0, hdc, 0, 0, 0);
    //app.update_window();
    return true;
}

void Agg_Flip(HDC hdc, int interval)
{
    //InitFlipAnimation();
	pthread_cond_t cond;
	pthread_mutex_t mutex;

	int interval_sec;
	int interval_nsec;
    int total_time=(2+g_frame_num)*interval;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	int start_time =tv.tv_sec*1000 + (tv.tv_usec+999)/1000;

	pthread_cond_init(&cond,NULL);
	pthread_mutex_init(&mutex, NULL);

	interval_sec = interval/1000;
	interval_nsec = (interval%1000)*1000000;

    while(1)
    //while(DrawFlipAnimation(HDC_SCREEN))
    {
		struct timeval tv;
		struct timespec timeout;
		DWORD before_time/*, end_time*/;
//		before_time = getcurtime();
		gettimeofday(&tv, NULL);
		timeout.tv_sec = tv.tv_sec + interval_sec;
		timeout.tv_nsec = tv.tv_usec*1000 + interval_nsec;
		if(timeout.tv_nsec > 1000000000){
			timeout.tv_sec ++;
			timeout.tv_nsec -= 1000000000;
		}
#if 0
		if( total_time > 0 ){
			before_time = tv.tv_sec*1000 + (tv.tv_usec+999)/1000;
            
			if((before_time - start_time) >= total_time)
				break;
		}
#endif

		pthread_mutex_lock(&mutex);
        if ( !DrawFlipAnimation(hdc) )
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
		pthread_cond_timedwait(&cond, &mutex, &timeout);

		pthread_mutex_unlock(&mutex);

        //sleep(1);
    }

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
}


#if 0
static int ImageperspectiveProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case MSG_CREATE:
        {
            InitFlip(240, 320);
            //SetTimer(hWnd, 1000, 5);
            break;
        }
        case MSG_KEYDOWN:
            cur_image = 0;
            LoadFrontBitmapFromDC(hWnd);
            LoadRearBitmapFromDC(hWnd);
            InitFlipAnimation();
            SetTimer(hWnd, 1000, 5);
            break;
        case MSG_TIMER:
        {
            switch(cur_type)
            {
                case 0:
                {
                    DrawFlipAnimation();
                }
                default:
                    break;
            }
            break;
        }
        case MSG_ERASEBKGND:
        {
            const char* img_name = "gyy.jpg";
            BITMAP bmp;
            HDC hdc = (HDC)wParam;
            const RECT* clip = (const RECT*) lParam;
            BOOL fGetDC = FALSE;
            RECT rcTemp;

            if (hdc == 0){
                hdc = GetClientDC (hWnd);
                fGetDC = TRUE;
            }
            if (clip){
                rcTemp = *clip;
                ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
                ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
                IncludeClipRect (hdc, &rcTemp);
            }
            //FillBoxWithBitmap (hdc, 0, 0, 240, 320, &g_BackBitmap);
            LoadBitmapFromFile(hdc, &bmp, img_name);
            FillBoxWithBitmap(hdc, 0, 0, 0, 0, &bmp);

            if (fGetDC)
                ReleaseDC (hdc);
            return 0;
        }
        case MSG_PAINT:
        {
            break;
        }
        case MSG_CLOSE:
            KillTimer(hWnd, 1000);
            break;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitCreateInfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_AUTOPRIVATEDC;
    pCreateInfo->spCaption = "";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor (IDC_ARROW);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ImageperspectiveProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = pCreateInfo->lx + 240;
    pCreateInfo->by = pCreateInfo->ty + 320;
    pCreateInfo->iBkColor = PIXEL_lightwhite;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int agg_main(int argc, char* argv[])
{
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;
    MSG Msg;
    int iRet = 0;

    if (InitGUI (argc, (const char **)argv) != 0) 
        return 1;

#ifdef _MGRM_PROCESSES
    int i;
    const char* layer = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp (argv[i], "-layer") == 0) {
            layer = argv[i + 1];
            break;
        }
    }

    GetLayerInfo (layer, NULL, NULL, NULL);

    if (JoinLayer (layer, argv[0], 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        return 1;
    }
#endif
    InitCreateInfo (&CreateInfo);
    hMainWnd = CreateMainWindow (&CreateInfo);
    if (hMainWnd == HWND_INVALID)
        return 3;

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);
    return 1;
}
#endif
