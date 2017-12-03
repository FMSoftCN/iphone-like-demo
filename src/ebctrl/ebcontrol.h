//
// $Id: ebcontrol.h,v 1.5 2005/10/17 07:55:53 tangjb Exp $
//
// edit.h: the head file of Edit Control module.
//
// Copyright (c) 1999, Wei Yongming.
//
// Create date: 1999/8/26
//
#ifndef EBCONTROL_H_
#define EBCONTROL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

/****** Edit and MEdit Control ***********************************************/
#define CTRL_EBEDIT           ("ebedit")

    /**
     * \defgroup ctrl_edit_styles Styles of edit control
     * @{
     */

/**
 * \def ES_LEFT
 * \brief Left-aligns text.
 */
#define EES_LEFT             0x00000000L
#define EES_TOP				 0x00000000L
#define EES_CENTER           0x00000001L
#define EES_RIGHT            0x00000002L    //�������ַ�����û���õ�����Ӧ�༭����˵�������ַ���ƺ�������
#define EES_VCENTER          0x00000004L
#define EES_BOTTOM           0x00000008L

#define EES_USEBORDER		 0x00000010L	//ʹ���û��Լ��������ɫ�߿�
#define EES_USEUNDERLINE	 0x00000020L	//ʹ���û��Լ��������ɫ�»���, ��ɫͨ��EEM_SETBORDERCOL ��������ɫ
#define EES_BASELINE		 0x00000030L
#define EES_STYLESMASK		 0x00000030L
/**
 * \def ES_UPPERCASE
 * \brief Converts all characters to uppercase as they are typed into the edit control.
 */
#define EES_UPPERCASE        0x00000040L
/**
 * \def ES_LOWERCASE
 * \brief Converts all characters to lowercase as they are typed into the edit control.
 */
#define EES_LOWERCASE        0x00000080L


//#define EES_MULTILINE        0x00000004L											

/**
 * \def ES_PASSWORD
 * \brief Displays an asterisk (*) for each character typed into the edit control.
 */
#define EES_PASSWORD         0x00000100L

//#define EES_AUTOVSCROLL      0x00000040L
//#define EES_AUTOHSCROLL      0x00000080L

//#define EES_NOHIDESEL        0x00000100L 	//edit by tjb û�õ�
#define EES_AUTOWRAP		 0x00000200L		//add by tjb 2004-4-6


//#define EES_OEMCONVERT      0x00000400L
#define EES_NUMONLY          0X00000400L
/**
 * \def ES_READONLY
 * \brief Prevents the user from typing or editing text in the edit control.
 */
#define EES_READONLY         0x00000800L

/**
 * \def ES_AUTOWRAP
 * \brief Automatically wraps against border when inputting.
 */
 
#define EES_BKIMAGEMASK		0x00003000L
#define EES_BKIMAGENONE	    0x00000000L
#define EES_BKICON	       	0x00001000L
#define EES_BKBITMAP	    0x00002000L
#define EES_BKBMPFILE       0x00003000L

#define EES_BKALPHA         0x00004000L
#define EES_BKTRANSPARENT   0x00008000L

    /** @} end of ctrl_edit_styles */

    /**
     * \defgroup ctrl_edit_msgs Messages of edit control
     * @{
     */

#define EEM_GETSEL               0xF0B0
#define EEM_SETSEL               0xF0B1
#define EEM_GETRECT              0xF0B2
#define EEM_SETRECT              0xF0B3
#define EEM_SETRECTNP            0xF0B4
#define EEM_SCROLL               0xF0B5
#define EEM_LINESCROLL           0xF0B6
#define EEM_SCROLLCARET          0xF0B7
#define EEM_GETMODIFY            0xF0B8
#define EEM_SETMODIFY            0xF0B9
#define EEM_GETLINECOUNT         0xF0BA
#define EEM_LINEINDEX            0xF0BB
#define EEM_SETHANDLE            0xF0BC
#define EEM_GETHANDLE            0xF0BD
#define EEM_GETTHUMB             0xF0BE
#define EEM_LINELENGTH           0xF0C1
#define EEM_REPLACESEL           0xF0C2
#define EEM_GETLINE              0xF0C4

/**
 * \def EM_LIMITTEXT
 * \brief Set text limit of an edit control.
 *
 * \code
 * EM_LIMITTEXT
 * int newLimit;
 *
 * wParam = (WPARAM)newLimit;
 * lParam = 0;
 * \endcode
 *
 * \param newLimit The new text limit of an edit control:
 */
#define EEM_LIMITTEXT            0xF0C5

#define EEM_CANUNDO              0xF0C6
#define EEM_UNDO                 0xF0C7
#define EEM_FMTLINES             0xF0C8
#define EEM_LINEFROMCHAR         0xF0C9
#define EEM_SETTABSTOPS          0xF0CB

/**
 * \def EM_SETPASSWORDCHAR
 * \brief Defines the character that edit control uses in conjunction with 
 * the ES_PASSWORD style.
 *
 * \code
 * EM_SETPASSWORDCHAR
 * int passwdChar;
 *
 * wParam = (WPARAM)passwdChar;
 * lParam = 0;
 * \endcode
 */
#define EEM_SETPASSWORDCHAR      0xF0CC

#define EEM_EMPTYUNDOBUFFER      0xF0CD
#define EEM_GETFIRSTVISIBLELINE  0xF0CE

/**
 * \def EM_SETREADONLY
 * \brief Sets or removes the read-only style (ES_READONLY) in an edit control.
 *
 * \code
 * EM_SETREADONLY
 * int readonly;
 *
 * wParam = (WPARAM)readonly;
 * lParam = 0;
 * \endcode
 *
 * \param readonly Indicates whether the edit control is read-only:
 *      - Zero\n
 *        Not read-only.
 *      - Non zero\n
 *        Read-only.
 */
#define EEM_SETREADONLY          0xF0CF

#define EEM_SETWORDBREAKPROC     0xF0D0
#define EEM_GETWORDBREAKPROC     0xF0D1

/**
 * \def EM_GETPASSWORDCHAR
 * \brief Returns the character that edit controls uses in conjunction with 
 * the ES_PASSWORD style.
 *
 * \code
 * EM_GETPASSWORDCHAR
 * int *passwdchar;
 *
 * wParam = 0;
 * lParam = (LPARAM)passwdchar;
 * \endcode
 *
 */
#define EEM_GETPASSWORDCHAR      0xF0D2

/***********************************************
*edit at 2005-4-28  by tangjb
*����:���ö��б༭��ı߿�հ�
*		wparam = �������ߵĿհ�
*		lparam = �������ߵĿհ�
************************************************/
#define EEM_SETMARGINS           0xF0D3
#define EEM_GETMARGINS           0xF0D4

#define EEM_SETLIMITTEXT         EM_LIMITTEXT
#define EEM_GETMARGINS           0xF0D4
#define EEM_GETLIMITTEXT         0xF0D5
#define EEM_POSFROMCHAR          0xF0D6
#define EEM_CHARFROMPOS          0xF0D7
#define EEM_SETIMESTATUS         0xF0D8
#define EEM_GETIMESTATUS         0xF0D9


#define EEM_SETTEXTCORLOR	0xF0DA
#define EEM_GETTEXTCORLOR	0xF0DB

#define EMEM_SCROLLCHANGE        0xF0DC

//add by tjb 2004-4-2
//���һ�ַ��,���û��Լ�������
/**
 * \def EEM_SETUSERCARET
 * \brief set edit control caret with bitmap 
 *
 * \code
 * EEM_SETUSERCARET
 * int *passwdchar;
 *
 * wParam = 0;
 * lParam = (LPARAM)bitmap name;
 * \endcode
 *
 */
#define EEM_SETUSERCARET	0xF0DD

//add by tjb 2004-4-2
//���û��Լ�����߿���»��ߵ���ɫ
/**
 * \def EEM_SETUSERCARET
 * \brief set edit control caret with bitmap 
 *
 * \code
 * EEM_SETUSERCARET
 * int *passwdchar;
 *
 * wParam = 0;
 * lParam = (LPARAM)new color;
 * \endcode
 *
 */
#define EEM_SETBORDERCOL	0xF0DE

/***********************************************
*add at 2004-9-3  by tangjb
*����:�����м�࣬Ĭ��Ϊ 5 pixel
*		lparam = newinterval;
************************************************/
#define EEM_SETINTERVAL		0xF0DF

#define EMED_STATE_YES           0x0
#define EMED_STATE_NOUP          0x1
#define EMED_STATE_NODN          0x2
#define EMED_STATE_NO            0x3

    /** @} end of ctrl_edit_msgs */

    /**
     * \defgroup ctrl_edit_ncs Notification codes of edit control
     * @{
     */

/**
 * \def EN_CLICKED
 * \brief Notifies a click in an edit control.
 *
 * An edit control sends the EN_CLICKED notification code when the user clicks
 * in an edit control.
 */
