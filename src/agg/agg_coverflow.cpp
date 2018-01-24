#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_u.h"
#include "agg_renderer_scanline.h"
#include "agg_path_storage.h"
#include "agg_conv_transform.h"
#include "agg_trans_affine.h"
#include "agg_trans_bilinear.h"
#include "agg_span_allocator.h"
#include "agg_span_interpolator_linear.h"
#include "agg_span_interpolator_trans.h"
#include "agg_span_subdiv_adaptor.h"
#include "agg_pixfmt_rgba.h"
#include "agg_image_accessors.h"
#include "agg_span_image_filter_rgba.h"
#include "agg_span_image_filter_rgb.h"
#include "agg_scanline_p.h"
#include "agg_platform_support.h"
#include "interactive_polygon.h"
#include "agg_coverflow.h"

#define AGG_BGRA32
#include "pixel_formats.h"

enum flip_y_e { flip_y = true };
agg::rasterizer_scanline_aa<> g_Rasterizer;
agg::scanline_p8 g_ScanLine;


/*
static double g_dMoveZ = 10.0;
static double g_dMoveX = 10.0;
static double g_dShiftRightMidX = 50.0;
*/

static bool g_bMoveDir = false;
static bool g_bMoveCoverFlow = false;
static double g_dScreenWidth;
static double g_dScreenHeight;
//static double g_dShiftRightRightX = 10.0;

static int g_nCoverFlowImg = 0;

typedef struct _ST_COVERFLOW_NODE
{
    double dUpMoveX [4];
    double dUpMoveY [4];
    double dDownMoveX [4];
    double dDownMoveY [4];
    struct _ST_COVERFLOW_NODE * pNextNode;
    struct _ST_COVERFLOW_NODE * pPreNode;
    int nImgIndex;
    int nCurPos;
}ST_COVERFLOW_NODE;

typedef struct _ST_COVERFLOW_RECT
{
    double left;
    double top;
    double right;
    double bottom;
}ST_COVERFLOW_RECT;

typedef ST_COVERFLOW_NODE* ST_COVERFLOW_NODE_PT;

static ST_COVERFLOW_NODE_PT g_pSTCoverFlowHead;
static ST_COVERFLOW_NODE_PT g_pSTCoverFlowCur;
static ST_COVERFLOW_NODE_PT g_pSTCoverFlowLeftPointer [4];
static ST_COVERFLOW_NODE_PT g_pSTCoverFlowRightPointer [4];

static int g_nLeftNumber;
static int g_nRightNumber;

static bool g_bMoveCur = false;
static bool g_bMoveBack = false;

typedef enum _EM_CUR_COVER
{
    EM_COVER_MID,
    EM_COVER_LEFT,
    EM_COVER_RIGHT
}EM_CUR_COVER;

typedef enum _EM_MOVE_DIRC
{
    EM_MID_MOVE_RIGHT = 0,
    EM_RIGHT_MOVE_MID,
    EM_MID_MOVE_LEFT,
    EM_LEFT_MOVE_MID,
    EM_MID_MOVE_FRONT,
    EM_FRONT_MOVE_MID,
    EM_RIGHT_MOVE_RIGHT,
    EM_LEFT_MOVE_LEFT,
    EM_RIGHT_MOVE_EXIT,
    EM_LEFT_MOVE_EXIT,
    EM_RIGHT_MOVE_LEFT,
    EM_EXIT_MOVE_RIGHT,
    EM_EXIT_MOVE_LEFT,
    EM_LEFT_MOVE_RIGHT
}EM_MOVE_DIRC;


class CoverFlowClass : public agg::platform_support
{
    public:
        typedef agg::pixfmt_bgra32                             pixfmt;
        typedef pixfmt::color_type                             color_type;
        typedef agg::renderer_base<pixfmt>                     renderer_base;
        typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;
        typedef agg::pixfmt_bgra32_pre         pixfmt_pre;
        typedef agg::renderer_base<pixfmt_pre> renderer_base_pre;

        agg::interactive_polygon        m_UpMove;
        agg::interactive_polygon        m_DownMove;
        double                       m_CenterX;
        double                       m_CenterY;
        int                          m_Width;
        int                          m_Height;
        int                          m_ImgNumber;

        CoverFlowClass (agg::pix_format_e format, bool flip_y) :
            agg::platform_support(format, flip_y),
            m_UpMove (4, 5.0),
            m_DownMove (4, 5.0)
    {
    }
        int GetScreenWidth (void)
        {
            return m_Width;
        }
        
        int GetScreenHeight (void)
        {
            return m_Height;
        }

        void InitInnerData (int nImgNumber, int nWidth, int nHeight)
        {
            if (!nImgNumber)
                fprintf  (stderr, "CoverFlow Error: The Number is 0 !\n");

            m_Width = nWidth;
            m_Height = nHeight;
            m_ImgNumber = nImgNumber;

            if (nImgNumber >= 7)
            {
                int i;
                ST_COVERFLOW_NODE_PT pCurNode;

                g_pSTCoverFlowHead = (ST_COVERFLOW_NODE_PT) malloc (sizeof (ST_COVERFLOW_NODE));

                pCurNode = g_pSTCoverFlowHead;
                pCurNode->nImgIndex = 0;
                pCurNode->pNextNode = NULL;
                pCurNode->pPreNode = NULL;

                for (i = 1; i < m_ImgNumber; i++)
                {
                    pCurNode->pNextNode = (ST_COVERFLOW_NODE_PT) malloc (sizeof (ST_COVERFLOW_NODE));
                    (pCurNode->pNextNode)->pPreNode = pCurNode;
                    pCurNode = pCurNode->pNextNode;
                    pCurNode->nImgIndex = i;
                    pCurNode->pNextNode = NULL;
                }
#if 1
                g_pSTCoverFlowCur = g_pSTCoverFlowHead;
                g_pSTCoverFlowLeftPointer [0] = NULL;
                g_pSTCoverFlowLeftPointer [1] = NULL;
                g_pSTCoverFlowLeftPointer [2] = NULL;
                g_pSTCoverFlowLeftPointer [3] = NULL;
                g_nLeftNumber = 0;

                g_nRightNumber = nImgNumber - 1;
                pCurNode = g_pSTCoverFlowHead->pNextNode;
                g_pSTCoverFlowRightPointer [0] = pCurNode;
                pCurNode = pCurNode->pNextNode;
                g_pSTCoverFlowRightPointer [1] = pCurNode;
                pCurNode = pCurNode->pNextNode;
                g_pSTCoverFlowRightPointer [2] = pCurNode;	
                pCurNode = pCurNode->pNextNode;
                g_pSTCoverFlowRightPointer [3] = pCurNode;	
#endif
            }
            else
            {
                int i;
                g_pSTCoverFlowHead = (ST_COVERFLOW_NODE_PT) malloc (sizeof (ST_COVERFLOW_NODE));
                ST_COVERFLOW_NODE_PT pCurNode;

                pCurNode = g_pSTCoverFlowHead;
                pCurNode->nImgIndex = 0;
                pCurNode->pNextNode = NULL;
                pCurNode->pPreNode = NULL;
                
                g_pSTCoverFlowRightPointer [0] = NULL;
                g_pSTCoverFlowRightPointer [1] = NULL;
                g_pSTCoverFlowRightPointer [2] = NULL;
                g_pSTCoverFlowRightPointer [3] = NULL;

                for (i = 1; i < nImgNumber; i++)
                {
                    pCurNode->pNextNode = (ST_COVERFLOW_NODE_PT) malloc (sizeof (ST_COVERFLOW_NODE));
                    (pCurNode->pNextNode)->pPreNode = pCurNode;
                    pCurNode = pCurNode->pNextNode;
                    g_pSTCoverFlowRightPointer [i - 1] = pCurNode;  
                    pCurNode->nImgIndex = i;
                    pCurNode->pNextNode = NULL;
                }

                g_pSTCoverFlowCur = g_pSTCoverFlowHead;
                g_pSTCoverFlowLeftPointer [0] = NULL;
                g_pSTCoverFlowLeftPointer [1] = NULL;
                g_pSTCoverFlowLeftPointer [2] = NULL;
                g_pSTCoverFlowLeftPointer [3] = NULL;
                g_nLeftNumber = 0;

                g_nRightNumber = nImgNumber - 1;

                //pCurNode = g_pSTCoverFlowHead;

            }

        }
        ~CoverFlowClass() 
        {
            ST_COVERFLOW_NODE_PT pCurNode;
            ST_COVERFLOW_NODE_PT pCurNodeNext;
            pCurNode = g_pSTCoverFlowHead;

            while (pCurNode != NULL)
            {
                pCurNodeNext = pCurNode->pNextNode;
                free (pCurNode);
                pCurNode = pCurNodeNext;
            }
        }

