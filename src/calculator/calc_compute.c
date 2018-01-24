/*
** $Id: calc_compute.c 224 2007-07-03 09:38:24Z xwyan $
**
** calc_compute.c: Implement the  algorithm of calc.
**
** Copyright (C) 2006 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Wang Xuguang.
**
** Create date: 2006/11/17
*/


#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "calc_btn_code.h"
#include "calc_type.h"
#include "calc_size.h"

#define CALC_COMPACT    1
#define CALC_SCIEN      2

FLOAT       last_opnd = 0;
FLOAT       dispnow = 0;
int         display_error = 0;
DIGIT_STACK digit_stack;

static FLOAT        pi;
static OPND_STACK   opnd_stack;
static OPTR_STACK   optr_stack;
static int          M_buffer = 0;

extern Num_Base     calc_cur_base;
extern Angle_Type   calc_cur_angle;
extern BOOL         calc_hyp_mode;
extern BOOL         calc_inv_mode;
extern int          calc_pre_btnid;
extern BOOL         calc_percent_mod;
extern int          calc_last_optr;
extern char         calc_strdisp[50];
extern int          calc_Status;
extern RECT         calc_rcArrow;

extern int          DecideSize (void);
extern void         show_var (HWND hwnd);        
extern void         show_digit_stack(HWND hwnd);

static inline void calc_InvalidDiaplayRect(HWND hwnd) 
{
    RECT rc;
    
    rc.left     = CALC_DISPLAY_L;
    rc.right    = CALC_DISPLAY_R;
    rc.top      = CALC_DISPLAY_T;
    rc.bottom   = CALC_DISPLAY_B;

    InvalidateRect(hwnd, &rc, TRUE);
}


static BOOL IsBinary (int btid)
{
    switch (btid) {
        case CODE_DIVIDE:
        case CODE_MULTIPLY:
        case CODE_ADD:
        case CODE_SUBTRACT:
        case CODE_XY:
        case CODE_MOD:
        case CODE_LEFTPAREN:
        case CODE_RIGHTPAREN:
        case CODE_AND:
        case CODE_OR:
        case CODE_XOR:
        case CODE_LSH:
        case CODE_RSH:
            return TRUE;
        break;
    }
    return FALSE;
}
static BOOL IsUnary (int btid)
{
    switch (btid) {
        case CODE_SQRT:
        case CODE_RECIP:
        case CODE_PLUSMINUS:
        case CODE_SQ:
        case CODE_FAC:
        case CODE_SIN:
        case CODE_COS:
        case CODE_TAN:
        case CODE_LOG:
        case CODE_LN:
        case CODE_EXP:
        case CODE_NOT:
            return TRUE;
        break;
    }
    return FALSE;
}


static BOOL IsDigit (int btid)
{
    switch (btid) {
        case CODE_0:
        case CODE_1:
        case CODE_2:
        case CODE_3:
        case CODE_4:
        case CODE_5:
        case CODE_6:
        case CODE_7:
        case CODE_8:
        case CODE_9:
        case CODE_POINT:
        case CODE_A:
        case CODE_B:
        case CODE_C:
        case CODE_D:
        case CODE_E:
        case CODE_F:
            return TRUE;
        break;
    }
    return FALSE;
}

static int get_priority (int btnid)
{
    switch (btnid) {
    case CODE_ADD:
    case CODE_SUBTRACT:
        return 1;
        break;
    case CODE_MULTIPLY:
    case CODE_DIVIDE:
        return 2;
        break;
    case CODE_MOD:
        return 3;
        break;
    case CODE_XY:
        return 4;
        break;
    }
    return 0;
}