#define EEN_CLICKED          0x0001

/**
 * \def EN_DBLCLK
 * \brief Notifies a double click in an edit control.
 *
 * An edit control sends the EN_CLICKED notification code when the user 
 * double clicks in an edit control.
 */
#define EEN_DBLCLK           0x0002

/**
 * \def EN_SETFOCUS
 * \brief Notifies the receipt of the input focus.
 *
 * The EN_SETFOCUS notification code is sent when an edit control receives 
 * the input focus.
 */
#define EEN_SETFOCUS         0x0100

/**
 * \def EN_KILLFOCUS
 * \brief Notifies the lost of the input focus.
 *
 * The EN_KILLFOCUS notification code is sent when an edit control loses 
 * the input focus.
 */
#define EEN_KILLFOCUS        0x0200

/**
 * \def EN_CHANGE
 * \brief Notifies that the text is altered.
 *
 * An edit control sends the EN_CHANGE notification code when the user takes 
 * an action that may have altered text in an edit control.
 */
#define EEN_CHANGE           0x0300

#define EEN_UPDATE           0x0400
#define EEN_ERRSPACE         0x0500

/**
 * \def EN_MAXTEXT
 * \brief Notifies reach of maximum text limitation.
 *
 * The EN_MAXTEXT notification message is sent when the current text 
 * insertion has exceeded the specified number of characters for the edit control.
 */
#define EEN_MAXTEXT          0x0501

#define EEN_HSCROLL          0x0601
#define EEN_VSCROLL          0x0602

/**
 * \def EN_ENTER
 * \brief Notifies the user has type the ENTER key in a single-line edit control.
 */
#define EEN_ENTER            0x0700

    /** @} end of ctrl_edit_ncs */

/* Edit control EM_SETMARGIN parameters */
/**
 * \def EC_LEFTMARGIN
 * \brief Value of wParam. Specifies the margins to set.
 */
#define EEC_LEFTMARGIN       0x0001
/**
 * \def EC_RIGHTMARGIN
 * \brief Value of wParam. Specifies the margins to set.
 */
#define EEC_RIGHTMARGIN      0x0002
/**
 * \def EC_USEFONTINFO
 * \brief Value of wParam. Specifies the margins to set.
 */
#define EEC_USEFONTINFO      0xffff

/* wParam of EM_GET/SETIMESTATUS  */
/**
 * \def EMSIS_COMPOSITIONSTRING
 * \brief Indicates the type of status to retrieve.
 */
#define EEMSIS_COMPOSITIONSTRING        0x0001

/* lParam for EMSIS_COMPOSITIONSTRING  */
/**
 * \def EIMES_GETCOMPSTRATONCE
 * \brief lParam for EMSIS_COMPOSITIONSTRING.
 */
#define EEIMES_GETCOMPSTRATONCE         0x0001
/**
 * \def EIMES_CANCELCOMPSTRINFOCUS
 * \brief lParam for EMSIS_COMPOSITIONSTRING.
 */
#define EEIMES_CANCELCOMPSTRINFOCUS     0x0002
/**
 * \def EIMES_COMPLETECOMPSTRKILLFOCUS
 * \brief lParam for EMSIS_COMPOSITIONSTRING.
 */
#define EEIMES_COMPLETECOMPSTRKILLFOCUS 0x0004

    /** @} end of ctrl_edit */

/****** Progress Bar Control *************************************************/
    /**
     * \defgroup ctrl_progbar ProgressBar control
     * @{
     */

/**
 * \def CTRL_PROGRESSBAR
 * \brief The class name of progressbar control.
 */
#define CTRL_EBPROGRESSBAR        ("ebprogressbar")

    /**
     * \defgroup ctrl_progbar_styles Styles of progressbar control
     * @{
     */

/**
 * \def PBS_NOTIFY
 * \brief Notifies the parent window.
 * 
 * Sends the parent window notification messages when 
 * the user clicks or double-clicks the control.
 */
#define EPBS_NOTIFY              0x0001L

/**
 * \def PBS_VERTICAL
 * \brief Creates progressbar vertically.
 */
#define EPBS_VERTICAL            0x0002L

//�������;
#define EPBS_FILLTYPE		0x00000070L
#define EPBS_FILLNORMAL		0x00000000L//��׼���;
#define EPBS_FILLBRUSH		0x00000010L//�������;
#define EPBS_FILLBMP		0x00000020L//��λͼ������;
#define EPBS_FILLBMPFILE	0x00000030L//��λͼ�ļ����;
#define EPBS_FILLICON		0x00000040L//��ͼ�����;

//�����ķ�ʽ;
#define EPBS_BKTYPE			0x00000F00L
#define EPBS_BKNORMAL		0x00000000L//���ñ�׼����;
#define EPBS_BKBITMAP		0x00000100L//λͼ���;
#define EPBS_BKBMPFILE		0x00000200L//λͼ�ļ�;
#define EPBS_BKCOLOR		0x00000400L//����ɫ��䱳��;
#define EPBS_BKALPHA		0x00000800L//����͸��;

//��䷽ʽ;
#define EPBS_POSITION		0x00001000L		//ʹ��ͼ�궨λ����ʾ��ǰλ��
#define EPBS_FILL 			0x00002000L		//ʹ�����ͼ������ʾ��ǰ����(����ͼ���ظ����)
#define EPBS_FILLLINE		0x00004000L		//ʹ�����ͼ������ʾ��ǰλ��(����ͼ���Գ�������ʾ)
#define EPBS_FASTFILLLINE   0x00008000L     //�������ͼ��
#define EPBS_SHOWMASK		0x0000F000L

    /** @} end of ctrl_progbar_styles */

    /**
     * \defgroup ctrl_progbar_msgs Messages of progressbar control
     * @{
     */

/**
 * \def PBM_SETRANGE
 * \brief Sets the limits of the range.
 *
 * Sets the upper and lower limits of the progress bar control's range, 
 * and redraws the bar to reflect the new ranges.
 *
 * \code
 * PBM_SETRANGE
 * int min, max;
 *
 * wParam = (WPARAM)min;
 * lParam = (LPARAM)max;
 * \endcode
 */
#define EPBM_SETRANGE            0xF0A0

/**
 * \def PBM_SETSTEP
 * \brief Specifies the step increment for a progress bar control.
 *
 * \code
 * PBM_SETSTEP
 * int stepinc;
 *
 * wParam = (WPARAM)stepinc;
 * lParam = 0;
 * \endcode
 *
 * \param stepinc Step increment for a progress bar control.
 */
#define EPBM_SETSTEP             0xF0A1

/**
 * \def PBM_SETPOS
 * \brief Sets the progress bar control's current position.
 *
 * Sets the progress bar control's current position as specified by nPos, 
 * and redraw the bar to reflect the new position.
 *
 * \code
 * PBM_SETPOS
 * int nPos;
 *
 * wParam = (WPARAM)nPos;
 * lParam = 0;
 * \endcode
 *
 * \param nPos The progress bar control's current position.
 */
#define EPBM_SETPOS              0xF0A2

/**
 * \def PBM_DELTAPOS
 * \brief Advances the progress bar controlps current position. 
 *
 * Advances the progress bar control's current position as specified by posInc, 
 * and redraw the bar to reflect the new position.
 *
 * \code
 * PBM_DELTAPOS
 * int posInc;
 *
 * wParam = (WPARAM)posInc;
 * lParam = 0;
 * \endcode
 *
 * \param posInc The progress bar control's position increment.
 */
#define EPBM_DELTAPOS            0xF0A3

/**
 * \def PBM_STEPIT
 * \brief Advances the current position by the step increment.
 *
 * Advances the current position for a progress bar control by 
 * the step increment, and redraw the bar to reflect the new position.
 *
 * \code
 * PBM_STEPIT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 */
#define EPBM_STEPIT              0xF0A4

#define EPBM_GETPOS             0xF0A5

#define EPBM_GETRANGE			0xF0A6//��ý����������Сֵ wParam = Min;lParam =Max 

//EPBM_SETCOLOR
//����ɫ�����ɫ�Ǵ�ɫʱ����Ҫ����Ϣ��������ɫ
//wParam = bkColor		��ɫ
//lParam = FillColor	���ɫ
#define EPBM_SETCOLOR			0xF0A7


//EPBM_SETBORDERWIDTH
//�����ͼ��������ؼ���ͼ�Ŀ�ȡ��߶�һ��ʱ����Ҫ���ñ߿�ĺ��
//lParam = BorderWidth	
#define EPBM_SETBORDERWIDTH		0xF0A8

    /** @} end of ctrl_progbar_msgs */

    /**
     * \defgroup ctrl_progbar_ncs Notification codes of progressbar control
     * @{
     */

/**
 * \def PBN_REACHMAX
 * \brief Notifies reach of maximum limit.
 *
 * The PBN_REACHMAX notification code is sent when the progressbar reachs its maximum limit.
 */
#define EPBN_REACHMAX            1

