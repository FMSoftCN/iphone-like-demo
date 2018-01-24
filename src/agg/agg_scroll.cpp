#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

//extern "C" {
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
//}


#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"
#include "agg_conv_bspline.h"
#include "agg_conv_segmentator.h"
#include "agg_trans_single_path.h"
#include "agg_image_accessors.h"
#include "agg_platform_support.h"
#include "interactive_polygon.h"
#include "agg_span_allocator.h"
#include "agg_span_interpolator_linear.h"
#include "agg_image_accessors.h"
#include "agg_span_image_filter_rgb.h"
#include "agg_span_image_filter_rgba.h"
#include "agg_span_image_filter_gray.h"
#include "agg_path_storage.h"
#include "agg_curves.h"
#include "agg_scroll.h"

#define AGG_BGRA32
#include "pixel_formats.h"

static RECT g_Rect = {0, 0, 0, 0};
static int g_nFlame = 0;
static int g_nCurrent = 0;
static double g_dMaxLength = 0.0;
static HDC g_hDownDC;
static BOOL g_bRender;
static DWORD g_rgbaFront;
static DWORD g_rgbaScroll;
//static PBITMAP g_pBitmap;

typedef agg::renderer_base <pixfmt> renderer_base;
typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;
typedef agg::scanline_p8 scanline_type;
typedef agg::renderer_base<pixfmt_pre> renderer_base_pre;

class ScrollPageAppClass: public agg::platform_support
{
    public:

        //font_engine_type             m_feng;
        //font_manager_type            m_fman;
        //agg::interactive_polygon     m_poly;
        //agg::interactive_polygon     m_line;
        //agg::interactive_polygon     m_curv;
        //agg::slider_ctrl<agg::rgba8> m_num_points;
        //agg::cbox_ctrl<agg::rgba8>   m_close;
        //agg::cbox_ctrl<agg::rgba8>   m_preserve_x_scale;
        //agg::cbox_ctrl<agg::rgba8>   m_fixed_len;
        //agg::cbox_ctrl<agg::rgba8>   m_animate;

        double                       m_dx[6];
        double                       m_dy[6];
        double                       mCurveUpX [4];
        double                       mCurveUpY [4];
        double                       mCurveDownX [4];
        double                       mCurveDownY [4];
        double                       mLineX [3];
        double                       mLineY [3];
        bool                         m_prev_animate;

        ScrollPageAppClass(agg::pix_format_e format, bool flip_y) :
            agg::platform_support(format, flip_y)
            //m_feng(dc),
            //m_fman(m_feng),
            //m_poly(6, 0.0),
            //m_line(4, 0.0),
            //m_curv(4, 0.0),
            //m_num_points      (5.0, 5.0, 340.0, 12.0, !flip_y),
            //m_close           (350, 5.0,  "Close", !flip_y),
            //m_preserve_x_scale(460, 5.0,  "Preserve X scale", !flip_y),
            //m_fixed_len       (350, 25.0, "Fixed Length", !flip_y),
            //m_animate         (460, 25.0, "Animate", !flip_y),
            //m_prev_animate(false)
            {
                //add_ctrl(m_close);
                //add_ctrl(m_preserve_x_scale);
                //add_ctrl(m_fixed_len);
                //add_ctrl(m_animate);
                //m_preserve_x_scale.status(true);
                //m_fixed_len.status(true);
                //m_num_points.range(10.0, 400.0);
                //m_num_points.value(200.0);
                //m_num_points.label("Number of intermediate Points = %.3f");
                //add_ctrl(m_num_points);
            }
        
        virtual void on_init()
        {
#if 0
            m_line.xn (0) = 50;
            m_line.yn (0) = 50;
            m_line.xn (1) = 550;
            m_line.yn (1) = 50;
            m_line.xn (2) = 550;
            m_line.yn (2) = 550;
            m_line.xn (3) = 50;
            m_line.yn (3) = 550;
            m_curv.xn (0) = 550;
            m_curv.yn (0) = 50;
            m_curv.xn (1) = 550;
            m_curv.yn (1) = 50;
            m_curv.xn (2) = 550;
            m_curv.yn (2) = 50;
            m_curv.xn (3) = 550;
            m_curv.yn (3) = 50;
#endif
            int i;
            for (i = 0; i < 4; i++)
            {
                mCurveUpX [i] = g_Rect.right;
                mCurveUpY [i] = g_Rect.top;
                mCurveDownX [i] = g_Rect.right;
                mCurveDownY [i] = g_Rect.top;
            }
            for (i = 0; i < 3; i++)
            {
                mLineX [i] = g_Rect.right;
                mLineY [i] = g_Rect.top;
            }
        }

