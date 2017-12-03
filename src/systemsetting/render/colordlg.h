/*
 * colordlg.h
 *
 * wangjian<wangjian@minigui.org>
 * 
 * 2008-8-15.
 */

#define CIRCLE_C_X  84              //*the center of the cirque */
#define CIRCLE_C_Y  84
#define CIRCLE_C_RW 84              //*the inside radius of the cirque*/
#define CIRCLE_C_RN (82 - 11)       //*the outside radius of the cirque*/
 
#define TRI_PT1     {84 , 14 }
#define TRI_PT2     {23 , 120}      //*the 3 piont for the triangle*/
#define TRI_PT3     {145, 120}

#define CL_PAL_LEN  256             //* the pallate length for mask bitmap*/
#define CL_MASK_R   0x0    
#define CL_MASK_G   0x0            //* the transparent pixel for mask bitmap*/
#define CL_MASK_B   0x0

#define H_POS_X     150             //* the point to choose the h value */
#define H_POS_Y     123


typedef struct _COLORRET {
    Uint8  r, g, b;
    Uint16 h;
    Uint8  s, v;
} COLORRET, *PCOLORRET;



/*
 * the context data structure used by CircleGenerator
 * when we draw the color cirque.
 *
 * hdc      hdc to draw on.
 * cx,cy    the center point of the cirque.
 * arc      the hue offset of the color 
 *          of the point (H_POS_X, H_POS_Y).
 */

typedef struct _CIRQUE_CONTEXT
{
    HDC hdc;
    int cx;
    int cy;
    int arc;
} CCON, *PCCON;