/**
 * \def PBN_REACHMIN
 * \brief Notifies reach of minimum limit.
 *
 * The PBN_REACHMIN notification code is sent when the progressbar reachs its minimum limit.
 */
#define EPBN_REACHMIN            2

    /** @} end of ctrl_progbar_ncs */

/************************** TrackBar Control ***********************************/
    /**
     * \defgroup ctrl_trackbar TrackBar control
     * @{
     */

/**
 * \def CTRL_TRACKBAR
 * \brief The class name of trackbar control.
 */
#define CTRL_EBTRACKBAR              ("ebtrackbar")

    /**
     * \defgroup ctrl_trackbar_styles Styles of trackbar control
     * @{
     */

/**
 * \def TBS_NOTIFY
 * \brief Causes the trackbar to notify the parent window with a notification message 
 * 		when the user clicks or doubleclicks the trackbar.
 */
#define ETBS_NOTIFY                 0x0001L

/**
 * \def TBS_VERTICAL
 * \brief The trackbar control will be oriented vertically.
 */
#define ETBS_VERTICAL               0x0002L

/**
 * \def TBS_BORDER
 * \brief The trackbar control will have border.
 */
#define ETBS_BORDER                 0x0004L

/**
 * \def TBS_TIP
 * \brief The trackbar control will display tick marks above the control.
 */
#define ETBS_TIP                    0x0008L

/**
 * \def TBS_FOCUS
 * \brief The trackbar control has the focus frame.
 */
#define ETBS_FOCUS                  0x0010L

/**
 * \def TBS_DRAGGED
 * \brief The user is dragging the slider.
 */
#define ETBS_DRAGGED                0x0020L

/**
 * \def TBS_MOUSEDOWN
 * \brief The mouse button is down.
 */
#define ETBS_MOUSEDOWN              0x0040L

#define ETBS_BKALPHA		    0x0080L



//ָ�뾭�������䷽ʽ;
#define ETBS_FILLTYPE		0x00000300L
#define ETBS_FILLNONE		0x00000000L//�����;
#define ETBS_FILLLINE		0x00000100L//�������;
#define ETBS_FILLIDTN		0x00000200L//��ָ��ͼ�����;
#define ETBS_FILLRECT		0x00000300L//�������;

//�����ķ�ʽ;
#define ETBS_BKTYPE		0x00000C00L
#define ETBS_BKNORMAL		0x00000000L//���ñ�׼����;
#define ETBS_BKBITMAP		0x00000400L//λͼ���;
#define ETBS_BKBMPFILE		0x00000800L//λͼ�ļ�;
#define ETBS_BKICON		0x00000C00L//ͼ����;

//ָʾ��ķ�ʽ;
#define ETBS_IDTNTYPE		0x00003000L
#define ETBS_IDTNAROW		0x00000000L//��ͷָʾ��;
#define ETBS_IDTNBITMAP		0x00001000L//λͼ���ָʾ��;
#define ETBS_IDTNBMPFILE	0x00002000L//λͼ�ļ�ָʾ��;
#define ETBS_IDTNICON		0x00003000L//λͼͼ����ָʾ��;
				

    /** @} end of ctrl_trackbar_styles */

    /**
     * \defgroup ctrl_trackbar_msgs Messages of trackbar control
     * @{
     */

/**
 * \def TBM_SETRANGE
 * \brief Sets the range of minimum and maximum logical positions for the 
 * 		slider in a trackbar.
 *
 * \code
 * TBM_SETRANGE
 * int min;
 * int max;
 *
 * wParam = (WPARAM)min;
 * lParam = (LPARAM)max;
 * \endcode
 *
 * \param min Minimum position for the slider.
 * \param max Maximum position for the slider.
 */
#define ETBM_SETRANGE               0xF090

/**
 * \def TBM_GETMIN
 * \brief Gets the minimum logical position for the slider.
 *
 * \code
 * TBM_GETMIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The minimum logical position for the slider.
 */
#define ETBM_GETMIN                 0xF091

/**
 * \def TBM_SETPOS
 * \brief Sets the current logical position of the slider.
 *
 * \code
 * TBM_SETPOS
 * int pos;
 *
 * wParam = (WPARAM)pos;
 * lParam = 0;
 * \endcode
 *
 * \param pos New logical position of the slider.
 */
#define ETBM_SETPOS                 0xF092

/**
 * \def TBM_GETPOS
 * \brief Gets the current logical position of the slider.
 *
 * \code
 * TBM_GETPOS
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current logical position of the slider.
 */
#define ETBM_GETPOS                 0xF093

/**
 * \def TBM_SETLINESIZE
 * \brief Sets the number of logical positions moved in response to keyboard
 * 	input from the arrow keys.
 *
 * Sets the number of logical positions the trackbar's slider moves in response
 * to keyboard input from the arrow keys. The logical positions are the integer
 * increments in the trackbar's range of minimum to maximum slider positions.
 *
 * \code
 * TBM_SETLINESIZE
 * int linesize;
 *
 * wParam = (WPARAM)linesize;
 * lParam = 0;
 * \endcode
 *
 * \param linesize New line size;
 */
#define ETBM_SETLINESIZE            0xF094

/**
 * \def TBM_GETLINESIZE
 * \brief Gets the number of logical positions moved in response to keyboard
 * 	input from the arrow keys.
 *
 * Gets the number of logical positions the trackbar's slider moves in response
 * to keyboard input from the arrow keys. The logical positions are the integer
 * increments in the trackbar's range of minimum to maximum slider positions.
 *
 * \code
 * TBM_GETLINESIZE
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current line size.
 */
#define ETBM_GETLINESIZE            0xF095

/**
 * \def TBM_SETPAGESIZE
 * \brief Sets the number of logical positions moved in response to keyboard
 * 		input from page keys..
 *
 * Sets the number of logical positions the trackbar's slider moves in response
 * to keyboard input form page keys, such as PAGE DOWN or PAGE UP keys. The 
 * logical positions are the integer increments in the trackbar's range of 
 * minimum to maximum slider positions.
 *
 * \code
 * TBM_SETPAGESIZE
 * int pagesize;
 *
 * wParam = (WPARAM)pagesize;
 * lParam = 0;
 * \endcode
 *
 * \param pagesize New page size;
 */
#define ETBM_SETPAGESIZE            0xF096

/**
 * \def TBM_GETPAGESIZE
 * \brief Gets the number of logical positions moved in response to keyboard
 * 		input from page keys..
 *
 * Gets the number of logical positions the trackbar's slider moves in response
 * to keyboard input form page keys, such as PAGE DOWN or PAGE UP keys. The 
 * logical positions are the integer increments in the trackbar's range of 
 * minimum to maximum slider positions.
 *
 * \code
 * TBM_GETPAGESIZE
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current page size;
 */
#define ETBM_GETPAGESIZE            0xF097

/**
 * \def TBM_SETSTARTTIP
 * \brief Sets the starting logical position of a tick mark.
 *
 * \code
 * TBM_SETSTARTTIP
 * int starttip;
 *
 * wParam = (WPARAM)starttip;
 * lParam = 0;
 * \endcode
 *
 * \param starttip New starting logical position of a tick mark;
 */
#define ETBM_SETSTARTTIP            0xF098

/**
 * \def TBM_SETENDTIP
 * \brief Sets the ending logical position of a tick mark.
 *
 * \code
 * TBM_SETENDTIP
 * int endtip;
 *
 * wParam = (WPARAM)endtip;
 * lParam = 0;
 * \endcode
 *
 * \param starttip New ending logical position of a tick mark;
 */
#define ETBM_SETENDTIP              0xF099

/**
 * \def TBM_GETTIP
 * \brief Gets the starting and ending logical position of a tick mark.
 *
 * \code
 * TBM_GETTIP
 * int *starttip;
 * int *endtip;
 *
 * wParam = (WPARAM)starttip;
 * lParam = (LPARAM)endtip;
 * \endcode
 *
 * \param starttip Retreives the starting logical position of a tick mark.
 * \param endtip Retreives the ending logical position of a tick mark.
 */
#define ETBM_GETTIP                 0xF09A

/**
 * \def TBM_SETTICKFREQ
 * \brief Sets the interval frequency for tick marks in a trackbar.
 *
 * \code
 * TBM_SETTICKFREQ
 * int tickfreq;
 *
 * wParam = (WPARAM)tickfreq;
 * lParam = 0;
 * \endcode
 *
 * \param tickfreq New interval frequency for tick marks in a trackbar;
 */
#define ETBM_SETTICKFREQ            0xF09B

/**
 * \def TBM_GETTICKFREQ
 * \brief Gets the interval frequency for tick marks in a trackbar.
 *
 * \code
 * TBM_GETTICKFREQ
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current interval frequency for tick marks in a trackbar;
 */
#define ETBM_GETTICKFREQ            0xF09C

