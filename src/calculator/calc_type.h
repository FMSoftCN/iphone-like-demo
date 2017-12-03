/*
** $Id: calc_type.h 224 2007-07-03 09:38:24Z xwyan $
**
** calc_type.h: type and Macro
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Wang Xuguang.
**
** Create date: 2006/11/17
*/



//FIXME
#if !defined (__uClinux__) && !defined(__ECOS__)
#ifndef WIN32
#define _ASINH_SUPPORT
#endif
#endif

typedef	double	FLOAT;

typedef struct {
	int *base;  
	int *top;
	int *mid;
	int eNum;
	int stacksize;
} DIGIT_STACK;

typedef	struct {
	FLOAT *base;
	FLOAT *top;
	int stacksize;
} OPND_STACK;

typedef	struct {
	int *base;
	int *top;
	int stacksize;
} OPTR_STACK;

typedef enum   _Num_Base	{ 
	BASE_BIN = 2, BASE_OCT = 8, BASE_DEC = 10, BASE_HEX = 16 
} Num_Base;

typedef enum   _Angle_Type	{ 
	ANG_DEG = 0, ANG_RAD = 1, ANG_GRA = 2
} Angle_Type;


#define FABS(X)		    fabs(X)
#define MODF(X,Y)    	modf(X,Y)
#define FMOD(X,Y)   	fmod(X,Y)
#define SIN(X)		    sin(X)
#define ASIN(X)		    asin(X)
#define SINH(X)		    sinh(X)

#ifdef _ASINH_SUPPORT
#define ASINH(X)	    asinh(X)
#define ACOSH(X)	    acosh(X)
#define ATANH(X)	    atanh(X)
#define ISINF(X)	    isinf(X)
#else
#define ASINH(X)	    (X)
#define ACOSH(X)	    (X)
#define ATANH(X)	    (X)
#define ISINF(X)	    (X)
#endif

#define COS(X)		    cos(X)
#define COSH(X)		    cosh(X)
#define ACOS(X)		    acos(X)
#define TAN(X)		    tan(X)
#define TANH(X)		    tanh(X)
#define ATAN(X)		    atan(X)
#define EXP(X)		    exp(X)
#define POW(X,Y)	    pow(X,Y)
#define LN(X)		    log(X)
#define LOG(X)		    log10(X)
#define SQRT(X)		    sqrt(X)

#define DEG2RAD(x)	    (((2L*pi)/360L)*x)
#define GRA2RAD(x)	    ((pi/200L)*x)
#define RAD2DEG(x)	    ((360L/(2L*pi))*x)
#define RAD2GRA(x)	    ((200L/pi)*x)
#define POS_ZERO	     1e-16L	 /* What we consider zero is   */
#define NEG_ZERO	    -1e-16L	 /* anything between these two */