static int cmp_priority (int btnid_left, int btnid_right)
{
    int result = -1;

    if (btnid_right == CODE_EQ || btnid_right == CODE_PERCENT) 
        return HIGHPRIO;

    if (btnid_left == CODE_LEFTPAREN) {
        if (btnid_right == CODE_RIGHTPAREN)
            result = EQUALPRIO;
        else
            result = LOWPRIO;
    }
    else if (btnid_left == CODE_RIGHTPAREN) {
        if (btnid_right == CODE_LEFTPAREN)
            result = -1;
        else
            result = HIGHPRIO;
    }
    else if (IsBinary (btnid_left)) {
        if (btnid_right == CODE_LEFTPAREN)
            result = LOWPRIO;
        else if (btnid_right == CODE_RIGHTPAREN)
            result = HIGHPRIO;
        else if (IsBinary (btnid_right)) {
            if (get_priority (btnid_left) < get_priority (btnid_right))
                result = LOWPRIO;
            else
                result = HIGHPRIO;
        }
    }
    return result;
}

// operations on stack
//---------------------------------------------------------------------------

static int init_digit_stack (void) 
{
    digit_stack.base = (int *) malloc (DIGIT_STACK_SIZE * sizeof(int));
    if (!digit_stack.base) return -1;
    digit_stack.mid = digit_stack.top = digit_stack.base;
    digit_stack.eNum = 0;
    digit_stack.stacksize = DIGIT_STACK_SIZE;
    return 0;
}

static int init_optr_stack (void)
{
    optr_stack.base = (int *) malloc (OPTR_STACK_SIZE * sizeof (int));
    if (!optr_stack.base) return -1;
    optr_stack.top = optr_stack.base;
    optr_stack.stacksize = OPTR_STACK_SIZE;
    return 0;
}

static int init_opnd_stack (void)
{
    opnd_stack.base = (FLOAT *) malloc (OPND_STACK_SIZE * sizeof (FLOAT));
    if (!opnd_stack.base) return -1;
    opnd_stack.top = opnd_stack.base;
    opnd_stack.stacksize = OPND_STACK_SIZE;
    return 0;
}

void FreeStack (void) 
{
    free (digit_stack.base);
    free (optr_stack.base);
    free (opnd_stack.base);
}

static int digit_stack_len (void)
{
    return (digit_stack.top - digit_stack.base);
}

BOOL digit_stack_empty (void) 
{
    if (digit_stack.top == digit_stack.base) 
        return TRUE;
    else 
        return FALSE;
}

static BOOL digit_stack_full (void)
{
    if (digit_stack.top - digit_stack.base + 1 == digit_stack.stacksize)
        return TRUE;
    else
        return FALSE;
}

static BOOL point_in_stack (void)
{
    if (!digit_stack_empty () && *(digit_stack.mid) == DIGIT_POINT)
        return TRUE;
    else
        return FALSE;
}

static void push_digit (int newdigit) 
{
    if ((newdigit <0 || newdigit > 15) && (newdigit != DIGIT_POINT)) 
        return;

    // stack is full -- not include dec base
    if (calc_cur_base == BASE_BIN) {
        if (digit_stack_len () >= BIN_STACK_SIZE)
            return;
    }
    else if (calc_cur_base == BASE_OCT) {
        if (digit_stack_len () >= OCT_STACK_SIZE)
            return;
    }
    else if (calc_cur_base == BASE_HEX) {
        if (digit_stack_len () >= HEX_STACK_SIZE)
            return;
    }

    if (calc_cur_base != BASE_DEC) {
        if((digit_stack.top == digit_stack.base + 1) 
                && *digit_stack.base == 0 && newdigit != DIGIT_POINT) {
        *digit_stack.base = newdigit;        
        }
        else {
        *digit_stack.top = newdigit;
        digit_stack.top++;
        }
        return;
    }

    /* now is dec base */
    // stack is full -- dec base
    if(digit_stack_full ()) {
        if (!point_in_stack ()) {
            if (digit_stack.eNum < MAX_E) digit_stack.eNum++;
        }
    }        
    else if((digit_stack.top == digit_stack.base + 1) 
                && *digit_stack.base == 0 && newdigit != DIGIT_POINT) {
        *digit_stack.base = newdigit;        
    }
    else if(!point_in_stack ()) {        // no point yet
        *digit_stack.top = newdigit;
        digit_stack.top++;
        if(newdigit == DIGIT_POINT) digit_stack.mid = digit_stack.top - 1;
    }else if(newdigit != DIGIT_POINT) {
        *digit_stack.top = newdigit;
        digit_stack.top++;
    }
}

