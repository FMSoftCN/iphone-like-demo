/*
** $Id: calc_size.h 224 2007-07-03 09:38:24Z xwyan $
**
** calc_size.h: define the buttons layout of calc
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Wang Xuguang.
**
** Create date: 2006/11/17
*/

#ifndef CALC_SIZE_H
#define CALC_SIZE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "config.h"

#define CALCWINDOW_LX        0
#define CALCWINDOW_TY        0
#define CALCWINDOW_RX        IPHONE_MAIN_WIDTH
#define CALCWINDOW_BY        IPHONE_MAIN_HEIGHT
#define CALCWINDOW_WIDTH     (CALCWINDOW_RX - CALCWINDOW_LX)
#define CALCWINDOW_HEIGHT    (CALCWINDOW_BY - CALCWINDOW_TY)

//#define CALC_BMP_BN         "res/calculator_num.png"
#define CALC_BMP_BN         "res/calc_num.png"

#ifdef  ENABLE_LANDSCAPE
    
#define CALC_BMP_BG         "res/calculator_bg.png"
#define CALC_BMP_BC         "res/calculator_click.png"
//the size of bottons
#define CALC_B_W            80
#define CALC_B_H            40
#define CALC_B_BORDERT      7
#define CALC_B_BORDERB      4

//static box displaying result
#define CALC_DISPLAY_T      10
#define CALC_DISPLAY_R      313
#define CALC_DISPLAY_L      65
#define CALC_DISPLAY_B      33

#define CALC_BEGIN_X        0
#define CALC_BEGIN_Y        39

#else
/*The following is for portrait.*/
//#define CALC_BMP_BG         "res/calculator_bg_pt.png"
//#define CALC_BMP_BC         "res/calculator_click_pt.png"
#define CALC_BMP_BG         "res/calc_bg_pt.png"
#define CALC_BMP_BC         "res/calc_ck_pt.png"
//the size of bottons
#define CALC_B_W            60
#define CALC_B_H            46//53
#define CALC_B_BORDERT      8//8//9
#define CALC_B_BORDERB      6//7

//static box displaying result
#define CALC_DISPLAY_T      31//15
#define CALC_DISPLAY_R      234
#define CALC_DISPLAY_L      10//25
#define CALC_DISPLAY_B      51//44

#define CALC_BEGIN_X        0
#define CALC_BEGIN_Y        90//52

#endif

/*the following is no use for this version*/
//the distance between bottons 
#define BD                  5	//border distance 
#define HD                  14//5 //H-distance
#define VD                  14//6

#define B_W_SC              42
#define B_H_SC              20

#define HD_SC               5
#define VD_SC               4  //V-distance

#define BEGIN_X_SC          (BD)
#define BEGIN_Y_SC          (CALC_DISPLAY_B + 5)

#define ARROW_LEFT          (BD)
#define ARROW_TOP           VD
#define ARROW_RIGHT         (ARROW_LEFT + 12)
#define ARROW_BOTTOM        (ARROW_TOP + 10)

//static box display "M"     
#define SM_W                20//13
#define SM_H                20//12
#define SM_X                263//(B_W + BD + 3)        
#define SM_Y                15//(VD)                

#define CHK_INV_W		    50	 
#define CHK_INV_H           17

#define CHK_INV_X           (CHK_HYP_X - HD - CHK_INV_W)
#define CHK_INV_Y           BEGIN_Y_SC

#define CHK_HYP_W           50
#define CHK_HYP_H           17
#define CHK_HYP_X           (CALCWINDOW_WIDTH - BD - CHK_HYP_W)
#define CHK_HYP_Y           BEGIN_Y_SC

#define BTN_ANG_W           (B_W_SC + 3)
#define BTN_ANG_H           B_H_SC
#define BTN_ANG_X           (CALCWINDOW_WIDTH - BD - 3 - BTN_ANG_W)
#define BTN_ANG_Y           (BEGIN_Y_SC + B_H_SC + VD)

#define BTN_BASE_W          BTN_ANG_W
#define BTN_BASE_H          B_H_SC
#define BTN_BASE_X          (BTN_ANG_X - BTN_BASE_W - HD + 1)
#define BTN_BASE_Y          BTN_ANG_Y


#define BTN_UNIT_W          86
#define BTN_UNIT_H          21
#define BTN_UNIT_X          (CALCWINDOW_WIDTH - BD - BTN_UNIT_W - BD - 1)
#define BTN_UNIT_Y          BEGIN_Y

#ifdef  __cplusplus
}
#endif

#endif /* CALC_SIZE_H */