        int ResetCoverFlowPos (ST_COVERFLOW_NODE_PT pCurNode, EM_CUR_COVER emCurCover, int nStep)
        {
            if (!pCurNode)
                return -1;

            switch (emCurCover)
            {
                case EM_COVER_RIGHT:
                    {
                        switch (nStep)
                        {
                            case 0:
                                {
                                    pCurNode->dUpMoveX [0] = 440 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [0] = 160 * g_dScreenHeight;

                                    pCurNode->dUpMoveX [1] = 560 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [1] = 120 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [2] = 360 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [3] = 320 * g_dScreenHeight;
                                }
                                break;
                            case 1:
                                {
                                    pCurNode->dUpMoveX [0] = 480 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [0] = 160 * g_dScreenHeight;

                                    pCurNode->dUpMoveX [1] = 600 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [1] = 120 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [2] = 360 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [3] = 320 * g_dScreenHeight;

                                }
                                break;
                            case 2:
                                {
                                    pCurNode->dUpMoveX [0] = 520 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [0] = 160 * g_dScreenHeight;

                                    pCurNode->dUpMoveX [1] = 640 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [1] = 120 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [2] = 360 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [3] = 320 * g_dScreenHeight;


                                }
                                break;
                            default:
                                {
                                    pCurNode->dUpMoveX [0] = 640 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [0] = 160 * g_dScreenHeight;

                                    pCurNode->dUpMoveX [1] = 760 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [1] = 120 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [2] = 360 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [3] = 320 * g_dScreenHeight;
                                }
                                break;
                        }
                    }
                    break;
                case EM_COVER_MID:
                    {
                        pCurNode->dUpMoveX [0] = 180 * g_dScreenWidth;
                        pCurNode->dUpMoveY [0] = 140 * g_dScreenHeight;

                        pCurNode->dUpMoveX [1] = 460 * g_dScreenWidth;
                        pCurNode->dUpMoveY [1] = pCurNode->dUpMoveY [0] ;

                        pCurNode->dUpMoveY [2] = 340 * g_dScreenHeight;
                        pCurNode->dUpMoveY [3] = pCurNode->dUpMoveY [2];
                    }
                    break;
                case EM_COVER_LEFT:
                    {
                        switch (nStep)
                        {
                            case 0:
                                {
                                    pCurNode->dUpMoveX [0] = 80 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [0] = 120 * g_dScreenHeight;

                                    pCurNode->dUpMoveX [1] = 200 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [1] = 160 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [2] = 320 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [3] = 360 * g_dScreenHeight;
                                }
                                break;
                            case 1:
                                {

                                    pCurNode->dUpMoveX [0] = 40 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [0] = 120 * g_dScreenHeight;

                                    pCurNode->dUpMoveX [1] = 160 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [1] = 160 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [2] = 320 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [3] = 360 * g_dScreenHeight;
                                }
                                break;
                            case 2:
                                {
                                    pCurNode->dUpMoveX [0] = 0;
                                    pCurNode->dUpMoveY [0] = 120 * g_dScreenHeight;

                                    pCurNode->dUpMoveX [1] = 120 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [1] = 160 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [2] = 320 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [3] = 360 * g_dScreenHeight;
                                }
                                break;
                            default:
                                {
                                    pCurNode->dUpMoveX [0] = -120 * g_dScreenWidth;
                                    pCurNode->dUpMoveY [0] = 120 * g_dScreenHeight;

                                    pCurNode->dUpMoveX [1] = 0;
                                    pCurNode->dUpMoveY [1] = 160 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [2] = 320 * g_dScreenHeight;

                                    pCurNode->dUpMoveY [3] = 360 * g_dScreenHeight;
                                }
                                break;
                        }

                    }
                    break;

            }

            pCurNode->dUpMoveX [2] = pCurNode->dUpMoveX [1];
            pCurNode->dUpMoveX [3] = pCurNode->dUpMoveX [0];				

            pCurNode->dDownMoveX [0] = pCurNode->dUpMoveX [0];
            pCurNode->dDownMoveY [0] = pCurNode->dUpMoveY [0];
            pCurNode->dDownMoveX [1] = pCurNode->dUpMoveX [1];
            pCurNode->dDownMoveY [1] = pCurNode->dUpMoveY [1];
            pCurNode->dDownMoveX [2] = pCurNode->dDownMoveX [1];
            pCurNode->dDownMoveY [2] = pCurNode->dUpMoveY [1] - (pCurNode->dUpMoveY [2] - pCurNode->dUpMoveY [1]);
            pCurNode->dDownMoveX [3] = pCurNode->dDownMoveX [0];
            pCurNode->dDownMoveY [3] = pCurNode->dUpMoveY [0] - (pCurNode->dUpMoveY [3] - pCurNode->dUpMoveY [0]);
            return 0;

        }

        virtual void on_init()
        {

            int i;
            m_CenterX = width () / 2;
            m_CenterY = height () / 2;

            ST_COVERFLOW_NODE_PT pCurNode;
            pCurNode = g_pSTCoverFlowCur;

#if 1		
            for (i = 0; i < m_ImgNumber; i++)
            {
                if (i == 0)
                {
                    pCurNode->dUpMoveX [0] = 140 * g_dScreenWidth;
                    pCurNode->dUpMoveY [0] = 100 * g_dScreenHeight;

                    pCurNode->dUpMoveX [1] = 500 * g_dScreenWidth;
                    pCurNode->dUpMoveY [1] = pCurNode->dUpMoveY [0] ;

                    pCurNode->dUpMoveY [2] = 380 * g_dScreenHeight;
                    pCurNode->dUpMoveY [3] = pCurNode->dUpMoveY [2];

                }
                else if (i == 1)
                {
#if 0
                    pCurNode->dUpMoveX [0] = 440 * g_dScreenWidth;
                    pCurNode->dUpMoveY [0] = 160 * g_dScreenHeight;

                    pCurNode->dUpMoveX [1] = 560 * g_dScreenWidth;
                    pCurNode->dUpMoveY [1] = 120 * g_dScreenHeight;

                    pCurNode->dUpMoveY [2] = 360 * g_dScreenHeight;

                    pCurNode->dUpMoveY [3] = 320 * g_dScreenHeight;
#endif
                    ResetCoverFlowPos (pCurNode, EM_COVER_RIGHT, 0);
                }
                else if (i == 2)
                {
#if 0
                    pCurNode->dUpMoveX [0] = 480 * g_dScreenWidth;
                    pCurNode->dUpMoveY [0] = 160 * g_dScreenHeight;

                    pCurNode->dUpMoveX [1] = 600 * g_dScreenWidth;
                    pCurNode->dUpMoveY [1] = 120 * g_dScreenHeight;

                    pCurNode->dUpMoveY [2] = 360 * g_dScreenHeight;

                    pCurNode->dUpMoveY [3] = 320 * g_dScreenHeight;
#endif

                    ResetCoverFlowPos (pCurNode, EM_COVER_RIGHT, 1);

                }
                else if (i == 3)
                {
#if 0
                    pCurNode->dUpMoveX [0] = 520 * g_dScreenWidth;
                    pCurNode->dUpMoveY [0] = 160 * g_dScreenHeight;

                    pCurNode->dUpMoveX [1] = 640 * g_dScreenWidth;
                    pCurNode->dUpMoveY [1] = 120 * g_dScreenHeight;

                    pCurNode->dUpMoveY [2] = 360 * g_dScreenHeight;

                    pCurNode->dUpMoveY [3] = 320 * g_dScreenHeight;
#endif

                    ResetCoverFlowPos (pCurNode, EM_COVER_RIGHT, 2);


                }
                else
                {
#if 0
                    pCurNode->dUpMoveX [0] = 640 * g_dScreenWidth;
                    pCurNode->dUpMoveY [0] = 160 * g_dScreenHeight;

                    pCurNode->dUpMoveX [1] = 760 * g_dScreenWidth;
                    pCurNode->dUpMoveY [1] = 120 * g_dScreenHeight;

                    pCurNode->dUpMoveY [2] = 360 * g_dScreenHeight;

                    pCurNode->dUpMoveY [3] = 320 * g_dScreenHeight;
#endif

                    ResetCoverFlowPos (pCurNode, EM_COVER_RIGHT, 3);
                }
#endif
                    pCurNode->dUpMoveX [2] = pCurNode->dUpMoveX [1];
                    pCurNode->dUpMoveX [3] = pCurNode->dUpMoveX [0];				

                    pCurNode->dDownMoveX [0] = pCurNode->dUpMoveX [0];
                    pCurNode->dDownMoveY [0] = pCurNode->dUpMoveY [0];
                    pCurNode->dDownMoveX [1] = pCurNode->dUpMoveX [1];
                    pCurNode->dDownMoveY [1] = pCurNode->dUpMoveY [1];
                    pCurNode->dDownMoveX [2] = pCurNode->dDownMoveX [1];
                    pCurNode->dDownMoveY [2] = pCurNode->dUpMoveY [1] - (pCurNode->dUpMoveY [2] - pCurNode->dUpMoveY [1]);
                    pCurNode->dDownMoveX [3] = pCurNode->dDownMoveX [0];
                    pCurNode->dDownMoveY [3] = pCurNode->dUpMoveY [0] - (pCurNode->dUpMoveY [3] - pCurNode->dUpMoveY [0]);

                    pCurNode->nCurPos = 0;

#if 1
                    pCurNode = pCurNode->pNextNode;
#endif
#if 0
                    pCurNode = pCurNode->pPreNode;
#endif

                }




            }