static int push_optr (int btnid)
{
    // stack is full, append memory
    if (optr_stack.top - optr_stack.base >= optr_stack.stacksize - 1) {
        optr_stack.base = (int *) realloc (optr_stack.base, 
               (OPTR_STACK_SIZE + STACK_INCREMENT) * sizeof (int));
        if (!optr_stack.base) return -1;
        optr_stack.stacksize += STACK_INCREMENT;
    }
    *optr_stack.top = btnid;
    optr_stack.top++;
    return 0;
}

static int push_opnd (FLOAT opnd)
{
    // stack is full, append memory
    if (opnd_stack.top - opnd_stack.base >= opnd_stack.stacksize - 1) {
        opnd_stack.base = (FLOAT *) realloc (opnd_stack.base, 
               (OPND_STACK_SIZE + STACK_INCREMENT) * sizeof (FLOAT));
        if (!opnd_stack.base) return -1;
        opnd_stack.stacksize += STACK_INCREMENT;
    }
    *opnd_stack.top = opnd;
    opnd_stack.top++;
    return 0;
}

static int pop_digit (void)
{
    if(digit_stack_empty ()) return -1;
    if (digit_stack.eNum != 0) {
        digit_stack.eNum--;
    }
    else {
        if(*(digit_stack.top - 1) == DIGIT_POINT) 
            digit_stack.mid = digit_stack.base;
        digit_stack.top--;
    }
    return 0;
}

static int pop_optr (int *ptr_optr)
{
    if (optr_stack.top == optr_stack.base) return -1;
    optr_stack.top--;
    if (ptr_optr) *ptr_optr = *optr_stack.top;
    return 0;
}

static int pop_opnd (FLOAT *ptr_opnd)
{
    if (opnd_stack.top == opnd_stack.base) return -1;
    opnd_stack.top--;
    if (ptr_opnd) *ptr_opnd = *opnd_stack.top;
    return 0;
}

static int GetOptrTop (void)
{
    if (optr_stack.top == optr_stack.base) 
        return -1;
    else
        return *(optr_stack.top - 1);
}

// transfer digits in digit_stack to the var -- last_opnd
static void stack_var(void)        
{
    int *pint, *pdigit;
    FLOAT stack_opnd = 0, stack_opnd2 = 0;
    
    if (digit_stack_empty ()) {
        last_opnd = 0;
        return;
    }

    if (point_in_stack ()) 
        pint = digit_stack.mid - 1;
    else
        pint = digit_stack.top - 1;

    for (pdigit = digit_stack.base; pdigit <= pint; pdigit++) {
        stack_opnd = stack_opnd * calc_cur_base + *pdigit;
        //stack_int_opnd = (unsigned long)stack_opnd;
    }

    if (point_in_stack ()) {
        for (pdigit = digit_stack.top - 1; pdigit > digit_stack.mid; pdigit--) {
            stack_opnd2 = (stack_opnd2 + *pdigit) / calc_cur_base;
        }
    }
    
    stack_opnd += stack_opnd2;
    if (digit_stack.eNum != 0)
        stack_opnd *= POW ((FLOAT) calc_cur_base, (FLOAT)digit_stack.eNum);

    last_opnd = stack_opnd;    

    // base transfer
    if (calc_cur_base != BASE_DEC) {
        FLOAT i_opnd;

        //if (point_in_stack())
        MODF (last_opnd, &i_opnd);
        //else
        //    i_opnd = stack_int_opnd;
        if (i_opnd < LONG_MIN || i_opnd > ULONG_MAX) {
            fprintf (stderr, "%f outside the ulong scope\n", i_opnd);
            display_error = 1;
        }
        else if (i_opnd > LONG_MAX) 
            last_opnd = (long) (i_opnd + LONG_MIN - LONG_MAX - 1);
        else 
            last_opnd = (long) i_opnd;
    }
}