/**
 * \def TBM_SETMIN
 * \brief Sets the minimum logical position for the slider in a trackbar.
 *
 * \code
 * TBM_SETMIN
 * int min;
 *
 * wParam = (WPARAM)min;
 * lParam = 0;
 * \endcode
 *
 * \param min The new minimum logical position for the slider in a trackbar.
 */
#define ETBM_SETMIN                 0xF09D

/**
 * \def TBM_SETMAX
 * \brief Sets the maximum logical position for the slider in a trackbar.
 *
 * \code
 * TBM_SETMAX
 * int max;
 *
 * wParam = (WPARAM)max;
 * lParam = 0;
 * \endcode
 *
 * \param min The new maximum logical position for the slider in a trackbar.
 */
#define ETBM_SETMAX                 0xF09E

/**
 * \def TBM_GETMAX
 * \brief Gets the maximum logical position for the slider in a trackbar.
 *
 * \code
 * TBM_GETMAX
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current maximum logical position for the slider in a trackbar.
 */
#define ETBM_GETMAX                 0xF09F

#define ETBM_SETFGNDCOLOR	    0xF0A0	/*����ǰ����ɫ*/
#define ETBM_GETFGNDCOLOR	    0xF0A1	/*����ǰ����ɫ*/

#define ETBM_SETTBBORDER	    0xF0A2
#define ETBM_GETTBBORDER	    0xF0A3

    /** @} end of ctrl_trackbar_msgs */

    /**
     * \defgroup ctrl_trackbar_ncs Notification codes of trackbar control
     * @{
     */

/**
 * \def TBN_REACHMIN
 * \brief Notifies that the slider has reached the minimum position.
 */
#define ETBN_REACHMIN                1

/**
 * \def TBN_REACHMAX
 * \brief Notifies that the slider has reached the maximum position.
 */
#define ETBN_REACHMAX                2

/**
 * \def TBN_CHANGE
 * \brief Notifies that the position of the slider has changed.
 */
#define ETBN_CHANGE                  3

    /** @} end of ctrl_trackbar_ncs */

    /** @} end of ctrl_trackbar */

/****** Static Control ******************************************************/
    /**
     * \defgroup ctrl_ebstatic Static control
     * @{
     */

/**
 * \def CTRL_EBSTATIC
 * \brief The class name of static control.
 */
#define CTRL_EBSTATIC         ("ebstatic")

    /**
     * \defgroup ctrl_static_styles Styles of static control
     * @{
     */

/**
 * \def SS_LEFT
 * \brief Displays the given text flush-left.
 */
#define ESS_LEFT             	0x00000000L

/**
 * \def SS_CENTER 
 * \brief Displays the given text centered in the rectangle. 
 */
#define ESS_CENTER           	0x00000001L

/**
 * \def SS_RIGHT
 * \brief Displays the given text flush-right.
 */
#define ESS_RIGHT            	0x00000002L

#define ESS_ALIGNMASK			0x00000003L//bit0~1


/**
 * \def SS_ICON 
 * \brief Designates an icon displayed in the static control.
 */
#define ESS_BKSYSDEFAULT		0x00000000L
#define ESS_ICON				0x00000004L
#define ESS_BITMAP				0x00000008L
#define ESS_BKBMPFILE			0x0000000CL
#define ESS_ENHMETAFILE			0x00000010L
#define ESS_BKTRANSPARENT		0x00000014L
#define ESS_BKUSECOLOR			0x00000018L
#define ESS_ALPHAIMAGE          0x0000001CL//ʹ��͸���ļ���ʵ�ֵ�
#define ESS_BKTYPEMASK			0x0000001CL//bit2~4


/**
 * \def SS_GRAYRECT
 * \brief Specifies a rectangle filled with the color used to 
 *        fill the screen background. The default is gray.
 */
#define ESS_GRAYRECT         	0x00000020L
#define ESS_BLACKRECT        	0x00000040L
#define ESS_WHITERECT       	0x00000060L
#define ESS_RECTMASK			0x00000060L//bit5~6



/**
 * \def SS_GRAYFRAME
 * \brief Specifies a box with a frame drawn with the same color as 
 *        window frames. The default is gray.
 */
#define ESS_BLACKFRAME       	0x00000080L

#define ESS_GRAYFRAME        	0x00000100L

#define ESS_WHITEFRAME       	0x00000180L
#define ESS_FRAMEMASK			0x00000180L//bit7~8;

/**
 * \def SS_GROUPBOX
 * \brief Creates a rectangle in which other controls can be grouped.
 */
#define ESS_GROUPBOX         	0x00000200L//bit9

/**
 * \def SS_SIMPLE 
 * \brief Designates a simple rectangle and displays a single line 
 *        of text flush-left in the rectangle.
 */
#define ESS_SIMPLE           	0x00000400L//bit10

/**
 * \def SS_LEFTNOWORDWRAP
 * \brief Designates a simple rectangle and displays the given text 
 *        flush-left in the rectangle. 
 *
 * Tabs are expanded, but words are not wrapped. 
 * Text that extends past the end of a line is clipped.
 */
#define ESS_LEFTNOWORDWRAP   	0x00000800L//bit11

//#define ESS_USERCOLOR        	0x00001000L//bit12
#define ESS_MULTILINE           0x00001000L
/**
 * \def SS_BITMAP
 * \brief Specifies that a bitmap will be displayed in the static control.
 */
#define ESS_TYPEMASK         	0x00001FFFL


/**
 * \def SS_NOPREFIX
 * \brief Prevents interpretation of any ampersand (&) characters in 
 *        the control's text as accelerator prefix characters.
 *
 * \note Not implemented so far.
 */
#define ESS_NOPREFIX         	0x00002000L//bit13


/**
 * \def SS_NOTIFY
 * \brief Sends the parent window notification messages when the user 
 *        clicks or double-clicks the control.
 */
#define ESS_NOTIFY           	0x00004000L//bit14
#define ESS_BKALPHA	    		0x00008000L//bit15


/** @} end of ctrl_static_styles */
    
    /**
     * \defgroup ctrl_static_msgs Messages of static control
     * @{
     */

#define ESTM_SETICON         0xF170
#define ESTM_GETICON         0xF171

/**
 * \def STM_SETIMAGE
 * \brief Associate a new image (icon or bitmap) with a static control.
 *
 * An application sends an STM_SETIMAGE message to 
 * associate a new image (icon or bitmap) with a static control.
 *
 * \code
 * STM_SETIMAGE
 * HICON image;
 *  or 
 * BITMAP* image;
 *
 * wParam = (WPARAM)image;
 * lParam = 0;
 * \endcode
 * 
 * \param image The handle to an icon if the type of static control type 
 * is SS_ICON, or the pointer to a BITMAP object if the type is SS_BITMAP.
 * \return The old image (handle or pointer).
 */
#define ESTM_SETIMAGE        0xF172

/**
 * \def STM_GETIMAGE
 * \brief Retrieves a handle to the image.
 *
 * An application sends an STM_GETIMAGE message to retrieve a handle 
 * to the image associated with a static control.
 *
 * \code
 * STM_GETIMAGE
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The handle to the icon if the type of static control type is SS_ICON, 
 * or the pointer to the BITMAP object if the type is SS_BITMAP.
 */
#define ESTM_GETIMAGE        	0xF173
#define ESTM_GETTEXTCOLOR	0xF174
#define ESTM_SETTEXTCOLOR	0xF175

/* Not supported */
#define ESTM_MSGMAX          0xF178

    /** @} end of ctrl_static_msgs */

    /**
     * \defgroup ctrl_static_ncs Notification codes of static control
     * @{
     */

/**
 * \def STN_DBLCLK
 * \brief Notifies a double-click.
 *
 * The STN_DBLCLK notification message is sent when 
 * the user double-clicks a static control that has the SS_NOTIFY style.
 */
#define ESTN_DBLCLK          1

#define ESTN_ENABLE          2
#define ESTN_DISABLE         3

/**
 * \def STN_CLICKED
 * \brief Notifies that a static control is clicked.
 *
 * The STN_CLICKED notification message is sent 
 * when the user clicks a static control that has the SS_NOTIFY style.
 */
#define ESTN_CLICKED         4

    /** @} end of ctrl_static_ncs */

    /** @} end of ctrl_static */

/****** Button Control ******************************************************/
    /**
     * \defgroup ctrl_button Button control
     * @{
     */

/**
 * \def CTRL_BUTTON
 * \brief The class name of button control.
 */
#define CTRL_BMPBUTTON         ("bmpbutton")

    /**
     * \defgroup ctrl_button_styles Styles of button control
     * @{
     */

/**
 * \def BS_PUSHBUTTON
 * \brief Creates a push button.
 */
 //��ԭʼ��������������̬����ֻ��һ��λͼ��
#define BBS_PUSHBUTTON      0x00000001L

//ֻ��ԭʼ��������ѡ����̬������λͼ��
#define BBS_THREESTATE      0x00000002L

