/*
** $Id: calc_btn_code.h 224 2007-07-03 09:38:24Z xwyan $
**
** btn_code.h: define the ID of button on the calc.
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Wang Xuguang.
**
** Create date: 2006/11/17
*/


#ifndef BTN_CODE_H
#define BTN_CODE_H

//compact calculator
#define CODE_1          1
#define CODE_2          2
#define CODE_3          3
#define CODE_4          4
#define CODE_5          5
#define CODE_6          6
#define CODE_7          7
#define CODE_8          8
#define CODE_9          9
#define CODE_0          10
#define CODE_POINT      11

#define CODE_EQ			12
#define CODE_ADD        13
#define CODE_SUBTRACT   14
#define CODE_MULTIPLY   15
#define CODE_DIVIDE     16
#define CODE_SQRT       17
#define CODE_PERCENT    18	//percent    
#define CODE_RECIP		19	//reciprocal
#define CODE_PLUSMINUS  20

#define CODE_MC			21
#define CODE_MR			22
#define CODE_MS			23
#define CODE_MPLUS		24

#define CODE_BS         25
#define CODE_CE         26
#define CODE_AC         27


// scientific calculator
#define CODE_MOD        28
#define CODE_SQ         29
#define CODE_FAC        30	//factorical
#define CODE_XY         31

#define CODE_SIN        32
#define CODE_COS        33
#define CODE_TAN        34

#define CODE_LOG        35
#define CODE_LN         36

#define CODE_A          37
#define CODE_B          38
#define CODE_C          39
#define CODE_D          40
#define CODE_E          41
#define CODE_F          42

#define CODE_INV        45
#define CODE_HYP        46

#define CODE_LEFTPAREN	47	//left parenthese
#define CODE_RIGHTPAREN 48	//right parenthese

#define CODE_AND        49
#define CODE_OR         50
#define CODE_NOT        51
#define CODE_EXP        52
#define CODE_XOR        53
#define CODE_LSH        54
#define CODE_RSH        55

#define CODE_MMINUS     56



#define DIGIT_POINT     20
#define HEX_STACK_SIZE  8
#define OCT_STACK_SIZE  10
#define BIN_STACK_SIZE  16


#define MAX_E				100
#define EFF_NUM				9
#define DIGIT_STACK_SIZE    (EFF_NUM+1)
#define OPTR_STACK_SIZE     10
#define OPND_STACK_SIZE     10
#define STACK_INCREMENT     5

#define LOWPRIO		    1
#define EQUALPRIO       2
#define HIGHPRIO        3

#endif