void clear_digit_stack (void)    
{
    digit_stack.mid = digit_stack.top = digit_stack.base;
    digit_stack.eNum = 0;
}

static void clear_opnd_stack (void)
{
    opnd_stack.top = opnd_stack.base;
}

static void clear_optr_stack (void)
{
    optr_stack.top = optr_stack.base;
}

static void ClearAll (void)
{
    clear_digit_stack ();
    clear_optr_stack ();
    clear_opnd_stack ();
    last_opnd = 0;
    dispnow = 0;
    display_error = 0;
}

// -----------------------------------------------------------
// clear the unneccessary zero in the string to be shown
void clearzero(char *f_string)    
{
    char *pe, *pzero;

    pe = f_string;
    while (*pe != 'E' && *pe != '\0') {
        pe++;    
    }    
    pzero = pe;

    do {
        pzero--;
    } while(*pzero == '0');
    if(*pzero != '.') pzero++;

    strcpy (pzero, pe);
}



int longint_bin (char *out_str, long amount, int max_digits)
{
    /*
     * A routine that converts a long int to
     * binary display format
     */

    char work_str[(sizeof(amount) * 8) + 1];
    int  work_char = 0, lead_zero = 1, lead_one = 1, lead_one_count = 0,
         work_size = sizeof(amount) * 8;
    unsigned long bit_mask = (1 << ((4 * 8) - 1));

    while (bit_mask) {

        if (amount & bit_mask) {
            if (lead_one)
                lead_one_count++;
            lead_zero = 0;
            work_str[work_char++] = '1';
        } else {
            lead_one = 0;
            if (!lead_zero)
                work_str[work_char++] = '0';
        }
                bit_mask >>= 1;
                bit_mask &= 0x7fffffff; //Sven: Uwe's Alpha adition
    }
    if (!work_char)
        work_str[work_char++] = '0';
    work_str[work_char] = '\0';

    if (work_char-lead_one_count < max_digits)
        return strlen(strcpy(out_str,
                      &work_str[lead_one_count ?
                      work_size - max_digits :
                      0]));
    else
       return -1;
}        


// computations of the math functions and operators
//---------------------------------------------------------------------------

static void other_rad (void)
{
      switch (calc_cur_angle) {
      case ANG_DEG:
        last_opnd = DEG2RAD(last_opnd);
        break;
      case ANG_GRA:
        last_opnd = GRA2RAD(last_opnd);
        break;
      case ANG_RAD:
        break;
      }
}

static void rad_other (void)
{
      switch (calc_cur_angle) {
      case ANG_DEG:
        last_opnd = RAD2DEG(last_opnd);
        break;
      case ANG_GRA:
        last_opnd = RAD2GRA(last_opnd);
        break;
      case ANG_RAD:
        break;
      }
}

static void ComputeSin(void)
{
  if (calc_hyp_mode){
    // sinh or arcsinh
    if (!calc_inv_mode){
      last_opnd = SINH(last_opnd);
    }
    else {
      last_opnd = ASINH(last_opnd);
      if (errno == EDOM || errno == ERANGE)
      display_error = 1;
    }
  }
  else {
    // sine or arcsine
    if (!calc_inv_mode){
      // sine
      other_rad ();    
      last_opnd = SIN(last_opnd);
      if (errno == EDOM || errno == ERANGE)
          display_error = 1;
    }
    else {
      // arcsine
      last_opnd = ASIN(last_opnd);
      rad_other ();
      if (errno == EDOM || errno == ERANGE)
        display_error = 1;
    }
  }

// Now a cheat to help the weird case of COS 90 degrees not being 0!!!

  if (last_opnd < POS_ZERO && last_opnd > NEG_ZERO)
    last_opnd=0;

}