//��ԭʼ�����������¡�ѡ����̬���ķ�λͼ��
#define BBS_FOURSTATE	    0x00000003L

//��ԭʼ�����������ʱ�ھ���ͼƬ�Ļ���������������λͼ��
#define BBS_MOTIVEBUTTON    0x00000004L

//ֻ��ԭʼ��������̬�������ʱ��Ȼ�Ǿ���̬������ƿ���Ϊԭʼ̬��
#define BBS_TWOSTATE        0x00000005L

//��ԭʼ\����\�������״̬,һ��λͼ,����ʱ��ԭλͼ�ϼ��Ͻ����;
#define BBS_SUNSTYLE	    0x00000006L

//ֻ��ԭʼ��������̬�������º�ֻ��ͨ����ϢBBM_SETSTATE���ܸı���״̬
#define BBS_PUSHDOWNBNT		0x00000007L

#define BBS_TYPEMASK        0x0000000FL

/**
 * \def BS_TEXT
 * \brief Specifies that the button displays text.
 */
#define BBS_TEXT            0x00000010L
#define BBS_ICON            0x00000040L
#define BBS_BMPFILE	     	0x00000080L

/**
 * \def BBS_LEFT
 * \brief Left-justifies the text in the button rectangle.
 *
 * However, if the button is a check box or radio button that 
 * does not have the BS_RIGHTBUTTON style, the text is left 
 * justified on the right side of the check box or radio button.
 */
#define BBS_LEFT            0x00000000L

/**
 * \def BS_RIGHT
 * \brief Right-justifies text in the button rectangle.
 * 
 * However, if the button is a check box or radio button that 
 * does not have the BS_RIGHTBUTTON style, the text is 
 * right justified on the right side of the check box or radio button.
 */
#define BBS_RIGHT            0x00000100L

/**
 * \def BS_CENTER
 * \brief Centers text horizontally in the button rectangle.
 */
#define BBS_CENTER           0x00000200L

/**
 * \def BS_TOP
 * \brief Places text at the top of the button rectangle.
 */
#define BBS_TOP              0x00000000L

/**
 * \def BS_BOTTOM
 * \brief Places text at the bottom of the button rectangle.
 */
#define BBS_BOTTOM           0x00000400L

/**
 * \def BS_VCENTER
 * \brief Places text in the middle (vertically) of the button rectangle.
 */
#define BBS_VCENTER          0x00000800L

#define BBS_ALIGNMASK        0x00000F00L


#define BBS_BKTRANSPARENT         0x00001000L

/**
 * \def BS_MULTLINE
 * \brief Wraps the button text to multiple lines.
 *
 * Wraps the button text to multiple lines if the text string is 
 * too long to fit on a single line in the button rectangle.
 */
#define BBS_BKALPHA         0x00002000L

/**
 * \def BS_NOTIFY
 * \brief Enables a button to send notification messages to its parent window.
 */
#define BBS_NOTIFY           0x00004000L

#define BBS_BKALPHARED		 0x00008000L		//BBS_FLAT û���õ�
    /** @} end of ctrl_button_styles */

    /**
     * \defgroup ctrl_button_states States of button control
     * @{
     */

/**
 * \def BST_UNCHECKED
 * \brief Indicates the button is unchecked.
 */
#define BBST_UNCHECKED       0x0000

/**
 * \def BST_CHECKED
 * \brief Indicates the button is checked.
 */
#define BBST_CHECKED         0x0001

/**
 * \def BST_INDETERMINATE
 * \brief Indicates the button is grayed because 
 * the state of the button is indeterminate.
 */
#define BBST_INDETERMINATE   0x0002

/**
 * \def BST_PUSHED
 * \brief Specifies the highlight state.
 */
#define BBST_PUSHED          0x0004

/**
 * \def BST_FOCUS
 * \brief Specifies the focus state.
 */
#define BBST_FOCUS           0x0008

    /** @} end of ctrl_button_states */

    /**
     * \defgroup ctrl_button_msgs Messages of button control
     * @{
     */

/**
 * \def BM_GETCHECK
 * \brief Retrieves the check state of a radio button or check box.
 *
 * An application sends a BM_GETCHECK message to retrieve 
 * the check state of a radio button or check box.
 *
 * \code
 * BM_GETCHECK
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return An integer indicates whether the button is checked.
 *
 * \retval BST_UNCHECKED The button is not checked.
 * \retval BST_CHECKED The button is checked.
 * \retval BST_INDETERMINATE The button is grayed because the state of the button is indeterminate.
 *
 * \sa ctrl_button_states
 */
#define BBM_GETCHECK             0xF0F0

/**
 * \def BM_SETCHECK
 * \brief Sets the check state of a radio button or check box.
 *
 * An application sends a BM_SETCHECK message to set 
 * the check state of a radio button or check box.
 *
 * \code
 * BM_SETCHECK
 * int check_state;
 *
 * wParam = (WPARAM)check_state;
 * lParam = 0;
 * \endcode
 *
 * \param check_state The check state of button, can be one of the following values:
 *      - BST_UNCHECKED\n
 *        Want the button to be unchecked.
 *      - BST_CHECKED\n
 *        Want the button to be checked.
 *      - BST_INDETERMINATE\n
 *        Want the button to be grayed if it is a three states button.
 */
#define BBM_SETCHECK             0xF0F1

/**
 * \def BM_GETSTATE
 * \brief Gets the state of a button or check box.
 *
 * An application sends a BM_GETSTATE message to 
 * determine the state of a button or check box.
 *
 * \code
 * BM_GETSTATE
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return An integer indicates the button state.
 *
 * \sa ctrl_button_states
 */
#define BBM_GETSTATE             0xF0F2

/**
 * \def BM_SETSTATE
 * \brief Sets the state of a button.
 *
 * An application sends a BM_GETSTATE message to set the state of a 
 * button.
 *
 * \code
 * BM_SETSTATE
 * int push_state;
 *
 * wParam = (WPARAM)push_state;
 * lParam = 0;
 * \endcode
 *
 * \param push_state The push state of a button, can be one of the following values:
 *      - Zero\n
 *        Want the button to be unpushed.
 *      - Non zero\n
 *        Want the button to be pushed.
 */
#define BBM_SETSTATE             0xF0F3

/**
 * \def BM_SETSTYLE
 * \brief Changes the style of a button.
 *
 * An application sends a BM_SETSTYLE message to change the style of a button.
 *
 * \code
 * BM_SETSTYLE
 * int button_style;
 *
 * wParam = (WPARAM)button_style;
 * lParam = 0;
 * \endcode
 *
 * \param button_style The styles of a button:
 *
 * \sa ctrl_button_styles
 */
#define BBM_SETSTYLE             0xF0F4

/**
 * \def BM_CLICK
 * \brief Simulates the user clicking a button.
 *
 * An application sends a BM_CLICK message to simulate the user clicking a button.
 *
 * \code
 * BM_CLICK
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 */
#define BBM_CLICK                0xF0F5

/**
 * \def BM_GETIMAGE
 * \brief Retrieves the handle to the image.
 *
 * An application sends a BM_GETIMAGE message to 
 * retrieve a handle to the image (icon or bitmap) associated with the button.
 *
 * \code
 * BM_GETIMAGE
 * int image_type;
 *
 * wParam = (WPARAM)image_type;
 * lParam = 0;
 * \endcode
 *
 * \param image_type The type of a button image, can be one of the following values:
 *      - BM_IMAGE_BITMAP\n
 *        Bitmap of a button.
 *      - BM_IMAGE_ICON\n
 *        Icon of a button.
 *
 * \return A handle of the bitmap or icon of a button.
 */
#define BBM_GETIMAGE            0xF0F6
#define BBM_SETTEXTCOLOR		0xF0F7
#define BBM_GETTEXTCOLOR		0xF0F8

#define BBM_IMAGE_BITMAP         1
#define BBM_IMAGE_ICON           2
    
/**
 * \def BM_SETIMAGE
 * \brief Associates a new image (icon or bitmap) with the button.
 *
 * An application sends a BM_SETIMAGE message to 
 * associate a new image (icon or bitmap) with the button.
 *
 * Please use BM_IMAGE_BITMAP or BM_IMAGE_ICON as the first parameter of the message 
 * to indicate the type of button control image:
 *  - BM_IMAGE_BITMAP\n
 *          Specifies the type of image to associate with the button to be a bitmap.
 *  - BM_IMAGE_ICON\n
 *          Specifies the type of image to associate with the button to be an icon.
 */
#define BBM_SETIMAGE             0xF0F9

//��̬ʱ����Ϊ������״̬Ϊδ���
#define BBM_SETUNCLICK           0xF0FA

    /** @} end of ctrl_button_msgs */
    
    /**
     * \defgroup ctrl_button_ncs Notification codes of button control
     * @{
     */

/**
 * \def BN_CLICKED
 * \brief The BN_CLICKED notification message is sent when the user clicks a button.
 */
#define BBN_CLICKED          0

