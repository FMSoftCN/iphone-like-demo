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
	DWORD shadowcolor;   //��Ӱ�ֵ���Ӱ����ɫ
	DWORD wordfont;      //��:��Ӱ��,��ͨ��
}SIWORDDATA,*PSIWORDDATA;

typedef struct _ebsubitemdata
{
	//struct _ebsubitemdata *pNext;
	//char *pszInfo;
	struct _ebsubitemdata *pNext;
	DWORD   mask;//LV_TEXT,LV_IMAGE,LV_IMAGEFILE,LV_ICON;
	char *  pszText;//���ֻ�ͼƬ�ļ���
	int	cchTextMax;//��������󳤶ȣ� 
	DWORD iImage;//ͼ���ļ���
	DWORD dwData;//�������ݣ�
	PSIWORDDATA  worddata;  //���ֵĸ�����Ϣ
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
	BOOL bChecked;	//�����Ƿ�ѡ��
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
	int nItemHeight;//��Ŀ�ĸ߶�(�����);
	int nItemGap;//��Ŀ��ļ��;
	int nHeadHeight;

	DWORD nSelectBKColor;//ѡ�к�ı�����ɫ,�����÷������Ч��
	DWORD nSelectTextColor;//ѡ�к����ֵ���ʾ��ɫ��

	DWORD nItemBKColor;//��Ŀ�ı���ɫ,�����÷������Ч;
	DWORD nItemTextColor;//��Ŀ��������ɫ;

	DWORD nMouseOverBkColor;//��꾭��ʱ����ɫ;
	DWORD nMouseOverTextColor;//��꾭��ʱ������ɫ;

	DWORD nItemBKImage;	//��Ŀ�ı���ͼƬ;
	DWORD nBKImage;		//���ڵı���ͼƬ;
	DWORD nCheckboxImg;	//����EVLS_USECHECKBOX���Ϊcheckbox��ͼƬ����
	
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
