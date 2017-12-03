//
// $Id: eblistview.h,v 1.1.1.1 2005/01/18 11:47:57 tangjb Exp $ 
//
// Copyright (C) 2001 Shu Ming
//
// Create date: 2001/12/03
//

#ifndef _EBLIST_VIEW_H
#define _EBLIST_VIEW_H

#ifdef __cplusplus
extern "C"
{
#endif

/************************** List View control ********************************/
    /**
     * \defgroup mgext_ctrl_listview ListView control
     * @{
     */

#define COLWIDTHDEFAULTS        62
#define HEADRWIDTHDEFAULTS      62
#define HEADRHEIGHTDEFAULTS     15
#define COLHEIGHTDEFAULTS       17
#define COLWIDTHMIN             17
#define HSCROLL                 5
#define VSCROLL                 15
#define LIGHTBLUE	        	180


#define ELV_TEXT		1
#define ELV_BITMAP		2
#define ELV_BMPFILE		3
#define ELV_ICON		4

enum sorttype
{ NOTSORTED = 0, HISORTED, LOSORTED };
typedef enum sorttype SORTTYPE;

typedef struct _EBLVSUBITEMWORDDATA
{
	int topoffset;
	int leftoffset;
	DWORD shadowcolor;   //阴影字的阴影的颜色
	DWORD wordfont;      //如:阴影字,普通字
}SIWORDDATA,*PSIWORDDATA;

typedef struct _ebsubitemdata
{
	//struct _ebsubitemdata *pNext;
	//char *pszInfo;
	struct _ebsubitemdata *pNext;
	DWORD   mask;//LV_TEXT,LV_IMAGE,LV_IMAGEFILE,LV_ICON;
	char *  pszText;//文字或图片文件名
	int	cchTextMax;//缓冲区最大长度； 
	DWORD iImage;//图标文件；
	DWORD dwData;//附加数据；
	PSIWORDDATA  worddata;  //文字的附加信息
}EBSUBITEMDATA;

typedef EBSUBITEMDATA *PEBSUBITEMDATA;


typedef void (*ITEM_DRAW)(HWND hWnd, HDC hdc, void* context);

typedef struct _ebitemdata
{
	struct _ebitemdata *pNext;
	DWORD nTextColor;
	BOOL bSelected;
	BOOL bMouseOver;
    int nCols;
	int nRows;      //used for item selected ,valid is bSelected = true
	BOOL bChecked;	//该行是否被选中
	PEBSUBITEMDATA pSubItemHead;
}EBITEMDATA;
typedef EBITEMDATA *PEBITEMDATA;

typedef struct _eblsthdr
{
	struct _eblsthdr *pNext;
	int x;
	int y;
	int width;
	int height;
	BOOL up;
	SORTTYPE sort;
	char *pTitle;
}EBLSTHDR;
typedef EBLSTHDR *PEBLSTHDR;

typedef struct _eblstvwdata
{
	int nCols;
	int nRows;
	int nVisableRows;
	int nFirstVisableRow;
	int nOriginalX;
	int nOriginalY;
	BOOL bBorderDraged;
	BOOL bItemClicked;
	//static BOOL bMouseInBorder = FALSE;

	PEBLSTHDR pItemDraged;
	PEBLSTHDR pItemClicked;
	PEBITEMDATA pItemSelected;
	PEBITEMDATA pItemMouseOver;
	int oldMouseX, oldMouseY;
	int nItemDraged;
	int nItemSelected;
	int nItemMouseOver;
	int nItemHeight;//条目的高度(含间隔);
	int nItemGap;//条目间的间隔;
	int nHeadHeight;

	DWORD nSelectBKColor;//选中后的背景颜色,需设置风格后才有效；
	DWORD nSelectTextColor;//选中后文字的显示颜色；

	DWORD nItemBKColor;//条目的背景色,需设置风格后才有效;
	DWORD nItemTextColor;//条目的文字颜色;

	DWORD nMouseOverBkColor;//鼠标经过时背景色;
	DWORD nMouseOverTextColor;//鼠标经过时文字颜色;

	DWORD nItemBKImage;	//条目的背景图片;
	DWORD nBKImage;		//窗口的背景图片;
	DWORD nCheckboxImg;	//若是EVLS_USECHECKBOX风格，为checkbox的图片名称
	
	HWND hWnd;
	PEBLSTHDR pLstHead;
	PEBITEMDATA pItemHead;

    ITEM_DRAW   itemdraw;
}EBLSTVWDATA;
typedef EBLSTVWDATA *PEBLSTVWDATA;

typedef struct _ebcmpitemstruct
{
	int nItemSeq;
	char *pszText;
}EBCMPITEMSTRUCT;
typedef EBCMPITEMSTRUCT *PEBCMPITEMSTRUCT;

typedef struct _ebcmpinfo
{
	int nCols;
	SORTTYPE sort;
}EBCMPINFO;
typedef EBCMPINFO *PEBCMPINFO;

BOOL RegisterEBListViewControl (void);
void EBListViewControlCleanup (void);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _LIST_VIEW_H */