            void InitCoverFlowRect (ST_COVERFLOW_NODE_PT pNode)
            {
                m_UpMove.xn (0) = pNode->dUpMoveX [0];
                m_UpMove.yn (0) = pNode->dUpMoveY [0];

                m_UpMove.xn (1) = pNode->dUpMoveX [1];
                m_UpMove.yn (1) = pNode->dUpMoveY [1];

                m_UpMove.xn (2) = pNode->dUpMoveX [2];
                m_UpMove.yn (2) = pNode->dUpMoveY [2];

                m_UpMove.xn (3) = pNode->dUpMoveX [3];
                m_UpMove.yn (3) = pNode->dUpMoveY [3];

                m_DownMove.xn (0) = pNode->dDownMoveX [0];
                m_DownMove.yn (0) = pNode->dDownMoveY [0];

                m_DownMove.xn (1) = pNode->dDownMoveX [1];
                m_DownMove.yn (1) = pNode->dDownMoveY [1];

                m_DownMove.xn (2) = pNode->dDownMoveX [2];
                m_DownMove.yn (2) = pNode->dDownMoveY [2];

                m_DownMove.xn (3) = pNode->dDownMoveX [3];
                m_DownMove.yn (3) = pNode->dDownMoveY [3];
            }


            virtual void DrawCoverFlow()
            {
                pixfmt screen_pixf ( rbuf_window() );
                pixfmt_pre screen_pixf_pre(rbuf_window());
                renderer_base rb_screen (screen_pixf);
                renderer_solid rs_screen (rb_screen);
                renderer_base_pre rb_pre(screen_pixf_pre);
                agg::image_filter_bilinear filter_kernel;
                agg::image_filter_lut filter(filter_kernel, false);
                agg::span_allocator<color_type> sa;
                int nRightNode;
                int nLeftNode;

                rb_screen.clear(agg::rgba(0, 0, 0));

                int i;

                if (g_nRightNumber > 3)
                    nRightNode = 4;
                else
                    nRightNode = g_nRightNumber;
                if (g_nLeftNumber > 3)
                    nLeftNode = 4;
                else 
                    nLeftNode = g_nLeftNumber;

                for (i = nLeftNode - 1; i >= 0 && (nLeftNode != 0); i--)
                {
                    pixfmt img_up_pixf ( rbuf_img (g_pSTCoverFlowLeftPointer [i]->nImgIndex) );

                    g_Rasterizer.clip_box ( 0, 0, width (), height ());


                    InitCoverFlowRect (g_pSTCoverFlowLeftPointer [i]);
                    g_Rasterizer.move_to_d (m_UpMove.xn(0), m_UpMove.yn(0));
                    g_Rasterizer.line_to_d (m_UpMove.xn(1), m_UpMove.yn(1));
                    g_Rasterizer.line_to_d (m_UpMove.xn(2), m_UpMove.yn(2));
                    g_Rasterizer.line_to_d (m_UpMove.xn(3), m_UpMove.yn(3));

                    agg::trans_bilinear imgTransUp ( m_UpMove.polygon(), 0, 0, 
                            rbuf_img(g_pSTCoverFlowLeftPointer [i]->nImgIndex).width(), 
                            rbuf_img(g_pSTCoverFlowLeftPointer [i]->nImgIndex).height ());

                    agg::trans_bilinear imgTransDown ( m_DownMove.polygon(), 0, 0, 
                            rbuf_img(g_pSTCoverFlowLeftPointer [i]->nImgIndex).width(), 
                            rbuf_img(g_pSTCoverFlowLeftPointer [i]->nImgIndex).height ());

                    typedef agg::image_accessor_clone <pixfmt> img_accessor_type;

                    typedef agg::span_interpolator_linear<agg::trans_bilinear> interpolator_type;

                    interpolator_type interPolatorUp (imgTransUp);
                    img_accessor_type imgAccessorUp (img_up_pixf);

                    interpolator_type interPolatorDown (imgTransDown);
                    img_accessor_type imgAccessorDown (img_up_pixf);

#if 0
                    typedef agg::span_image_filter_rgba_nn <img_accessor_type,
                            interpolator_type> span_gen_type;

                    span_gen_type sg_up (imgAccessorUp, interPolatorUp);
                    span_gen_type sg_down (imgAccessorDown, interPolatorDown);
#endif
                    typedef agg::span_image_filter_rgba_2x2 <img_accessor_type,
                            interpolator_type> span_gen_type;

                    span_gen_type sg_up (imgAccessorUp, interPolatorUp, filter);
                    span_gen_type sg_down (imgAccessorDown, interPolatorDown, filter);

                    agg::render_scanlines_aa (g_Rasterizer, g_ScanLine, rb_pre, sa, sg_up);

                    g_Rasterizer.reset ();

                    g_Rasterizer.move_to_d (m_DownMove.xn(0), m_DownMove.yn(0));
                    g_Rasterizer.line_to_d (m_DownMove.xn(1), m_DownMove.yn(1));
                    g_Rasterizer.line_to_d (m_DownMove.xn(2), m_DownMove.yn(2));
                    g_Rasterizer.line_to_d (m_DownMove.xn(3), m_DownMove.yn(3));

                    agg::render_scanlines_aa (g_Rasterizer, g_ScanLine, rb_pre, sa, sg_down);

                    rs_screen.color (agg::rgba8(0, 0, 0, 127));                     
                    agg::render_scanlines (g_Rasterizer, g_ScanLine, rs_screen);
                }


                for (i = nRightNode - 1; i >= 0  && (nRightNode != 0); i--)
                {
                    pixfmt img_up_pixf ( rbuf_img (g_pSTCoverFlowRightPointer [i]->nImgIndex) );

                    g_Rasterizer.clip_box ( 0, 0, width (), height ());


                    InitCoverFlowRect (g_pSTCoverFlowRightPointer [i]);
                    g_Rasterizer.move_to_d (m_UpMove.xn(0), m_UpMove.yn(0));
                    g_Rasterizer.line_to_d (m_UpMove.xn(1), m_UpMove.yn(1));
                    g_Rasterizer.line_to_d (m_UpMove.xn(2), m_UpMove.yn(2));
                    g_Rasterizer.line_to_d (m_UpMove.xn(3), m_UpMove.yn(3));

                    agg::trans_bilinear imgTransUp ( m_UpMove.polygon(), 0, 0, 
                            rbuf_img(g_pSTCoverFlowRightPointer [i]->nImgIndex).width(), 
                            rbuf_img(g_pSTCoverFlowRightPointer [i]->nImgIndex).height ());

                    agg::trans_bilinear imgTransDown ( m_DownMove.polygon(), 0, 0, 
                            rbuf_img(g_pSTCoverFlowRightPointer [i]->nImgIndex).width(), 
                            rbuf_img(g_pSTCoverFlowRightPointer [i]->nImgIndex).height ());

                    typedef agg::image_accessor_clone <pixfmt> img_accessor_type;

                    typedef agg::span_interpolator_linear<agg::trans_bilinear> interpolator_type;

                    interpolator_type interPolatorUp (imgTransUp);
                    img_accessor_type imgAccessorUp (img_up_pixf);

                    interpolator_type interPolatorDown (imgTransDown);
                    img_accessor_type imgAccessorDown (img_up_pixf);



#if 0
                    typedef agg::span_image_filter_rgba_nn <img_accessor_type,
                            interpolator_type> span_gen_type;

                    span_gen_type sg_up (imgAccessorUp, interPolatorUp);
                    span_gen_type sg_down (imgAccessorDown, interPolatorDown);
#endif

                    typedef agg::span_image_filter_rgba_2x2 <img_accessor_type,
                            interpolator_type> span_gen_type;

                    span_gen_type sg_up (imgAccessorUp, interPolatorUp, filter);
                    span_gen_type sg_down (imgAccessorDown, interPolatorDown, filter);

                    agg::render_scanlines_aa (g_Rasterizer, g_ScanLine, rb_pre, sa, sg_up);

                    g_Rasterizer.reset ();

                    g_Rasterizer.move_to_d (m_DownMove.xn(0), m_DownMove.yn(0));
                    g_Rasterizer.line_to_d (m_DownMove.xn(1), m_DownMove.yn(1));
                    g_Rasterizer.line_to_d (m_DownMove.xn(2), m_DownMove.yn(2));
                    g_Rasterizer.line_to_d (m_DownMove.xn(3), m_DownMove.yn(3));

                    agg::render_scanlines_aa (g_Rasterizer, g_ScanLine, rb_pre, sa, sg_down);

                    rs_screen.color (agg::rgba8(0, 0, 0, 127));                     
                    agg::render_scanlines (g_Rasterizer, g_ScanLine, rs_screen);
                }

                {
                    pixfmt img_up_pixf ( rbuf_img (g_pSTCoverFlowCur->nImgIndex) );

                    g_Rasterizer.clip_box ( 0, 0, width (), height ());


                    InitCoverFlowRect (g_pSTCoverFlowCur);
                    g_Rasterizer.move_to_d (m_UpMove.xn(0), m_UpMove.yn(0));
                    g_Rasterizer.line_to_d (m_UpMove.xn(1), m_UpMove.yn(1));
                    g_Rasterizer.line_to_d (m_UpMove.xn(2), m_UpMove.yn(2));
                    g_Rasterizer.line_to_d (m_UpMove.xn(3), m_UpMove.yn(3));

                    agg::trans_bilinear imgTransUp ( m_UpMove.polygon(), 0, 0, 
                            rbuf_img(g_pSTCoverFlowCur->nImgIndex).width(), 
                            rbuf_img(g_pSTCoverFlowCur->nImgIndex).height ());

                    agg::trans_bilinear imgTransDown ( m_DownMove.polygon(), 0, 0, 
                            rbuf_img(g_pSTCoverFlowCur->nImgIndex).width(), 
                            rbuf_img(g_pSTCoverFlowCur->nImgIndex).height ());

                    typedef agg::image_accessor_clone <pixfmt> img_accessor_type;

                    typedef agg::span_interpolator_linear<agg::trans_bilinear> interpolator_type;

                    interpolator_type interPolatorUp (imgTransUp);
                    img_accessor_type imgAccessorUp (img_up_pixf);

                    interpolator_type interPolatorDown (imgTransDown);
                    img_accessor_type imgAccessorDown (img_up_pixf);


#if 0
                    typedef agg::span_image_filter_rgba_nn <img_accessor_type,
                            interpolator_type> span_gen_type;

                    span_gen_type sg_up (imgAccessorUp, interPolatorUp);
                    span_gen_type sg_down (imgAccessorDown, interPolatorDown);
#endif

                    typedef agg::span_image_filter_rgba_2x2 <img_accessor_type,
                            interpolator_type> span_gen_type;

                    span_gen_type sg_up (imgAccessorUp, interPolatorUp, filter);
                    span_gen_type sg_down (imgAccessorDown, interPolatorDown, filter);

                    agg::render_scanlines_aa (g_Rasterizer, g_ScanLine, rb_pre, sa, sg_up);

                    g_Rasterizer.reset ();


                    g_Rasterizer.move_to_d (m_DownMove.xn(0), m_DownMove.yn(0));
                    g_Rasterizer.line_to_d (m_DownMove.xn(1), m_DownMove.yn(1));
                    g_Rasterizer.line_to_d (m_DownMove.xn(2), m_DownMove.yn(2));
                    g_Rasterizer.line_to_d (m_DownMove.xn(3), m_DownMove.yn(3));

                    agg::render_scanlines_aa (g_Rasterizer, g_ScanLine, rb_pre, sa, sg_down);

                    rs_screen.color (agg::rgba8(0, 0, 0, 127));                     
                    agg::render_scanlines (g_Rasterizer, g_ScanLine, rs_screen);
                }

#if 0
        pixfmt            pixf(rbuf_window());
        pixfmt_pre        pixf_pre(rbuf_window());
        renderer_base     rb(pixf);

        renderer_solid r(rb);
        rb.clear(agg::rgba(0, 0, 0));

        pixfmt pixf_img(rbuf_img(g_pSTCoverFlowCur->nImgIndex));

        typedef agg::image_accessor_clone<pixfmt> img_accessor_type;
        img_accessor_type ia(pixf_img);

agg::rasterizer_scanline_aa<> g_rasterizer;
agg::scanline_u8  g_scanline;
                    g_rasterizer.clip_box ( 0, 0, width (), height ());


                    InitCoverFlowRect (g_pSTCoverFlowCur);
                    g_rasterizer.move_to_d (m_UpMove.xn(0), m_UpMove.yn(0));
                    g_rasterizer.line_to_d (m_UpMove.xn(1), m_UpMove.yn(1));
                    g_rasterizer.line_to_d (m_UpMove.xn(2), m_UpMove.yn(2));
                    g_rasterizer.line_to_d (m_UpMove.xn(3), m_UpMove.yn(3));
                    
                    agg::trans_bilinear tr ( m_UpMove.polygon(), 0, 0, 
                            rbuf_img(g_pSTCoverFlowCur->nImgIndex).width(), 
                            rbuf_img(g_pSTCoverFlowCur->nImgIndex).height ());

                    typedef agg::span_interpolator_linear<agg::trans_bilinear> interpolator_type;
                    interpolator_type interpolator(tr);

                    typedef agg::span_image_filter_rgba_2x2<img_accessor_type,
                                                            interpolator_type> span_gen_type;
                    span_gen_type sg(ia, interpolator, filter);
                    agg::render_scanlines_aa(g_rasterizer, g_scanline, rb_pre, sa, sg);
#endif
            }