        virtual void DrawScrollPage()
        {
            pixfmt pixf (rbuf_window());
            pixfmt img_pixf_test (rbuf_img(0));
            pixfmt img_pixf_test2 (rbuf_img(1));
            pixfmt_pre        pixf_pre(rbuf_window());
            renderer_base_pre rb_pre(pixf_pre);
            renderer_base rb(pixf);
            renderer_solid r(rb);

#if 0
            rb.clear(agg::rgba(1, 1, 1));
#endif

            agg::curve4 cTest, cTest2;
            agg::path_storage pLine;
            agg::path_storage pStrokeLine;
            agg::path_storage pLeftPath;
            //agg::path_storage pRightPath;

            scanline_type sl;
            agg::rasterizer_scanline_aa<> ras;

            //unsigned int tick_count = GetTickCount ();
            //pRightPath.move_to (g_Rect.right, g_Rect.top);
           // pRightPath.line_to (mCurveDownX [0], mCurveDownY [0]);
            //pLeftPath.move_to (g_Rect.left, g_Rect.top);
            pLeftPath.move_to (g_Rect.right, g_Rect.bottom);
            pLeftPath.line_to (mCurveDownX [0], mCurveDownY [0]);
#if 1
            cTest.init(mCurveDownX [0], mCurveDownY [0],
                    mCurveDownX [1], mCurveDownY [1],
                    mCurveDownX [2], mCurveDownY [2],
                    mCurveDownX [3], mCurveDownY [3]);
#endif

#if 1
            cTest2.init(mCurveUpX [0], mCurveUpY [0],
                    mCurveUpX [1], mCurveUpY [1],
                    mCurveUpX [2], mCurveUpY [2],
                    mCurveUpX [3], mCurveUpY [3]);
#endif
            pLeftPath.join_path (cTest);
            //pLeftPath.join_path (cTest2);

            //pRightPath.join_path (cTest);

            if (mLineX [1] < 0)
            {
                double /*a, */b;
                double m;
                m = mCurveDownY [0] - g_Rect.top;
                //a = ((2 * m - (g_Rect.right - g_Rect.left)) * m) / (2 * m - m / 3);
                b = (2 * (2 * m - (g_Rect.right - g_Rect.left)) * m) / (3 * (2 *m - m / 6));
                pLeftPath.move_to (mLineX [2], mLineY [2]);
                pLeftPath.line_to (g_Rect.left, b);
                pLeftPath.line_to (g_Rect.left, g_Rect.bottom);
                pLeftPath.line_to (g_Rect.right, g_Rect.bottom);

            }
            else
            {
                pLeftPath.move_to (mLineX [2], mLineY [2]);
                pLeftPath.line_to (mLineX [1], mLineY [1]);
                //pLeftPath.line_to (g_Rect.right, g_Rect.bottom);
                pLeftPath.line_to (g_Rect.left, g_Rect.top);
                pLeftPath.line_to (g_Rect.left, g_Rect.bottom);
                //pLeftPath.line_to (g_Rect.left, g_Rect.top);
                pLeftPath.line_to (g_Rect.right, g_Rect.bottom);
            }

            //pRightPath.move_to (mLineX [2], mLineY [2]);
            //pRightPath.line_to (mLineX [1], mLineY [1]);
            //pRightPath.line_to (g_Rect.right, g_Rect.top);

            //printf ("1:start:%d,end=%d\n", tick_count, GetTickCount());
            //tick_count = GetTickCount ();


            typedef agg::conv_bspline<agg::curve4> curve4_type;
            //typedef agg::conv_bspline<agg::path_storage> path_storage_type;


            curve4_type bezier (cTest);
            curve4_type bezier2 (cTest2);

            //bezier.interpolation_step(1.0 / m_num_points.value());

            typedef agg::conv_stroke<curve4_type> beziler_stroke_type;
            beziler_stroke_type strokebeziner(bezier);
            beziler_stroke_type strokebeziner2 (bezier2);
            
            //typedef agg::conv_stroke<curve4_type> beziler_stroke_type;
            typedef agg::conv_stroke<agg::path_storage> storage_type;
            storage_type strokepath (pStrokeLine);


            agg::span_allocator<color_type> sa_test;
            agg::span_allocator<color_type> sa_test2;

            agg::trans_affine img_mtx_test;
            agg::trans_affine img_mtx_test2;

#if 0
            img_mtx_test *= agg::trans_affine_translation (g_Rect.left, g_Rect.top);
            img_mtx_test *= trans_affine_resizing();
            img_mtx_test.invert();

            img_mtx_test2 *= agg::trans_affine_translation (g_Rect.left, g_Rect.top);
            img_mtx_test2 *= trans_affine_resizing();
            img_mtx_test2.invert();
#endif

            typedef agg::span_interpolator_linear<> interpolator_type;
            typedef agg::image_accessor_clip<pixfmt> img_source_type;
            typedef agg::span_image_filter_rgb_nn<img_source_type,
                    interpolator_type> span_gen_type;

            interpolator_type interpolator_test (img_mtx_test);
            interpolator_type interpolator_test2 (img_mtx_test2);

#if 0
            typedef agg::span_image_filter_rgb_bilinear_clip<pixfmt,
                    interpolator_type> span_gen_type;
            span_gen_type sg_test(img_pixf_test, agg::rgba_pre(0, 0.4, 0, 0.5), interpolator_test);
            span_gen_type sg_test2(img_pixf_test2, agg::rgba_pre(0, 0.4, 0, 0.5), interpolator_test2);
#endif
            img_source_type img_src(img_pixf_test, agg::rgba_pre(0, 0.4, 0, 0.5)); 
            img_source_type img_src2(img_pixf_test2, agg::rgba_pre(0, 0.4, 0, 0.5)); 
            
            span_gen_type sg_test(img_src, interpolator_test);
            span_gen_type sg_test2(img_src2, interpolator_test2);
            
            strokebeziner.width(2.0);
            strokebeziner2.width(2.0);
            strokepath.width(2.0);

            r.color (agg::rgba8(170, 50, 20, 100));

            //printf ("2:start:%d,end=%d\n", tick_count, GetTickCount());
            //tick_count = GetTickCount ();

            //r.color (agg::rgba8(170, 150, 20, 100));
#if 1
            ras.reset();
            ras.add_path (pLeftPath);
            agg::render_scanlines_aa (ras, sl, rb_pre, sa_test, sg_test);

            //ras.reset();
            //ras.add_path (pRightPath);
            //agg::render_scanlines_aa (ras, sl, rb_pre, sa_test2, sg_test2);
#endif

            if ((g_nCurrent != 0) && (g_bRender))
            {
                unsigned h, g, b, a;
                h = (g_rgbaFront >> 24) & 0x000000ff;
                g = (g_rgbaFront >> 16) & 0x000000ff;
                b = (g_rgbaFront >> 8) & 0x000000ff;
                a = g_rgbaFront & 0x000000ff;

                //r.color (agg::rgba8(110, 110, 110, 127));
                r.color (agg::rgba8 (h, g, b, a));
                //r.color (agg::rgba8(70, 70, 70, 100));
                ras.reset ();
                ras.add_path (pLeftPath);
                agg::render_scanlines (ras, sl, r);
            }
            //printf ("3:start:%d,end=%d\n", tick_count, GetTickCount());
            //tick_count = GetTickCount ();

#if 0
            ras.reset();
            ras.add_path (strokebeziner);
            agg::render_scanlines (ras, sl, r);

            printf ("3-1:start:%d,end=%d\n", tick_count, GetTickCount());
            tick_count = GetTickCount ();

            ras.reset();
            ras.add_path (strokebeziner2);
            agg::render_scanlines (ras, sl, r);

            printf ("3-2:start:%d,end=%d\n", tick_count, GetTickCount());
            tick_count = GetTickCount ();

            ras.reset();
            pStrokeLine.move_to(mLineX[0], mLineY[0]);
            pStrokeLine.line_to(mLineX[1], mLineY[1]);
            pStrokeLine.line_to(mLineX[2], mLineY[2]);
            ras.add_path (strokepath);
            agg::render_scanlines (ras, sl, r);

            printf ("3-4:start:%d,end=%d\n", tick_count, GetTickCount());
            tick_count = GetTickCount ();
#endif
            
            //r.color (agg::rgba8(45, 65, 95, 204));

            if (g_bRender)
            {
                unsigned h, g, b, a;
                h = (g_rgbaScroll >> 24) & 0x000000ff;
                g = (g_rgbaScroll >> 16) & 0x000000ff;
                b = (g_rgbaScroll >> 8) & 0x000000ff;
                a = g_rgbaScroll & 0x000000ff;

                r.color (agg::rgba8 (h, g, b, a));
            }

            if (mLineX [1] < 0)
            {
                double a, b;
                double m;
                m = mCurveDownY [0] - g_Rect.top;
                //a = 6 *m - 3 * (g_Rect.right - g_Rect.left);
                a = ((2 * m - (g_Rect.right - g_Rect.left)) * m) / (2 * m - m / 3);
                b = (2 * (2 * m - (g_Rect.right - g_Rect.left)) * m) / (3 * (2 *m - m / 6));
                //printf ("mLineX [1] %f, a %f, b %f\n", mLineX [1], a, b);
                pLine.move_to (mLineX [0], mLineY [0]);
                pLine.line_to (g_Rect.left, a);
                pLine.line_to (g_Rect.left, b);
                pLine.line_to (mLineX [2], mLineY [2]);
                //pLine.move_to (mLineX [0], mLineY [0]);
                //pLine.line_to (mLineX [1], mLineY [1]);
                //pLine.line_to (mLineX [2], mLineY [2]);
            }
            else
            {
                pLine.move_to (mLineX [0], mLineY [0]);
                pLine.line_to (mLineX [1], mLineY [1]);
                pLine.line_to (mLineX [2], mLineY [2]);
            }
            pLine.join_path (cTest2);




            ras.reset ();
            ras.add_path (pLine);
            agg::render_scanlines (ras, sl, r);

            //printf ("4:start:%d,end=%d\n", tick_count, GetTickCount());
            //tick_count = GetTickCount ();
        }