static void ComputeCos(void)
{
  if (calc_hyp_mode){
    // cosh or arccosh
    if (!calc_inv_mode){
      last_opnd = COSH(last_opnd);
    }
    else {
      last_opnd = ACOSH(last_opnd);
      if (errno == EDOM || errno == ERANGE)
      display_error = 1;
    }
  }
  else {
    // cosine or arccosine
    if (!calc_inv_mode){
      // cosine
      other_rad ();    
      last_opnd = COS(last_opnd);
      if (errno == EDOM || errno == ERANGE)
          display_error = 1;
    }
    else {
      // arccosine
      last_opnd = ACOS(last_opnd);
      rad_other ();    
      
      if (errno == EDOM || errno == ERANGE)
          display_error = 1;
    }
  }

  if (last_opnd < POS_ZERO && last_opnd > NEG_ZERO)
    last_opnd=0;
}

static void ComputeTan(void)
{
  if (calc_hyp_mode){
    // tanh or arctanh
    if (!calc_inv_mode){
      last_opnd = TANH(last_opnd);
    }
    else {
      last_opnd = ATANH(last_opnd);
      if (errno == EDOM || errno == ERANGE)
          display_error = 1;
    }
  }
  else {
    // tan or arctan
    if (!calc_inv_mode){
      /* tan, FIXME, tan(90) != error */
      if (calc_cur_angle == ANG_DEG && fmod(last_opnd+90, 180) == 0)
      display_error = 1;
      else if (calc_cur_angle == ANG_DEG && fmod(last_opnd, 180) == 0) {
      last_opnd = 0;
      }
      else {
          other_rad ();    
          last_opnd = TAN(last_opnd);
          if (errno == EDOM || errno == ERANGE)
          display_error = 1;
      }
    }
    else {
      // arctan
      last_opnd = ATAN(last_opnd);
      rad_other ();
      if (errno == EDOM || errno == ERANGE)
          display_error = 1;
    }
  }

  if (last_opnd < POS_ZERO && last_opnd > NEG_ZERO)
    last_opnd=0;
}

static void ComputeLog (void)
{
    if (!calc_inv_mode) {
      if (last_opnd <= 0)
          display_error = 1;
      else
          last_opnd = LOG (last_opnd);    
    } else if (calc_inv_mode) {
      last_opnd = POW (10, last_opnd);
    }
}

static void ComputeLn (void)
{
    if (!calc_inv_mode) {
      if (last_opnd <= 0)
        display_error = 1;
      else
        last_opnd = LN (last_opnd);    
    } else if (calc_inv_mode) {
      last_opnd = EXP (last_opnd);    
    }
}

static void ComputeExp (void)
{
    if (!calc_inv_mode) {
        last_opnd = EXP (last_opnd);
    }
    else {
        if (last_opnd <= 0)
            display_error = 1;
        else
            last_opnd = LN (last_opnd);
    }
}

// bit operations
static FLOAT ComputeBitOp (FLOAT left_opnd, int bit_optr, FLOAT right_opnd)
{
    FLOAT    f_tmp;
    long     i_left, i_right;

    MODF(left_opnd, &f_tmp);
    if (FABS(f_tmp) > ULONG_MAX) {
        display_error = 1;
        return 0;
    }
    i_left = (long int) f_tmp;
    
    if (bit_optr == CODE_NOT) {
        return (~i_left);
    }
    
    MODF(right_opnd, &f_tmp);
    if (FABS(f_tmp) > ULONG_MAX) {
        display_error = 1;
        return 0;
    }
    i_right = (long int) f_tmp;

    switch (bit_optr) {
        case CODE_AND:
            return (i_left & i_right);
            break;
        case CODE_OR:
            return (i_left | i_right);
            break;
        case CODE_XOR:
            return (i_left ^ i_right);
            break;
        case CODE_LSH:
            if (!calc_inv_mode)
                return (i_left << i_right);
            else
                return (i_left >>i_right);
            break;
        case CODE_RSH:
            if (!calc_inv_mode)
                return (i_left >>i_right);
            else
                return (i_left << i_right);
            break;
    }
    return 0;
}

static FLOAT ComputeMod (FLOAT left_op, FLOAT right_op)
{
  FLOAT temp =0.0;

  if (right_op == 0) {
    display_error = 1;
    return 0L;
  } else {
    right_op = FABS(right_op);
    temp = FMOD(left_op, right_op);
    if( temp < 0 ) temp = right_op + temp;
    temp = FABS(temp);
    return temp;
  }
}