#define BBN_PAINT            1
#define BBN_HILITE           2
#define BBN_UNHILITE         3
#define BBN_DISABLE          4
#define BBN_DOUBLECLICKED    5

/**
 * \def BN_PUSHED
 * \brief The BN_PUSHED notification message is sent when the user pushes a button.
 */
#define BBN_PUSHED           BBN_HILITE

/**
 * \def BN_UNPUSHED
 * \brief The BN_UNPUSHED notification message is sent when the user unpushes a button.
 */
#define BBN_UNPUSHED         BBN_UNHILITE

/**
 * \def BN_DBLCLK
 * \brief The BN_DBLCLK notification message is sent when the user double-clicks a button.
 */
#define BBN_DBLCLK           BBN_DOUBLECLICKED

/**
 * \def BN_SETFOCUS
 * \brief The BN_SETFOCUS notification message is sent when a button receives the keyboard focus.
 */
#define BBN_SETFOCUS         6

/**
 * \def BN_KILLFOCUS
 * \brief The BN_KILLFOCUS notification message is sent when a button loses the keyboard focus.
 */
#define BBN_KILLFOCUS        7

#define BBN_MOUSEOVER		 8

/** @} end of ctrl_button */

/************************** List View control ********************************/
    /**
     * \defgroup mgext_ctrl_listview ListView control
     * @{
     */

#define CTRL_EBLISTVIEW       ("EBListView")


/**/
typedef struct _EBLVEXTDATA
{
    int   nItemHeight;        //��Ŀ�ĸ߶�(�����);
    int   nItemGap;           //��Ŀ��ļ��;
    DWORD nSelectBKColor;     //ѡ�к�ı�����ɫ��
    DWORD nSelectTextColor;   //ѡ�к����ֵ���ʾ��ɫ��
    DWORD nItemBKColor;       //��Ŀ�ı���ɫ;
    DWORD nItemTextColor;     //��Ŀ��������ɫ;
    DWORD nMouseOverBkColor;  //��꾭��ʱ����ɫ;
    DWORD nMouseOverTextColor;//��꾭��ʱ������ɫ;
    DWORD nItemBKImage;       //��Ŀ�ı���ͼƬ;  
    DWORD nBKImage;           //���ڵı���ͼƬ;
}EBLVEXTDATA,*PEBLVEXTDATA;   
    
/** Struct of the listview item info */
typedef struct _EBLVITEM
{
    /** the rows of the item */
    int nItem;
}EBLVITEM;

typedef EBLVITEM *PEBLVITEM;

#define  SUBITEM_WORD_NORMALFONT    0x00000001L     //��ͨ����
#define  SUBITEM_WORD_SHADOWFONT    0x00000002L     //��Ӱ��

typedef struct _EBLVSUBITEMWORDTYPE
{
	int   topoffset;
	int   leftoffset;
	DWORD shadowcolor;   //��Ӱ�ֵ���Ӱ����ɫ
	DWORD wordfont;      //��:��Ӱ��,��ͨ��
}SIWORDTYPE,*PSIWORDTYPE;

/** Struct of the listview subitem info */ 
typedef struct _EBLVSUBITEM
{
    int   nItem;           //this Subitem vertical position(rows).
    int   subItem;         //this Subitem horizontal position(columns).
    DWORD mask;            //LV_TEXT,LV_IMAGE,LV_IMAGEFILE,LV_ICON,LVIF_PARAM ;
    char* pszText;         //���ֻ�ͼƬ�ļ���
    int	  cchTextMax;      //��������󳤶ȣ� 
    DWORD iImage;          //ͼ���ļ���
    DWORD lparam;          //�������ݣ�
    PSIWORDTYPE  wordtype; //���ֵĸ�����Ϣ
    //char *pszText;       //the pointer of the Content of this subitem. 
} EBLVSUBITEM;
typedef EBLVSUBITEM *PEBLVSUBITEM;

/** Struct of the listview column info */
typedef struct _EBLVCOLOUM
{
    /** the horizontal position */
    int nCols;
    /** column's width */
    int width;
    /** the title of this column */
    char *pszHeadText;
} EBLVCOLOUM;
typedef EBLVCOLOUM *PEBLVCOLOUM;

/** Struct of finding a certain item info */
typedef struct _EBLVFINDINFO
{
    /** the found item's row */
    int nCols;

    /** all the subitem's content of this item */
    char **pszInfo;
} EBLVFINDINFO;
typedef EBLVFINDINFO *PEBLVFINDINFO;

#define ELVS_BKMASK				0x00000003L
#define ELVS_BKNONE				0x00000000L//�ޱ���ͼƬ
#define ELVS_BKBRUSH			0x00000001L//�û�ˢ����䱳��;
#define ELVS_BKBITMAP			0x00000002L//��BITMAP������ͼ;
#define ELVS_BKBMPFILE			0x00000003L//��ͼƬ�ļ�������ͼ;

#define ELVS_BKIMAGEALIGNMASK	0x0000000CL
#define ELVS_BKIMAGEFLAT		0x00000000L//ƽ��;
#define ELVS_BKIMAGECENTER		0x00000004L//����;
#define ELVS_BKIMAGELEFTTOP		0x00000008L//���Ͻ�;


#define ELVS_TEXTTRANSPARENT	0x00000010L//�����Ƿ񱳾�͸��;
#define ELVS_ITEMUSERCOLOR		0x00000020L//�Ƿ���Ŀ�����ѵ���ɫ;
#define ELVS_USEHEADER			0x00000040L//�Ƿ���ʾͷ;
#define ELVS_BKALPHA			0x00000080L//�����Ƿ��͸��ֵ;

#define ELVS_ITEMBKMASK			0x00000300L
#define ELVS_ITEMBKINONE		0x00000000L
#define ELVS_ITEMBKICON			0x00000100L
#define ELVS_ITEMBKBITMAP		0x00000200L
#define ELVS_ITEMBKBMPFILE		0x00000300L


#define ELVS_TYPE3STATE			0x00000400L//֧����̬,��������꾭����������̬;
#define ELVS_TYPE2STATE			0x00000800L//֧����̬,��������꾭����̬;
#define ELVS_BKSCROLL			0x00001000L//֧�ֱ�������;

#define ELVS_SPECIAL2STATE		0x00002000L//֧����̬����꾭���Ͱ���״̬��

#define ELVS_MULTILINE			0x00004000L//������ʾ�ı�

#define ELVS_USECHECKBOX		0x00008000L//֧���б�ѡ���(���б������ʾһ��ѡ���)
											//add by tjb 2004-8-4
/**
 * \defgroup mgext_ctrl_listview_msgs Messages of ListView control
 * @{
 */

#define  ROWSTATE_NORMAL    1
#define  ROWSTATE_MOUSEOVER       2
#define  ROWSTATE_SELECTED        3
#define  ROWSTATE_INVALIDE        4

typedef struct _item_draw_context{
    RECT   paint_area;  //the rect area in which can draw picture or text
    int    row;    //the number of row of this item in the listview
    int    total_rows;  //total number of rows in the listview
    int    state;  //1:normal ,2:mouseover, 3:selected, 4:invalid
}ITEM_DRAW_CON;


/**
 *  \def LVM_ADDITEM Add a item to listview.
 *  \brief Adds a item to listview, this item is also called a row. 
 *  
 *  \code 
 *  LVM_ADDITEM
 *  PLVITEM p;
 *
 *  lParam =(LPARAM)p;
 *  wParam = 0;
 *  \endcode
 *
 *  \param p the Pointer of the item to be added.
 */
#define ELVM_ADDITEM            0xF110

/**
 * \def LVM_FILLSUBITEM        Add a subitem to listview.
 * \brief Adds a subitem to listview, indentified by rows and columns.
 * 
 * \code
 * LVM_FILLSUBITEM
 * PLVSUBITEM p;
 *
 * lParam = (LPARAM)p;
 * wParam = 0;
 * \endcode
 *
 * \param p the Pointer of the subitem to be added.
 */
#define ELVM_FILLSUBITEM        0xF111

/**
 * \def LVM_ADDCOLUMN
 * \brief Adds a column to listview, indentified by columns.
 * 
 * \code
 *  LVM_ADDCOLUMN
 *  PLVCOLOUMN p;
 *
 *  lParam =(LPARAM)p;
 *  wParam = 0;
 * \endcode
 *
 * \param p the Pointer of the column to be added.
 */
#define ELVM_ADDCOLUMN          0xF112

/**
 * \def LVM_DELITEM
 * \brief Deletes a item form listview, also called a row.
 *
 * \code
 * LVM_DELITEM
 * PLVITEM p;
 *
 * lParam = (LPARAM)p;
 * wParam = 0;
 * \endcode
 *
 * \param p the ponter of the column to be removed.
 */
#define ELVM_DELITEM            0xF113