            int CoverFlowFromRightToLeft (void)
            {   
                if (g_pSTCoverFlowRightPointer [0] != NULL)
                {
                    if  ((g_pSTCoverFlowCur->dUpMoveY [0] < (140 * g_dScreenHeight)) && (g_bMoveCur == false))
                    {
                        g_bMoveCur = false;
                        MoveNode (g_pSTCoverFlowCur, EM_FRONT_MOVE_MID);
                        //force_redraw();
                        return 0;
                    }

                    if (g_bMoveCur == false)
                    {
                        g_bMoveCur = true;  
                        g_nLeftNumber ++;

                        if (g_nLeftNumber >= 3)
                        {
                            g_pSTCoverFlowLeftPointer [3] = g_pSTCoverFlowLeftPointer [2];
                            g_pSTCoverFlowLeftPointer [2] = g_pSTCoverFlowLeftPointer [1];
                            g_pSTCoverFlowLeftPointer [1] = g_pSTCoverFlowLeftPointer [0];
                            g_pSTCoverFlowLeftPointer [0] = g_pSTCoverFlowCur;
                        }
                        else if (g_nLeftNumber == 2)
                        {
                            g_pSTCoverFlowLeftPointer [2] = NULL;
                            g_pSTCoverFlowLeftPointer [1] = g_pSTCoverFlowLeftPointer [0];
                            g_pSTCoverFlowLeftPointer [0] = g_pSTCoverFlowCur;
                        }
                        else if (g_nLeftNumber == 1)
                        {
                            g_pSTCoverFlowLeftPointer [2] = NULL;
                            g_pSTCoverFlowLeftPointer [1] = NULL;
                            g_pSTCoverFlowLeftPointer [0] = g_pSTCoverFlowCur;
                        }

                        g_pSTCoverFlowCur = g_pSTCoverFlowRightPointer [0];
                        g_nRightNumber -- ;
                        if (g_nRightNumber >= 3)
                        {
                            g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowRightPointer [1];
                            g_pSTCoverFlowRightPointer [1] = g_pSTCoverFlowRightPointer [2];
                            g_pSTCoverFlowRightPointer [2] = g_pSTCoverFlowRightPointer [2]->pNextNode;
                        }
                        else if (g_nRightNumber == 2)
                        {
                            g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowRightPointer [1];
                            g_pSTCoverFlowRightPointer [1] = g_pSTCoverFlowRightPointer [2];
                            g_pSTCoverFlowRightPointer [2] = g_pSTCoverFlowRightPointer [2]->pNextNode;    
                        }
                        else if (g_nRightNumber == 1)
                        {
                            g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowRightPointer [1];
                            g_pSTCoverFlowRightPointer [1] = g_pSTCoverFlowRightPointer [2];
                        }
                        else
                        {
                            g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowRightPointer [1];
                        }
                    }


                    if ((g_pSTCoverFlowCur->dUpMoveY [1] < (138 * g_dScreenHeight)) && (g_bMoveBack == false))
                    {
                        if (g_nLeftNumber > 3)
                        {
                            MoveNode (g_pSTCoverFlowLeftPointer [2]->pPreNode, EM_LEFT_MOVE_EXIT);
                        }
                        MoveNode (g_pSTCoverFlowLeftPointer [0], EM_MID_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowLeftPointer [1], EM_LEFT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowLeftPointer [2], EM_LEFT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowCur, EM_RIGHT_MOVE_MID);
                        MoveNode (g_pSTCoverFlowRightPointer [0], EM_RIGHT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowRightPointer [1], EM_RIGHT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowRightPointer [2], EM_EXIT_MOVE_RIGHT);

                        //force_redraw();
                    }
                    else
                    {
                        if (g_bMoveBack == false)
                        {
                            if (g_nLeftNumber > 3)
                            {
                                ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [2]->pPreNode, EM_COVER_LEFT, 3);
                            }
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [0], EM_COVER_LEFT, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [1], EM_COVER_LEFT, 1);
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [2], EM_COVER_LEFT, 2);
                            ResetCoverFlowPos (g_pSTCoverFlowCur, EM_COVER_MID, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [0], EM_COVER_RIGHT, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [1], EM_COVER_RIGHT, 1);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [2], EM_COVER_RIGHT, 2);
                        }
                        if (g_pSTCoverFlowCur->dUpMoveY [0] > (100 * g_dScreenHeight))
                        {
                            MoveNode (g_pSTCoverFlowCur, EM_MID_MOVE_FRONT);
                            g_bMoveBack = true;
                            //force_redraw();
                        }
                        else
                        {
                            g_bMoveCur = false;
                            g_bMoveBack = false;
                            g_bMoveCoverFlow = false;
                            //m_wait_mode = true;
                        }
                    }
                }
                else
                {
                    if ((g_pSTCoverFlowCur->dUpMoveY [1] < (140 * g_dScreenHeight)) && (g_bMoveBack == false))
                    {
                        if (g_nLeftNumber > 3)
                        {
                            MoveNode (g_pSTCoverFlowLeftPointer [2]->pPreNode, EM_LEFT_MOVE_EXIT);
                        }
                        MoveNode (g_pSTCoverFlowLeftPointer [0], EM_MID_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowLeftPointer [1], EM_LEFT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowLeftPointer [2], EM_LEFT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowCur, EM_RIGHT_MOVE_MID);
                        MoveNode (g_pSTCoverFlowRightPointer [0], EM_RIGHT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowRightPointer [1], EM_RIGHT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowRightPointer [2], EM_EXIT_MOVE_RIGHT);

                        //force_redraw();
                    }
                    else
                    {
                        if (g_bMoveBack == false)
                        {
                            if (g_nLeftNumber > 3)
                            {
                                ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [2]->pPreNode, EM_COVER_LEFT, 3);
                            }
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [0], EM_COVER_LEFT, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [1], EM_COVER_LEFT, 1);
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [2], EM_COVER_LEFT, 2);
                            ResetCoverFlowPos (g_pSTCoverFlowCur, EM_COVER_MID, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [0], EM_COVER_RIGHT, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [1], EM_COVER_RIGHT, 1);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [2], EM_COVER_RIGHT, 2);
                        }

                        if (g_pSTCoverFlowCur->dUpMoveY [0] > (100 * g_dScreenHeight))
                        {
                            MoveNode (g_pSTCoverFlowCur, EM_MID_MOVE_FRONT);
                            g_bMoveBack = true;
                            //force_redraw();
                        }
                        else
                        {
                            g_bMoveCur = false;
                            g_bMoveBack = false;
                            g_bMoveCoverFlow = false;
                            //m_wait_mode = true;
                        }
                    }
                }
                return 0;

            }


            int CoverFlowFromLeftToRight (void)
            {   
                if (g_pSTCoverFlowLeftPointer [0] != NULL)
                {
                    if  ((g_pSTCoverFlowCur->dUpMoveY [0] < (140 * g_dScreenHeight)) && (g_bMoveCur == false))
                    {
                        g_bMoveCur = false;
                        MoveNode (g_pSTCoverFlowCur, EM_FRONT_MOVE_MID);
                        //force_redraw();
                        return 0;
                    }

                    if (g_bMoveCur == false)
                    {
                        g_bMoveCur = true;  
                        g_nRightNumber ++;

                        if (g_nRightNumber >= 3)
                        {
                            g_pSTCoverFlowRightPointer [3] = g_pSTCoverFlowRightPointer [2];
                            g_pSTCoverFlowRightPointer [2] = g_pSTCoverFlowRightPointer [1];
                            g_pSTCoverFlowRightPointer [1] = g_pSTCoverFlowRightPointer [0];
                            g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowCur;
                        }
                        else if (g_nRightNumber == 2)
                        {
                            g_pSTCoverFlowRightPointer [3] = NULL;                
                            g_pSTCoverFlowRightPointer [2] = NULL;
                            g_pSTCoverFlowRightPointer [1] = g_pSTCoverFlowRightPointer [0];
                            g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowCur;
                        }
                        else if (g_nRightNumber == 1)
                        {
                            g_pSTCoverFlowRightPointer [3] = NULL;
                            g_pSTCoverFlowRightPointer [2] = NULL;
                            g_pSTCoverFlowRightPointer [1] = NULL;
                            g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowCur;
                        }

                        g_pSTCoverFlowCur = g_pSTCoverFlowLeftPointer [0];
                        g_nLeftNumber -- ;
                        if (g_nLeftNumber >= 3)
                        {
                            g_pSTCoverFlowLeftPointer [0] = g_pSTCoverFlowLeftPointer [1];
                            g_pSTCoverFlowLeftPointer [1] = g_pSTCoverFlowLeftPointer [2];
                            g_pSTCoverFlowLeftPointer [2] = g_pSTCoverFlowLeftPointer [2]->pPreNode;
                        }
                        else if (g_nLeftNumber == 2)
                        {
                            g_pSTCoverFlowLeftPointer [0] = g_pSTCoverFlowLeftPointer [1];
                            g_pSTCoverFlowLeftPointer [1] = g_pSTCoverFlowLeftPointer [2];
                            g_pSTCoverFlowLeftPointer [2] = NULL;    
                        }
                        else if (g_nLeftNumber == 1)
                        {
                            g_pSTCoverFlowLeftPointer [0] = g_pSTCoverFlowLeftPointer [1];
                            g_pSTCoverFlowLeftPointer [1] = NULL;
                        }
                        else
                        {
                            g_pSTCoverFlowLeftPointer [0] = NULL;
                        }
                    }


                    if ((g_pSTCoverFlowCur->dUpMoveY [0] < (140 * g_dScreenHeight)) && (g_bMoveBack == false))
                    {
                        if (g_nRightNumber > 3)
                        {
                            MoveNode (g_pSTCoverFlowRightPointer [2]->pNextNode, EM_RIGHT_MOVE_EXIT);
                        }
                        MoveNode (g_pSTCoverFlowRightPointer [0], EM_MID_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowRightPointer [1], EM_RIGHT_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowRightPointer [2], EM_RIGHT_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowCur, EM_LEFT_MOVE_MID);
                        MoveNode (g_pSTCoverFlowLeftPointer [0], EM_LEFT_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowLeftPointer [1], EM_LEFT_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowLeftPointer [2], EM_EXIT_MOVE_LEFT);

                        //force_redraw();
                    }
                    else
                    {
                        if (g_bMoveBack == false)
                        {
                            if (g_nRightNumber > 3)
                            {
                                ResetCoverFlowPos (g_pSTCoverFlowRightPointer [2]->pNextNode, EM_COVER_RIGHT, 3);
                            }
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [0], EM_COVER_LEFT, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [1], EM_COVER_LEFT, 1);
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [2], EM_COVER_LEFT, 2);
                            ResetCoverFlowPos (g_pSTCoverFlowCur, EM_COVER_MID, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [0], EM_COVER_RIGHT, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [1], EM_COVER_RIGHT, 1);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [2], EM_COVER_RIGHT, 2);
                        }
                        if (g_pSTCoverFlowCur->dUpMoveY [1] > (100 * g_dScreenHeight))
                        {
                            MoveNode (g_pSTCoverFlowCur, EM_MID_MOVE_FRONT);
                            g_bMoveBack = true;
                            //force_redraw();
                        }
                        else
                        {
                            g_bMoveCur = false;
                            g_bMoveBack = false;
                            g_bMoveCoverFlow = false;
                            //m_wait_mode = true;
                        }
                    }
                }
                else
                {
                    if ((g_pSTCoverFlowCur->dUpMoveY [0] < (140 * g_dScreenHeight)) && (g_bMoveBack == false))
                    {
                        if (g_nRightNumber > 3)
                        {
                            MoveNode (g_pSTCoverFlowRightPointer [2]->pNextNode, EM_RIGHT_MOVE_EXIT);
                        }
                        MoveNode (g_pSTCoverFlowRightPointer [0], EM_MID_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowRightPointer [1], EM_RIGHT_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowRightPointer [2], EM_RIGHT_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowCur, EM_LEFT_MOVE_MID);
                        MoveNode (g_pSTCoverFlowLeftPointer [0], EM_LEFT_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowLeftPointer [1], EM_LEFT_MOVE_RIGHT);
                        MoveNode (g_pSTCoverFlowLeftPointer [2], EM_EXIT_MOVE_LEFT);

                        //force_redraw();
                    }
                    else
                    {
                        if (g_bMoveBack == false)
                        {
                            if (g_nRightNumber > 3)
                            {
                                ResetCoverFlowPos (g_pSTCoverFlowRightPointer [2]->pNextNode, EM_COVER_RIGHT, 3);
                            }
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [0], EM_COVER_LEFT, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [1], EM_COVER_LEFT, 1);
                            ResetCoverFlowPos (g_pSTCoverFlowLeftPointer [2], EM_COVER_LEFT, 2);
                            ResetCoverFlowPos (g_pSTCoverFlowCur, EM_COVER_MID, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [0], EM_COVER_RIGHT, 0);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [1], EM_COVER_RIGHT, 1);
                            ResetCoverFlowPos (g_pSTCoverFlowRightPointer [2], EM_COVER_RIGHT, 2);
                        }

                        if (g_pSTCoverFlowCur->dUpMoveY [1] > (100 * g_dScreenHeight))
                        {
                            MoveNode (g_pSTCoverFlowCur, EM_MID_MOVE_FRONT);
                            g_bMoveBack = true;
                            //force_redraw();
                        }
                        else
                        {
                            g_bMoveCur = false;
                            g_bMoveBack = false;
                            g_bMoveCoverFlow = false;
                            //m_wait_mode = true;
                        }
                    }
                }
                return 0;

            }

            virtual void on_mouse_button_down(int x, int y, unsigned flags)
            {
#if 0
                if(flags & agg::mouse_left)
                {
                    if(m_poly.on_mouse_button_down(x, y))
                    {
                        force_redraw();
                    }
                }
#endif
#if 0
                while ( g_dMoveZ > 1 )
                {
                    int i;
                    for(i=0;i<1000;i++);
                    g_dMoveZ --;
                    g_dWidth -= 5;
                    g_dHeight -= 5;
                    MoveKeyPoints (g_dWidth, g_dHeight);
                    force_redraw();
                }
#endif
#if 1
                g_bMoveDir = false;
                if (x < m_CenterX && g_pSTCoverFlowCur->pPreNode != NULL)
                {
                    g_bMoveDir = true;
                    //m_wait_mode = false;
                }
                //g_bMoveDir = false;
                //if (g_pSTCoverFlowCur->pNextNode != NULL)
                //	g_bMoveDir = true;
                if (g_pSTCoverFlowCur->pNextNode != NULL)
                {
                    //m_wait_mode = false;
                }
#endif
#if 0
                if (g_pSTCoverFlowRightPointer [0] != NULL)
                {
                    while (g_pSTCoverFlowCur->dUpMoveY [0] < 140)
                    {
                        MoveNode (g_pSTCoverFlowCur, EM_FRONT_MOVE_MID);
                        force_redraw();
                    }
                    g_nLeftNumber ++;

                    if (g_nLeftNumber >= 3)
                    {
                        g_pSTCoverFlowLeftPointer [2] = g_pSTCoverFlowLeftPointer [1];
                        g_pSTCoverFlowLeftPointer [1] = g_pSTCoverFlowLeftPointer [0];
                        g_pSTCoverFlowLeftPointer [0] = g_pSTCoverFlowCur;
                    }
                    else if (g_nLeftNumber == 2)
                    {
                        g_pSTCoverFlowLeftPointer [2] = NULL;
                        g_pSTCoverFlowLeftPointer [1] = g_pSTCoverFlowLeftPointer [0];
                        g_pSTCoverFlowLeftPointer [0] = g_pSTCoverFlowCur;
                    }
                    else if (g_nLeftNumber == 1)
                    {
                        g_pSTCoverFlowLeftPointer [2] = NULL;
                        g_pSTCoverFlowLeftPointer [1] = NULL;
                        g_pSTCoverFlowLeftPointer [0] = g_pSTCoverFlowCur;
                    }

                    g_pSTCoverFlowCur = g_pSTCoverFlowRightPointer [0];
                    g_nRightNumber -- ;
                    if (g_nRightNumber >= 3)
                    {
                        g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowRightPointer [1];
                        g_pSTCoverFlowRightPointer [1] = g_pSTCoverFlowRightPointer [2];
                        g_pSTCoverFlowRightPointer [2] = g_pSTCoverFlowRightPointer [2]->pNextNode;
                    }
                    else if (g_nRightNumber == 2)
                    {
                        g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowRightPointer [1];
                        g_pSTCoverFlowRightPointer [1] = g_pSTCoverFlowRightPointer [2];
                        g_pSTCoverFlowRightPointer [2] = g_pSTCoverFlowRightPointer [2]->pNextNode;    
                    }
                    else if (g_nRightNumber == 1)
                    {
                        g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowRightPointer [1];
                        g_pSTCoverFlowRightPointer [1] = g_pSTCoverFlowRightPointer [2];
                    }
                    else
                    {
                        g_pSTCoverFlowRightPointer [0] = g_pSTCoverFlowRightPointer [1];
                    }


                    while (g_pSTCoverFlowCur->dUpMoveY [1] < 140)
                    {
                        MoveNode (g_pSTCoverFlowLeftPointer [0], EM_MID_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowLeftPointer [1], EM_LEFT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowLeftPointer [2], EM_LEFT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowCur, EM_RIGHT_MOVE_MID);
                        MoveNode (g_pSTCoverFlowRightPointer [0], EM_RIGHT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowRightPointer [1], EM_RIGHT_MOVE_LEFT);
                        MoveNode (g_pSTCoverFlowRightPointer [2], EM_EXIT_MOVE_RIGHT);

                        force_redraw();
                    }

                    while (g_pSTCoverFlowCur->dUpMoveY [0] > 120)
                    {
                        MoveNode (g_pSTCoverFlowCur, EM_MID_MOVE_FRONT);
                        force_redraw();
                    }
                }
#endif
            }


            virtual void on_mouse_move(int x, int y, unsigned flags)
            {
#if 0
                if(flags & agg::mouse_left)
                {
                    if(m_poly.on_mouse_move(x, y))
                    {
                        force_redraw();
                    }
                }
                if((flags & agg::mouse_left) == 0)
                {
                    on_mouse_button_up(x, y, flags);
                }
#endif

            }


            virtual void on_mouse_button_up(int x, int y, unsigned flags)
            {
#if 0
                if(m_poly.on_mouse_button_up(x, y))
                {
                    force_redraw();
                }
#endif
            }

            int MoveNode (ST_COVERFLOW_NODE_PT pNode, EM_MOVE_DIRC emMoveDir)
            {
                if (!pNode)
                    return 1;

                switch (emMoveDir)  
                {
                    case EM_MID_MOVE_RIGHT:
                        {
                            //m_CenterX += 6;
                            pNode->dUpMoveX [0] += 26 * g_dScreenWidth;
                            pNode->dUpMoveY [0] += 2 * g_dScreenHeight;
                            pNode->dUpMoveX [1] += 10 * g_dScreenWidth;
                            pNode->dUpMoveY [1] -= 2 * g_dScreenHeight;
                            pNode->dUpMoveX [2] = pNode->dUpMoveX [1];
                            pNode->dUpMoveY [2] += 2 * g_dScreenHeight;
                            pNode->dUpMoveX [3] = pNode->dUpMoveX [0];
                            pNode->dUpMoveY [3] -= 2 * g_dScreenHeight;

                        }
                        break;
                    case EM_RIGHT_MOVE_MID:
                        {
                            pNode->dUpMoveX [0] -= 26 * g_dScreenWidth;
                            pNode->dUpMoveY [0] -= 2 * g_dScreenHeight;
                            pNode->dUpMoveX [1] -= 10 * g_dScreenWidth;
                            pNode->dUpMoveY [1] += 2 * g_dScreenHeight;
                            pNode->dUpMoveX [2] = pNode->dUpMoveX [1];
                            pNode->dUpMoveY [2] -= 2 * g_dScreenHeight;
                            pNode->dUpMoveX [3] = pNode->dUpMoveX [0];
                            pNode->dUpMoveY [3] += 2 * g_dScreenHeight;
                        }
                        break;
                    case EM_LEFT_MOVE_MID:
                        {
                            pNode->dUpMoveX [0] += 10 * g_dScreenWidth;
                            pNode->dUpMoveY [0] += 2 * g_dScreenHeight;
                            pNode->dUpMoveX [1] += 26 * g_dScreenWidth;
                            pNode->dUpMoveY [1] -= 2 * g_dScreenHeight;
                            pNode->dUpMoveX [2] = pNode->dUpMoveX [1];
                            pNode->dUpMoveY [2] += 2 * g_dScreenHeight;
                            pNode->dUpMoveX [3] = pNode->dUpMoveX [0];
                            pNode->dUpMoveY [3] -= 2 * g_dScreenHeight;
                        }
                        break;
                    case EM_LEFT_MOVE_RIGHT:
                    case EM_RIGHT_MOVE_RIGHT:
                        {
                            pNode->dUpMoveX [0] += 4 * g_dScreenWidth;
                            pNode->dUpMoveX [1] += 4 * g_dScreenWidth;
                            pNode->dUpMoveX [2] += 4 * g_dScreenWidth;
                            pNode->dUpMoveX [3] += 4 * g_dScreenWidth;
                        }
                        break;
                    case EM_EXIT_MOVE_LEFT:
                    case EM_RIGHT_MOVE_EXIT:
                        {
                            pNode->dUpMoveX [0] += 12 * g_dScreenWidth;
                            pNode->dUpMoveX [1] += 12 * g_dScreenWidth;
                            pNode->dUpMoveX [2] += 12 * g_dScreenWidth;
                            pNode->dUpMoveX [3] += 12 * g_dScreenWidth;
                        }
                        break;
                    case EM_MID_MOVE_LEFT:
                        {	
                            pNode->dUpMoveX [0] -= 10 * g_dScreenWidth;
                            pNode->dUpMoveY [0] -= 2 * g_dScreenHeight;
                            pNode->dUpMoveX [1] -= 26 * g_dScreenWidth;
                            pNode->dUpMoveY [1] += 2 * g_dScreenHeight;
                            pNode->dUpMoveX [2] = pNode->dUpMoveX [1];
                            pNode->dUpMoveY [2] -= 2 * g_dScreenHeight;
                            pNode->dUpMoveX [3] = pNode->dUpMoveX [0];
                            pNode->dUpMoveY [3] += 2 * g_dScreenHeight;
                        }
                        break;
                    case EM_RIGHT_MOVE_LEFT:
                    case EM_LEFT_MOVE_LEFT:
                        {
                            pNode->dUpMoveX [0] -= 4 * g_dScreenWidth;
                            pNode->dUpMoveX [1] -= 4 * g_dScreenWidth;
                            pNode->dUpMoveX [2] -= 4 * g_dScreenWidth;
                            pNode->dUpMoveX [3] -= 4 * g_dScreenWidth;
                        }
                        break;
                    case EM_LEFT_MOVE_EXIT:
                    case EM_EXIT_MOVE_RIGHT:
                        {
                            pNode->dUpMoveX [0] -= 12 * g_dScreenWidth;
                            pNode->dUpMoveX [1] -= 12 * g_dScreenWidth;
                            pNode->dUpMoveX [2] -= 12 * g_dScreenWidth;
                            pNode->dUpMoveX [3] -= 12 * g_dScreenWidth;
                        }
                        break;
                    case EM_FRONT_MOVE_MID:
                        {

                            pNode->dUpMoveX [0] += 4 * g_dScreenWidth;
                            pNode->dUpMoveY [0] += 4 * g_dScreenHeight;

                            pNode->dUpMoveX [1] -= 4 * g_dScreenWidth;
                            pNode->dUpMoveY [1] = pNode->dUpMoveY [0];

                            pNode->dUpMoveX [2] = pNode->dUpMoveX [1];
                            pNode->dUpMoveY [2] -= 4 * g_dScreenHeight;

                            pNode->dUpMoveX [3] = pNode->dUpMoveX [0];
                            pNode->dUpMoveY [3] = pNode->dUpMoveY [2];

                        }
                        break;
                    case EM_MID_MOVE_FRONT:
                        {

                            pNode->dUpMoveX [0] -= 4 * g_dScreenWidth;
                            pNode->dUpMoveY [0] -= 4 * g_dScreenHeight;

                            pNode->dUpMoveX [1] += 4 * g_dScreenWidth;
                            pNode->dUpMoveY [1] = pNode->dUpMoveY [0];

                            pNode->dUpMoveX [2] = pNode->dUpMoveX [1];
                            pNode->dUpMoveY [2] += 4 * g_dScreenHeight;

                            pNode->dUpMoveX [3] = pNode->dUpMoveX [0];
                            pNode->dUpMoveY [3] = pNode->dUpMoveY [2];

                        }
                        break;
                    default:
                        break;
                }

                pNode->dDownMoveX [0] = pNode->dUpMoveX [0];
                pNode->dDownMoveY [0] = pNode->dUpMoveY [0];
                pNode->dDownMoveX [1] = pNode->dUpMoveX [1];
                pNode->dDownMoveY [1] = pNode->dUpMoveY [1];
                pNode->dDownMoveX [2] = pNode->dDownMoveX [1];
                pNode->dDownMoveY [2] = pNode->dUpMoveY [1] - (pNode->dUpMoveY [2] - pNode->dUpMoveY [1]);
                pNode->dDownMoveX [3] = pNode->dDownMoveX [0];
                pNode->dDownMoveY [3] = pNode->dUpMoveY [0] - (pNode->dUpMoveY [3] - pNode->dUpMoveY [0]);	

                return 0;

            }

            virtual void on_idle()
            {

                if (g_bMoveDir)
                    CoverFlowFromLeftToRight ();
                else
                    CoverFlowFromRightToLeft ();

                force_redraw();
#if 0
                if ( g_dMoveZ >= 1 )
                {
                    g_dMoveZ --;
                    //g_dWidth -= 2;
                    //g_dHeight -= 2;
                    //MoveKeyPoints (g_dWidth, g_dHeight);
                    MoveNode (g_pSTCoverFlowHead, EM_FRONT_MOVE_MID);

                    force_redraw();
                }
                else
                {
                    if (g_dMoveX >= 1)
                    {
                        if (g_bMoveDir)
                            MoveNode (g_pSTCoverFlowHead, EM_MID_MOVE_LEFT);
                        else
                            MoveNode (g_pSTCoverFlowHead, EM_MID_MOVE_RIGHT);
                        g_dMoveX --;
                        force_redraw();

                    }
                    else
                    {
#if 1
                        if (g_dShiftRightMidX >= 1)
                        {
                            if (g_bMoveDir)
                                MoveNode (g_pSTCoverFlowHead, EM_LEFT_MOVE_LEFT);	 	
                            else
                                MoveNode (g_pSTCoverFlowHead, EM_RIGHT_MOVE_RIGHT);
                            g_dShiftRightMidX --;
                            force_redraw();
                        }
                        else
#endif
                            m_wait_mode = true;
                    }
                }
#endif


            }


        };