static void ComputeFactorial (void)
{
    FLOAT i_opnd, opnd_temp;
    int     incr;

    MODF(last_opnd, &i_opnd);

    incr = i_opnd < 0 ? -1 : 1;
    opnd_temp = i_opnd - incr;
    while (i_opnd != 0 && opnd_temp != 0 && !display_error) {
        i_opnd *= opnd_temp;
        opnd_temp -= incr;
        if(ISINF(i_opnd)) {
          display_error=1;
           break;
        }
    }

    if( i_opnd == 0.0)
      i_opnd = 1.0;

    last_opnd = i_opnd;
}

//---------------------------------------------------------------------------

// function to compute with unary
static int ufunc(int btnid)        
{
    switch(btnid) {
        case CODE_SQRT:
            if (!calc_inv_mode) {
                if (last_opnd < 0) {
                    display_error = 1;
                }
                else {
                    last_opnd = SQRT ((FLOAT)(last_opnd));
                }
            }
            else
                last_opnd = last_opnd * last_opnd;
            break;
        case CODE_RECIP:
            if (last_opnd == 0) {
                display_error = 1;
            }
            else
                last_opnd = 1 / (last_opnd);
            break;
        case CODE_PLUSMINUS:
            last_opnd = -(last_opnd);
            break;
        case CODE_SQ:
            if (!calc_inv_mode)
                last_opnd = last_opnd * last_opnd;
            else {
                if (last_opnd < 0) {
                    display_error = 1;
                }
                else {
                    last_opnd = SQRT ((FLOAT)(last_opnd));
                }
            }
            break;
        case CODE_FAC:
            ComputeFactorial ();
        break;
        
        case CODE_SIN:
            ComputeSin ();
        break;

        case CODE_COS:
            ComputeCos ();
        break;

        case CODE_TAN:
            ComputeTan ();
        break;
        
        case CODE_LOG:
            ComputeLog ();
        break;

        case CODE_LN:
            ComputeLn ();
        break;

        case CODE_EXP:
            ComputeExp ();
        break;

        case CODE_NOT:
            last_opnd = ComputeBitOp (last_opnd, CODE_NOT, 0);
            break;

        default:
            break;
    }
    return 0;
}

static FLOAT Operate (FLOAT pre_opnd, int optr, FLOAT last_opnd)
{
    FLOAT result = 0;

    switch(optr) {
    case     CODE_DIVIDE:
        if (last_opnd == 0) {
            display_error = 1;
        }
        else
            result = pre_opnd / last_opnd;
        break;
    case    CODE_MULTIPLY:
        result = pre_opnd * last_opnd;
        break;
    case    CODE_SUBTRACT:
        result = pre_opnd - last_opnd;
        break;
    case    CODE_ADD:
        result = pre_opnd + last_opnd;
        break;
    case CODE_XY:
        result = POW (pre_opnd, last_opnd);
        break;
    case CODE_MOD:
        result = ComputeMod (pre_opnd, last_opnd);
        break;
    case CODE_AND:
        result = ComputeBitOp (pre_opnd, CODE_AND, last_opnd);
        break;
    case CODE_OR:
        result = ComputeBitOp (pre_opnd, CODE_OR, last_opnd);
        break;
    case CODE_XOR:
        result = ComputeBitOp (pre_opnd, CODE_XOR, last_opnd);
        break;
    case CODE_LSH:
        result = ComputeBitOp (pre_opnd, CODE_LSH, last_opnd);
        break;
    case CODE_RSH:
        result = ComputeBitOp (pre_opnd, CODE_RSH, last_opnd);
        break;
            
    }
    return result;
}

//---------------------------------------------------------------------------