        void MoveKeyPoints (double dMove)
        {
            int i;
            for (i = 0; i < 4; i++)
            {
                mCurveDownX [i] = g_Rect.right;
                mCurveDownY [i] = g_Rect.top;
                mCurveUpX [i] = g_Rect.right;
                mCurveUpY [i] = g_Rect.top;
            }

            for (i = 0; i < 3; i++)
            {
                mLineX [i] = g_Rect.right;
                mLineY [i] = g_Rect.top;
            }

#if 0
            mCurveDownX [0] = mCurveDownX [0] - dMove;

            //2.move the down arc x bottom right
            mCurveDownX [1] = mCurveDownX[0] + (g_Rect.right - mCurveDownX [0]) / 3;
            //3.move the down arc x top right
            mCurveDownX [2] = mCurveDownX [1];
            mCurveDownY [2] = g_Rect.top + (g_Rect.right - mCurveDownX [0]) / 6;
            mCurveDownX [3] = mCurveDownX [2];
            mCurveDownY [3] = mCurveDownY [2];
            //4.move the top arc x bottom right
            mCurveUpX [0] = mCurveDownX [3];
            mCurveUpY [0] = mCurveDownY [3];
            mCurveUpX [1] = mCurveUpX [0];
            mCurveUpY [1] = mCurveUpY [0];
            //5.move the top arc x top right
            mCurveUpX [2] = mCurveDownX [1];
            mCurveUpY [2] = g_Rect.top + (g_Rect.right - mCurveDownX [0]) / 3;
            //6.move the top arc x top left
            mCurveUpX [3] = mCurveDownX [0];
            mCurveUpY [3] = mCurveUpY [2];
            mLineX [0] = mCurveUpX [3];
            mLineY [0] = mCurveUpY [3];
            mLineY [1] = g_Rect.top + (g_Rect.right - mCurveDownX [0]) * 2;
            mLineX [2] = mCurveUpX [0];
            mLineY [2] = mCurveUpY [0];
#endif
            mCurveDownY [0] = mCurveDownY [0] + dMove;
            //2.move the down arc x bottom right
            mCurveDownY [1] = mCurveDownY[0] - (mCurveDownY [0] - g_Rect.top) / 3;
            // 3.move the down arc x top right
            mCurveDownY [2] = mCurveDownY [1];
            mCurveDownX [2] = g_Rect.right - (mCurveDownY [0] - g_Rect.top) / 6;
            mCurveDownY [3] = mCurveDownY [2];
            mCurveDownX [3] = mCurveDownX [2];
            // 4.move the top arc x bottom right
            mCurveUpX [0] = mCurveDownX [3];
            mCurveUpY [0] = mCurveDownY [3];
            mCurveUpX [1] = mCurveUpX [0];
            mCurveUpY [1] = mCurveUpY [0];
            //5.move the top arc x top right
            mCurveUpX [2] = g_Rect.right - (mCurveDownY [0] - g_Rect.top)  / 3;
            mCurveUpY [2] = mCurveDownY [1];//g_Rect.top + (g_Rect.right - mCurveDownX [0]) / 3;
            //6.move the top arc x top left
            mCurveUpY [3] = mCurveDownY [0];
            mCurveUpX [3] = mCurveUpX [2];
            mLineX [0] = mCurveUpX [3];
            mLineY [0] = mCurveUpY [3];
            mLineX [1] = g_Rect.right - (mCurveDownY [0] - g_Rect.top)  * 2;
            mLineX [2] = mCurveUpX [0];
            mLineY [2] = mCurveUpY [0];
        }
};