CoverFlowClass g_CoverFlow (pix_format, flip_y);

#ifdef __cplusplus
extern "C" {
#endif

int ReturnCurCoverFlowIndex (void)
{
    if (!g_pSTCoverFlowCur)
        return -1;

    return g_pSTCoverFlowCur->nImgIndex;

}

BOOL CoverFlowMoveFromLeftToRightSingle (HDC hDC)
{
    HDC hdc;

    if (g_pSTCoverFlowLeftPointer [0] != NULL)
        g_bMoveCoverFlow = TRUE;

    while (g_bMoveCoverFlow)
    {
        g_CoverFlow.CoverFlowFromLeftToRight ();
        g_CoverFlow.DrawCoverFlow ();
        hdc = g_CoverFlow.rbuf_dc ();
        BitBlt (hdc, 0, 0, g_CoverFlow.GetScreenWidth (), g_CoverFlow.GetScreenHeight (), hDC, 0, 0, 0);
    }
    return TRUE;

}

BOOL CoverFlowMoveFromRightToLeftSingle (HDC hDC)
{
    HDC hdc;

    if (g_pSTCoverFlowRightPointer [0] != NULL)
        g_bMoveCoverFlow = TRUE;

    while (g_bMoveCoverFlow)
    {
        g_CoverFlow.CoverFlowFromRightToLeft ();
        g_CoverFlow.DrawCoverFlow ();
        hdc = g_CoverFlow.rbuf_dc ();
        BitBlt (hdc, 0, 0, g_CoverFlow.GetScreenWidth (), g_CoverFlow.GetScreenHeight (), hDC, 0, 0, 0);
    }
    return TRUE;

}

BOOL ShowCoverFlow (HDC hDC)
{
    HDC hdc;
    g_CoverFlow.DrawCoverFlow ();
    hdc = g_CoverFlow.rbuf_dc ();
    BitBlt (hdc, 0, 0, g_CoverFlow.GetScreenWidth (), g_CoverFlow.GetScreenHeight (), hDC, 0, 0, 0);
    return TRUE;
}

BOOL RegisterCoverFlowRes (char* pCoverFlowImg)
{
    g_CoverFlow.load_img (g_nCoverFlowImg, pCoverFlowImg);
    g_nCoverFlowImg ++;
    return TRUE;
}

BOOL StartUpCoverFlow (int nWidth, int nHeight)
{
    //pString = (*pBitmapFile);

    if (!nWidth || !nHeight)
        return FALSE;

    g_dScreenHeight = (double) nHeight;
    g_dScreenWidth = (double) nWidth;

    g_dScreenHeight = g_dScreenHeight / 480;
    g_dScreenWidth = g_dScreenWidth / 640;


    //g_CoverFlow.InitInnerData (/*g_nCoverFlowImg*/6, g_CoverFlow.m_Width, g_CoverFlow.m_Height);
    if (!g_CoverFlow.init(nWidth, nHeight, agg::window_resize))
    {
        fprintf (stderr, "CoverFlow Error: Init error !\n");
        return FALSE;
    }
    //g_CoverFlow.InitInnerData (7, g_CoverFlow.m_Width, g_CoverFlow.m_Height);
    return TRUE;
}

BOOL InitCoverFlowByRegisterRes (void)
{
    if (!g_nCoverFlowImg)
    {
        fprintf (stderr, "CoverFlow Error: Register Resource Please!\n");
    }
    g_CoverFlow.InitInnerData (g_nCoverFlowImg, g_CoverFlow.m_Width, g_CoverFlow.m_Height);
    g_CoverFlow.on_init ();
    return TRUE;
}

BOOL InitCoverFlow (char** pBitmapFile, int nCount, int nWidth, int nHeight)
{
    int i;
    //pString = (*pBitmapFile);

    if (!nWidth || !nHeight)
        return FALSE;

    g_dScreenHeight = (double) nHeight;
    g_dScreenWidth = (double) nWidth;

    g_dScreenHeight = g_dScreenHeight / 480;
    g_dScreenWidth = g_dScreenWidth / 640;

    g_CoverFlow.InitInnerData (nCount, nWidth, nHeight);

    if (!g_CoverFlow.init(nWidth, nHeight, agg::window_resize))
    {
        fprintf (stderr, "CoverFlow Error: Init error !\n");
        return FALSE;
    }

    for (i = 0; i < nCount; i++)
    {
        g_CoverFlow.load_img (i, *(pBitmapFile + i));
    }

    return TRUE;
}

#ifdef __cplusplus
}
#endif