static void Enter_Digit (HWND hwnd, int btnid)
{
    if (!IsDigit (btnid)) return;

    //processing invalid input
    if (IsUnary (calc_pre_btnid) || calc_pre_btnid == CODE_EQ 
                            || calc_pre_btnid == CODE_PERCENT
                            || calc_pre_btnid == CODE_RIGHTPAREN) {
        ClearAll ();
    }
    
    switch (btnid) {

        case CODE_0:
            push_digit(0);
            break;

        case CODE_1:
            push_digit(1);
            break;

        case CODE_2:
            if (calc_cur_base == BASE_BIN) return;
            push_digit(2);
            break;

        case CODE_3:
            if (calc_cur_base == BASE_BIN) return;
            push_digit(3);
            break;

        case CODE_4:
            if (calc_cur_base == BASE_BIN) return;
            push_digit(4);
            break;

        case CODE_5:
            if (calc_cur_base == BASE_BIN) return;
            push_digit(5);
            break;

        case CODE_6:
            if (calc_cur_base == BASE_BIN) return;
            push_digit(6);
            break;

        case CODE_7:
            if (calc_cur_base == BASE_BIN) return;
            push_digit(7);
            break;

        case CODE_8:
            if (calc_cur_base == BASE_BIN || calc_cur_base == BASE_OCT) return;
            push_digit(8);
            break;

        case CODE_9:
            if (calc_cur_base == BASE_BIN || calc_cur_base == BASE_OCT) return;
            push_digit(9);
            break;

        case CODE_POINT:
            if (calc_cur_base != BASE_DEC) return;
            push_digit(DIGIT_POINT);
            break;

        case CODE_A:
            if (calc_cur_base != BASE_HEX) return;
            push_digit(10);
            break;

        case CODE_B:
            if (calc_cur_base != BASE_HEX) return;
            push_digit(11);
            break;

        case CODE_C:
            if (calc_cur_base != BASE_HEX) return;
            push_digit(12);
            break;

        case CODE_D:
            if (calc_cur_base != BASE_HEX) return;
            push_digit(13);
            break;

        case CODE_E:
            if (calc_cur_base != BASE_HEX) return;
            push_digit(14);
            break;

        case CODE_F:
            if (calc_cur_base != BASE_HEX) return;
            push_digit(15);
            break;

        default:
            break;
    }
    stack_var();        
    show_digit_stack (hwnd);
}

static void Enter_Unary (HWND hwnd, int btnid)
{
    ufunc (btnid);
    clear_digit_stack();        
    show_var(hwnd);
}

static void enter_percent (int optr, FLOAT opnd2)
{
    if (!calc_percent_mod) return;
    switch (optr) {
    case CODE_ADD:
    case CODE_SUBTRACT:
        last_opnd = last_opnd / opnd2 * 100;
        break;
    case CODE_MULTIPLY:
        last_opnd = last_opnd / 100;
        break;
    case CODE_DIVIDE:
        last_opnd = last_opnd * 100;
        break;
    }
    calc_percent_mod = FALSE;
}

static int evaluate_expression (HWND hwnd, int btnid)
{
    BOOL end_while = FALSE;
    int pre_optr = -1;
    FLOAT opnd2 = 0;

    if (!IsBinary (btnid) && btnid != CODE_EQ && btnid != CODE_PERCENT) {
        return -1;
    }

    calc_last_optr = btnid;
    while (!end_while) {
        if (GetOptrTop () == -1) {
            if (IsBinary (calc_last_optr) && calc_last_optr != CODE_RIGHTPAREN) {
                push_optr (calc_last_optr);
                push_opnd (last_opnd);
            }
            end_while = TRUE;
        }
        else {
            switch (cmp_priority (GetOptrTop (), calc_last_optr)) {
            case LOWPRIO: 
                push_optr (calc_last_optr);
                if (calc_last_optr != CODE_LEFTPAREN) push_opnd (last_opnd);
                end_while = TRUE;
                break;
            case EQUALPRIO:
                pop_optr (NULL);
                end_while = TRUE;
                break;
            case HIGHPRIO:
                {
                FLOAT pre_opnd = 0;
    
                pop_optr (&pre_optr);
                pop_opnd (&pre_opnd);
                // percent process
                if (calc_percent_mod) opnd2 = last_opnd;
                //
                last_opnd = Operate (pre_opnd, pre_optr, last_opnd);
                }    
                break;
            }    
        }
    }
    // percent process
    if (calc_percent_mod && pre_optr != -1) {
        enter_percent (pre_optr, opnd2);
    }
    //
    show_var (hwnd);
    return 0;
}