/**
 * \def LVM_CLEARSUBITEM
 * \brief Deletes a subitem from listview, indentified by rows and columns.
 * 
 * \code
 * LVM_CLEARSUBITEM
 * PLVSUBITEM p
 * lParam = (LPARAM)p;
 * wParam = 0;
 * \endcode
 *
 * \param p the Pointer of the sub item to be removed.
 */
#define ELVM_CLEARSUBITEM       0xF114

/**
 * \def LVM_DELCOLUMN
 * \brief Deletes a column from listview, all subitem in this column are removed.  
 *
 * \code
 * LVM_DELCOLUMN
 * lParam =(LPARAM)p;
 * wParam = 0;
 * \endcode
 *
 * \param p the Pointer of the column to be removed.
 */
#define ELVM_DELCOLUMN          0xF115

/**
 * \def LVM_COLSORT
 * \brief Sorts all subitems in a certain columns in hi-order or low-order. 
 *
 * \code
 * LVM_COLSORT
 * lParam = (LPARAM)p;
 * wParam = 0;
 * \endcode
 *
 * \param p the Pointer to the struct of how and which column is tobe sorted.
 */
#define ELVM_COLSORT            0xF116

/**
 * \def ELVM_SETITEMCOLOR
 * \brief Sets a item in listview to a certain color.
 *
 * \code
 * LVM_SETITEMCOLOR
 * int color;
 * int rows;
 * lParam = (LPARAM)color;
 * wParam = (WPARAM)rows;
 * \endcode
 *
 * \param color the color to set.
 * \param rows the item to set.
 */
#define ELVM_SETITEMCOLOR       0xF117

/**
 * \def LVM_FINDITEM
 *
 * \code
 * LVM_FINDITEM
 * \endcode
 */
#define ELVM_FINDITEM           0xF118

/**
 *
 * \def LVM_GETSUBITEM
 * \brief Gets a subitem copy from listview in certain rows and columns. 
 *
 * \code 
 * LVM_GETSUBITEM
 * PLVSUBITEM p;
 * lParam = (LPARAM)p;
 * \endcode
 * 
 * \param p the struct of the subitem to be finded, rows, cols etc.
 * \return the target subitem's copy.
 */

#define ELVM_GETSUBITEM         0xF119

/**
 * \def LVM_GETITEMCOUNT
 * \brief Gets the number of all the items(rows) in listview.
 *
 * \code
 * LVM_GETITEMCOUNT
 * \endcode
 * \return the number of all the items in listview.
 */
#define ELVM_GETITEMCOUNT       0xF11A

/**
 * \def LVM_GETCOLUMNCOUNT
 * \brief Gets the number of all the columns in listview.
 *
 * \code
 * LVM_GETCOLUMNCOUNT
 * \endcode
 *
 * \return the number of all the coloumns in listview.
 */
#define ELVM_GETCOLUMNCOUNT     0xF11B

/**
 * \def LVM_GETSELECTEDITEM
 * \brief Gets the current selected item. This message is a internal message of listview.
 *
 * \code
 * LVM_GETSELECTEDITEM
 * \endcode
 *
 * \return the current selected item.
 */
#define ELVM_GETSELECTEDITEM    0xF11C

/**
 * \def LVM_DELALLITEM
 * \brief Removes all the items in listview.
 *
 * \code
 * LVM_DEALLITEM
 * \endcode
 */
#define ELVM_DELALLITEM           0xF11D

/**
 * \def LVM_MODIFYHEAD
 * \brief Changes the title of a column.
 *
 * \code
 * LVM_MODIFYHEAD
 * \endcode
 *
 */
#define ELVM_MODIFYHEAD           0xF11E

//���ÿؼ��ı���ɫ
//add by tjb
//wparam = newcolor
#define ELVM_SETBKCOLOR           0xF11F

//����ĳ��ITEM�Ƿ�ѡ��
/*
* \code 
 * int p;    //��һ�н�����Ϊѡ��״̬
 * lParam = (LPARAM)p;
 * \endcode
 */
#define ELVM_SETITEMSELECTED           0xF120

//�������EVLS_USECHECKBOXʱ������checkbox ��ͼƬ
//lparam = (LPARAM)szName;
//
#define ELVM_SETCHECKBOXIMG         0xF121

//iRows�Ƿ�ѡ�У�ѡ�з���1, ����ѡ���з���0
//int iRows
//lParam = (LPARAM)iRows
#define ELVM_GETROWCHECKED			0xF122

//
/*
     ��ȡ��ǰ��һ�д�����꾭��״̬
*/
#define  ELVM_GETMOUSEOVERITEM                  0xF123


//
/*
     ����ĳһ��Ϊ��꾭��״̬
     int p =(int)lParam;
     ������-1
*/
#define  ELVM_SETMOUSEOVERITEM                  0xF124

//��ȡ��ǰ������꾭��״̬����һ�о���ɼ��ĵ�һ���м���
//�����ǰû����һ�д�����꾭��״̬�򷵻� -1
#define  ELVM_GETMOUSEOVER_OFFSET               0xF125

//���þ���ɼ��ĵ�һ��p�е���һ�д�����꾭��״̬
/*
          int p2 =(int)lParam;
          ������-1
*/
#define  ELVM_SETMOUSEOVER_OFFSET               0xF126

//�Զ���İ���������Ϣ������MSG_KEYDOWN����ͬ�������ܷ�������
/*
    ����wParam��MSG_KEYDOWNһ��Ҳ��SCANCODE_ENTER,SCANCODE_CURSORBLOCKLEFT,
    SCANCODE_CURSORBLOCKRIGHT,SCANCODE_CURSORBLOCKDOWN,SCANCODE_CURSORBLOCKUP
    ,SCANCODE_PAGEUP��
*/
#define  ELVM_MSG_KEYDOWN                       0xF127

//���ÿɼ��� wparam = visable row
#define  ELVM_SETVISABLEROW						0xF128

//���õ�һ�пɼ��� wparam = first visable row
#define  ELVM_SETFIRSTVISABLEITEM				0xF129

/**
* \def  ELVM_SET_ITEMDRAWCALLBACK
* \brief Message for set item draw callback function
*/
#define  ELVM_SET_ITEMDRAWCALLBACK              0xF12A

    /** @} end of mgext_ctrl_listview_msgs */

    /**
     * \defgroup mgext_ctrl_listview_ncs Notification code of ListView control
     * @{
     */

/**
 * \def LVN_ADDITEM
 * \brief This notification code informs parent window a certain item (row) added.
 */
#define ELVN_ADDITEM            1

/**
 * \def LVN_ADDITEM
 * \brief This notification code informs parent window a certain item (row) removed.
 */
#define ELVN_DELITEM            2

/**
 * \def LVN_ADDCOLUMN
 * \brief This notification code informs parent window a certain column added.
 */
#define ELVN_ADDCOLUMN           3

/**
 * \def LVN_DELCOLUMN
 * \brief This notification code informs parent window a certain column removed.
 */
#define ELVN_DELCOLUMN          4

/**
 * \def LVN_ITEMCOLORCHANGED
 * \brief This notification code informs parent window a certain item's color changed.
 */
#define ELVN_ITEMCOLORCHANGED   5

/**
 * \def LVN_DELALLITEM
 * \brief This notification code informs parent window all items removed.
 */
#define ELVN_DELALLITEM         6

/**
 * \def LVN_SELCHANGE
 * \brief This notification code informs parent window the current selected item 
 *        has changed.
 */
#define ELVN_SELCHANGE          7

/**
 *
 * \def LVN_ITEMDBCLK
 * \brief This notification code informs parent window the current selected item 
 *        has be double clicked.
 */
#define ELVN_ITEMDBCLK          8

/**
 * \def LVN_CRKEYDOWN
 * \brief This notification code informs parent window the ENTER key has been pressed.
 */
#define ELVN_CRKEYDOWN          9
#define ELVN_VSCROLL			10
//��ǰ�ؼ���û����꾭��״̬תΪ����꾭��״̬
#define ELVN_ITEMMOUSEOVERED            11
//��ǰ�ؼ��ɴ�����꾭��״̬תΪʧȥ���
#define ELVN_NCITEMMOUSEOVERED            12

//The LBUTTONUP MESSAGE
#define ELVN_LBUTTONUP            13

#define ELV_TEXT		1
#define ELV_BITMAP		2
#define ELV_BMPFILE		3
#define ELV_ICON		4
#define ELV_BOTH		5		//add by tjb 2004-4-14
#define BMP_TYPE_ALPHAVALUE     256
								//�÷�����ͼƬ����������
/** @} end of mgext_ctrl_listview_ncs */

/****** Checkbox Control ******************************************************/
    /**
     * \defgroup Checkbox control
     * @{
     */

#define CTRL_EBCHECKBOX         ("ebcheckbox")
  
/** * \defgroup ctrl_Checkbox_styles Styles of button control*/
/***/
#define ECBS_CHECKBOX		0x00000001L//ѡ���
#define ECBS_RADIOBOX       0x00000002L//��ѡ��
#define ECBS_TYPEMASK       0x0000000FL