enum flip_y_e { flip_y = true };

ScrollPageAppClass g_ScrollPageClass (pix_format, flip_y);

#ifdef __cplusplus
extern "C" {
#endif

BOOL InitScrollPage (HDC hUpLayerDC, HDC hDownLayerDC, int nWidth, int nHeight, int nFlame)
{

    if (!nFlame)
    {
        fprintf (stderr, "ScrollPage Error: nFlame is 0 \n");
        return FALSE;
    }

    g_Rect.left = 0;
    g_Rect.right = nWidth;
    g_Rect.top = 0;
    g_Rect.bottom = nHeight;

    g_nFlame = nFlame;
    g_dMaxLength = (4 * (g_Rect.right - g_Rect.left) + 11 * (g_Rect.bottom - g_Rect.top)) / 8;

    if(!g_ScrollPageClass.init (nWidth, nHeight, agg::window_resize))
    {
        fprintf (stderr, "ScrollPage Error: ScrollPage init error!\n");
        return FALSE; 
    }

    //g_pBitmap = NULL;  
    g_hDownDC = hDownLayerDC;
    g_bRender = FALSE;

    g_ScrollPageClass.load_img_from_dc (0, hUpLayerDC);
    //g_ScrollPageClass.load_img_from_dc (1, hDownLayerDC);
    

    return TRUE;
}

void SetScrollPageRender (BOOL bOpen, DWORD rgbaFront, DWORD rgbaScroll)
{
    g_bRender = bOpen;
    g_rgbaFront = rgbaFront;
    g_rgbaScroll = rgbaScroll;
}

//BOOL InitRollPage (HDC hUpLayerDC, PBITMAP pbmp, int nWidth, int nHeight, int nFlame)
BOOL InitRollPage (PBITMAP psrcbmp, PBITMAP pbmp, int nWidth, int nHeight, int nFlame)
{

    if (!nFlame)
    {
        fprintf (stderr, "ScrollPage Error: nFlame is 0 \n");
        return FALSE;
    }

    g_Rect.left = 0;
    g_Rect.right = nWidth;
    g_Rect.top = 0;
    g_Rect.bottom = nHeight;

    g_nFlame = nFlame;
    g_dMaxLength = (4 * (g_Rect.right - g_Rect.left) + 11 * (g_Rect.bottom - g_Rect.top)) / 8;

    if(!g_ScrollPageClass.init (nWidth, nHeight, agg::window_resize))
    {
        fprintf (stderr, "ScrollPage Error: ScrollPage init error!\n");
        return FALSE; 
    }

    //g_ScrollPageClass.load_img_from_dc (0, hUpLayerDC);
    g_ScrollPageClass.load_img_from_bitmap (0, psrcbmp);
    g_ScrollPageClass.load_img_from_bitmap (1, pbmp);

    g_hDownDC = NULL;
    g_bRender = FALSE;
    //g_pBitmap = g_ScrollPageClass.m_specific->m_surf_img[1];  
    
    return TRUE;
}

BOOL ScrollPageFlame (int nIdxFlame, PBITMAP pBitmap, HDC hdc_show) 
{
     double dMove = 0.0;
     HDC hdc;

     if (nIdxFlame > g_nFlame)
         return FALSE;//nIdxFlame = g_nFlame;

     if (nIdxFlame < 0)
         return FALSE;//nIdxFlame = 0;

     dMove = (nIdxFlame * g_dMaxLength) / g_nFlame;

     //unsigned int tick_count = GetTickCount ();
     g_ScrollPageClass.MoveKeyPoints (dMove);
     //printf ("MoveKeyPoint start:%d,end=%d\n", tick_count, GetTickCount());
     //tick_count = GetTickCount ();
     g_nCurrent = nIdxFlame; 

     if (g_hDownDC)
         BitBlt (g_hDownDC, g_Rect.left, g_Rect.top, g_Rect.right, g_Rect.bottom, g_ScrollPageClass.rbuf_dc(), 0, 0, 0);

     g_ScrollPageClass.DrawScrollPage ();
     //printf ("DrawPage end start:%d,end=%d\n", tick_count, GetTickCount());
      
     //tick_count = GetTickCount ();
     hdc = g_ScrollPageClass.rbuf_dc ();
     BitBlt (hdc, g_Rect.left, g_Rect.top, g_Rect.right, g_Rect.bottom, hdc_show, 0, 0, 0);

     //printf ("DrawToScreen end start:%d,end=%d\n", tick_count, GetTickCount());
     return TRUE;
}

#ifdef __cplusplus
}
#endif