static void Enter_Mc (void)
{
    M_buffer = 0;
}

static void Enter_Mr (HWND hwnd)
{
    clear_digit_stack();
    last_opnd = M_buffer;
    show_var(hwnd);
}

static void Enter_Mminus (void)
{
    M_buffer = M_buffer - dispnow;
}

static void Enter_Ms (void)
{
    M_buffer = dispnow;
}

static void Enter_Mplus (void)
{
    M_buffer = M_buffer + dispnow;
}

static void Enter_Bs (HWND hwnd)
{ 
    if(digit_stack.base != digit_stack.top) {
        pop_digit();
        stack_var();
        show_digit_stack (hwnd);
    }
}

static void Enter_Ce (HWND hwnd)
{
    
    if(!digit_stack_empty ()) {
        clear_digit_stack();
        stack_var ();
        show_digit_stack (hwnd);
    }else if(calc_pre_btnid == CODE_MR) {
        last_opnd = dispnow = 0;
        if (!display_error) 
        {
            strcpy(calc_strdisp, "0");
            calc_InvalidDiaplayRect(hwnd);
        }
    }
}

static void Enter_Ac (HWND hwnd)
{
    ClearAll ();
    strcpy(calc_strdisp, "0");
    calc_InvalidDiaplayRect(hwnd);
}

//---------------------------------------------------------------------------

void InitCalculator (void)
{
    calc_Status = CALC_COMPACT;
    calc_cur_base = BASE_DEC;
    calc_cur_angle = ANG_DEG;
    calc_hyp_mode = FALSE, calc_inv_mode = FALSE, calc_percent_mod = FALSE;

    pi = ASIN (1L) * 2L;

    display_error = 0;
    M_buffer = 0;

    calc_pre_btnid = -1;
    last_opnd = 0, dispnow = 0;
    calc_last_optr = 0;

    init_digit_stack ();
    init_optr_stack ();
    init_opnd_stack ();

    SetRect (&calc_rcArrow, ARROW_LEFT, ARROW_TOP, ARROW_RIGHT, ARROW_BOTTOM);
    DecideSize ();
}

// main function for processing input
void InputProc (HWND hwnd, int btnid)
{
    if (display_error && btnid != CODE_AC) return;

    if (IsDigit (btnid)) {
        Enter_Digit (hwnd, btnid);    
    }
    else if (IsUnary (btnid)) {
        clear_digit_stack ();
        Enter_Unary (hwnd, btnid);
    }
    else if (IsBinary (btnid)) {
        clear_digit_stack ();
        evaluate_expression (hwnd, btnid);
    }
    else {
        switch (btnid) {

        case CODE_EQ:
            clear_digit_stack ();
            evaluate_expression (hwnd, btnid);
            break;
        
        case CODE_PERCENT:
            calc_percent_mod = TRUE;
            clear_digit_stack ();
            evaluate_expression (hwnd, btnid);
            break;
            
        case CODE_MC:
            Enter_Mc ();
            break;

        case CODE_MR:
            Enter_Mr (hwnd);
            break;
        
        case CODE_MMINUS:
            Enter_Mminus ();
            break;

        case CODE_MS:
            Enter_Ms ();
            break;

        case CODE_MPLUS:
            Enter_Mplus ();
            break;

        case CODE_BS:
            Enter_Bs (hwnd);
            break;

        case CODE_CE:
            Enter_Ce (hwnd);
            break;

        case CODE_AC:
            Enter_Ac (hwnd);
            break;
        }
    }    

    if(btnid == CODE_MC || btnid == CODE_MPLUS || btnid == CODE_MS) ;
    else if((btnid == CODE_BS || btnid == CODE_CE) && digit_stack_empty ());
    else    calc_pre_btnid = btnid;
}