/**/
#define ECBS_DEFAULT		0x00000000L//ʹ��ϵͳĬ�ϵ�ͼƬ 
#define ECBS_BMPFILE        0x00000010L//�Զ���ͼƬ
#define ECBS_STYLEMASK       0x000000F0L

#define ECBS_BKALPHA		0x00000100L//����͸����

/** @} end of ctrl_Checkbox_styles */
  
/*** \defgroup ctrl_Checkbox_states States of button control*/

#define ECBT_UNCHECKED      0x0000//δѡ��
#define ECBT_CHECKED        0x0001//ѡ��
#define ECBT_INDETERMINATE	0x0002//��Ч
#define ECBT_MASK			0x000F
/** @} end of ctrl_button_states */

/*** \defgroup ctrl_Checkbox_msgs Messages of button control */

/**
 * \def ECBM_GETCHECK
 * \brief Retrieves the check state of a radio button or check box.
 *
 * An application sends a BM_GETCHECK message to retrieve 
 * the check state of a radio button or check box.
 *
 * \code
 * ECBM_GETCHECK
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return An integer indicates whether the button is checked.
 *
 * \retval BST_UNCHECKED The button is not checked.
 * \retval BST_CHECKED The button is checked.
 * \retval BST_INDETERMINATE The button is grayed because the state of the button is indeterminate.
 *
 * \sa ctrl_button_states
 */
#define ECBM_GETCHECK             0xF0F0

/**
 * \def ECBM_SETCHECK
 * \brief Sets the check state of a radio button or check box.
 *
 * An application sends a BM_SETCHECK message to set 
 * the check state of a radio button or check box.
 *
 * \code
 * ECBM_SETCHECK
 * int check_state;
 *
 * wParam = (WPARAM)check_state;
 * lParam = 0;
 * \endcode
 *
 * \param check_state The check state of button, can be one of the following values:
 *      - BST_UNCHECKED\n
 *        Want the button to be unchecked.
 *      - BST_CHECKED\n
 *        Want the button to be checked.
 *      - BST_INDETERMINATE\n
 *        Want the button to be grayed if it is a three states button.
 */
#define ECBM_SETCHECK             0xF0F1

/**
 * \def ECBM_GETSTATE
 * \brief Gets the state of a button or check box.
 *
 * An application sends a BM_GETSTATE message to 
 * determine the state of a button or check box.
 *
 * \code
 * ECBM_GETSTATE
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return An integer indicates the button state.
 *
 * \sa ctrl_button_states
 */
#define ECBM_GETSTATE             0xF0F2

/**
 * \def ECBM_SETSTATE
 * \brief Sets the state of a button.
 *
 * An application sends a BM_GETSTATE message to set the state of a 
 * button.
 *
 * \code
 * BM_SETSTATE
 * int push_state;
 *
 * wParam = (WPARAM)push_state;
 * lParam = 0;
 * \endcode
 *
 * \param push_state The push state of a button, can be one of the following values:
 *      - Zero\n
 *        Want the button to be unpushed.
 *      - Non zero\n
 *        Want the button to be pushed.
 */
#define ECBM_SETSTATE             0xF0F3

/**
 * \def BM_SETSTYLE
 * \brief Changes the style of a checkbox.
 *
 * An application sends a BM_SETSTYLE message to change the style of a button.
 *
 * \code
 * BM_SETSTYLE
 * int button_style;
 *
 * wParam = (WPARAM)checkbox_style;
 * lParam = 0;
 * \endcode
 *
 * \param button_style The styles of a checkbox:
 *
 * \sa ctrl_checkbox_styles
 */
#define ECBM_SETSTYLE             0xF0F4

/**
 * \def ECBM_CLICK
 * \brief Simulates the user clicking a button.
 *
 * An application sends a BM_CLICK message to simulate the user clicking a button.
 *
 * \code
 * ECBM_CLICK
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 */
#define ECBM_CLICK                0xF0F5

#define ECBM_SETTEXTCOLOR		0xF0F7
#define ECBM_GETTEXTCOLOR		0xF0F8
   
/** @} end of ctrl_checkbox_msgs */
    
    /**
     * \defgroup ctrl_checkbox_ncs Notification codes of button control
     * @{
     */

/**
 * \def ECBN_CLICKED
 * \brief The ECBN_CLICKED notification message is sent when the user clicks a button.
 */
#define ECBN_CLICKED          0

#define ECBN_PAINT            1
#define ECBN_HILITE           2
#define ECBN_UNHILITE         3
#define ECBN_DISABLE          4
#define ECBN_DOUBLECLICKED    5

/**
 * \def ECBN_PUSHED
 * \brief The BN_PUSHED notification message is sent when the user pushes a button.
 */
#define ECBN_PUSHED           BBN_HILITE

/**
 * \def ECBN_UNPUSHED
 * \brief The BN_UNPUSHED notification message is sent when the user unpushes a button.
 */
#define ECBN_UNPUSHED         BBN_UNHILITE

/**
 * \def ECBN_DBLCLK
 * \brief The BN_DBLCLK notification message is sent when the user double-clicks a button.
 */
#define ECBN_DBLCLK           BBN_DOUBLECLICKED

/** @} end of ctrl_checkbox */


/********************************************
 * \def CTRL_PLAYGIF
 * \brief The class name of static control.
 */
#define CTRL_PLAYGIF         ("playgif")

typedef struct tagPlayGifData
{
	char * 	giffile;
	char *  bkfile;
	int 	gifx;
	int 	gify;
}PLAYGIFDATA;
typedef PLAYGIFDATA * PPLAYGIFDATA;

#define GFS_BKTRANSPARENT	0x00000001	//����͸�������û�����
#define GFS_BKTRANSIMAGE	0x00000002	//����Ϊ͸��ͼƬ
#define GFS_BKNOTRANSFILE	0x00000004	//����Ϊ��͸��ͼƬ
#define GFS_BKSAVEBITMAP	0x00000008	//��ͼƬװ�غ󣬱���ͼƬ�ṹֱ���ؼ�����

#define GFS_CENTER			0x00000010
#define GFS_RIGHT			0x00000020
#define GFS_VCENTER			0x00000040
#define GFS_BOTTOM			0x00000080

#define GFS_NOLOOPFIRST		0x00000100	// ��һ��ͼƬ������ѭ��
#define GFS_NORMALGIF		0x00000200	// ����GIF ����Ź�ͼGIF�����𣬼����û�ý���Ͳ��Ŷ���

#define GFS_GIFNORMAL		0x00001000	//���GIF��ͼƬ��ʱ�ķ��
#define GFS_GIFALPHA		0x00002000
#define GFS_GIFALPHAVALUE	0x00004000
#define GFS_GIFALPHACHANNEL	0x00008000

#define GFSS_UNFOCUS		0x0000		//δ��ý���
#define GFSS_FOCUS			0x0001		//��ý���

#define GFM_NEXTIMAGE       0xF0F0
#define GFM_GETTIMES		0xF0F1
#define GFM_SETTIMER		0xF0F2		//�趨��ʱ����ʹGIF��ɶ�̬
#define GFM_KILLTIMER		0xF0F3		//ɾ����ʱ����ʹGIF��ɾ�̬

#define GFN_SETFOCUS		0xF0F4		//֪ͨ��ý���
#define GFN_KILLFOCUS		0xF0F5		//֪ͨʧȥ����
#define GFN_CLICKED			0xF0F6

/**
 * \def GFM_CHANGEGIF
 * \�ı�gifͼƬ
 * \code
 * GFM_CHANGEGIF
 *
 * wParam = 0;
 * lParam = (LPARAM) pPlaygifData;
 * \endcode
 */
#define GFM_CHANGEGIF		0xF0F7

    /**
     * 
     * 
**********************************************/



/********************************/
#define CTRL_EBFLASHPLAYER         ("ebflashplayer")
typedef struct tagFlashPlayerAddData
{	
	int         IsSound; 		// �����Ƿ񲥷�����	
	int         timer_id; 		// ��ʱ��ID	
	char *      url; 			//�ļ���������·��
}FLASHPLAYERADDDATA;

/*wp:url,lp=bsound*/
#define FLP_PLAYFILE        0xF0F1
#define FLP_STOPFILE        0xF0F2
#define FLP_PAUSEFILE       0xF0F3
#define FLP_REWINDFILE      0xF0F4

/*************************************/

BOOL InitEbControl ( void );
void EbControlCleanUp (void);

void CtrlDrawPictrueToBK(HDC hdc, RECT * rect, const char * picname, int bmtype, int alpha);
extern BOOL RefreshChildInParent(HWND hwnd);
extern int GetFontCCharWidth (PLOGFONT font);
extern int GetFontCharWidth (PLOGFONT font);

extern BOOL RegisterPlayGifControl (void);
extern void PlayGifControlCleanup (void);

#ifdef __cplusplus
}
#endif

#endif

