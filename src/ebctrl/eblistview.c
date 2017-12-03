// 
// $Id: eblistview.c,v 1.6 2006/05/25 08:21:44 gaolh Exp $
//
// listview.c: the spin box control
//
// Copyright (C) 2001, Shu Ming
// 
// Create date: 2001/12/03
/*
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Library General Public
**  License as published by the Free Software Foundation; either
**  version 2 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Library General Public License for more details.
**
**  You should have received a copy of the GNU Library General Public
**  License along with this library; if not, write to the Free
**  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
**  MA 02111-1307, USA
*/

/*
**  Alternatively, the contents of this file may be used under the terms
**  of the Mozilla Public License (the "MPL License") in which case the
**  provisions of the MPL License are applicable instead of those above.
*/

//
// Modify records:
//
//  Who             When        Where       For What                Status
//-----------------------------------------------------------------------------
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "eblistview.h"
#include "ebcontrol.h"

#define   HJ_DEBUG(x...)

/********************************************** List internals data **********************************************/
static void setHeadHeight (int height, PEBLSTVWDATA pLVInternalData);
static int sGetItemSeq (PEBLSTVWDATA pLVInternalData);
static int sAddColumnToList (int nCols, const char *pszHeadText, int w,
                             PEBLSTVWDATA pLVInternalData);
static int sModifyHead (int nCols, const char *pszHeadText,
                        PEBLSTVWDATA pLVInternalData);
static BOOL sRemoveColumnFromList (int nCols, PEBLSTVWDATA pLVInternalData);
static int sRemoveItemFromList (int nItem, PEBLSTVWDATA pLVInternalData);
static int sRemoveAllItem (PEBLSTVWDATA pLVInternalData);
static int sFillSubItemToList (PEBLVSUBITEM p0,
                               PEBLSTVWDATA pLVInternalData);
static int sAddItemToList (int nItem, PEBLSTVWDATA pLVInternalData);
static int sFindItemFromList (PEBLVFINDINFO pFindInfo,
                              PEBLSTVWDATA pLVInternalData);
static int sGetSubItemCopy (PEBLVSUBITEM pSubItem, PEBLSTVWDATA pLVInternalData);


static int itemDelete (PEBITEMDATA pHead);
static void InitListViewData (HWND hwnd);
static PEBLSTHDR lsthdrNew (const char *pszHeadText, int nCols,
                          PEBLSTVWDATA pLVInternalData);
static PEBSUBITEMDATA subitemNew (const char *pszInfoText);
static void subitemDelete (PEBSUBITEMDATA pSubItemData);
static int itemDelete (PEBITEMDATA pItem);

static void lsthdrDelete (PEBLSTHDR pLstHdr);
static PEBSUBITEMDATA sGetSubItemFromList (int nItem, int subItem,
                                         PEBLSTVWDATA pLVInternalData);
static PEBITEMDATA itemNew (int nCols,PEBLSTVWDATA pLVInternalData);
static int sCmpInfo (const char *szSrc, const char *szDest);
static PEBITEMDATA sGetItemFromList (int nSeq, PEBLSTVWDATA pLVInternalData);
static int isInListViewHead (int mouseX, int mouseY, PEBLSTHDR * pRet,
                             PEBLSTVWDATA pLVInternalData);
static int isInLVHeadBorder (int mouseX, int mouseY, PEBLSTHDR * pRet,
                             PEBLSTVWDATA pLVInternalData);
static int isInLVItem (int mouseX, int mouseY, PEBITEMDATA * pRet,
                       PEBLSTVWDATA pLVInternalData);
static int isInListViewHead (int mouseX, int mouseY, PEBLSTHDR * pRet,
                             PEBLSTVWDATA pLVInternalData);

static void sDrawTextToSubItem (HWND hwnd,HDC hdc, PEBSUBITEMDATA psubitem, int nRows,
                                int nCols, PEBITEMDATA pItem,
                                PEBLSTVWDATA pLVInternalData);
static void sDrawText (HDC hdc, int x, int y, int width, int height,
                       const char *pszText);
static int sSortItemByCol (unsigned int nCols, SORTTYPE sort,
                           PEBLSTVWDATA pLVInternalData);

static int sGetFrontSubItemsWidth (int end, PEBLSTVWDATA pLVInternalData);
static int sGetItemWidth (PEBLSTVWDATA pLVInternalData);
static int sGetSubItemWidth (int nCols, PEBLSTVWDATA pLVInternalData);
static int sGetSubItemX (int nCols, PEBLSTVWDATA pLVInternalData);
static int sAddOffsetToTailSubItem (int nCols, int offset,
                                    PEBLSTVWDATA pLVInternalData);
static int sAddOffsetToSubItem (PEBLSTHDR p, int offset);
static int setItemColor (int color, int rows, PEBLSTVWDATA plvinternaldata);


static int lstSetVScrollInfo (PEBLSTVWDATA pLVInternalData);
static int lstSetHScrollInfo (PEBLSTVWDATA pLVInternalData);

static void lvDataDestory (PEBLSTVWDATA pLvInternalData);

char * PrefixFileName(const char * path,char * prefix,char *ppfile,int maxlen)
{
	char *str1;        		  	
	if (!ppfile)
		return NULL;
	str1 = rindex(path,'/'); 
	if(str1) {
	    int rpath;
	    rpath = (int)(str1-path)+1;
		
	    if(rpath <= 0 || rpath > maxlen)
	    	return NULL;
	    	
	    strncpy(ppfile,path,rpath);
	    ppfile[rpath] = 0;
	    strncat(ppfile,prefix,maxlen);
	    strncat(ppfile,str1+1,maxlen);
	}
	else {
	    strncpy(ppfile,prefix,maxlen);
	    strncat(ppfile,path,maxlen);
	}
	
	return ppfile;
}

static void InitListViewData (HWND hwnd)
{
    RECT rcClient;
    int nHeight = 0;
    PEBLSTVWDATA pLVInternalData;
    PEBLVEXTDATA pdata1 = (PEBLVEXTDATA)GetWindowAdditionalData(hwnd);
    DWORD dwStyle = GetWindowStyle (hwnd);

    pLVInternalData = (PEBLSTVWDATA) GetWindowAdditionalData2 (hwnd);


    GetClientRect (hwnd, &rcClient);
    pLVInternalData->nCols = 0;
    pLVInternalData->nRows = 0;
    pLVInternalData->hWnd = hwnd;
    pLVInternalData->pLstHead = NULL;
    pLVInternalData->pItemHead = NULL;
    pLVInternalData->nOriginalX = 0;
    pLVInternalData->nOriginalY = 0;
    pLVInternalData->bBorderDraged = FALSE;
    pLVInternalData->bItemClicked = FALSE;
    pLVInternalData->nFirstVisableRow =1;               //add by hejian 2004.12.11

    pLVInternalData->pItemDraged = NULL;
    pLVInternalData->pItemClicked = NULL;
    pLVInternalData->pItemSelected = NULL;
    pLVInternalData->pItemMouseOver = NULL;

    pLVInternalData->oldMouseX = 0;
    pLVInternalData->oldMouseY = 0;
    pLVInternalData->nItemDraged = 0;
    pLVInternalData->nItemSelected = 0;
    pLVInternalData->nItemMouseOver = 0;

    pLVInternalData->nItemHeight = pdata1->nItemHeight;
    pLVInternalData->nItemGap = pdata1->nItemGap;

    pLVInternalData->itemdraw =NULL;

    if(dwStyle & ELVS_ITEMUSERCOLOR)
    {
        pLVInternalData->nSelectBKColor = pdata1->nSelectBKColor;
        pLVInternalData->nItemBKColor = pdata1->nItemBKColor;
        pLVInternalData->nSelectTextColor = pdata1->nSelectTextColor;
        pLVInternalData->nItemTextColor = pdata1->nItemTextColor;
        pLVInternalData->nMouseOverBkColor = pdata1->nMouseOverBkColor;
        pLVInternalData->nMouseOverTextColor = pdata1->nMouseOverTextColor;
    }
    else
    {
        pLVInternalData->nSelectBKColor = RGB2Pixel (HDC_SCREEN, 0, 0, LIGHTBLUE);
        pLVInternalData->nItemBKColor = PIXEL_lightwhite;
        pLVInternalData->nSelectTextColor =PIXEL_lightwhite;
        pLVInternalData->nItemTextColor = PIXEL_black;
        pLVInternalData->nMouseOverBkColor = PIXEL_black;
        pLVInternalData->nMouseOverTextColor = PIXEL_yellow;
    }

    pLVInternalData->nItemBKImage = pdata1->nItemBKImage;
    pLVInternalData->nBKImage = pdata1->nBKImage;
    nHeight = GetWindowFont (hwnd)->size;

    if((nHeight + pLVInternalData->nItemGap) > pLVInternalData->nItemHeight)
        pLVInternalData->nItemHeight = nHeight + pLVInternalData->nItemGap;

    if(dwStyle & ELVS_USEHEADER)
        pLVInternalData->nHeadHeight = nHeight + 2;
    else
        pLVInternalData->nHeadHeight = 0;

    //pLVInternalData->nHeadHeight = 50;
    pLVInternalData->nVisableRows =(rcClient.bottom -rcClient.top -pLVInternalData->nHeadHeight)/(pLVInternalData->nItemHeight);

    pLVInternalData->nCheckboxImg = 0;

}


static void lvDataDestory (PEBLSTVWDATA pLVInternalData)
{
	
	PEBITEMDATA p1, p2;
	p1 = pLVInternalData->pItemHead;
	lsthdrDelete (pLVInternalData->pLstHead);
	while (p1 != NULL)
	{
		  p2 = p1;
		  p1 = p1->pNext;
		  itemDelete (p2);
	}

	free (pLVInternalData);
}

static PEBLSTHDR lsthdrNew (const char *pszHeadText, int nCols, PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p1;
	int x;

	p1 = (PEBLSTHDR) malloc (sizeof (EBLSTHDR));
	p1->pNext = NULL;
	if (pszHeadText != NULL)
	{
		p1->pTitle = (char *) malloc (strlen (pszHeadText) + 1);
		strcpy (p1->pTitle, pszHeadText);
	}
	else
	p1->pTitle = NULL;
	x = sGetFrontSubItemsWidth (nCols - 1, pLVInternalData);
	p1->x = x;
	p1->width = COLWIDTHDEFAULTS;
	p1->sort = NOTSORTED;
	p1->height = pLVInternalData->nHeadHeight;

	return p1;
}


static void lsthdrDelete (PEBLSTHDR pLstHdr)
{
	if (pLstHdr != NULL)
	{
		if (pLstHdr->pTitle != NULL)
			free (pLstHdr->pTitle);
		free (pLstHdr);
	}
}


static void setHeadHeight (int height, PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p1;

	p1 = pLVInternalData->pLstHead;

	if (height < 0)
		return;

	while (p1 != NULL)
	{
		p1->height = height;
		p1 = p1->pNext;
	}
}


static PEBSUBITEMDATA subitemNew (const char *pszInfoText)
{
	PEBSUBITEMDATA p1;
	p1 = (PEBSUBITEMDATA) malloc (sizeof (EBSUBITEMDATA));
	memset(p1,0,sizeof(EBSUBITEMDATA));
	//p1->pNext = NULL;
	if (pszInfoText != NULL)
	{
		p1->cchTextMax = strlen (pszInfoText) + 1;
		p1->pszText = (char *) malloc (p1->cchTextMax);
		strcpy (p1->pszText, pszInfoText);
	}
	else
		p1->pszText = NULL;
	return p1;

}


static int setItemColor(int rows, int color, PEBLSTVWDATA pLVInternalData)
{
	PEBITEMDATA p1;
	int oldColor;

	if (!(p1 = sGetItemFromList (rows, pLVInternalData)))
		return -1;
	oldColor = p1->nTextColor;
	p1->nTextColor = color;
	return oldColor;
}

static void subitemDelete (PEBSUBITEMDATA pSubItemData)
{
    if (pSubItemData != NULL) {
        if (pSubItemData->pszText != NULL)
            free (pSubItemData->pszText);
        if (pSubItemData->worddata != NULL)
            free(pSubItemData->worddata);
        /* houhh 20080717. */
#if 0
        if(pSubItemData->iImage != 0)
        {
            free((char *)pSubItemData->iImage);
        }
#endif
        free (pSubItemData);
    }
}


static int sAddColumnToList(int nCols,const char *pszHeadText,int w,PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p1 = NULL;
	PEBLSTHDR p2 = NULL;
	PEBLSTHDR p4 = NULL;
	PEBITEMDATA p3 = NULL;
	int nPosition;
	int x = 0;
	int y = 0;
	RECT rcClient;
	int width;


	nPosition = nCols;
	width = w;
	if (pszHeadText == NULL)
		return -1;
	if ((nPosition > pLVInternalData->nCols) || (nPosition < 1))
		nPosition = pLVInternalData->nCols + 1;
	//if (width < COLWIDTHDEFAULTS)
	//  width = COLWIDTHDEFAULTS;


	if (nPosition >= (pLVInternalData->nCols + 1))
	{
		x = sGetItemWidth (pLVInternalData);
	}
	else
		x = sGetSubItemX (nPosition, pLVInternalData);
	p1 = pLVInternalData->pLstHead;

	GetClientRect (pLVInternalData->hWnd, &rcClient);
	if (width > (rcClient.right - rcClient.left))
		width = rcClient.right - rcClient.left;
	y = rcClient.top;

	p4 = lsthdrNew (pszHeadText, nCols, pLVInternalData);
	p4->x = x;
	p4->y = y;
	p4->width = width;
	p4->height = pLVInternalData->nHeadHeight;
	p4->up = TRUE;

	sAddOffsetToTailSubItem (nCols, width, pLVInternalData);

	if (nPosition == 1)
	{
		p4->pNext = p1;
		pLVInternalData->pLstHead = p4;
	}
	else
	{
		while (nPosition != 1)
		{
			p2 = p1;
			p1 = p1->pNext;
			nPosition = nPosition - 1;
		}
		p2->pNext = p4;
		p4->pNext = p1;
	}

	p3 = pLVInternalData->pItemHead;
	while (p3 != NULL)
	{		
		PEBSUBITEMDATA p5, p6, p7;
		if (nPosition == 1)
		{
			p7 = p3->pSubItemHead;
			p6 = subitemNew (NULL);
			p3->pSubItemHead = p6;
			p6->pNext = p7;
		}
		else
		{
			p5 = p3->pSubItemHead;
			while (nPosition != 1)
			{
				p6 = p5;
				p5 = p5->pNext;
				nPosition = nPosition - 1;
			}
			p7 = subitemNew (NULL);
			p6->pNext = p7;
			p7->pNext = p5;	
		}
		p3 = p3->pNext;
	}

	pLVInternalData->nCols = pLVInternalData->nCols + 1;
	return 0;
}


static int sGetItemSeq (PEBLSTVWDATA pLVInternalData)
{
	PEBITEMDATA p1;
	int i = 0;

	p1 = pLVInternalData->pItemHead;
	while (p1)
	{
		i++;
		if (p1->bSelected)
		return i;
		p1 = p1->pNext;
	}
	return -1;
}


static int sModifyHead (int nCols, const char *pszHeadText, PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p1 = NULL;
	int i;

	if ((nCols < 1) || (nCols > pLVInternalData->nCols) || (pszHeadText == NULL))
		return -1;
	p1 = pLVInternalData->pLstHead;
	i = nCols;
	while (i > 1)
	{
		i--;
		p1 = p1->pNext;
	}

	if (p1->pTitle != NULL)
		free (p1->pTitle);
	p1->pTitle = (char *) malloc (sizeof (pszHeadText) + 1);
	strcpy (p1->pTitle, pszHeadText);
	return 0;
}

static PEBSUBITEMDATA sGetSubItemFromList (int nItem, int nSubItem, PEBLSTVWDATA pLVInternalData)
{
	PEBSUBITEMDATA p1 = NULL;
	PEBSUBITEMDATA p3 = NULL;
	PEBITEMDATA p2 = NULL;
	PEBITEMDATA p4 = NULL;
	PEBSUBITEMDATA p5 = NULL;

	int i, j;

	if ((nItem < 1) || (nSubItem < 1))
		return NULL;

	i = nItem;
	j = nSubItem;

	p2 = pLVInternalData->pItemHead;
	while ((p2 != NULL) && (i > 0))
	{
		p4 = p2;
		i = i - 1;
		p2 = p2->pNext;
	}
	if (i == 0)
	{
		p1 = p4->pSubItemHead;
		while ((p1 != NULL) && (j > 0))
		{
			p3 = p1;
			j = j - 1;
			p1 = p1->pNext;
		}
		if (j == 0)
			p5 = p3;
	}

	return p5;
}

static int sFillSubItemToList (PEBLVSUBITEM p0, PEBLSTVWDATA pLVInternalData)
{
	PEBSUBITEMDATA p1;
		
	if (p0 == NULL)
		return -1;
	
	p1 = sGetSubItemFromList (p0->nItem, p0->subItem, pLVInternalData);
	if (p1 == NULL)
		return -1;
	
	p1->mask = p0->mask;

	if ((p0->mask == ELV_TEXT) || (p0->mask == ELV_BOTH))//edit by tjb 2004-4-14
	{
		if (p1->pszText != NULL)
		{
	  		free (p1->pszText);
	  		p1->pszText = (char *) malloc (p0->cchTextMax + 1);
	  		strcpy (p1->pszText, p0->pszText);
		}
		else
		{
	  		p1->pszText = (char *) malloc (p0->cchTextMax + 1);
	  		strcpy (p1->pszText, p0->pszText);
		}
	}
	else
	{
		if (p1->pszText != NULL)
			free (p1->pszText);
		p1->pszText = NULL;
	}
    
    p1->cchTextMax = p0->cchTextMax;
    /* houhh 20080717. */
#if 1
    p1->iImage = p0->iImage;
#else
    if(p0->iImage != 0) {
    	p1->iImage = (DWORD)calloc(strlen((char *)p0->iImage)+1,1);
    	strcpy((char *)p1->iImage,(char *)p0->iImage);
    }
    else {
    	p1->iImage =0;
    }
#endif
    p1->dwData = p0->lparam;
    if(p0->wordtype != NULL)
    {
    	p1->worddata = (PSIWORDDATA)calloc(1,sizeof(struct _EBLVSUBITEMWORDDATA));
    	memcpy(p1->worddata,p0->wordtype,sizeof(struct _EBLVSUBITEMWORDDATA));
    }
    else
    {
    	p1->worddata = NULL;
    }

	return 0;
}

static int sGetSubItemCopy (PEBLVSUBITEM pSubItem, PEBLSTVWDATA pLVInternalData)
{
	PEBSUBITEMDATA p1;

	if (pSubItem == NULL || pSubItem->pszText == NULL)
//	if (pSubItem == NULL)
    	return -1;
	
  	p1 =  sGetSubItemFromList (pSubItem->nItem, pSubItem->subItem, pLVInternalData);
  	if (p1 == NULL)
    	return -1;
    
	if (p1->pszText != NULL)
    {
		if(pSubItem->cchTextMax >= strlen(p1->pszText)+1)
		{
			strcpy (pSubItem->pszText, p1->pszText);
			pSubItem->pszText[strlen(p1->pszText)+1]='\0';
		}
		else
		{
			strncpy(pSubItem->pszText, p1->pszText, pSubItem->cchTextMax-1);
			pSubItem->pszText[pSubItem->cchTextMax]='\0';
		}

/*		if(pSubItem->pszText != NULL)
      	{
      		free(pSubItem->pszText);
      		pSubItem->pszText =NULL;
      	}
      	pSubItem->pszText = (char *)calloc(strlen(p1->pszText)+1,1);
      	strcpy (pSubItem->pszText, p1->pszText);
		
		pSubItem->cchTextMax =strlen(p1->pszText);
*/
    }
	else
    {
    	if(pSubItem->pszText !=NULL)
    	{
    		pSubItem->pszText[0] = 0;
    	}
    } 
      
    pSubItem->mask = p1->mask;
 
    pSubItem->iImage = p1->iImage;//图标文件；
    pSubItem->lparam = p1->dwData;//附加数据；
    pSubItem->wordtype = (PSIWORDTYPE)p1->worddata;  //文字附加信息

  return 0;
}

static BOOL sRemoveColumnFromList (int nCol, PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p1 = NULL;
	PEBLSTHDR p2 = NULL;
	PEBITEMDATA p3 = NULL;
	PEBSUBITEMDATA p5 = NULL;
	PEBSUBITEMDATA p6 = NULL;
	int i;
	int offset;


	if ((nCol > pLVInternalData->nCols) || (nCol < 1) || (pLVInternalData->nCols < 1))
		return FALSE;

	offset = -(sGetSubItemWidth (nCol, pLVInternalData));
	sAddOffsetToTailSubItem (nCol + 1, offset, pLVInternalData);
	if (nCol == 1)
	{
		p1 = pLVInternalData->pLstHead;
		pLVInternalData->pLstHead = pLVInternalData->pLstHead->pNext;
		lsthdrDelete (p1);
	}
	else
	{
		i = nCol;
		p1 = pLVInternalData->pLstHead;
		while (i != 2)
		{
			i = i - 1;
			p1 = p1->pNext;
		}
		p2 = p1->pNext;
		p1->pNext = p2->pNext;
		lsthdrDelete (p2);
	}

	p3 = pLVInternalData->pItemHead;
	while (p3 != NULL)
	{
		if (nCol == 1)
		{
			p5 = p3->pSubItemHead;
			p3->pSubItemHead = p3->pSubItemHead->pNext;
			subitemDelete (p5);
		}
		else
		{
			i = nCol;
			p5 = p3->pSubItemHead;
			while (i != 2)
			{
				i = i - 1;
				p5 = p5->pNext;
			}
			p6 = p5->pNext;
			p5->pNext = p5->pNext->pNext;
			subitemDelete (p6);
		}
		p3 = p3->pNext;
	}

	pLVInternalData->nCols = pLVInternalData->nCols - 1;
	return 0;
}

static PEBITEMDATA itemNew (int nCols,PEBLSTVWDATA pLVInternalData)
{
	PEBSUBITEMDATA pHead = NULL;
	PEBSUBITEMDATA p1 = NULL;
	PEBSUBITEMDATA p2 = NULL;
	PEBITEMDATA p3 = NULL;
	int i;
	int j;


	j = nCols;
	if (j >= 1)
	{
		pHead = subitemNew (NULL);
		p1 = pHead;
	}
	else
		return NULL;
	for (i = 1; i <= j - 1; i++)
	{
		p2 = subitemNew (NULL);
		p1->pNext = p2;
		p1 = p2;
	}
	p3 = (PEBITEMDATA) malloc (sizeof (EBITEMDATA));
	p3->nRows = -1;
	p3->pNext = NULL;
	p3->bSelected = FALSE;
	p3->bMouseOver = FALSE;

	p3->nTextColor = pLVInternalData->nItemTextColor;
	p3->pSubItemHead = pHead;
	p3->bChecked = FALSE;
	return p3;
}

static int itemDelete (PEBITEMDATA pItem)
{
	PEBSUBITEMDATA p1 = NULL;
	PEBSUBITEMDATA p2 = NULL;

	p1 = pItem->pSubItemHead;
	while (p1 != NULL)
	{
		p2 = p1;
		p1 = p1->pNext;
		subitemDelete (p2);
	}
	pItem->pSubItemHead = NULL;

	free (pItem);
	return 0;
}

static int sAddItemToList (int nItem, PEBLSTVWDATA pLVInternalData)
{
	int nPosition, i;
	PEBITEMDATA p1 = NULL;
	PEBITEMDATA p2 = NULL;
	PEBITEMDATA p3 = NULL;

	if (pLVInternalData->nRows > 1000000)
		return -1;

	nPosition = nItem;

	if ((nItem < 1) || (nItem > pLVInternalData->nRows))
		nPosition = pLVInternalData->nRows + 1;

 	p1 = itemNew(pLVInternalData->nCols,pLVInternalData);
	if (p1 == NULL)
		return -1;
	if (nPosition == 1)
	{
		p2 = pLVInternalData->pItemHead;
		pLVInternalData->pItemHead = p1;
		p1->pNext = p2;
	}
	else
	{
		i = nPosition;
		p2 = pLVInternalData->pItemHead;
		while (i != 2)
		{
			i = i - 1;
			p2 = p2->pNext;
		}
		p3 = p2->pNext;
		p2->pNext = p1;
		p1->pNext = p3;
	}

	pLVInternalData->nRows = pLVInternalData->nRows + 1;
	return 0;
}

static int sRemoveItemFromList (int nItem, PEBLSTVWDATA pLVInternalData)
{
	int nPosition;
	PEBITEMDATA p1 = NULL;
	PEBITEMDATA p2 = NULL;

	if ((nItem < 1) || (nItem > pLVInternalData->nRows) || (pLVInternalData->nRows < 1))
		return -1;
	if (nItem == pLVInternalData->nItemSelected)
	{
		pLVInternalData->pItemSelected = NULL;
		pLVInternalData->nItemSelected = 0;
	}

	if(nItem == pLVInternalData->nItemMouseOver)
	{
		pLVInternalData->pItemMouseOver = NULL;
		pLVInternalData->nItemMouseOver = 0;
	}
  	
  	nPosition = nItem;

  	if (nPosition == 1)
    {
		p1 = pLVInternalData->pItemHead;
		pLVInternalData->pItemHead = pLVInternalData->pItemHead->pNext;
		itemDelete (p1);
    }
  	else
    {
		p1 = pLVInternalData->pItemHead;
      	while (nPosition != 2)
        {
          nPosition = nPosition - 1;
          p1 = p1->pNext;
        }
		p2 = p1->pNext;
		p1->pNext = p2->pNext;
		itemDelete (p2);
    }

	pLVInternalData->nRows = pLVInternalData->nRows - 1;
	return 0;
}


static int sRemoveAllItem (PEBLSTVWDATA pLVInternalData)
{
	PEBITEMDATA p1 = NULL;
	PEBITEMDATA p2 = NULL;

	p1 = pLVInternalData->pItemHead;
	while (p1 != NULL)
    {
		p2 = p1;
		p1 = p1->pNext;
		itemDelete (p2);
    }

	pLVInternalData->nRows = 0;
	pLVInternalData->pItemHead = NULL;
	/*add by hejian 2004-9-8*/
	pLVInternalData->nOriginalX = 0;
  	pLVInternalData->nOriginalY = 0;
	
	pLVInternalData->pItemDraged = NULL;
	pLVInternalData->pItemClicked = NULL;
	pLVInternalData->pItemSelected = NULL;
	pLVInternalData->pItemMouseOver = NULL;
	  
	pLVInternalData->oldMouseX = 0;
	pLVInternalData->oldMouseY = 0;
	pLVInternalData->nItemDraged = 0;
	pLVInternalData->nItemSelected = 0;
	pLVInternalData->nItemMouseOver = 0;
	pLVInternalData->nFirstVisableRow =1;
	/*end add*/
	return 0;
}

static int sFindItemFromList (PEBLVFINDINFO pFindInfo, PEBLSTVWDATA pLVInternalData)
{
	PEBITEMDATA p1;
	PEBSUBITEMDATA p2;
	int i = 0;
	int j = 0;

	p1 = pLVInternalData->pItemHead;
	if (pFindInfo == NULL)
		return -1;

	while (p1 != NULL)
    {
		p2 = p1->pSubItemHead;
		i = pFindInfo->nCols;
		while ((p2 != NULL) && (i > 0))
        {
			if (strcmp (p2->pszText, pFindInfo->pszInfo[i - 1]) != 0)
				break;
			i--;
			p2 = p2->pNext;
        }
		j++;
		p1 = p1->pNext;

		if (i == 0)
			return j;
    }
	return -1;
}

static int sCmpInfo (const char *szSrc, const char *szDest)
{
	if (szDest == NULL)
		return 1;
	if (szSrc == NULL)
		return -1;
	return strcmp (szSrc, szDest);
}

static int sCmpStructExchange (PEBCMPITEMSTRUCT pSrc, PEBCMPITEMSTRUCT pDest)
{
	char *p1;
	int n1;

	if ((pSrc == NULL) || (pDest == NULL))
		return -1;
	n1 = pSrc->nItemSeq;
	pSrc->nItemSeq = pDest->nItemSeq;
	pDest->nItemSeq = n1;

	if (pSrc->pszText != NULL)
    {
		p1 = (char *) malloc (strlen (pSrc->pszText) + 1);
		strcpy (p1, pSrc->pszText);
		free (pSrc->pszText);
		if (pDest->pszText != NULL)
		{
			pSrc->pszText = (char *) malloc (strlen (pDest->pszText) + 1);
			strcpy (pSrc->pszText, pDest->pszText);
		}
		else
			pSrc->pszText = NULL;
    }
	else
    {
		p1 = NULL;
		if (pDest->pszText != NULL)
		{
			pSrc->pszText = (char *) malloc (strlen (pDest->pszText) + 1);
			strcpy (pSrc->pszText, pDest->pszText);
		}
		else
			pSrc->pszText = NULL;
    }
	if (pDest->pszText != NULL)
		free (pDest->pszText);
	pDest->pszText = p1;
	return 0;
}

static PEBITEMDATA sGetItemFromList (int nItemSeq, PEBLSTVWDATA pLVInternalData)
{
	PEBITEMDATA p1;
	int nPosition;
	if ((nItemSeq < 1) || (nItemSeq > pLVInternalData->nRows))
		return NULL;
	nPosition = nItemSeq;
	p1 = pLVInternalData->pItemHead;
	while (nPosition != 1)
    {
		nPosition = nPosition - 1;
		p1 = p1->pNext;
    }
	return p1;
}

static int sGetSubItemWidth (int nCols, PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p;
	int nPosition;

	nPosition = nCols;
	if ((nCols < 1) || (nCols > pLVInternalData->nCols))
    {
      return -1;
    }

	p = pLVInternalData->pLstHead;

	while (nPosition != 1)
    {
      nPosition--;
      p = p->pNext;
    }
	return p->width;
}

static int sGetSubItemX (int nCols, PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p;
	int nPosition;

	nPosition = nCols;
	if ((nCols < 1) || (nCols > pLVInternalData->nCols))
    {
		return -1;
    }

	p = pLVInternalData->pLstHead;

	while (nPosition != 1)
    {
		nPosition--;
		p = p->pNext;
    }
	return p->x;
}

static int sGetItemWidth (PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p;
	int width;

	p = pLVInternalData->pLstHead;
	width = 0;
	while (p != NULL)
    {
		width += p->width;
		p = p->pNext;
    }
	return width;
}

static int sGetFrontSubItemsWidth (int end, PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p;
	int nPosition;
	int width;

	nPosition = end;
	if ((end < 1) || (end > pLVInternalData->nCols))
		return -1;

	p = pLVInternalData->pLstHead;
	width = 0;

	while (nPosition != 1)
    {
		width += p->width;
		nPosition--;
		p = p->pNext;
    }
	return width;
}

static int sAddOffsetToSubItem (PEBLSTHDR p, int offset)
{
	p->width += offset;
	return 0;
}

static int sAddOffsetToTailSubItem (int nCols, int offset, PEBLSTVWDATA pLVInternalData)
{
	PEBLSTHDR p;
	int nPosition;

	nPosition = nCols;
	if ((nCols < 1) || (nCols > pLVInternalData->nCols))
      return -1;

	p = pLVInternalData->pLstHead;

	while (nPosition != 1)
    {
		nPosition--;
		p = p->pNext;
    }
	while (p != NULL)
    {
		p->x += offset;
		p = p->pNext;
    }
	return 0;
}

static int sSortItemByCol (unsigned int nCols, SORTTYPE sort, PEBLSTVWDATA pLVInternalData)
{
	PEBCMPITEMSTRUCT *p1;
	int j;
	int i;
	PEBITEMDATA p2 = NULL;
	PEBSUBITEMDATA p3 = NULL;
	PEBITEMDATA *p4;
	PEBITEMDATA p5;

	if ((nCols > pLVInternalData->nCols) || (nCols < 1))
		return -1;
	if (pLVInternalData->nRows < 1)
		return 0;
	p1 = (PEBCMPITEMSTRUCT *) malloc (sizeof (char *) * pLVInternalData->nRows);
	p2 = pLVInternalData->pItemHead;
	j = 0;
	while (p2 != NULL)
    {
        int nPosition;

        nPosition = nCols;
        p3 = p2->pSubItemHead;
        while (nPosition != 1)
        {
            nPosition--;
            p3 = p3->pNext;
        }

        p1[j] = (PEBCMPITEMSTRUCT) malloc (sizeof (EBCMPITEMSTRUCT));
        if (p3->pszText != NULL)
        {
            p1[j]->pszText = (char *) malloc (strlen (p3->pszText) + 1);
            strcpy (p1[j]->pszText, p3->pszText);
        }
        else
            p1[j]->pszText = NULL;
        p1[j]->nItemSeq = j + 1;
    }
	p2 = p2->pNext;
	j++;
/********************************************sort item***************************************************************/
	for (i = 0; i < pLVInternalData->nRows - 1; i++)
    {
		for (j = i + 1; j < pLVInternalData->nRows; j++)
        {
			if (sort == HISORTED)
            {
				if (sCmpInfo (p1[i]->pszText, p1[j]->pszText) < 0)
                  sCmpStructExchange (p1[i], p1[j]);
            }
			else if (sCmpInfo (p1[i]->pszText, p1[j]->pszText) > 0)
            {
				sCmpStructExchange (p1[i], p1[j]);
            }
        }
    }
/*********************************************sort item***************************************************************/
/***********************************************rearrange item list****************************************************/
	p4 = (PEBITEMDATA *) malloc (sizeof (PEBITEMDATA) * pLVInternalData->nRows);
	for (i = 0; i < pLVInternalData->nRows; i++)
    {
		p5 = sGetItemFromList (p1[i]->nItemSeq, pLVInternalData);
		p4[i] = p5;
    }
	for (i = 0; i < pLVInternalData->nRows - 1; i++)
    {
		p4[i]->pNext = p4[i + 1];
    }
	pLVInternalData->pItemHead = p4[0];
	p4[pLVInternalData->nRows - 1]->pNext = NULL;
	free (p4);

/***********************************************rearrange item list****************************************************/

	for (i = 0; i < pLVInternalData->nRows; i++)
    {
		if (p1[i]->pszText != NULL)
			free (p1[i]->pszText);
		free (p1[i]);
    }
	free (p1);
	return 0;
}

/********************************************** List internals data **********************************************/
/********************************************** List Report	    **********************************************/
static int lstSetVScrollInfo (PEBLSTVWDATA pLVInternalData)
{
	SCROLLINFO si;
	RECT rect;
	GetClientRect (pLVInternalData->hWnd, &rect);
	if ((rect.bottom - rect.top - pLVInternalData->nHeadHeight) >
	  ((pLVInternalData->nRows - 1) * pLVInternalData->nItemHeight))
    {
		SetScrollPos (pLVInternalData->hWnd, SB_VERT, 0);
		EnableScrollBar (pLVInternalData->hWnd, SB_VERT, FALSE);
		ShowScrollBar (pLVInternalData->hWnd, SB_VERT, FALSE);
		return 0;
    }
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMax = pLVInternalData->nRows * pLVInternalData->nItemHeight;
	si.nMin = 0;
	si.nPage = rect.bottom - rect.top - pLVInternalData->nHeadHeight;
	si.nPos = pLVInternalData->nOriginalY;

	SetScrollInfo (pLVInternalData->hWnd, SB_VERT, &si, TRUE);
	EnableScrollBar (pLVInternalData->hWnd, SB_VERT, TRUE);
	ShowScrollBar (pLVInternalData->hWnd, SB_VERT, TRUE);

	return 0;
}

static int lstSetHScrollInfo (PEBLSTVWDATA pLVInternalData)
{
	SCROLLINFO si;
	RECT rect;

	GetClientRect (pLVInternalData->hWnd, &rect);

	if ((rect.right - rect.left) > (sGetItemWidth (pLVInternalData)))
    {
		SetScrollPos (pLVInternalData->hWnd, SB_HORZ, 0);
		EnableScrollBar (pLVInternalData->hWnd, SB_HORZ, FALSE);
		ShowScrollBar (pLVInternalData->hWnd, SB_HORZ, FALSE);
		return 0;
    }
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMax = sGetItemWidth (pLVInternalData);
	si.nMin = 0;
	si.nPage = rect.right - rect.left;
	si.nPos = pLVInternalData->nOriginalX;
	SetScrollInfo (pLVInternalData->hWnd, SB_HORZ, &si, TRUE);
	EnableScrollBar (pLVInternalData->hWnd, SB_HORZ, TRUE);
	ShowScrollBar (pLVInternalData->hWnd, SB_HORZ, TRUE);

	return 0;
}

static int getSubItemCols(int mouseX, int mouseY,
                  PEBLSTVWDATA pLVInternalData)
{
    int nPosition = 0;
    PEBLSTHDR p1 = NULL;

    p1 = pLVInternalData->pLstHead;
    while (p1 != NULL)
    {
        nPosition++;
        if ((mouseX > p1->x - pLVInternalData->nOriginalX + 1)
                && (mouseX < (p1->x + p1->width - pLVInternalData->nOriginalX - 1))
                && mouseY > 0)
            break;
        p1 = p1->pNext;
    }

    if ((nPosition > pLVInternalData->nCols) || (nPosition == 0))
        return -1;
    else
    {
        if (p1 != NULL) return nPosition;
        return -1;
    }
}

static int isInListViewHead (int mouseX, int mouseY, PEBLSTHDR * pRet,
                  PEBLSTVWDATA pLVInternalData)
{
    int nPosition = 0;

    PEBLSTHDR p1 = NULL;


    p1 = pLVInternalData->pLstHead;
    while (p1 != NULL)
    {
        nPosition++;
        if ((mouseX > p1->x - pLVInternalData->nOriginalX + 1)
                && (mouseX < (p1->x + p1->width - pLVInternalData->nOriginalX - 1))
                && (mouseY > 0) && (mouseY < pLVInternalData->nHeadHeight))
            break;
        p1 = p1->pNext;
    }
    *pRet = NULL;

    if ((nPosition > pLVInternalData->nCols) || (nPosition == 0))
        return -1;
    else
    {
        *pRet = p1;
        if (p1 != NULL)
            return nPosition;
        else
            return -1;
    }
}

static int isInLVHeadBorder (int mouseX, int mouseY, PEBLSTHDR * pRet,
                  PEBLSTVWDATA pLVInternalData)
{
	int nPosition = 0;
	PEBLSTHDR p1 = NULL;

	p1 = pLVInternalData->pLstHead;
	while (p1 != NULL)
    {
      nPosition++;
      if ((mouseX >= (p1->x + p1->width - pLVInternalData->nOriginalX - 1))
          && (mouseX <= (p1->x + p1->width - pLVInternalData->nOriginalX + 1))
          && (mouseY >= 0) && (mouseY <= pLVInternalData->nHeadHeight))
        break;
      p1 = p1->pNext;
    }
  *pRet = NULL;

	if ((nPosition > pLVInternalData->nCols) || (nPosition == 0))
		return -1;
	else
    {
      *pRet = p1;
      if (p1 != NULL)
        return nPosition;
      else
        return -1;
    }

}

static int isInLVItem (int mouseX, int mouseY, PEBITEMDATA * pRet, PEBLSTVWDATA pLVInternalData)
{
	int ret, j;
	PEBITEMDATA p1;

  //因为列表框的行的宽度和列表框等同，所以这里不用判断mouseX是否在列表框内，因为如果鼠标的横坐标
  //不在列表框内，列表框就接收不到MOUSEMOVE的消息
	if ((mouseY < pLVInternalData->nHeadHeight))
		return -1;

	ret = (mouseY+pLVInternalData->nOriginalY-pLVInternalData->nHeadHeight)/pLVInternalData->nItemHeight;
	if((mouseY+ pLVInternalData->nOriginalY - pLVInternalData->nHeadHeight) > ((ret+1)*pLVInternalData->nItemHeight-pLVInternalData->nItemGap) && 
	 (mouseY+ pLVInternalData->nOriginalY - pLVInternalData->nHeadHeight) < (ret+1)*pLVInternalData->nItemHeight)
	{
		return -1;
	}
	ret++;
	*pRet = NULL;

	p1 = pLVInternalData->pItemHead;
	j = 0;
	while ((p1 != NULL) && (j < ret))
    {
		*pRet = p1;
		p1 = p1->pNext;
		j++;
    }
	if (ret > j) return -1;

    (*pRet)->nRows = ret;
    (*pRet)->nCols = getSubItemCols(mouseX, mouseY, pLVInternalData);

	return ret;
}

static void sDrawText (HDC hdc, int x, int y, int width, int height, const char *pszText)
{
	RECT rect;
	SIZE size;

	rect.left = x + 2;
	rect.top = y + 2;
	rect.right = x + width;
	rect.bottom = y + height;

	if (pszText != NULL)
	{
		GetTextExtent (hdc, pszText, -1, &size);

		if (width > size.cx)
		{
			DrawText (hdc, pszText, -1, &rect, DT_SINGLELINE | DT_LEFT |DT_VCENTER);
		}
		else
		{
			DrawText (hdc, pszText, -1, &rect, DT_SINGLELINE | DT_LEFT |DT_VCENTER);
		}
	}
}

static void
sDrawTextToSubItem (HWND hwnd,HDC hdc, PEBSUBITEMDATA psubitem,int nRows, int nCols,
                    PEBITEMDATA pItem, PEBLSTVWDATA pLVInternalData)
{
	int nOldBrushColor;
	int x;
	int y,dy;
	int temp1,temp3, temp2 = 0,temp4;
	int fit_char;
	int *pos_char, *dx_char;
	SIZE size;
	int i;
	RECT rect, rect1;
	DWORD dwStyle = GetWindowStyle (hwnd); 
	UINT uFormat;
		
	y =(nRows - 1) * pLVInternalData->nItemHeight - pLVInternalData->nOriginalY;
	x = sGetSubItemX (nCols, pLVInternalData) - pLVInternalData->nOriginalX;

	switch(psubitem->mask)
    {
        case ELV_BITMAP:
            {
                if(!pLVInternalData->nBKImage)
                    break;

                if(dwStyle &ELVS_TYPE3STATE)
                {
                    if (!pItem->bSelected)
                    {
                        if(!pItem->bMouseOver)
                        {
                            dy = (pLVInternalData->nItemHeight - ((PBITMAP*)(psubitem->iImage))[0]->bmHeight)/2;
                            FillBoxWithBitmap(hdc,x+2, y + pLVInternalData->nHeadHeight+dy,
                                    0,0,((PBITMAP*)(psubitem->iImage))[0]);
                        }
                        else
                        {
                            dy = (pLVInternalData->nItemHeight - ((PBITMAP*)(psubitem->iImage))[1]->bmHeight)/2;
                            FillBoxWithBitmap(hdc,x+2, y + pLVInternalData->nHeadHeight+dy,
                                    0,0,((PBITMAP*)(psubitem->iImage))[1]);	
                        }
                    }
                    else
                    {
                        dy = (pLVInternalData->nItemHeight - ((PBITMAP*)(psubitem->iImage))[2]->bmHeight)/2;
                        FillBoxWithBitmap(hdc,x+2, y + pLVInternalData->nHeadHeight+dy,0,0,
                                ((PBITMAP*)(psubitem->iImage))[2]);	
                    }
                }
                else
                {
                    if (!pItem->bSelected)
                    {
                        dy = (pLVInternalData->nItemHeight - ((PBITMAP*)(psubitem->iImage))[0]->bmHeight)/2;
                        FillBoxWithBitmap(hdc,x+2, y + pLVInternalData->nHeadHeight+dy,
                                0,0,((PBITMAP*)(psubitem->iImage))[0]);
                    }
                    else
                    {
                        dy = (pLVInternalData->nItemHeight - ((PBITMAP*)(psubitem->iImage))[1]->bmHeight)/2;
                        FillBoxWithBitmap(hdc,x+2, y + pLVInternalData->nHeadHeight+dy,0,0,
                                ((PBITMAP*)(psubitem->iImage))[1]);
                    }	
                }

            }
            return;
        case ELV_BMPFILE:
            {
                BITMAP bmp;
                char bmpfile[MAX_PATH+1];
                memset(&bmp,0,sizeof(BITMAP));
                bmpfile[0] = 0;

                if(psubitem->iImage)
                {
                    char * tmpfile,*str1;        		  	
                    tmpfile = (char *)(psubitem->iImage);

                    if (dwStyle & ELVS_TYPE2STATE)//2// 2?
                    {
                        if (!pItem->bMouseOver)
                            PrefixFileName(tmpfile, "nm", bmpfile, MAX_PATH);
                        else
                            PrefixFileName(tmpfile, "ov", bmpfile, MAX_PATH);
                    }
                    else
                    {
                        if (!pItem->bSelected)
                        {
                            if (!pItem->bMouseOver)
                                PrefixFileName(tmpfile, "nm", bmpfile, MAX_PATH);
                            else
                                PrefixFileName(tmpfile, "ov", bmpfile, MAX_PATH);
                        }
                        else
                            PrefixFileName(tmpfile, "st", bmpfile, MAX_PATH);
                    }

                    if(!LoadBitmap( HDC_SCREEN, &bmp, bmpfile))
                    {
                        //	dy = (pLVInternalData->nItemHeight - bmp.bmHeight)/2;
                        dy = (pLVInternalData->nItemHeight -pLVInternalData->nItemGap - bmp.bmHeight)/2;
                        FillBoxWithBitmap(hdc,x+2, y + pLVInternalData->nHeadHeight+dy,0,0,&bmp);
                        UnloadBitmap(&bmp);
                    }
                }
                else
                    break;
            }
            return;
        case ELV_ICON:
            {
                if(!pLVInternalData->nBKImage)
                    break;

                if(dwStyle &ELVS_TYPE3STATE)
                {	
                    if (!pItem->bSelected)
                    {
                        if(!pItem->bMouseOver)
                            DrawIcon(hdc,x+2, y + pLVInternalData->nHeadHeight+2,0,0,
                                    ((HICON*)(psubitem->iImage))[0]);
                        else
                            DrawIcon(hdc,x+2, y + pLVInternalData->nHeadHeight+2,0,0,
                                    ((HICON*)(psubitem->iImage))[1]);
                    }
                    else
                        DrawIcon(hdc,x+2, y + pLVInternalData->nHeadHeight+2,0,0,
                                ((HICON*)(psubitem->iImage))[2]);
                }
#if 0
                else if ((dwStyle & ELVS_SPECIAL2STATE) ) //特殊2态 
                {
                    if (!pItem->bSelected)
                        DrawIcon(hdc,x+2, y + pLVInternalData->nHeadHeight+2,0,0,
                                ((HICON*)(psubitem->iImage))[0]);  					
                    else
                        DrawIcon(hdc,x+2, y + pLVInternalData->nHeadHeight+2,0,0,
                                ((HICON*)(psubitem->iImage))[1]);
                }
#endif
                else
                {
                    if (!pItem->bSelected)
                        DrawIcon(hdc,x+2, y + pLVInternalData->nHeadHeight+2,0,0,
                                ((HICON*)(psubitem->iImage))[0]);  					
                    else
                        DrawIcon(hdc,x+2, y + pLVInternalData->nHeadHeight+2,0,0,
                                ((HICON*)(psubitem->iImage))[1]);
                }	
            }
            return;
        case ELV_BOTH:
            {
                BITMAP bmp;

                memset(&bmp,0,sizeof(BITMAP));
                if(psubitem->iImage)
                {     				
                    if(!LoadBitmap( HDC_SCREEN, &bmp, (char *)(psubitem->iImage)))
                    {
                        dy = (pLVInternalData->nItemHeight - bmp.bmHeight)/2;
                        FillBoxWithBitmap(hdc,x+2, y + pLVInternalData->nHeadHeight+dy,0,0,&bmp);
                        UnloadBitmap(&bmp);
                    }
                }
            }
            break;
        default:
            break;
    }
      
  /*FillBox (hdc, x, y + pLVInternalData->nHeadHeight,
           sGetSubItemWidth (nCols, pLVInternalData),
           pLVInternalData->nItemHeight-pLVInternalData->nItemGap);
  SetBrushColor (hdc, nOldBrushColor);
  */
	if (psubitem->pszText != NULL)
  	{
  		if (dwStyle & ELVS_TYPE2STATE)//2// 2态
  		{
		    if(!pItem->bMouseOver)
      	 	{
	      	 	nOldBrushColor = SetBrushColor (hdc, pLVInternalData->nItemBKColor);
		        temp2 = SetBkColor (hdc, pLVInternalData->nItemBKColor);
		        temp1 = SetTextColor (hdc, pItem->nTextColor);
      	 	}
      	 	else
      	 	{	
		         nOldBrushColor = SetBrushColor (hdc, pLVInternalData->nMouseOverBkColor);
		         temp2 = SetBkColor (hdc, pLVInternalData->nMouseOverBkColor);
		         temp1 = SetTextColor (hdc, pLVInternalData->nMouseOverTextColor);
         	}
  		}
		else if ((dwStyle & ELVS_TYPE3STATE)||(dwStyle & ELVS_SPECIAL2STATE) ) //3// 3态 or 特殊2态 
		{
			if (!pItem->bSelected)
	      	{
	      	 //if(!(dwStyle &ELVS_TYPE3STATE) || !(dwStyle &ELVS_TYPE2STATE) ||(!pItem->bMouseOver))
	      	 	if(!pItem->bMouseOver)
	      	 	{
		      	 	nOldBrushColor = SetBrushColor (hdc, pLVInternalData->nItemBKColor);
			        temp2 = SetBkColor (hdc, pLVInternalData->nItemBKColor);
			        temp1 = SetTextColor (hdc, pItem->nTextColor);
	      	 	}
	      	 	else
	      	 	{	
			         nOldBrushColor = SetBrushColor (hdc, pLVInternalData->nMouseOverBkColor);
			         temp2 = SetBkColor (hdc, pLVInternalData->nMouseOverBkColor);
			         temp1 = SetTextColor (hdc, pLVInternalData->nMouseOverTextColor);
	         	}
	      	}
	      	else
	      	{
		        temp2 = SetBkColor (hdc, pLVInternalData->nSelectBKColor);
		        nOldBrushColor = SetBrushColor (hdc, pLVInternalData->nSelectBKColor);
		        temp1 = SetTextColor (hdc, pLVInternalData->nSelectTextColor);
	      	}
		}
		else
		{
			nOldBrushColor = SetBrushColor (hdc, pLVInternalData->nItemBKColor);
	        temp2 = SetBkColor (hdc, pLVInternalData->nItemBKColor);
	        temp1 = SetTextColor (hdc, pItem->nTextColor);
		}

		//add by tjb 2004-5-26
		if (dwStyle & ELVS_MULTILINE)
			uFormat = DT_VCENTER | DT_LEFT;
		else
			uFormat = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
		
		temp3 = SetBkMode(hdc,BM_TRANSPARENT); 
		i = sGetSubItemWidth (nCols, pLVInternalData);
		GetTextExtent (hdc, psubitem->pszText, -1, &size);
		if(!(dwStyle &ELVS_MULTILINE))
		{
			rect.left = x + 2;
			rect.right = x + i - 2;
		      //rect.top = y + pLVInternalData->nHeadHeight + 2 +
		      //			(pLVInternalData->nItemHeight-pLVInternalData->nItemGap-size.cy-2)/2;
		      //搞不懂为什么会在y方向偏差4个象素，解：因为字体的问题，字体的字在其显示区域偏上偏左，导致
		      //下面留下很大空间，把temp3 = SetBkMode(hdc,BM_TRANSPARENT);注释掉，就能看出为什么在y方向上偏移了
		      //4个象素
		      	rect.top = y + pLVInternalData->nHeadHeight+(pLVInternalData->nItemHeight-pLVInternalData->nItemGap-size.cy)/2;
		      	rect.bottom =
		        	y + pLVInternalData->nHeadHeight + pLVInternalData->nItemHeight -pLVInternalData->nItemGap-(pLVInternalData->nItemHeight-pLVInternalData->nItemGap-size.cy)/2;
		}
		else
		{
			rect.left = x + 2;
		//	rect.right = x + 2 + size.cx;
			rect.right = x + i -2;
			rect.top = y + pLVInternalData->nHeadHeight + 2;
			rect.bottom =y + pLVInternalData->nItemHeight +
				 		pLVInternalData->nHeadHeight-pLVInternalData->nItemGap;
		}

       	if (i > size.cx)
        {
			//edit by tjb 2004-3-8
			//printf("___DEBUG sDrawTextToSubItem draw text2 !\n");
			//printf("__DEBUG pLVInternalData->nItemHeight=%d,pLVInternalData->nItemGap=%d.\n",pLVInternalData->nItemHeight,pLVInternalData->nItemGap);
			//printf("__DEBUG rect.left=%d,rect.right=%d,rect.top=%d,rect.bottom=%d.\n",rect.left,rect.right,rect.top,rect.bottom);
			//DrawText (hdc, psubitem->pszText, -1, &rect, DT_SINGLELINE | DT_VCENTER |DT_CENTER);
			if(psubitem->worddata == NULL)
			{
				DrawText (hdc, psubitem->pszText, -1, &rect, uFormat);
				//TextOut(hdc,rect.left,rect.top+2,psubitem->pszText);
			}
			else
			{
				if(psubitem->worddata->wordfont &SUBITEM_WORD_NORMALFONT)
				{
					TextOut(hdc,x+psubitem->worddata->leftoffset,y+pLVInternalData->nHeadHeight+psubitem->worddata->topoffset,psubitem->pszText);
				}
				else if(psubitem->worddata->wordfont &SUBITEM_WORD_SHADOWFONT)
				{
					SetBkMode(hdc,BM_TRANSPARENT); 
					printf("____________DEBUF SUBITEM_WORD_SHADOWFONT.\n");
					temp4 = SetTextColor (hdc, psubitem->worddata->shadowcolor);
					TextOut(hdc,x+psubitem->worddata->leftoffset+2,y+pLVInternalData->nHeadHeight+psubitem->worddata->topoffset+2,psubitem->pszText);
					SetTextColor (hdc, temp4);
					TextOut(hdc,x+psubitem->worddata->leftoffset,y+pLVInternalData->nHeadHeight+psubitem->worddata->topoffset,psubitem->pszText);
				}
			}
        }
      	else
        {

          	pos_char = (int *) malloc (sizeof (int) * strlen (psubitem->pszText));
			dx_char = (int *) malloc (sizeof (int) * strlen (psubitem->pszText));

			GetTextExtent (hdc, "...", -1, &size);
			GetTextExtentPoint (hdc,
			                  psubitem->pszText, strlen (psubitem->pszText), i - 4 - size.cx,
			                  &fit_char, pos_char, dx_char, &size);


			rect.left = x + 2;
			rect.right = x + 2 + size.cx;
			rect.top = y + pLVInternalData->nHeadHeight + 2;
			rect.bottom =y + pLVInternalData->nItemHeight +
				 		pLVInternalData->nHeadHeight-pLVInternalData->nItemGap;
			//edit by tjb 2004-3-8
			//printf("___DEBUG sDrawTextToSubItem draw text !\n");
          	//DrawText (hdc, psubitem->pszText, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);//DT_LEFT
          	DrawText (hdc, psubitem->pszText, -1, &rect, uFormat);
			/*if(strlen(psubitem->pszText)>0)
			{
				TextOut(hdc,rect.left,rect.top+2,psubitem->pszText);
			}*/
			//end edit

			if (!(dwStyle & ELVS_MULTILINE))//若是多行，则不用显示省略号
			{
				rect1.left = x + 2;
				rect1.right = x + i - 2;
				rect1.top = y + pLVInternalData->nHeadHeight + 2;
				rect1.bottom =y + pLVInternalData->nItemHeight + 
	            	pLVInternalData->nHeadHeight - pLVInternalData->nItemGap;;

	          	DrawText (hdc, "...", -1, &rect1, DT_SINGLELINE | DT_RIGHT);
			}
			
          	free (pos_char);
          	free (dx_char);
        }

		SetTextColor (hdc, temp1);
		SetBkColor (hdc, temp2);
		SetBrushColor (hdc, nOldBrushColor);
		SetBkMode(hdc,temp3);

    }
 
}

//画项的背景
static void lvDrawItemBK(HWND hwnd, HDC hdc, PEBITEMDATA pitem, int rows, DWORD pbitmap)
{
    RECT rcClient;
    int temp;
    DWORD dwStyle = GetWindowStyle (hwnd);
    DWORD dwExStyle = GetWindowExStyle (hwnd);
    PEBLSTVWDATA pLVInternalData = (PEBLSTVWDATA) GetWindowAdditionalData2 (hwnd);

    GetClientRect (hwnd, &rcClient);

    if(pbitmap)
    {
        if((dwStyle & ELVS_ITEMBKMASK) == ELVS_ITEMBKICON)
        {
        }
        else
        {
            if(dwStyle & ELVS_TYPE3STATE)
            {
                PBITMAP * ppbmp = (PBITMAP *)pbitmap;
                if(pitem->bSelected)
                    FillBoxWithBitmap(hdc, rcClient.left - pLVInternalData->nOriginalX,
                            (rows - 1) * pLVInternalData->nItemHeight +
                            pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY,
                            0, 0,(PBITMAP)ppbmp[2]);
                else
                {
                    if(pitem->bMouseOver)
                        FillBoxWithBitmap(hdc, rcClient.left - pLVInternalData->nOriginalX,
                                (rows - 1) * pLVInternalData->nItemHeight +
                                pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY,
                                0, 0,(PBITMAP)ppbmp[1]);
                    else
                    {
                        SetBrushColor (hdc, GetWindowBkColor(hwnd));
                        FillBox (hdc, rcClient.left - pLVInternalData->nOriginalX,
                                (rows - 1) * pLVInternalData->nItemHeight +
                                pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY,
                                rcClient.right - rcClient.left, 
                                pLVInternalData->nItemHeight-pLVInternalData->nItemGap);

                        FillBoxWithBitmap(hdc, rcClient.left - pLVInternalData->nOriginalX,
                                (rows - 1) * pLVInternalData->nItemHeight +
                                pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY,
                                0, 0,(PBITMAP)ppbmp[0]);
                    }
                }

            }
            else if(dwStyle & ELVS_TYPE2STATE)//edit by tjb 2004-3-11
            {
                PBITMAP * ppbmp = (PBITMAP *)pbitmap;

                if(pitem->bMouseOver)
                {
                    FillBoxWithBitmap(hdc, rcClient.left - pLVInternalData->nOriginalX,
                            (rows - 1) * pLVInternalData->nItemHeight +
                            pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY,
                            0, 0,(PBITMAP)ppbmp[1]);
                }
                else
                {
                    /*	temp = SetBrushColor (hdc, pLVInternalData->nItemBKColor);
                        FillBox (hdc, rcClient.left-pLVInternalData->nOriginalX,
                        (rows - 1) * pLVInternalData->nItemHeight +
                        pLVInternalData->nHeadHeight-pLVInternalData->nOriginalY,
                        rcClient.right - rcClient.left,
                        pLVInternalData->nItemHeight-pLVInternalData->nItemGap);
                        SetBrushColor (hdc, temp);*/

                    SetBrushColor (hdc, GetWindowBkColor(hwnd));
                    FillBox (hdc, rcClient.left - pLVInternalData->nOriginalX,
                            (rows - 1) * pLVInternalData->nItemHeight +
                            pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY,
                            rcClient.right - rcClient.left, 
                            pLVInternalData->nItemHeight-pLVInternalData->nItemGap);

                    FillBoxWithBitmap(hdc, rcClient.left - pLVInternalData->nOriginalX,
                            (rows - 1) * pLVInternalData->nItemHeight +
                            pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY,
                            0, 0,(PBITMAP)ppbmp[0]);
                }
            }//end edit
            else if(dwStyle & ELVS_SPECIAL2STATE)
            {
                PBITMAP * ppbmp = (PBITMAP *)pbitmap;
                if(pitem->bSelected)
                    FillBoxWithBitmap(hdc, rcClient.left - pLVInternalData->nOriginalX,
                            (rows - 1) * pLVInternalData->nItemHeight +
                            pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY,
                            0, 0,(PBITMAP)ppbmp[1]);
                else {
                    if(pitem->bMouseOver)
                        FillBoxWithBitmap(hdc, rcClient.left - pLVInternalData->nOriginalX,
                                (rows - 1) * pLVInternalData->nItemHeight +
                                pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY,
                                0, 0,(PBITMAP)ppbmp[0]);
                    else    //当列表框某一行处于正常态时只有背景色
                    {
                        temp = SetBrushColor (hdc, pLVInternalData->nItemBKColor);
                        //temp =SetBrushColor (hdc, RGBA2Pixel(hdc,147,169,213,230));
                        FillBox (hdc, rcClient.left-pLVInternalData->nOriginalX,
                                (rows - 1) * pLVInternalData->nItemHeight +
                                pLVInternalData->nHeadHeight-pLVInternalData->nOriginalY,
                                rcClient.right - rcClient.left,
                                pLVInternalData->nItemHeight-pLVInternalData->nItemGap);
                        SetBrushColor (hdc, temp);
                    }
                }
            }
            else
            {
                FillBoxWithBitmap(hdc, rcClient.left - pLVInternalData->nOriginalX,
                        (rows - 1) * pLVInternalData->nItemHeight +
                        pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY, 0, 0,(PBITMAP)pbitmap);
            }
        }
    }
    else
    { 
        //edit by tjb 2004-4-20
        //在2态时
        int ss = (dwExStyle & WS_EX_TRANSPARENT);
        if (!ss) {
            if(dwStyle & ELVS_TYPE2STATE)
            {
                if(pitem->bMouseOver)
                    temp = SetBrushColor (hdc, pLVInternalData->nMouseOverBkColor);
                else
                    temp = SetBrushColor (hdc, pLVInternalData->nItemBKColor);

                FillBox (hdc, rcClient.left-pLVInternalData->nOriginalX,
                        (rows - 1) * pLVInternalData->nItemHeight +
                        pLVInternalData->nHeadHeight-pLVInternalData->nOriginalY,
                        rcClient.right - rcClient.left,
                        pLVInternalData->nItemHeight-pLVInternalData->nItemGap);
                SetBrushColor (hdc, temp);
            }
            else//3//3态
            {
                if (pitem->bSelected)//画选中条目的背景
                    temp = SetBrushColor (hdc, pLVInternalData->nSelectBKColor);
                else//画没选中条目的背景
                {
                    if(pitem->bMouseOver)
                        temp = SetBrushColor (hdc, pLVInternalData->nMouseOverBkColor);
                    else
                        temp = SetBrushColor (hdc, pLVInternalData->nItemBKColor);
                }
                FillBox (hdc, rcClient.left-pLVInternalData->nOriginalX,
                        (rows - 1) * pLVInternalData->nItemHeight +
                        pLVInternalData->nHeadHeight-pLVInternalData->nOriginalY,
                        rcClient.right - rcClient.left,
                        pLVInternalData->nItemHeight-pLVInternalData->nItemGap);
                SetBrushColor(hdc, temp);
            }
        }
        if (pitem->bSelected)//画选中条目的背景
        {
            temp = SetBrushColor (hdc, pLVInternalData->nSelectBKColor);
            FillBox (hdc, rcClient.left-pLVInternalData->nOriginalX+2,
                    (rows - 1) * pLVInternalData->nItemHeight +
                    pLVInternalData->nHeadHeight-pLVInternalData->nOriginalY,
                    rcClient.right - rcClient.left,
                    pLVInternalData->nItemHeight-pLVInternalData->nItemGap);
            SetBrushColor(hdc, temp);
        }
    }

    //add by tjb 2004-8-4 画选择框
    if (dwStyle & ELVS_USECHECKBOX) {
        char bmpfile[MAX_PATH+1];
        BITMAP bmp;
        int off;

        if (pLVInternalData->nCheckboxImg) {
            if(pitem->bChecked)
                PrefixFileName((char *)pLVInternalData->nCheckboxImg, "ck", bmpfile, MAX_PATH);
            else
                PrefixFileName((char *)pLVInternalData->nCheckboxImg, "nm", bmpfile, MAX_PATH);

            if(!LoadBitmap( HDC_SCREEN, &bmp, bmpfile))
            {
                off = (pLVInternalData->nItemHeight -pLVInternalData->nItemGap - bmp.bmHeight)/2;

                FillBoxWithBitmap(hdc,rcClient.left-pLVInternalData->nOriginalX, 
                        (rows - 1) * pLVInternalData->nItemHeight + off +
                        pLVInternalData->nHeadHeight-pLVInternalData->nOriginalY, 0, 0, &bmp);
                UnloadBitmap(&bmp);
            }
        }
    }
}

//获得
int lvGetRowsChecked(HWND hWnd, int iRows)
{
	PEBLSTVWDATA pLVInternalData = (PEBLSTVWDATA) GetWindowAdditionalData2 (hWnd);
	PEBITEMDATA pitem;
	int nPosition= iRows;
	
	if (iRows > pLVInternalData->nRows)
		return 0;
	
	if (nPosition == 1)
		pitem = pLVInternalData->pItemHead;
  	else
    {
		pitem = pLVInternalData->pItemHead;
      	while (nPosition != 1)
        {
          nPosition = nPosition - 1;
          pitem = pitem->pNext;
        }
    }
	
	if (pitem->bChecked)
	{
		return 1;
	}
	return 0;
}

static lvFillRectWithBitmap(HDC hdc,RECT * rect,PBITMAP pbmp)
{
	int x,y;
	int xend=0,yend=0;
	if(RECTW((*rect)) < pbmp->bmWidth || RECTH((*rect)) < pbmp->bmHeight)
	{
		FillBoxWithBitmap(hdc,0,
 			0,RECTW((*rect)),RECTH((*rect)),pbmp);
 		return;
	}
	
	for(y = rect->top; y < rect->bottom;)
	{
		if(y + pbmp->bmHeight > rect->bottom)
		{
			y = rect->bottom - pbmp->bmHeight;
		}
		
		for(x = rect->left; x < rect->right; )
		{
			if(x + pbmp->bmWidth > rect->right)
			{
				x = rect->right-pbmp->bmWidth;
			}
			FillBoxWithBitmap(hdc,x,
 			y,0,0,pbmp);
 			x+=pbmp->bmWidth;
		
		}
		y += pbmp->bmHeight;
	}
}


static void lvDrawBKBitmap(HWND hwnd,HDC hdc,PBITMAP pbmp)
{
  RECT rcClient;
  DWORD dwStyle = GetWindowStyle (hwnd);
  
  PEBLSTVWDATA pLVInternalData = (PEBLSTVWDATA) GetWindowAdditionalData2 (hwnd);
  
  if(!pbmp)
  	return;
  	
  GetClientRect (hwnd, &rcClient);
  if(dwStyle &ELVS_BKALPHA)
  {
  	unsigned char r,g,b,a;
        Pixel2RGBA(HDC_SCREEN,GetWindowBkColor(hwnd),&r,&g,&b,&a);
        pbmp->bmType |= BMP_TYPE_ALPHACHANNEL;
        pbmp->bmAlpha = a;
  }
  switch(dwStyle & ELVS_BKIMAGEALIGNMASK)
  {
 	case ELVS_BKIMAGEFLAT:
 	{
 		lvFillRectWithBitmap(hdc,&rcClient,pbmp);
 		/*FillBoxWithBitmap(hdc,0,
 			0,RECTW(rcClient),RECTH(rcClient),pbmp);
 			*/
 	}
 	break;
 	case ELVS_BKIMAGECENTER:
 	{
 		int x0,y0;
 		x0 = rcClient.left + (rcClient.right-rcClient.left-pbmp->bmWidth)/2;
 		y0 = rcClient.top + (rcClient.bottom-rcClient.top-pbmp->bmHeight)/2;
 		SetBkColor (hdc, GetWindowBkColor(hwnd));
  		SetBrushColor (hdc, GetWindowBkColor(hwnd));
  		FillBox (hdc, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
           		rcClient.bottom - rcClient.top);
           	FillBoxWithBitmap(hdc, x0, y0, 0, 0,pbmp);
           				
 	}
 	break;
 	case ELVS_BKIMAGELEFTTOP:
 	{
 		SetBkColor (hdc, GetWindowBkColor(hwnd));
  		SetBrushColor (hdc, GetWindowBkColor(hwnd));
  		FillBox (hdc, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
           		rcClient.bottom - rcClient.top);
 		FillBoxWithBitmap(hdc, 0, 
 			0, 0, 0,pbmp);
 	}
 	break;
 	default:
  	break;
   }
}

static void lvDrawGlobalBK(HWND hwnd,HDC hdc)
{
  RECT rcClient;
  DWORD dwStyle = GetWindowStyle (hwnd);
  
  PEBLSTVWDATA pLVInternalData = (PEBLSTVWDATA) GetWindowAdditionalData2 (hwnd);
   
  GetClientRect (hwnd, &rcClient);
 
  switch(dwStyle & ELVS_BKMASK)
  {
  	case ELVS_BKBRUSH:
  	{
  		SetBkColor (hdc, GetWindowBkColor(hwnd));
  		SetBrushColor (hdc, GetWindowBkColor(hwnd));
  		FillBox (hdc, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
  			rcClient.bottom - rcClient.top); 
  	}
  	break;
  	case ELVS_BKBITMAP:
  	{
  		lvDrawBKBitmap(hwnd,hdc,(PBITMAP)(pLVInternalData->nBKImage));
  	}
  	break;
  	case ELVS_BKBMPFILE:
  	{
  		BITMAP bmp;
		
        memset(&bmp,0,sizeof(BITMAP));
  		if(pLVInternalData->nBKImage && 
  			(!LoadBitmap( HDC_SCREEN, &bmp, (char*)(pLVInternalData->nBKImage))))
    	{
    		lvDrawBKBitmap(hwnd,hdc,&bmp);         		
       		UnloadBitmap(&bmp);
    	}
    	else
    	{
    		SetBkColor (hdc, GetWindowBkColor(hwnd));
				SetBrushColor (hdc, GetWindowBkColor(hwnd));
				FillBox (hdc, rcClient.left, rcClient.top,rcClient.right - rcClient.left, 
					rcClient.bottom - rcClient.top);
		}
  	}
  	break;
	case ELVS_BKNONE:
		break;
  	default:
  	break;
  }
	
}

//edit by tjb 2005-6-29
static void  SetItemSelected(WORD dwStyle, int nItem, PEBLSTVWDATA pLVInternalData)
{
	int i;
	PEBITEMDATA p2 = NULL;
	
	if(pLVInternalData->nRows <=0)
		return ;
	p2= pLVInternalData->pItemHead;

	if(pLVInternalData->pItemSelected != NULL && pLVInternalData->nItemSelected !=nItem)
	{
		pLVInternalData->pItemSelected->bSelected = FALSE;
		pLVInternalData->pItemSelected = NULL;
		pLVInternalData->nItemSelected =0;
	}
	
	if(nItem == 1)
	{
		p2->bSelected = TRUE;
	}
	else if(nItem == 0)
	{
		if(pLVInternalData->pItemSelected != NULL)
		{
			pLVInternalData->pItemSelected->bSelected = FALSE;
			pLVInternalData->pItemSelected = NULL;
			pLVInternalData->nItemSelected =0;
		}
		p2 = NULL;
	}
	for(i=2;(i<=nItem && p2 != NULL);i++)
	{
		p2 = p2->pNext;
	}
	if(p2 != NULL)
	{
		if (dwStyle & ELVS_USECHECKBOX)//edit by tjb 2005-6-29
		{
			if (p2->bChecked)
				p2->bChecked = FALSE;
			else
				p2->bChecked = TRUE;
		}
		p2->bSelected = TRUE;
		pLVInternalData->pItemSelected = p2;
		pLVInternalData->nItemSelected = nItem;
		p2 = NULL;
	}
}


static void  SetItemMouseOver(int nItem, PEBLSTVWDATA pLVInternalData)
{
	int i;
	PEBITEMDATA p2 = NULL;
	
	if(nItem >pLVInternalData->nRows || pLVInternalData->nRows <=0)
		return ;
	p2= pLVInternalData->pItemHead;

	if(pLVInternalData->pItemMouseOver != NULL && pLVInternalData->nItemMouseOver !=nItem)
	{
		pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
		pLVInternalData->pItemMouseOver = NULL;
		pLVInternalData->nItemMouseOver =0;
	}
	
	if(nItem == 1)
	{
		p2->bMouseOver = TRUE;
	}
	else if(nItem == 0)
	{
		if(pLVInternalData->pItemMouseOver != NULL)
		{
			pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
			pLVInternalData->pItemMouseOver = NULL;
			pLVInternalData->nItemMouseOver =0;
		}
		p2 = NULL;
	}
	for(i=2;(i<=nItem && p2 != NULL);i++)
	{
		p2 = p2->pNext;
	}
	if(p2 != NULL)
	{
		p2->bMouseOver = TRUE;
		pLVInternalData->pItemMouseOver = p2;
		pLVInternalData->nItemMouseOver = nItem;
		p2 = NULL;
	}
}


//为了能快速处理状态改变(如鼠标移动、选中)而创建的函数
//itemnum1,itemnum2:要处理的行编号,一个是失去焦点的行,一个是获得焦点的行,当然,两者都可能为0
//创建于:2004-07-13 创建者:何剑
static void  lvStatusChangeProcess(HWND hwnd, HDC hdc, int itemnum1, int itemnum2)
{
	RECT rcClient;
	PEBLSTHDR p1 = NULL;
	PEBSUBITEMDATA p2 = NULL;
	PEBITEMDATA p3 = NULL;
	int i, j;
	int nOldBkColor, nOldBrushColor;
	int temp;
	unsigned char red,green,blue,alpha;
	BITMAP bmp[3],*pbmp[3];
	BITMAP twobmp[2],*ptwobmp[2];//add by tjb
	DWORD pbitmap = 0;
	DWORD dwStyle;

	PEBLSTVWDATA pLVInternalData;
	PLOGFONT pfont = GetWindowFont(hwnd);

	GetClientRect (hwnd, &rcClient);
	pLVInternalData = (PEBLSTVWDATA) GetWindowAdditionalData2 (hwnd);
	dwStyle = GetWindowStyle(hwnd);

	//画全局背景;
	//lvDrawGlobalBK(hwnd,hdc);
	p3 = pLVInternalData->pItemHead;
	
	//装载条目背景位图;
	memset(&bmp,0,sizeof(BITMAP)*3);
	memset(&pbmp,0,sizeof(BITMAP*)*3);
	Pixel2RGBA(HDC_SCREEN,GetWindowBkColor(hwnd),&red,&green,&blue,&alpha);

	switch(dwStyle & ELVS_ITEMBKMASK)
	{
  		case ELVS_ITEMBKICON:
  		{
  			pbitmap = pLVInternalData->nItemBKImage;
  		}
  		break;
    		case ELVS_ITEMBKBITMAP:
  		{
  			pbitmap = pLVInternalData->nItemBKImage;
  		}
  		break;
  		case ELVS_ITEMBKBMPFILE:
  		{
  			if(dwStyle & ELVS_TYPE3STATE)
  			{
  				char * tmpfile,*str1;  
  				char bmpfile1[MAX_PATH+1],bmpfile2[MAX_PATH+1],bmpfile3[MAX_PATH+1];      		  	

				tmpfile = (char *)(pLVInternalData->nItemBKImage);        	
        		str1 = rindex(tmpfile,'/'); 
	
        		if(str1)
        		{
        			int rpath;
        			rpath = (int)(str1-tmpfile)+1;
        			strncpy(bmpfile1,tmpfile,MAX_PATH);
        			bmpfile1[rpath] = 0;
        			strncpy(bmpfile2,bmpfile1,MAX_PATH);
        			strncpy(bmpfile3,bmpfile1,MAX_PATH);
        		
        			strncat(bmpfile1,"nm",MAX_PATH);
        			strncat(bmpfile2,"ov",MAX_PATH);
        			strncat(bmpfile3,"st",MAX_PATH);
          			
          			strncat(bmpfile1,str1+1,MAX_PATH);
          			strncat(bmpfile2,str1+1,MAX_PATH);
          			strncat(bmpfile3,str1+1,MAX_PATH);
          		}
          		else
          		{
          			strncpy(bmpfile1,"nm",MAX_PATH);
          			strncpy(bmpfile2,"ov",MAX_PATH);
          			strncpy(bmpfile3,"st",MAX_PATH);
          		
          			strncat(bmpfile1,tmpfile,MAX_PATH);
          			strncat(bmpfile2,tmpfile,MAX_PATH);
          			strncat(bmpfile3,tmpfile,MAX_PATH);
          		}
            		
	  			if(!LoadBitmap( HDC_SCREEN, &bmp[0], bmpfile1))
	        	{
	        		alpha =(unsigned char)((pLVInternalData->nItemBKColor >>24) &0xff);
		        	bmp[0].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
		        	bmp[0].bmAlpha = alpha;
					pbmp[0] = &bmp[0];
	  	    	}
  	        	if(!LoadBitmap( HDC_SCREEN, &bmp[1], bmpfile2))
         		{
         			alpha =(unsigned char)((pLVInternalData->nMouseOverBkColor >>24) &0xff);
	        		bmp[1].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
	        		bmp[1].bmAlpha = alpha;
					pbmp[1] = &bmp[1];
  	        	}
  	        	if(!LoadBitmap( HDC_SCREEN, &bmp[2], bmpfile3))
           		{
           			alpha =(unsigned char)((pLVInternalData->nSelectBKColor >>24) &0xff);
	        		bmp[2].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
	        		bmp[2].bmAlpha = alpha;
					pbmp[2] = &bmp[2];
  	        	}
  	        	pbitmap = (DWORD)&pbmp;
  			
  			}
  			else if (dwStyle & ELVS_TYPE2STATE)//??
  			{
//edit by tjb 2004-3-11
//修改原因:若不是ELVS_TYPE3STATE,则没有获得焦点一态
				char * twotmpfile,*twostr1;  
	  			char twobmpfile1[MAX_PATH+1],twobmpfile2[MAX_PATH+1];
				
	    		twotmpfile = (char *)(pLVInternalData->nItemBKImage);
				PrefixFileName(twotmpfile, "nm", twobmpfile1, MAX_PATH);
				PrefixFileName(twotmpfile, "ov", twobmpfile2, MAX_PATH);
	  			if(!LoadBitmap( HDC_SCREEN, &twobmp[0], twobmpfile1))
	        	{
        			alpha =(unsigned char)((pLVInternalData->nItemBKColor >>24) &0xff);
        			twobmp[0].bmType |= BMP_TYPE_ALPHAVALUE;   //BMP_TYPE_ALPHACHANNEL;
        			twobmp[0].bmAlpha = alpha;
					ptwobmp[0] = &twobmp[0];
	  	   		}
	  	        if(!LoadBitmap( HDC_SCREEN, &twobmp[1], twobmpfile2))
	         	{
	         		alpha =(unsigned char)((pLVInternalData->nMouseOverBkColor >>24) &0xff);
        			twobmp[1].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
        			twobmp[1].bmAlpha = alpha;
					ptwobmp[1] = &twobmp[1];
	  	        }
	
		  	    pbitmap = (DWORD)&ptwobmp;
  			}
	  		else if(dwStyle & ELVS_SPECIAL2STATE)//????
	  		{
	  			char * twotmpfile,*twostr1;  
	  			char twobmpfile1[MAX_PATH+1],twobmpfile2[MAX_PATH+1];
				
        		twotmpfile = (char *)(pLVInternalData->nItemBKImage);        	
        		twostr1 = rindex(twotmpfile,'/');
				
        		if(twostr1)
        		{
        			int rpath;
        			rpath = (int)(twostr1-twotmpfile)+1;
        			strncpy(twobmpfile1,twotmpfile,MAX_PATH);
        			twobmpfile1[rpath] = 0;
        			strncpy(twobmpfile2,twobmpfile1,MAX_PATH);
        		
        			strncat(twobmpfile1,"ov",MAX_PATH);
        			strncat(twobmpfile2,"st",MAX_PATH);
          			
          			strncat(twobmpfile1,twostr1+1,MAX_PATH);
          			strncat(twobmpfile2,twostr1+1,MAX_PATH);
          		}
          		else
          		{
          			strncpy(twobmpfile1,"ov",MAX_PATH);
          			strncpy(twobmpfile2,"st",MAX_PATH);
          		
          			strncat(twobmpfile1,twotmpfile,MAX_PATH);
          			strncat(twobmpfile2,twotmpfile,MAX_PATH);
          		}
	            		
		  		if(!LoadBitmap( HDC_SCREEN, &twobmp[0], twobmpfile1))
	        	{
	        		alpha =(unsigned char)((pLVInternalData->nMouseOverBkColor >>24) &0xff);
	        		twobmp[0].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
	        		twobmp[0].bmAlpha = alpha;
					ptwobmp[0] = &twobmp[0];
		  	    }
	  	        if(!LoadBitmap( HDC_SCREEN, &twobmp[1], twobmpfile2))
	         	{
	         		alpha =(unsigned char)((pLVInternalData->nSelectBKColor >>24) &0xff);
	        		twobmp[1].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
	        		twobmp[1].bmAlpha = alpha;
					ptwobmp[1] = &twobmp[1];
	  	        }

	  	        pbitmap = (DWORD)&ptwobmp;
	  		}
			else
			{
				if(pLVInternalData->nItemBKImage && 
				  (!LoadBitmap( HDC_SCREEN, &bmp[0], (char*)(pLVInternalData->nItemBKImage))))
	    		{
	     			pbitmap = (DWORD)&bmp[0];
	     			pbmp[0] = &bmp[0];
	    		}
			}
	//end edit
	  	}
  		break;
  		default:
  		break;
  	}

//edit by tjb  
	if(pbitmap && (!(dwStyle & ELVS_TYPE3STATE)) && (!(dwStyle & ELVS_TYPE2STATE)) &&
		(!(dwStyle & ELVS_SPECIAL2STATE)) && ((dwStyle & ELVS_ITEMBKMASK) != ELVS_ITEMBKICON) &&
		(dwStyle &ELVS_BKALPHA))
	{
		unsigned char r,g,b,a;
		
	    Pixel2RGBA(HDC_SCREEN,GetWindowBkColor(hwnd),&r,&g,&b,&a);
	    ((PBITMAP)pbitmap)->bmType |= BMP_TYPE_ALPHACHANNEL;
	    ((PBITMAP)pbitmap)->bmAlpha = a;
	}
  
  
	if (pfont)
		SelectFont (hdc, pfont);
     
	if(p3)
    { 	
		for (j = 1; j <= pLVInternalData->nRows; j++)
		{
			if(j !=itemnum1 && j !=itemnum2)
			{
				p3 = p3->pNext;
				continue;
			}
            if(pLVInternalData->itemdraw){
                static ITEM_DRAW_CON  drawcontext;
                RECT rcClient;

                GetClientRect (hwnd, &rcClient);
                drawcontext.paint_area.left =rcClient.left - pLVInternalData->nOriginalX;
                drawcontext.paint_area.top = (j - 1) * pLVInternalData->nItemHeight +
                    pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY;
                drawcontext.paint_area.right = rcClient.right; 
                drawcontext.paint_area.bottom = drawcontext.paint_area.top + pLVInternalData->nItemHeight;
                drawcontext.row =j;
                drawcontext.total_rows =pLVInternalData->nRows;
                drawcontext.state =1;
                if(j == pLVInternalData->nItemMouseOver && 
                        j !=pLVInternalData->nItemSelected){
                    drawcontext.state =2;
                }
                else if(j == pLVInternalData->nItemSelected){
                    drawcontext.state =3;
                }

                pLVInternalData->itemdraw(hwnd,hdc,(void *)&drawcontext);
            }
            else{
                p1 = pLVInternalData->pLstHead;
                p2 = p3->pSubItemHead;
                lvDrawItemBK(hwnd,hdc,p3,j,pbitmap);
                //输出内容;
                for (i = 1; i <= pLVInternalData->nCols; i++)
                {
                    sDrawTextToSubItem (hwnd,hdc, p2, j, i, p3, pLVInternalData);
                    p1 = p1->pNext;
                    p2 = p2->pNext;
                }
                p3 = p3->pNext;
            }
		}
    }

    if((dwStyle & ELVS_ITEMBKMASK) == ELVS_ITEMBKBMPFILE && pbitmap)
    {
    	if(dwStyle & ELVS_TYPE3STATE)
    	{
    		if(pbmp[0])
    			UnloadBitmap(pbmp[0]);
    		if(pbmp[1])
    			UnloadBitmap(pbmp[1]);
    		if(pbmp[2])
    			UnloadBitmap(pbmp[2]);
    	}
//edit by tjb		
    	else if ((dwStyle & ELVS_TYPE2STATE) || (dwStyle & ELVS_SPECIAL2STATE))
		{
			if (ptwobmp[0])
				UnloadBitmap(ptwobmp[0]);
			if (ptwobmp[1])
				UnloadBitmap(ptwobmp[1]);   		
    	}
//end edit
	else
		UnloadBitmap((PBITMAP)pbitmap);
    }
    /******************************draw listview header*******************************************/
    
    if(pLVInternalData->nHeadHeight > 0)
    {
#if 0
	    p1 = pLVInternalData->pLstHead;
	    SetBkColor (hdc, PIXEL_lightgray);
	    SetBrushColor (hdc, PIXEL_lightgray);
	    FillBox (hdc, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
	           pLVInternalData->nHeadHeight);
	
	    for (i = 1; i <= pLVInternalData->nCols; i++) {
	
#ifdef _FLAT_WINDOW_STYLE
	        DrawFlatControlFrameEx (hdc, p1->x - pLVInternalData->nOriginalX - 1,
	                          p1->y - pLVInternalData->nOriginalY - 1,
	                          p1->x - pLVInternalData->nOriginalX + p1->width - 1,
	                          p1->y + p1->height, PIXEL_lightgray, 0, p1->up);
#else
	        Draw3DControlFrame (hdc, p1->x - pLVInternalData->nOriginalX + 1,
	                          p1->y - pLVInternalData->nOriginalY,
	                          p1->x - pLVInternalData->nOriginalX + p1->width - 1,
	                          p1->y + p1->height, PIXEL_lightgray, p1->up);
#endif
	
	        sDrawText (hdc, p1->x - pLVInternalData->nOriginalX + 1, p1->y,
	                    p1->width - 1, p1->height, p1->pTitle);
	        p1 = p1->pNext;
	    }
#endif
    }
}

static void lvOnDraw (HWND hwnd, HDC hdc)
{
	RECT rcClient;
	PEBLSTHDR p1 = NULL;
	PEBSUBITEMDATA p2 = NULL;
	PEBITEMDATA p3 = NULL;
	int i, j;
	int nOldBkColor, nOldBrushColor;
	int temp;
	BITMAP bmp[3],*pbmp[3];
	BITMAP twobmp[2],*ptwobmp[2];//add by tjb
	DWORD pbitmap = 0;
	DWORD dwStyle;
	unsigned char red,green,blue,alpha;
	
	PEBLSTVWDATA pLVInternalData;
	PLOGFONT pfont = GetWindowFont(hwnd);
	
	GetClientRect (hwnd, &rcClient);
	pLVInternalData = (PEBLSTVWDATA) GetWindowAdditionalData2 (hwnd);
	dwStyle = GetWindowStyle(hwnd);

	//画全局背景;
	lvDrawGlobalBK(hwnd,hdc);
	p3 = pLVInternalData->pItemHead;
	
	//装载条目的背景位图;
	memset(&bmp,0,sizeof(BITMAP)*3);
	memset(&pbmp,0,sizeof(BITMAP*)*3);
	Pixel2RGBA(HDC_SCREEN,GetWindowBkColor(hwnd),&red,&green,&blue,&alpha);
	
	switch(dwStyle & ELVS_ITEMBKMASK)
	{
  		case ELVS_ITEMBKICON:
  		{
  			pbitmap = pLVInternalData->nItemBKImage;
  		}
  		break;
    	case ELVS_ITEMBKBITMAP:
  		{
  			pbitmap = pLVInternalData->nItemBKImage;
  		}
  		break;
  		case ELVS_ITEMBKBMPFILE:
  		{
  			if(dwStyle & ELVS_TYPE3STATE)
  			{
  				char * tmpfile,*str1;  
  				char bmpfile1[MAX_PATH+1],bmpfile2[MAX_PATH+1],bmpfile3[MAX_PATH+1];      		  	

				tmpfile = (char *)(pLVInternalData->nItemBKImage);        	
        		str1 = rindex(tmpfile,'/'); 
	
        		if(str1)
        		{
        			int rpath;
        			rpath = (int)(str1-tmpfile)+1;
        			strncpy(bmpfile1,tmpfile,MAX_PATH);
        			bmpfile1[rpath] = 0;
        			strncpy(bmpfile2,bmpfile1,MAX_PATH);
        			strncpy(bmpfile3,bmpfile1,MAX_PATH);
        		
        			strncat(bmpfile1,"nm",MAX_PATH);
        			strncat(bmpfile2,"ov",MAX_PATH);
        			strncat(bmpfile3,"st",MAX_PATH);
          			
          			strncat(bmpfile1,str1+1,MAX_PATH);
          			strncat(bmpfile2,str1+1,MAX_PATH);
          			strncat(bmpfile3,str1+1,MAX_PATH);
          		}
          		else
          		{
          			strncpy(bmpfile1,"nm",MAX_PATH);
          			strncpy(bmpfile2,"ov",MAX_PATH);
          			strncpy(bmpfile3,"st",MAX_PATH);
          		
          			strncat(bmpfile1,tmpfile,MAX_PATH);
          			strncat(bmpfile2,tmpfile,MAX_PATH);
          			strncat(bmpfile3,tmpfile,MAX_PATH);
          		}
            		
	  			if(!LoadBitmap( HDC_SCREEN, &bmp[0], bmpfile1))
	        	{
	        		alpha =(unsigned char)((pLVInternalData->nItemBKColor >>24) &0xff);
	        		bmp[0].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
	        		bmp[0].bmAlpha = alpha;
				pbmp[0] = &bmp[0];
	  	    	}
  	        	if(!LoadBitmap( HDC_SCREEN, &bmp[1], bmpfile2))
         		{
         			alpha =(unsigned char)((pLVInternalData->nMouseOverBkColor >>24) &0xff);
	        		bmp[1].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
	        		bmp[1].bmAlpha = alpha;
				pbmp[1] = &bmp[1];
  	        	}
  	        	if(!LoadBitmap( HDC_SCREEN, &bmp[2], bmpfile3))
           		{
           			alpha =(unsigned char)((pLVInternalData->nSelectBKColor >>24) &0xff);
	        		bmp[2].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
	        		bmp[2].bmAlpha = alpha;
				pbmp[2] = &bmp[2];
  	        	}
  	        	pbitmap = (DWORD)&pbmp;
  			
  			}
  			else if (dwStyle & ELVS_TYPE2STATE)//两态
  			{
//edit by tjb 2004-3-11
//修改原因:若不是ELVS_TYPE3STATE,则没有获得焦点一态
				char * twotmpfile,*twostr1;  
	  			char twobmpfile1[MAX_PATH+1],twobmpfile2[MAX_PATH+1];
				
	    		twotmpfile = (char *)(pLVInternalData->nItemBKImage);
				PrefixFileName(twotmpfile, "nm", twobmpfile1, MAX_PATH);
				PrefixFileName(twotmpfile, "ov", twobmpfile2, MAX_PATH);
            		
	  			if(!LoadBitmap( HDC_SCREEN, &twobmp[0], twobmpfile1))
	        		{
	        			alpha =(unsigned char)((pLVInternalData->nItemBKColor >>24) &0xff);
	        			twobmp[0].bmType |= BMP_TYPE_ALPHAVALUE;   //BMP_TYPE_ALPHACHANNEL;
	        			twobmp[0].bmAlpha = alpha;
					ptwobmp[0] = &twobmp[0];
	  	   		}
	  	        if(!LoadBitmap( HDC_SCREEN, &twobmp[1], twobmpfile2))
	         	{
	         		alpha =(unsigned char)((pLVInternalData->nMouseOverBkColor >>24) &0xff);
	        		twobmp[1].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
	        		twobmp[1].bmAlpha = alpha;
				ptwobmp[1] = &twobmp[1];
	  	        }

	  	        pbitmap = (DWORD)&ptwobmp;
  			}
	  		else if(dwStyle & ELVS_SPECIAL2STATE)//特殊两态
	  		{
	  			char * twotmpfile,*twostr1;  
	  			char twobmpfile1[MAX_PATH+1],twobmpfile2[MAX_PATH+1];
				
	        		twotmpfile = (char *)(pLVInternalData->nItemBKImage);        	
	        		twostr1 = rindex(twotmpfile,'/');
					
	        		if(twostr1)
	        		{
	        			int rpath;
	        			rpath = (int)(twostr1-twotmpfile)+1;
	        			strncpy(twobmpfile1,twotmpfile,MAX_PATH);
	        			twobmpfile1[rpath] = 0;
	        			strncpy(twobmpfile2,twobmpfile1,MAX_PATH);
	        		
	        			strncat(twobmpfile1,"ov",MAX_PATH);
	        			strncat(twobmpfile2,"st",MAX_PATH);
	          			
	          			strncat(twobmpfile1,twostr1+1,MAX_PATH);
	          			strncat(twobmpfile2,twostr1+1,MAX_PATH);
	          		}
	          		else
	          		{
	          			strncpy(twobmpfile1,"ov",MAX_PATH);
	          			strncpy(twobmpfile2,"st",MAX_PATH);
	          		
	          			strncat(twobmpfile1,twotmpfile,MAX_PATH);
	          			strncat(twobmpfile2,twotmpfile,MAX_PATH);
	          		}
	            		
		  		if(!LoadBitmap( HDC_SCREEN, &twobmp[0], twobmpfile1))
		        	{
		        		alpha =(unsigned char)((pLVInternalData->nMouseOverBkColor >>24) &0xff);
		        		twobmp[0].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
		        		twobmp[0].bmAlpha = alpha;
					ptwobmp[0] = &twobmp[0];
		  	    	}
	  	        if(!LoadBitmap( HDC_SCREEN, &twobmp[1], twobmpfile2))
	         	{
	         		alpha =(unsigned char)((pLVInternalData->nSelectBKColor >>24) &0xff);
	        		twobmp[1].bmType |= BMP_TYPE_ALPHAVALUE;    //BMP_TYPE_ALPHACHANNEL;
	        		twobmp[1].bmAlpha = alpha;
					ptwobmp[1] = &twobmp[1];
	  	        }

	  	        pbitmap = (DWORD)&ptwobmp;
	  		}
			else
			{
				if(pLVInternalData->nItemBKImage && 
					(!LoadBitmap( HDC_SCREEN, &bmp[0], (char*)(pLVInternalData->nItemBKImage))))
	    		{
	     			pbitmap = (DWORD)&bmp[0];
	     			pbmp[0] = &bmp[0];
	    		}
			}
	//end edit
	  	}
  		break;
  		default:
  		break;
  	}

//edit by tjb  
	if(pbitmap && (!(dwStyle & ELVS_TYPE3STATE)) && (!(dwStyle & ELVS_TYPE2STATE)) &&
		(!(dwStyle & ELVS_SPECIAL2STATE)) && ((dwStyle & ELVS_ITEMBKMASK) != ELVS_ITEMBKICON) &&
		(dwStyle &ELVS_BKALPHA))
	{
		unsigned char r,g,b,a;
		
	    Pixel2RGBA(HDC_SCREEN,GetWindowBkColor(hwnd),&r,&g,&b,&a);
	    ((PBITMAP)pbitmap)->bmType |= BMP_TYPE_ALPHACHANNEL;
	    ((PBITMAP)pbitmap)->bmAlpha = a;
	}

	if (pfont)
		SelectFont (hdc, pfont);
     
	if(p3)
    { 	
		for (j = 1; j <= pLVInternalData->nRows; j++)
		{
            if(pLVInternalData->itemdraw){
                static ITEM_DRAW_CON  drawcontext;
                RECT rcClient;

                GetClientRect (hwnd, &rcClient);
                drawcontext.paint_area.left =rcClient.left - pLVInternalData->nOriginalX;
                drawcontext.paint_area.top = (j - 1) * pLVInternalData->nItemHeight +
                    pLVInternalData->nHeadHeight - pLVInternalData->nOriginalY;
                drawcontext.paint_area.right = rcClient.right; 
                drawcontext.paint_area.bottom = drawcontext.paint_area.top + pLVInternalData->nItemHeight;
                drawcontext.row =j;
                drawcontext.total_rows =pLVInternalData->nRows;
                drawcontext.state =1;
                if(j == pLVInternalData->nItemMouseOver && 
                        j !=pLVInternalData->nItemSelected){
                    drawcontext.state =2;
                }
                else if(j == pLVInternalData->nItemSelected){
                    drawcontext.state =3;
                }

                pLVInternalData->itemdraw(hwnd,hdc,(void *)&drawcontext);
            }
            else{
                p1 = pLVInternalData->pLstHead;
                p2 = p3->pSubItemHead;
                lvDrawItemBK(hwnd, hdc, p3, j, pbitmap);
                //输出内容;
                for (i = 1; i <= pLVInternalData->nCols; i++)
                {
                    sDrawTextToSubItem (hwnd, hdc, p2, j, i, p3, pLVInternalData);
                    p1 = p1->pNext;
                    p2 = p2->pNext;
                }
                p3 = p3->pNext;
            }
		}
    }

    if((dwStyle & ELVS_ITEMBKMASK) == ELVS_ITEMBKBMPFILE && pbitmap)
    {
        if(dwStyle & ELVS_TYPE3STATE)
        {
            if(pbmp[0])
                UnloadBitmap(pbmp[0]);
            if(pbmp[1])
                UnloadBitmap(pbmp[1]);
            if(pbmp[2])
                UnloadBitmap(pbmp[2]);
        }
        //edit by tjb		
        else if ((dwStyle & ELVS_TYPE2STATE) || (dwStyle & ELVS_SPECIAL2STATE))
        {
            if (ptwobmp[0])
                UnloadBitmap(ptwobmp[0]);
            if (ptwobmp[1])
                UnloadBitmap(ptwobmp[1]);   		
        }
        //end edit
        else
            UnloadBitmap((PBITMAP)pbitmap);
    }

    /******************************draw listview header*******************************************/
    if(pLVInternalData->nHeadHeight > 0)
    {
#if 0
	    p1 = pLVInternalData->pLstHead;
	    SetBkColor (hdc, PIXEL_lightgray);
	    SetBrushColor (hdc, PIXEL_lightgray);
	    FillBox (hdc, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
	           pLVInternalData->nHeadHeight);
	
	    for (i = 1; i <= pLVInternalData->nCols; i++) {
	
#ifdef _FLAT_WINDOW_STYLE
	        DrawFlatControlFrameEx (hdc, p1->x - pLVInternalData->nOriginalX - 1,
	                          p1->y - pLVInternalData->nOriginalY - 1,
	                          p1->x - pLVInternalData->nOriginalX + p1->width - 1,
	                          p1->y + p1->height, PIXEL_lightgray, 0, p1->up);
#else
	        Draw3DControlFrame (hdc, p1->x - pLVInternalData->nOriginalX + 1,
	                          p1->y - pLVInternalData->nOriginalY,
	                          p1->x - pLVInternalData->nOriginalX + p1->width - 1,
	                          p1->y + p1->height, PIXEL_lightgray, p1->up);
#endif
	
	        sDrawText (hdc, p1->x - pLVInternalData->nOriginalX + 1, p1->y,
	                    p1->width - 1, p1->height, p1->pTitle);
	        p1 = p1->pNext;
	    }
#endif
    }

}

/**********************************************************************
*功能:在多页时计算当前焦点行的位置
*返回:百分比
***********************************************************************/
int lvCountPos(HWND hWnd)
{
	int pos;
	PEBLSTVWDATA pLVInternalData = (PEBLSTVWDATA) GetWindowAdditionalData2 (hWnd);
//edit by tjb 2005-6-28 当方向键移动获得焦点行时，向父窗口发送其位置
#if 0
    RECT rect;
    int scrollBoundMax;
    int scrollBoundMin;

    GetClientRect (hWnd, &rect);
    scrollBoundMin = 0;
    scrollBoundMax = pLVInternalData->nRows * pLVInternalData->nItemHeight -
        (rect.bottom - rect.top - pLVInternalData->nHeadHeight);
    if(scrollBoundMax <=0)
        pos =0;
    pos = 100*pLVInternalData->nOriginalY/(scrollBoundMax - scrollBoundMin);
#else
    if (pLVInternalData->nRows > 1 && pLVInternalData->nItemMouseOver > 1)
        pos= 100 * (pLVInternalData->nItemMouseOver-1) / (pLVInternalData->nRows-1);
    else 
        pos = 0;
#endif
	return pos;
}

/**********************************************************************
*功能:通知父窗口显示改变了，根据pos来同步滚动条
***********************************************************************/
void lvNotifyParentPos(HWND hWnd)
{
	int id = GetDlgCtrlID(hWnd);
	int pos= lvCountPos(hWnd);	
	SendNotifyMessage(GetParent(hWnd),MSG_COMMAND,(WPARAM)MAKELONG(id,ELVN_VSCROLL),(LPARAM)pos);
}

/********************************************** List Report	    **********************************************/
static int sListViewProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	PEBLSTVWDATA pLVInternalData;
  	DWORD dwStyle = GetWindowStyle (hwnd);
  	int id = GetDlgCtrlID (hwnd); 
   
  	pLVInternalData = (PEBLSTVWDATA) GetWindowAdditionalData2 (hwnd);
  
  	switch (message)
    {
    case MSG_CREATE:
       	if (!(pLVInternalData = (PEBLSTVWDATA) malloc (sizeof (EBLSTVWDATA))))
        	return -1;

      	SetWindowAdditionalData2 (hwnd, (DWORD) pLVInternalData);

      	InitListViewData (hwnd);
      	if(dwStyle & WS_HSCROLL)
      		lstSetHScrollInfo (pLVInternalData);
      
      	if(dwStyle & WS_HSCROLL)
      		lstSetVScrollInfo (pLVInternalData);
	  	break;
	
    case MSG_SETCURSOR:
    {
        PEBLSTHDR p1;
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);
    
        if ((dwStyle & ELVS_USEHEADER)& ((pLVInternalData->bBorderDraged)
            || (isInLVHeadBorder (mouseX, mouseY, &p1, pLVInternalData) > 0)))
       	{
            SetCursor (GetSystemCursor (IDC_SPLIT_VERT));
            return 0;
        }
	}
      break;
	//add by tjb 2005-4-15 当获得焦点时
	case MSG_SETFOCUS:
		SetFocusChild (hwnd);
		break; 
	//end add	
    case MSG_GETDLGCODE:
        return DLGC_WANTARROWS;
        break;
	
    case MSG_ERASEBKGND:
    	return 0;
    	break;
		
    case MSG_PAINT:
	{
        HDC hdc;
        hdc = BeginPaint (hwnd);

        lvOnDraw (hwnd, hdc);
        EndPaint (hwnd, hdc);			
      	break;
	}
	
    case ELVM_COLSORT:
    {
        PEBCMPINFO pInfo;
  
        pInfo = (PEBCMPINFO) lParam;
        sSortItemByCol (pInfo->nCols, pInfo->sort, pLVInternalData);
        pLVInternalData->nItemSelected = sGetItemSeq (pLVInternalData);

        InvalidateRect (hwnd, NULL, FALSE);     
      	break;
	}
    case ELVM_SET_ITEMDRAWCALLBACK:
    {
        pLVInternalData->itemdraw =(ITEM_DRAW)wParam;

        break;
    }  

//edit by tjb 2004-3-11
//原因:当鼠标移动到控件外后，仍有行获得焦点，应清掉所以焦点	
	case MSG_NCMOUSEMOVE:
	{
		RECT rcClient, rect;
		
		if(wParam == HT_OUT)			
		{		
			if (pLVInternalData->pItemMouseOver != NULL)
			{
				//edit by tjb 2005-7-4 当鼠标从控件中移出时，若使用了确定键则会闪烁一下
				int olditem = pLVInternalData->nItemMouseOver;
				//HDC hdc= GetClientDC(hwnd);
				HDC hdc= GetSecondaryClientDC(hwnd);
				
				pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
				pLVInternalData->pItemMouseOver = NULL;
				pLVInternalData->nItemMouseOver = 0;
				
				lvStatusChangeProcess(hwnd, hdc, olditem, 0);
				//ReleaseDC(hdc);
				ReleaseSecondaryDC(hwnd, hdc);
				
				//向父窗口发送失去鼠标的消息
		        NotifyParent (hwnd, id, ELVN_NCITEMMOUSEOVERED);
			}			
		}
	}
	break;
	
    case MSG_MOUSEMOVE:
    {
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);  
        int nRows,oldfocus;
        HDC   hdc;
        RECT rect, rcClient;
        PEBITEMDATA p2;

        GetClientRect (hwnd, &rcClient);

        hdc = GetSecondaryClientDC(hwnd);

        //fprintf(stderr,"listview:mouse over,mousex=%d,mousey=%d.\n",mouseX,mouseY);
        if ((nRows =  isInLVItem (mouseX, mouseY, &p2, pLVInternalData)) > 0)
        {

            if(pLVInternalData->nItemMouseOver == nRows) {
                ReleaseSecondaryDC(hwnd, hdc);
                break;
            }

            if (pLVInternalData->pItemMouseOver != NULL)
            {
                //选中某条目;
                /*
                   rect.left = rcClient.left;
                   rect.right = rcClient.right;
                   rect.top = rcClient.top + pLVInternalData->nHeadHeight +
                   pLVInternalData->nItemHeight *
                   (pLVInternalData->nItemMouseOver - 1) -
                   pLVInternalData->nOriginalY;
                   rect.bottom =rcClient.top + pLVInternalData->nHeadHeight +
                   pLVInternalData->nItemHeight *
                   pLVInternalData->nItemMouseOver -
                   pLVInternalData->nOriginalY;
                   InvalidateRect (hwnd, &rect, FALSE);

                   rect.left = rcClient.left;
                   rect.right = rcClient.right;
                   rect.top = rcClient.top + pLVInternalData->nHeadHeight +
                   pLVInternalData->nItemHeight * (nRows - 1) -
                   pLVInternalData->nOriginalY;
                   rect.bottom = rcClient.top + pLVInternalData->nHeadHeight +
                   pLVInternalData->nItemHeight * nRows -
                   pLVInternalData->nOriginalY;

                   InvalidateRect (hwnd, &rect, FALSE);
                   */

                //fprintf(stderr,"listview:kill old focus.\n");
                pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
                oldfocus =pLVInternalData->nItemMouseOver;
            }
            else
            {
                /*
                   rect.left = rcClient.left;
                   rect.right = rcClient.right;
                   rect.top = rcClient.top + pLVInternalData->nHeadHeight +
                   pLVInternalData->nItemHeight * (nRows - 1) -
                   pLVInternalData->nOriginalY;
                   rect.bottom =rcClient.top + pLVInternalData->nHeadHeight +
                   pLVInternalData->nItemHeight * nRows -
                   pLVInternalData->nOriginalY;

                   InvalidateRect (hwnd, &rect, FALSE);
                   */
                oldfocus =0;

            }
            p2->bMouseOver = TRUE;
            pLVInternalData->pItemMouseOver = p2;
            pLVInternalData->nItemMouseOver = nRows;

            //hdc = GetClientDC(hwnd);
            lvStatusChangeProcess(hwnd,hdc,oldfocus,nRows);
            //ReleaseDC(hdc);

            if(oldfocus ==0)
            {
                //向父窗口发送ITEM被鼠标经过消息
                NotifyParent (hwnd, id, ELVN_ITEMMOUSEOVERED);
            }
            //fprintf(stderr,"listview:new mouseoveritem=%d.\n",nRows);
            //NotifyParent (hwnd, id, ELVN_MOUSEONCHANGE);
        }
        else
        {
            if (pLVInternalData->pItemMouseOver != NULL)
            {
                /*rect.left = rcClient.left;
                  rect.right = rcClient.right;
                  rect.top = rcClient.top + pLVInternalData->nHeadHeight +
                  pLVInternalData->nItemHeight *
                  (pLVInternalData->nItemMouseOver - 1) -
                  pLVInternalData->nOriginalY;
                  rect.bottom =rcClient.top + pLVInternalData->nHeadHeight +
                  pLVInternalData->nItemHeight *
                  pLVInternalData->nItemMouseOver -
                  pLVInternalData->nOriginalY;*/
                oldfocus =pLVInternalData->nItemMouseOver;
                pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
                pLVInternalData->pItemMouseOver = NULL;
                pLVInternalData->nItemMouseOver = 0;
                //hdc = GetClientDC(hwnd);
                lvStatusChangeProcess(hwnd,hdc,oldfocus,0);
                //ReleaseDC(hdc);
                //向父窗口发送失去鼠标的消息
                NotifyParent (hwnd, id, ELVN_NCITEMMOUSEOVERED);
                //InvalidateRect (hwnd, &rect, FALSE);
            }
        } 
        ReleaseSecondaryDC(hwnd, hdc);

        //edit end
    }
    break;
	
    case MSG_LBUTTONDOWN:
    {
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);
        

        int nCols, nRows;

        RECT rect, rcClient;
        PEBLSTHDR p1;
        PEBITEMDATA p2;
        EBCMPINFO info;

		if(dwStyle & ELVS_USEHEADER)
        	nCols = isInListViewHead (mouseX, mouseY, &p1, pLVInternalData);
        else
        	nCols = 0;

        GetClientRect (hwnd, &rcClient);

        if (nCols > 0)
        {
            switch (p1->sort)
            {
              case NOTSORTED:
                p1->sort = HISORTED;
                break;
              case HISORTED:
                p1->sort = LOSORTED;
                break;
              case LOSORTED:
                p1->sort = HISORTED;
                break;
            }
            p1->up = FALSE;
            rect.left = p1->x;
            rect.top = p1->y;
            rect.right = p1->x + p1->width;
            rect.left = p1->y + p1->height;
            pLVInternalData->bItemClicked = TRUE;
            pLVInternalData->pItemClicked = p1;
            info.nCols = nCols;
            info.sort = p1->sort;
            SendMessage (hwnd, ELVM_COLSORT, 0, (LPARAM) & info);
            InvalidateRect (hwnd, NULL, FALSE);
        }
        else
        {
            if ((dwStyle & ELVS_USEHEADER) 
                    && (nCols = isInLVHeadBorder (mouseX, mouseY, 
                            &p1, pLVInternalData)) > 0)
            {
                pLVInternalData->bBorderDraged = TRUE;
                pLVInternalData->pItemDraged = p1;
                pLVInternalData->nItemDraged = nCols;
                pLVInternalData->oldMouseX = mouseX;
                pLVInternalData->oldMouseY = mouseY;
                SetCapture (hwnd);
            }
            else if ((nRows = isInLVItem (mouseX, mouseY, 
                            &p2, pLVInternalData)) > 0)
            {				
                if (pLVInternalData->pItemSelected != NULL)
                {//选中某条目;
                    rect.left = rcClient.left;
                    rect.right = rcClient.right;

                    rect.top = rcClient.top + pLVInternalData->nHeadHeight +
                        pLVInternalData->nItemHeight * (pLVInternalData->nItemSelected - 1) 
                        - pLVInternalData->nOriginalY;

                    rect.bottom = rcClient.top + pLVInternalData->nHeadHeight +
                        pLVInternalData->nItemHeight * pLVInternalData->nItemSelected -
                        pLVInternalData->nOriginalY;

                    InvalidateRect (hwnd, &rect, FALSE);

                    rect.left = rcClient.left;
                    rect.right = rcClient.right;
                    rect.top = rcClient.top + pLVInternalData->nHeadHeight +
                        pLVInternalData->nItemHeight * (nRows - 1) -  pLVInternalData->nOriginalY;

                    rect.bottom = rcClient.top + pLVInternalData->nHeadHeight +                    
                        pLVInternalData->nItemHeight * nRows - pLVInternalData->nOriginalY;

                    InvalidateRect (hwnd, &rect, FALSE);

                    pLVInternalData->pItemSelected->bSelected = FALSE;
                }
                else
                {
                    rect.left = rcClient.left;
                    rect.right = rcClient.right;
                    rect.top = rcClient.top + pLVInternalData->nHeadHeight +
                        pLVInternalData->nItemHeight * (nRows - 1) -  pLVInternalData->nOriginalY;

                    rect.bottom = rcClient.top + pLVInternalData->nHeadHeight +                    
                        pLVInternalData->nItemHeight * nRows - pLVInternalData->nOriginalY;

                    InvalidateRect (hwnd, &rect, FALSE);
                }
				
                p2->bSelected = TRUE;
				if (dwStyle & ELVS_USECHECKBOX)
				{
					if (p2->bChecked)
						p2->bChecked = FALSE;
					else
						p2->bChecked = TRUE;
				}
                pLVInternalData->pItemSelected = p2;
                pLVInternalData->nItemSelected = nRows;
                //NotifyParent (hwnd, id, ELVN_SELCHANGE);
                NotifyParentEx (hwnd, id, ELVN_SELCHANGE, (DWORD)pLVInternalData);
              }

        }
    }
    break;
	
    case MSG_LBUTTONUP:
    {
        RECT rect;
        PEBLSTHDR p1;
   
        if (!pLVInternalData->bBorderDraged)
        {
            p1 = pLVInternalData->pItemClicked;
            if (pLVInternalData->bItemClicked)
            {
                p1->up = TRUE;
                rect.left = p1->x;
                rect.top = p1->y;
                rect.right = p1->x + p1->width;
                rect.left = p1->y + p1->height;
                InvalidateRect (hwnd, NULL, FALSE);
                pLVInternalData->bItemClicked = FALSE;
              }
        }
        else
        {
            pLVInternalData->bBorderDraged = FALSE;
            ReleaseCapture ();
        }
        NotifyParentEx (hwnd, id, ELVN_LBUTTONUP, (DWORD)pLVInternalData);
	break;
    }
    
    case ELVM_SETITEMCOLOR:
    {
        int color;
        int rows;
        //int id = LOWORD (wParam);
  
        color = (int) lParam;
        rows = (int) wParam;

        if (setItemColor (rows, color, pLVInternalData) != color)
          NotifyParent (hwnd, id, ELVN_ITEMCOLORCHANGED);

		break;
    }
      
    case ELVM_ADDITEM:
    {
        PEBLVITEM p1;
 
        p1 = (PEBLVITEM) lParam;
        if (sAddItemToList (p1->nItem, pLVInternalData) >= 0)
        	NotifyParent (hwnd, id, ELVN_ADDITEM);
          
        if(dwStyle & WS_VSCROLL)
        	lstSetVScrollInfo (pLVInternalData);
        	
        InvalidateRect (hwnd, NULL, FALSE);
		break;
    }	
    case ELVM_FILLSUBITEM:
    {
        PEBLVSUBITEM p1;
        RECT rect;
    
        p1 = (PEBLVSUBITEM) lParam;
        if (sFillSubItemToList (p1,pLVInternalData) < 0)
        	return -1;
		
		rect.left = sGetSubItemX (p1->subItem, pLVInternalData);
        //rect.right = sGetSubItemWidth (p1->subItem, pLVInternalData);
        //rect.top = p1->nItem * pLVInternalData->nItemHeight;
        //rect.bottom = (p1->nItem + 1) * pLVInternalData->nItemHeight;
        
        rect.right = rect.left + sGetSubItemWidth (p1->subItem, pLVInternalData);
		
        rect.top = pLVInternalData->nHeadHeight+(p1->nItem-1) * 
			pLVInternalData->nItemHeight-pLVInternalData->nOriginalY;
		
        rect.bottom = pLVInternalData->nHeadHeight+p1->nItem * 
			pLVInternalData->nItemHeight-pLVInternalData->nOriginalY;   
		
        InvalidateRect (hwnd, &rect, FALSE);

    }
      break;
	
    case ELVM_GETITEMCOUNT:
    {
        return pLVInternalData->nRows;

    }
    case ELVM_SETITEMSELECTED:    //add by hejian 2004.04.06
    {
    	int p,times;
    	POINT  *action =NULL;
    	
    	action =(POINT *)wParam;
    	p = (int)lParam;
    	if(p <0 || p >pLVInternalData->nRows || pLVInternalData->nRows <=0)
    		return -1;
    	SetItemSelected(dwStyle, p, pLVInternalData);
    	if(p >0 && p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows*2))
    	{
    		times =0;
	    	while(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
	    	{
	    		SendMessage (hwnd, MSG_VSCROLL, SB_PAGEDOWN, 0);
	    		times +=1;
	    	}
	    	if(action !=NULL)
	    	{
	    		action->x =0;   //
	    		action->y =times;
	    	}
	    	/*if(pLVInternalData->nItemMouseOver <pLVInternalData->nFirstVisableRow && 
	    	   pLVInternalData->nItemMouseOver >0)
	    	{
	    		pLVInternalData->nItemMouseOver =pLVInternalData->nFirstVisableRow;
	    		SetItemMouseOver(pLVInternalData->nItemMouseOver,pLVInternalData);
	    	}*/
	}
	else if(p >0 && p>=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
	{
		times =0;
		while(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
    	{
    		SendMessage (hwnd, MSG_VSCROLL, SB_LINEDOWN, 0);
    		times +=1;
    	}
    	if(action !=NULL)
    	{
    		action->x =1;
    		action->y =times;
    	}
	}
	if(p >0 && p <=(pLVInternalData->nFirstVisableRow -pLVInternalData->nVisableRows))
	{
		times =0;
    	while(p <pLVInternalData->nFirstVisableRow)
    	{
    		SendMessage (hwnd, MSG_VSCROLL, SB_PAGEUP, 0);
    		times +=1;
    	}
    	if(action !=NULL)
    	{
    		action->x =2;
    		action->y =times;
    	}
    	/*if(pLVInternalData->nItemMouseOver >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
    	{
    		pLVInternalData->nItemMouseOver =pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows -1;
    		SetItemMouseOver(pLVInternalData->nItemMouseOver,pLVInternalData);
    	}*/
	}
	else if(p >0 && p <pLVInternalData->nFirstVisableRow)
	{
		times =0;
		while(p <pLVInternalData->nFirstVisableRow)
    	{
    		SendMessage (hwnd, MSG_VSCROLL, SB_LINEUP, 0);
    		times +=1;
    	}
    	if(action !=NULL)
    	{
    		action->x =3;
    		action->y =times;
    	}
	}
   // 	InvalidateRect (hwnd, NULL, FALSE);
   	InvalidateRect (hwnd, NULL, FALSE);
    	SendMessage (hwnd, MSG_PAINT, 0, 0);
    }
    break;
    case ELVM_GETSELECTEDITEM:
    {
         return pLVInternalData->nItemSelected;
    }
    case ELVM_GETMOUSEOVERITEM:
    {
    	return pLVInternalData->nItemMouseOver;
    }
    case ELVM_SETMOUSEOVERITEM:
    {
    	int p,oldfocus;
    	HDC   hdc;
		
    	p =(int)lParam;
    	//if(p< 0 || p >pLVInternalData->nRows || (p ==pLVInternalData->nItemSelected && p >0))
    	if(p < 0 || p >pLVInternalData->nRows || pLVInternalData->nRows <=0)
    		return -1;
    	if (pLVInternalData->pItemMouseOver != NULL)
        {//选中某条目;
        	pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
			oldfocus =pLVInternalData->nItemMouseOver;
        }
        else
        {
        	oldfocus =0;
        }
        SetItemMouseOver(p, pLVInternalData);
        //hdc = GetClientDC(hwnd);
        hdc = GetSecondaryClientDC(hwnd);
        lvStatusChangeProcess(hwnd,hdc,oldfocus,p);
        //ReleaseDC(hdc);
        ReleaseSecondaryDC(hwnd, hdc);
        if(p >0 && oldfocus ==0)
        {
	        //向父窗口发送ITEM被鼠标经过消息
	        NotifyParent (hwnd, id, ELVN_ITEMMOUSEOVERED);
		}
		return 1;
    }
    break;
    case ELVM_GETMOUSEOVER_OFFSET:
    {
    	int  offset;
    	
    	if(pLVInternalData->nItemMouseOver <=0)
    	{
    		return -1;
    	}
    	else
    	{
    		offset =pLVInternalData->nItemMouseOver -pLVInternalData->nFirstVisableRow;
    		
    		return offset;
    	}
    }
    case ELVM_SETMOUSEOVER_OFFSET:
    {
    	int p,oldfocus;
    	HDC   hdc;
    	int p2 =(int)lParam;
    	
    	if(pLVInternalData->nRows <=0 || p2 <0)
    		return -1;
    	
    	p =pLVInternalData->nItemMouseOver;
    	p =pLVInternalData->nFirstVisableRow +p2;
    	
    	if(p >pLVInternalData->nRows)
    	{
    		p =pLVInternalData->nRows;
    	}
    	if(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
    	{
    		//针对那些行高不一致的控件来作出的多余的判断
    		p =pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows -1;
    	}
    	if(p <0)
    	{
    		p =0;
    	}
    	
    	SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
    	return 1;
    	/*pLVInternalData->nItemMouseOver =pLVInternalData->nFirstVisableRow +p2;
    	if(pLVInternalData->nItemMouseOver >pLVInternalData->nRows)
    	{
    		pLVInternalData->nItemMouseOver =pLVInternalData->nRows;
    	}
    	if(pLVInternalData->nItemMouseOver >(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows -1))
    	{
    		//针对那些行高不一致的控件来作出的多余的判断
    		pLVInternalData->nItemMouseOver =pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows -1;
    	}
    	if(pLVInternalData->nItemMouseOver <0)
    	{
    		pLVInternalData->nItemMouseOver =0;
    	}
    	
    	p =pLVInternalData->nItemMouseOver;
    	//if(p<= 0 || p >pLVInternalData->nRows || p ==pLVInternalData->nItemSelected)
    	if(p<= 0 || p >pLVInternalData->nRows)
    		break;
    	if (pLVInternalData->pItemMouseOver != NULL)
        {//选中某条目;
        	pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
		oldfocus =pLVInternalData->nItemMouseOver;
        }
        else
        {
        	oldfocus =0;
        }
        SetItemMouseOver(p,pLVInternalData);
        hdc =GetClientDC(hwnd);
        lvStatusChangeProcess(hwnd,hdc,oldfocus,p);
        ReleaseDC(hdc);*/
    }
    break;
    //added by xuwh on 2006-6-5 begin here.
    case ELVM_SETFIRSTVISABLEITEM:
    {
    	int scrollHeight = 0;
        RECT rect;
        int scrollBoundMax;
        int scrollBoundMin;
        int scrollNewPos;
        int p ;
  
  		p = (WPARAM)lParam ;
  		
  		if(p <= 0 || p > pLVInternalData->nRows || pLVInternalData->nRows <=0)
    		return -1;
    		
    	GetClientRect (hwnd, &rect);
        scrollBoundMax = pLVInternalData->nRows * pLVInternalData->nItemHeight -
          					(rect.bottom - rect.top - pLVInternalData->nHeadHeight);
		
		if(scrollBoundMax <= 0)
        	break;
		
        scrollBoundMin = 0;
        
  		if(p > 0 && p > pLVInternalData->nFirstVisableRow + pLVInternalData->nVisableRows - 1 &&
  		pLVInternalData->nFirstVisableRow + pLVInternalData->nVisableRows*2 -1 < pLVInternalData->nRows)
  		{//向下滚动一页
  			while(p > (pLVInternalData->nFirstVisableRow + pLVInternalData->nVisableRows - 1) &&
  			pLVInternalData->nFirstVisableRow + pLVInternalData->nVisableRows*2 -1 < pLVInternalData->nRows)
  			{
  				if(pLVInternalData->nOriginalY < scrollBoundMax)
          		{
          			HJ_DEBUG("scroll down one page>>>>>>>>>>\n") ;
  					pLVInternalData->nFirstVisableRow += pLVInternalData->nVisableRows;
  					if ((pLVInternalData->nOriginalY + (rect.bottom - rect.top)) > scrollBoundMax)
		            {
		            	scrollHeight += (pLVInternalData->nOriginalY - scrollBoundMax);
		            	pLVInternalData->nOriginalY = scrollBoundMax;
		            	
		            	break ;
		          	}
		        	else
		          	{
		          		pLVInternalData->nOriginalY += (rect.bottom - rect.top);
		            	scrollHeight += (rect.top - rect.bottom);
		          	}
		        }
  			}
  		}
		if(p > 0 && p > pLVInternalData->nFirstVisableRow &&
		pLVInternalData->nFirstVisableRow + pLVInternalData->nVisableRows -1 < pLVInternalData->nRows)
		{//向下滚一行
			HJ_DEBUG("FirstVisableRow+VisableRows-1<rows\n") ;
			while(p > pLVInternalData->nFirstVisableRow &&
			pLVInternalData->nFirstVisableRow + pLVInternalData->nVisableRows -1 < pLVInternalData->nRows)
			{
				if(pLVInternalData->nOriginalY < scrollBoundMax)
          		{
          			HJ_DEBUG("scroll down one line>>>>>>>>>>\n") ;
					
					pLVInternalData->nFirstVisableRow += 1;
					HJ_DEBUG("FirstVisableRow:%d\n", pLVInternalData->nFirstVisableRow) ;
					
					if ((pLVInternalData->nOriginalY + pLVInternalData->nItemHeight) > scrollBoundMax)
					{
						scrollHeight += (pLVInternalData->nOriginalY - scrollBoundMax);
						pLVInternalData->nOriginalY = scrollBoundMax;
						
						break ;
					}
					else
					{
						pLVInternalData->nOriginalY += pLVInternalData->nItemHeight;
						scrollHeight += (-pLVInternalData->nItemHeight);
					}
				}
			}
		}
    	if(p >0 && p <=(pLVInternalData->nFirstVisableRow - pLVInternalData->nVisableRows))
		{//向上滚一页
			HJ_DEBUG("scroll up one page>>>>>>>>>>\n") ;
	    	while(p <=(pLVInternalData->nFirstVisableRow - pLVInternalData->nVisableRows))
	    	{
	    		if (pLVInternalData->nOriginalY > scrollBoundMin)
				{
					HJ_DEBUG("scroll up one page>>>>>>>>>>\n") ;
					if(pLVInternalData->nFirstVisableRow - pLVInternalData->nVisableRows >= 1)
					{
						pLVInternalData->nFirstVisableRow -= pLVInternalData->nVisableRows;
					}
					else
					{
						pLVInternalData->nFirstVisableRow = 1;
					}
					if ((pLVInternalData->nOriginalY - (rect.bottom - rect.top)) > scrollBoundMin)
					{
						pLVInternalData->nOriginalY -= (rect.bottom - rect.top);
						scrollHeight += (rect.bottom - rect.top);
					}
					else
					{
						scrollHeight += (pLVInternalData->nOriginalY - scrollBoundMin);
						pLVInternalData->nOriginalY = scrollBoundMin;
						
						break ;
					}
				}
	    	}
		}
		if(p >0 && p < pLVInternalData->nFirstVisableRow)
		{//向上滚一行
			HJ_DEBUG("scroll up one line>>>>>>>>>>\n") ;
			while(p <pLVInternalData->nFirstVisableRow)
	    	{
	    		if (pLVInternalData->nOriginalY > scrollBoundMin)
				{
					HJ_DEBUG("scroll up one line>>>>>>>>>>\n") ;
		    		if(pLVInternalData->nFirstVisableRow >1)
				  	{
				  		pLVInternalData->nFirstVisableRow -=1;
				  	}
				    if ((pLVInternalData->nOriginalY - pLVInternalData->nItemHeight) > scrollBoundMin)
				    {
				        pLVInternalData->nOriginalY -= pLVInternalData->nItemHeight;
				        scrollHeight += pLVInternalData->nItemHeight;
				    }
				    else
				    {
				        scrollHeight += (pLVInternalData->nOriginalY - scrollBoundMin);
				        pLVInternalData->nOriginalY = scrollBoundMin;
				        
				        break ;
				    }
				}
	    	}
		}
        rect.top = rect.top + pLVInternalData->nHeadHeight + 1;
        if(dwStyle & ELVS_BKSCROLL)
        	ScrollWindow (hwnd, 0, scrollHeight, &rect, NULL);
        else
        	InvalidateRect (hwnd, NULL, FALSE);	
        
        if(dwStyle & WS_VSCROLL)
        	lstSetVScrollInfo (pLVInternalData);
		
		lvNotifyParentPos(hwnd);
    }
    break ;
    //added by xuwh on 2006-6-5 end.
    case ELVM_GETCOLUMNCOUNT:
    {
         return pLVInternalData->nCols;
    }
	
    case ELVM_GETSUBITEM:
    {
        PEBLVSUBITEM p1;
        p1 = (PEBLVSUBITEM) lParam;

        return sGetSubItemCopy (p1, pLVInternalData);
    }
	
    case ELVM_ADDCOLUMN:
    {
        PEBLVCOLOUM p1;
        //int id = LOWORD (wParam);

        p1 = (PEBLVCOLOUM) lParam;
        if (sAddColumnToList
            (p1->nCols, p1->pszHeadText, p1->width, pLVInternalData) >= 0)
          NotifyParent (hwnd, id, ELVN_ADDCOLUMN);
          
        if(dwStyle & WS_HSCROLL)
        	lstSetHScrollInfo (pLVInternalData);
        InvalidateRect (hwnd, NULL, FALSE);
    }
      break;
	
    case ELVM_MODIFYHEAD:
    {
        PEBLVCOLOUM p1;

        p1 = (PEBLVCOLOUM) lParam;

        if (sModifyHead (p1->nCols, p1->pszHeadText, pLVInternalData) >= 0)
        {
            InvalidateRect (hwnd, NULL, FALSE);
        }
    }
      	break;
	
    case ELVM_FINDITEM:
    {
        PEBLVFINDINFO p1;
        int ret;
        p1 = (PEBLVFINDINFO) lParam;
 
        ret = sFindItemFromList (p1, pLVInternalData);
        return ret;
    }
    case ELVM_DELITEM:
    {
#if 0
        PEBLVITEM p1;
        //int id = LOWORD (wParam);

        p1 = (PEBLVITEM) lParam;
        if (sRemoveItemFromList (p1->nItem, pLVInternalData) >= 0)
          NotifyParent (hwnd, id, ELVN_SELCHANGE);
         
        if(dwStyle & WS_VSCROLL)
        	lstSetVScrollInfo (pLVInternalData);
        	
        InvalidateRect (hwnd, NULL, FALSE);
#else
		PEBLVITEM p1;
        HDC   hdc;
        RECT   Rect,rcRect;
        //int id = LOWORD (wParam);

        p1 = (PEBLVITEM) lParam;
        if (sRemoveItemFromList (p1->nItem, pLVInternalData) >= 0)
          NotifyParent (hwnd, id, ELVN_SELCHANGE);
         
        if(dwStyle & WS_VSCROLL)
        	lstSetVScrollInfo (pLVInternalData);
        	
        GetClientRect(hwnd, &rcRect);
        Rect.left = sGetSubItemX (1, pLVInternalData);
        Rect.top = pLVInternalData->nHeadHeight+(p1->nItem-1) * 
			pLVInternalData->nItemHeight-pLVInternalData->nOriginalY;
		Rect.right = rcRect.right;
		Rect.bottom = rcRect.bottom;
		//hdc = GetClientDC(hwnd);
        hdc = GetSecondaryClientDC(hwnd);
		SetBrushColor (hdc, GetWindowBkColor(hwnd));
	  	FillBox (hdc, Rect.left, Rect.top, Rect.right - Rect.left,
	  			Rect.bottom - Rect.top);
  		//ReleaseDC(hdc);
        ReleaseSecondaryDC(hwnd, hdc);
        InvalidateRect (hwnd, &Rect, FALSE);
#endif
    }
      break;
    case ELVM_DELALLITEM:
    {
        //int id = LOWORD (wParam);
 
        if (sRemoveAllItem (pLVInternalData) >= 0)
        	NotifyParent (hwnd, id, ELVN_DELALLITEM);
		if(dwStyle & WS_VSCROLL)
        	lstSetVScrollInfo (pLVInternalData);
        	
        InvalidateRect (hwnd, NULL, FALSE);
		break;
    }
  
    case ELVM_CLEARSUBITEM:
    	break;
	
    case ELVM_DELCOLUMN:
    {
        PEBLVCOLOUM p1;
        //int id = LOWORD (wParam);

        p1 = (PEBLVCOLOUM) lParam;
 
        if (sRemoveColumnFromList (p1->nCols, pLVInternalData) < 0)
          NotifyParent (hwnd, id, ELVN_DELCOLUMN);
        if(dwStyle & WS_HSCROLL)
        	lstSetHScrollInfo (pLVInternalData);
        InvalidateRect (hwnd, NULL, FALSE);
		break;
      }
  
    case MSG_VSCROLL:
      {
        int scrollHeight = 0;
        RECT rect;
        int scrollBoundMax;
        int scrollBoundMin;
        int scrollNewPos;
  
        GetClientRect (hwnd, &rect);
        scrollBoundMax = pLVInternalData->nRows * pLVInternalData->nItemHeight -
          					(rect.bottom - rect.top - pLVInternalData->nHeadHeight);
		
		if(scrollBoundMax <= 0)      //add by hejian 2004-5-19
        	break;
		
        scrollBoundMin = 0;
        switch (wParam)
        {
        	case SB_LINEDOWN:
			{
				if (pLVInternalData->nOriginalY < scrollBoundMax)
				{
				/*************************************add by hejian 2004.12.11*/
					if(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows -1 < 
						pLVInternalData->nRows)
					{
						pLVInternalData->nFirstVisableRow +=1;
					}
				/************************************/
					if ((pLVInternalData->nOriginalY + pLVInternalData->nItemHeight) > scrollBoundMax)
					{
						scrollHeight = pLVInternalData->nOriginalY - scrollBoundMax;
						pLVInternalData->nOriginalY = scrollBoundMax;
					}
					else
					{
						pLVInternalData->nOriginalY += pLVInternalData->nItemHeight;
						scrollHeight = -pLVInternalData->nItemHeight;
					}

				//pLVInternalData->nOriginalY += pLVInternalData->nItemHeight;
				//scrollHeight = -pLVInternalData->nItemHeight;


				}
            	break;
        	}
          	case SB_LINEUP:
			{
				if (pLVInternalData->nOriginalY > scrollBoundMin)
				{
				  	/*************************************add by hejian 2004.12.11*/
				  	if(pLVInternalData->nFirstVisableRow >1)
				  	{
				  		pLVInternalData->nFirstVisableRow -=1;
				  	}
				  	/******************************************/
				    if ((pLVInternalData->nOriginalY - pLVInternalData->nItemHeight) > scrollBoundMin)
				    {
				        pLVInternalData->nOriginalY -= pLVInternalData->nItemHeight;
				        scrollHeight = pLVInternalData->nItemHeight;
				    }
				    else
				    {
				        scrollHeight = pLVInternalData->nOriginalY - scrollBoundMin;
				        pLVInternalData->nOriginalY = scrollBoundMin;
				    }
				   //pLVInternalData->nOriginalY -= pLVInternalData->nItemHeight;
				   //scrollHeight = pLVInternalData->nItemHeight;
				}
            	break;
          	}
          case SB_PAGEDOWN:
          {
          	//printf("receive message SB_PAGEDOWN.\n");
          	if(pLVInternalData->nOriginalY < scrollBoundMax)
          	{
          		/*************************************add by hejian 2004.12.11*/
          		if(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows*2 -1 <=pLVInternalData->nRows)
          		{
          			pLVInternalData->nFirstVisableRow +=pLVInternalData->nVisableRows;
          		}
          		else
          		{
          			pLVInternalData->nFirstVisableRow =pLVInternalData->nRows -pLVInternalData->nVisableRows +1;
          		}
          		/************************************************/
          		if ((pLVInternalData->nOriginalY + (rect.bottom - rect.top)) > scrollBoundMax)
                  	{
                    		scrollHeight =pLVInternalData->nOriginalY - scrollBoundMax;
                    		pLVInternalData->nOriginalY = scrollBoundMax;
                  	}
                	else
                  	{
                  		 pLVInternalData->nOriginalY += (rect.bottom - rect.top);
                    		scrollHeight = -(rect.bottom - rect.top);
                  	}
          	}
          }
          break;
          case SB_PAGEUP:
          {
			//printf("receive message SB_PAGEUP.\n");
			if (pLVInternalData->nOriginalY > scrollBoundMin)
			{
				/*************************************add by hejian 2004.12.11*/
				if(pLVInternalData->nFirstVisableRow -pLVInternalData->nVisableRows >=1)
				{
					pLVInternalData->nFirstVisableRow -=pLVInternalData->nVisableRows;
				}
				else
				{
					pLVInternalData->nFirstVisableRow =1;
				}
				/***********************************************/
				if ((pLVInternalData->nOriginalY - (rect.bottom - rect.top)) > scrollBoundMin)
				{
					pLVInternalData->nOriginalY -= (rect.bottom - rect.top);
					scrollHeight = (rect.bottom - rect.top);
				}
				else
				{
					scrollHeight = pLVInternalData->nOriginalY - scrollBoundMin;
					pLVInternalData->nOriginalY = scrollBoundMin;
				}
			}
          }
          break;
          case SB_THUMBTRACK:
            scrollNewPos = (int) lParam;
            if (((scrollNewPos - pLVInternalData->nOriginalY) < 5) &&
                ((scrollNewPos - pLVInternalData->nOriginalY) > -5) &&
                (scrollNewPos > 5) && ((scrollBoundMax - scrollNewPos) > 5))
              return 0;
            if ((scrollNewPos < pLVInternalData->nOriginalY)
                && (scrollNewPos <= pLVInternalData->nItemHeight))
            {
                scrollHeight = pLVInternalData->nOriginalY - 0;
                pLVInternalData->nOriginalY = 0;
            }
            else
            {
                if ((scrollNewPos > pLVInternalData->nOriginalY)
                    && ((scrollBoundMax - scrollNewPos) < pLVInternalData->nItemHeight))
                {
                    scrollHeight =
                      pLVInternalData->nOriginalY - scrollBoundMax;
                    pLVInternalData->nOriginalY = scrollBoundMax;
                }
                else
                {
                    scrollHeight = pLVInternalData->nOriginalY - scrollNewPos;
                    pLVInternalData->nOriginalY = scrollNewPos;
                }

            }

            break;
        }


        rect.top = rect.top + pLVInternalData->nHeadHeight + 1;
        if(dwStyle & ELVS_BKSCROLL)
        	ScrollWindow (hwnd, 0, scrollHeight, &rect, NULL);
        else
        	InvalidateRect (hwnd, NULL, FALSE);	
        
        if(dwStyle & WS_VSCROLL)
        	lstSetVScrollInfo (pLVInternalData);
		
		lvNotifyParentPos(hwnd);

      }
      break;
    case MSG_HSCROLL:
      {
        int scrollWidth = 0;
        int scrollBoundMax;
        int scrollBoundMin;
        int scrollNewPos;
        RECT rect;
   
        GetClientRect (hwnd, &rect);
        GetClientRect (hwnd, &rect);
        scrollBoundMax =
          sGetItemWidth (pLVInternalData) - (rect.right - rect.left);

		if(scrollBoundMax <=0)      //add by hejian 2004-5-19
        	break;
		
        scrollBoundMin = 0;

        switch (wParam)
          {
          case SB_LINERIGHT:
            if (pLVInternalData->nOriginalX < scrollBoundMax)
              {
                if ((pLVInternalData->nOriginalX + HSCROLL) > scrollBoundMax)
                  {
                    scrollWidth =
                      pLVInternalData->nOriginalX - scrollBoundMax;
                    pLVInternalData->nOriginalX = scrollBoundMax;
                  }
                else
                  {
                    pLVInternalData->nOriginalX += HSCROLL;
                    scrollWidth = -HSCROLL;
                  }

              }
            break;

          case SB_LINELEFT:
            if (pLVInternalData->nOriginalX > scrollBoundMin)
              {
                if ((pLVInternalData->nOriginalX - HSCROLL) > scrollBoundMin)
                  {
                    pLVInternalData->nOriginalX -= HSCROLL;
                    scrollWidth = HSCROLL;
                  }
                else
                  {
                    scrollWidth =
                      pLVInternalData->nOriginalX - scrollBoundMin;
                    pLVInternalData->nOriginalX = scrollBoundMin;
                  }

              }

            break;
          case SB_THUMBTRACK:
            scrollNewPos = (int) lParam;
            if (((scrollNewPos - pLVInternalData->nOriginalX) < HSCROLL) &&
                ((scrollNewPos - pLVInternalData->nOriginalX) > -HSCROLL) &&
                (scrollNewPos > HSCROLL)
                && ((scrollBoundMax - scrollNewPos) > HSCROLL))
              return 0;
            if ((scrollNewPos < pLVInternalData->nOriginalX)
                && (scrollNewPos <= HSCROLL))
              {
                scrollWidth = pLVInternalData->nOriginalX - 0;
                pLVInternalData->nOriginalX = 0;
              }
            else
              {
                if ((scrollNewPos > pLVInternalData->nOriginalX)
                    && ((scrollBoundMax - scrollNewPos) < HSCROLL))
                  {
                    scrollWidth =
                      pLVInternalData->nOriginalX - scrollBoundMax;
                    pLVInternalData->nOriginalX = scrollBoundMax;
                  }
                else
                  {
                    scrollWidth = pLVInternalData->nOriginalX - scrollNewPos;
                    pLVInternalData->nOriginalX = scrollNewPos;
                  }

              }

            break;
          }

	if(dwStyle & ELVS_BKSCROLL)
        	ScrollWindow (hwnd, scrollWidth, 0, NULL, NULL);
        else
        	InvalidateRect (hwnd, NULL, FALSE);
        
        if(dwStyle & WS_HSCROLL)
        	lstSetHScrollInfo (pLVInternalData);
      }
      break;

    case MSG_FONTCHANGED:
      {
      	
      	int nHeight = GetWindowFont (hwnd)->size;
  	if((nHeight + pLVInternalData->nItemGap) > pLVInternalData->nItemHeight)
  		pLVInternalData->nItemHeight = nHeight + pLVInternalData->nItemGap;
  		
  	if(dwStyle & ELVS_USEHEADER)
  		pLVInternalData->nHeadHeight = nHeight + 2;
  	else
  		pLVInternalData->nHeadHeight = 0;
  	
        setHeadHeight (pLVInternalData->nHeadHeight, pLVInternalData);

	if(dwStyle & WS_VSCROLL)
        	lstSetVScrollInfo (pLVInternalData);
        	
        InvalidateRect (hwnd, NULL, FALSE);
        return 0;
      }

    case MSG_LBUTTONDBLCLK:
      {
        int mouseX = LOSWORD (lParam);
        int mouseY = HISWORD (lParam);
        //int id = LOWORD (wParam);
        PEBITEMDATA p2;
    
        if (isInLVItem (mouseX, mouseY, &p2, pLVInternalData) > 0)
          NotifyParent (hwnd, id, ELVN_ITEMDBCLK);

      }
      break;
    case ELVM_MSG_KEYDOWN:
    {
    switch(wParam)
 	{
 		case SCANCODE_ENTER:
 		{
			HDC hdc = GetSecondaryClientDC(hwnd);
			if (dwStyle & ELVS_USECHECKBOX)
			{
				if(pLVInternalData->nItemMouseOver >0)
				{
					int oldselected = pLVInternalData->nItemSelected;
					
					SetItemSelected(dwStyle, pLVInternalData->nItemMouseOver, pLVInternalData);
	   				//wndInvalidateRect (hwnd, NULL, FALSE);
	   				InvalidateRect (hwnd, NULL, FALSE);
	    		//	SendMessage (hwnd, MSG_PAINT, 0, 0); //edit by tjb 2005-6-29
	    		    ///hdc = GetClientDC(hwnd);
            		lvStatusChangeProcess(hwnd,hdc, oldselected, pLVInternalData->nItemMouseOver);
            		///ReleaseDC(hdc);
				//end edit
			        NotifyParent (hwnd, id, ELVN_SELCHANGE);
				}
			}
			else
			{
	 			if(pLVInternalData->nItemMouseOver >0 && 
	 			   pLVInternalData->nItemMouseOver !=pLVInternalData->nItemSelected)
	 			{
	 				///HDC hdc;
					int oldselected = pLVInternalData->nItemSelected;
	 				SetItemSelected(dwStyle, pLVInternalData->nItemMouseOver, pLVInternalData);
	   				//wndInvalidateRect (hwnd, NULL, FALSE);
	   				InvalidateRect (hwnd, NULL, FALSE);
	    		//	SendMessage (hwnd, MSG_PAINT, 0, 0); //edit by tjb 2005-6-29
					///hdc = GetClientDC(hwnd);
            		lvStatusChangeProcess(hwnd,hdc, oldselected, pLVInternalData->nItemMouseOver);
            		///ReleaseDC(hdc);
				//  end edit
			        NotifyParent (hwnd, id, ELVN_SELCHANGE);
			    }
			}
            ReleaseSecondaryDC(hwnd, hdc);
 		}
 		break;
 		case SCANCODE_CURSORBLOCKLEFT:
 		{
 			SendMessage (hwnd, MSG_HSCROLL, SB_LINELEFT, 0);
 		}
 		break;
 		case SCANCODE_CURSORBLOCKRIGHT:
 		{
 			SendMessage (hwnd, MSG_HSCROLL, SB_LINERIGHT, 0);
 		}
 		break;
 		case SCANCODE_CURSORBLOCKDOWN:
 		{
 			int p,oldfocus;
			HDC   hdc;
			POINT  *action =NULL;
			
			action =(POINT *)lParam;
 			if(pLVInternalData->nItemMouseOver >=0 && 
 			   pLVInternalData->nItemMouseOver < pLVInternalData->nRows)
 			{
 				p =pLVInternalData->nItemMouseOver;
 				p +=1;   //行数加一
 				if(p <pLVInternalData->nFirstVisableRow)
 				{
 					//如果发现当前要设置为经过状态的行小于可见的第一行，
 					//说明控件刚刚做了向下翻页的动作,因此做下面的修改
 					p =pLVInternalData->nFirstVisableRow;
 				}
 				else if(p >(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
 				{
 					//如果发现向下移一行之后，被设置的行即使向下滚一行页看不到
 					//则说明说明这个控件刚刚发生了向上翻页,因此要做如下处理
 					p =pLVInternalData->nFirstVisableRow;
 				}
 				SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
 				if(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
 				{
 					if(action !=NULL)
 					{
 						action->x =1;
 						action->y =1;
 					}
 					SendMessage (hwnd, MSG_VSCROLL, SB_LINEDOWN, 0);
 				}
 				//SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
 			}
 		}
 		break;
 		case SCANCODE_CURSORBLOCKUP:
 		{
 			int p,oldfocus;
			HDC   hdc;
			POINT  *action =NULL;
			
			action =(POINT *)lParam;			
 			if(pLVInternalData->nRows >0)
 			{
 				p =pLVInternalData->nItemMouseOver;
 				if(p >1)
 				{
 					p -=1;
 				}
 				else if(p ==0)
 				{
 					p =1;
 				}
 				if((p +1) <pLVInternalData->nFirstVisableRow)
 				{
 					//如果发现当前鼠标经过行，在可见第一行之前，说明控件
 					//前面做了向下翻页动作，因此应作如下调整
 					p =pLVInternalData->nFirstVisableRow;
 				}
 				else if(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
 				{
 					//如果发现向上移动一行之后，仍然在可见范围之下，可见控件
 					//刚刚做了向上翻页的动作，因此应作如下调整
 					p =pLVInternalData->nFirstVisableRow;
 				}
 				
 				SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
 				if(p <pLVInternalData->nFirstVisableRow)
 				{
 					if(action !=NULL)
 					{
 						action->x =3;
 						action->y =1;
 					}
 					SendMessage (hwnd, MSG_VSCROLL, SB_LINEUP, 0);
 				}
 				//SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
 			}
 		}
 		break;
 		case SCANCODE_PAGEUP:
		{
			int p,oldfocus;
			HDC   hdc;
			POINT  *action =NULL;
			
			action =(POINT *)lParam;
			if(pLVInternalData->nFirstVisableRow >1 && pLVInternalData->nRows >0)
			{
				p =pLVInternalData->nItemMouseOver;
				if(p <pLVInternalData->nFirstVisableRow)
				{
					//如果当前鼠标经过行，小于可见的第一行，则从可见的第一行
					//开始向上翻页
					p =pLVInternalData->nFirstVisableRow;
				}
				else if(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
				{
					//如果当前鼠标经过行在可见范围之下，也从可见的第一行
					//开始向上翻页
					p =pLVInternalData->nFirstVisableRow;
				}
				
				if(p >pLVInternalData->nVisableRows)
				{
					p -=pLVInternalData->nVisableRows;
				}
				else
				{
					p =1;
				}
				if(action !=NULL)
				{
					action->x =2;
					action->y =1;
				}
				SendMessage (hwnd, MSG_VSCROLL, SB_PAGEUP, 0);
				
				SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
			}
 			break;
 		}
  		case SCANCODE_PAGEDOWN:
		{
			int p,oldfocus;
			HDC   hdc;
			POINT  *action =NULL;
			
			action =(POINT *)lParam;
			if(pLVInternalData->nItemMouseOver <pLVInternalData->nRows && 
			   (pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows) <=pLVInternalData->nRows)
			{
				p =pLVInternalData->nItemMouseOver;
				if(p <pLVInternalData->nFirstVisableRow)
				{
					p =pLVInternalData->nFirstVisableRow;
				}
				else if(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
				{
					p =pLVInternalData->nFirstVisableRow;
				}
				
				if(p +pLVInternalData->nVisableRows <=pLVInternalData->nRows)
				{
					p +=pLVInternalData->nVisableRows;
				}
				else
				{
					p =pLVInternalData->nRows;
				}
				if(action !=NULL)
				{
					action->x =0;
					action->y =1;
				}
				SendMessage (hwnd, MSG_VSCROLL, SB_PAGEDOWN, 0);
				
				SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
			}
 			break;
  		}
  		default:
  			break;
  	}
    }
    break;
    case MSG_KEYDOWN:
    {

        //int id = LOWORD (wParam);
 	switch(wParam)
 	{
 		case SCANCODE_ENTER:
 		{
			HDC hdc = GetSecondaryClientDC(hwnd);
			if (dwStyle & ELVS_USECHECKBOX)
			{
				if(pLVInternalData->nItemMouseOver >0)
				{
					///HDC hdc;
					int oldselected = pLVInternalData->nItemSelected;
					
					SetItemSelected(dwStyle, pLVInternalData->nItemMouseOver, pLVInternalData);
	   				//wndInvalidateRect (hwnd, NULL, FALSE);
	   				InvalidateRect (hwnd, NULL, FALSE);
	    		//	SendMessage (hwnd, MSG_PAINT, 0, 0); //edit by tjb 2005-6-29
	    		    ///hdc =GetClientDC(hwnd);
            		lvStatusChangeProcess(hwnd,hdc, oldselected, pLVInternalData->nItemMouseOver);
            		///ReleaseDC(hdc);
				//end edit
			        NotifyParent (hwnd, id, ELVN_SELCHANGE);
				}
			}
			else
			{
	 			if(pLVInternalData->nItemMouseOver >0 && 
	 			   pLVInternalData->nItemMouseOver !=pLVInternalData->nItemSelected)
	 			{
	 				///HDC hdc;
					int oldselected = pLVInternalData->nItemSelected;
	 				SetItemSelected(dwStyle, pLVInternalData->nItemMouseOver, pLVInternalData);
	   				//wndInvalidateRect (hwnd, NULL, FALSE);
	   				InvalidateRect (hwnd, NULL, FALSE);
	    		//	SendMessage (hwnd, MSG_PAINT, 0, 0); //edit by tjb 2005-6-29
					///hdc =GetClientDC(hwnd);
            		lvStatusChangeProcess(hwnd,hdc, oldselected, pLVInternalData->nItemMouseOver);
            		///ReleaseDC(hdc);
				//  end edit
			        NotifyParent (hwnd, id, ELVN_SELCHANGE);
			    }
			}
            ReleaseSecondaryDC(hwnd, hdc);
 		}
 		break;
 		case SCANCODE_CURSORBLOCKLEFT:
 		{
 			SendMessage (hwnd, MSG_HSCROLL, SB_LINELEFT, 0);
 		}
 		break;
 		case SCANCODE_CURSORBLOCKRIGHT:
 		{
 			SendMessage (hwnd, MSG_HSCROLL, SB_LINERIGHT, 0);
 		}
 		break;
 		case SCANCODE_CURSORBLOCKDOWN:
 		{
 			int p,oldfocus;
			HDC   hdc;
			//POINT  *action =NULL;			
			//action =(POINT *)lParam;
 			if(pLVInternalData->nItemMouseOver >=0 && 
 			   pLVInternalData->nItemMouseOver < pLVInternalData->nRows)
 			{				
 				p =pLVInternalData->nItemMouseOver;
 				p +=1;   //行数加一
 				if(p < pLVInternalData->nFirstVisableRow)
 				{
 					//如果发现当前要设置为经过状态的行小于可见的第一行，
 					//说明控件刚刚做了向下翻页的动作,因此做下面的修改
 					p =pLVInternalData->nFirstVisableRow;
 				}
 				else if(p >(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
 				{
 					//如果发现向下移一行之后，被设置的行即使向下滚一行页看不到
 					//则说明说明这个控件刚刚发生了向上翻页,因此要做如下处理
 					p =pLVInternalData->nFirstVisableRow;
 				}
				
 				SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
								
 				if(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
 				{
 					/*if(action !=NULL)
 					{
 						action->x =1;
 						action->y =1;
 					}*/
 					SendMessage (hwnd, MSG_VSCROLL, SB_LINEDOWN, 0);
 				}
				//add by tjb 2005-6-28				
 				lvNotifyParentPos(hwnd);

 				//SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
 				/*if (pLVInternalData->pItemMouseOver != NULL)
			        {//选中某条目;
			        	pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
					oldfocus =pLVInternalData->nItemMouseOver;
			        }
			        else
			        {
			        	oldfocus =0;
			        }
 				pLVInternalData->nItemMouseOver +=1;
 				if(pLVInternalData->nItemMouseOver -pLVInternalData->nFirstVisableRow >=pLVInternalData->nVisableRows)
 				{
 					SendMessage (hwnd, MSG_VSCROLL, SB_LINEDOWN, 0);
 				}
			    	
			    	p =pLVInternalData->nItemMouseOver;
			        SetItemMouseOver(p,pLVInternalData);
			        hdc =GetClientDC(hwnd);
			        lvStatusChangeProcess(hwnd,hdc,oldfocus,p);
			        ReleaseDC(hdc);*/
 			}
 		}
 		break;
 		case SCANCODE_CURSORBLOCKUP:
 		{
 			int p,oldfocus;
			HDC   hdc;
			//POINT  *action =NULL;
			
			//action =(POINT *)lParam;			
 			if(pLVInternalData->nRows >0)
 			{
 				p =pLVInternalData->nItemMouseOver;
 				if(p >1)
 				{
 					p -=1;
 				}
 				else if(p ==0)
 				{
 					p =1;
 				}
 				if((p +1) <pLVInternalData->nFirstVisableRow)
 				{
 					//如果发现当前鼠标经过行，在可见第一行之前，说明控件
 					//前面做了向下翻页动作，因此应作如下调整
 					p =pLVInternalData->nFirstVisableRow;
 				}
 				else if(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
 				{
 					//如果发现向上移动一行之后，仍然在可见范围之下，可见控件
 					//刚刚做了向上翻页的动作，因此应作如下调整
 					p =pLVInternalData->nFirstVisableRow;
 				}
 				
 				SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
 				if(p <pLVInternalData->nFirstVisableRow)
 				{
 					/*if(action !=NULL)
 					{
 						action->x =3;
 						action->y =1;
 					}*/
 					SendMessage (hwnd, MSG_VSCROLL, SB_LINEUP, 0);
 				}
 				//add by tjb 2005-6-28	
 				lvNotifyParentPos(hwnd);

 				//SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
 				/*if (pLVInternalData->pItemMouseOver != NULL)
			        {//选中某条目;
			        	pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
					oldfocus =pLVInternalData->nItemMouseOver;
			        }
			        else
			        {
			        	oldfocus =0;
			        }
 				pLVInternalData->nItemMouseOver -=1;
 				if(pLVInternalData->nItemMouseOver <pLVInternalData->nFirstVisableRow)
 				{
 					SendMessage (hwnd, MSG_VSCROLL, SB_LINEUP, 0);
 				}
 				
 				p =pLVInternalData->nItemMouseOver;
			        SetItemMouseOver(p,pLVInternalData);
			        hdc =GetClientDC(hwnd);
			        lvStatusChangeProcess(hwnd,hdc,oldfocus,p);
			        ReleaseDC(hdc);*/
 			}
 		}
 		break;
 		case SCANCODE_PAGEUP:
		{
			int p,oldfocus;
			HDC   hdc;
			//POINT  *action =NULL;
			
			//action =(POINT *)lParam;
			if(pLVInternalData->nFirstVisableRow >1 && pLVInternalData->nRows >0)
			{
				p = pLVInternalData->nItemMouseOver;
				if(p < pLVInternalData->nFirstVisableRow)
				{
					//如果当前鼠标经过行，小于可见的第一行，则从可见的第一行
					//开始向上翻页
					p =pLVInternalData->nFirstVisableRow;
				}
				else if(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
				{
					//如果当前鼠标经过行在可见范围之下，也从可见的第一行
					//开始向上翻页
					p =pLVInternalData->nFirstVisableRow;
				}
				
				if(p >pLVInternalData->nVisableRows)
				{
					p -=pLVInternalData->nVisableRows;
				}
				else
				{
					p =1;
				}
				/*if(action !=NULL)
				{
					action->x =2;
					action->y =1;
				}*/
				SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
				SendMessage (hwnd, MSG_VSCROLL, SB_PAGEUP, 0);
				/*if (pLVInternalData->pItemMouseOver != NULL)
			        {//选中某条目;
			        	pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
					oldfocus =pLVInternalData->nItemMouseOver;
			        }
			        else
			        {
			        	oldfocus =0;
			        }
				if(pLVInternalData->nItemMouseOver -pLVInternalData->nVisableRows >=1)
				{
					pLVInternalData->nItemMouseOver -=pLVInternalData->nVisableRows;
				}
				else
				{
					pLVInternalData->nItemMouseOver =1;
				}
				SendMessage (hwnd, MSG_VSCROLL, SB_PAGEUP, 0);
				
				p =pLVInternalData->nItemMouseOver;
			        SetItemMouseOver(p,pLVInternalData);
			        hdc =GetClientDC(hwnd);
			        lvStatusChangeProcess(hwnd,hdc,oldfocus,p);
			        ReleaseDC(hdc);*/
			}
 			break;
 		}
  		case SCANCODE_PAGEDOWN:
		{
			int p,oldfocus;
			HDC   hdc;
			//POINT  *action =NULL;
			
			//action =(POINT *)lParam;
			if(pLVInternalData->nItemMouseOver <pLVInternalData->nRows && 
			   (pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows) <=pLVInternalData->nRows)
			{
				p =pLVInternalData->nItemMouseOver;
				if(p <pLVInternalData->nFirstVisableRow)
				{
					p =pLVInternalData->nFirstVisableRow;
				}
				else if(p >=(pLVInternalData->nFirstVisableRow +pLVInternalData->nVisableRows))
				{
					p =pLVInternalData->nFirstVisableRow;
				}
				
				if(p +pLVInternalData->nVisableRows <=pLVInternalData->nRows)
				{
					p +=pLVInternalData->nVisableRows;
				}
				else
				{
					p =pLVInternalData->nRows;
				}
				/*if(action !=NULL)
				{
					action->x =0;
					action->y =1;
				}*/
				SendMessage(hwnd,ELVM_SETMOUSEOVERITEM,0,(LPARAM)p);
				SendMessage (hwnd, MSG_VSCROLL, SB_PAGEDOWN, 0);
				/*if (pLVInternalData->pItemMouseOver != NULL)
			        {//选中某条目;
			        	pLVInternalData->pItemMouseOver->bMouseOver = FALSE;
					oldfocus =pLVInternalData->nItemMouseOver;
			        }
			        else
			        {
			        	oldfocus =0;
			        }
				if(pLVInternalData->nItemMouseOver +pLVInternalData->nVisableRows <=pLVInternalData->nRows)
				{
					pLVInternalData->nItemMouseOver +=pLVInternalData->nVisableRows;
				}
				else
				{
					pLVInternalData->nItemMouseOver =pLVInternalData->nRows;
				}
				SendMessage (hwnd, MSG_VSCROLL, SB_PAGEDOWN, 0);
				
				p =pLVInternalData->nItemMouseOver;
			        SetItemMouseOver(p,pLVInternalData);
			        hdc =GetClientDC(hwnd);
			        lvStatusChangeProcess(hwnd,hdc,oldfocus,p);
			        ReleaseDC(hdc);*/
			}
 			break;
  		}
 		default:
 		break;
 		
 	}
 	
      }
      break;
	  
//add by tjb 2004-3-8
	case ELVM_SETBKCOLOR:
	{
		int oldcolor;
		oldcolor = SetWindowBkColor(hwnd, wParam);
		return oldcolor;
	}
//end add
    case MSG_DESTROY:
      lvDataDestory (pLVInternalData);
      break;
	  
	//add by tjb 2004-8-4	
	case ELVM_SETCHECKBOXIMG:		
		pLVInternalData->nCheckboxImg = (DWORD)lParam;
		break;
	//edit by tjb 2004-8-23
	case ELVM_GETROWCHECKED:
		if (dwStyle & ELVS_USECHECKBOX)
			return lvGetRowsChecked(hwnd, (int)lParam);
		else
			return 0;
	//end add
	//add by tjb 2005-8-26
	case ELVM_SETVISABLEROW:
	{
		int scrollHeight = 0;
        RECT rect;
        int scrollBoundMax;
        int scrollBoundMin = 0;
        int oldvisalberow = pLVInternalData->nFirstVisableRow;
		
		if(wParam <= 0 || wParam > pLVInternalData->nRows)
			break;
		if (wParam == oldvisalberow)
			break;
		
        GetClientRect (hwnd, &rect);
        scrollBoundMax = pLVInternalData->nRows * pLVInternalData->nItemHeight -
          					(rect.bottom - rect.top - pLVInternalData->nHeadHeight);
		
		if(scrollBoundMax <= 0)
        	break;
		
		pLVInternalData->nFirstVisableRow = wParam;
		pLVInternalData->nOriginalY = (wParam-1) * pLVInternalData->nItemHeight;
		if (pLVInternalData->nOriginalY > scrollBoundMax)
		{
			pLVInternalData->nOriginalY = scrollBoundMax;
			pLVInternalData->nFirstVisableRow = 
				pLVInternalData->nRows - pLVInternalData->nVisableRows + 1;
		}
		else if (pLVInternalData->nOriginalY < 0)
		{
			pLVInternalData->nOriginalY = scrollBoundMin;
			pLVInternalData->nFirstVisableRow = 1;
		}
		scrollHeight = -(pLVInternalData->nFirstVisableRow - oldvisalberow) 
				* pLVInternalData->nItemHeight;//为什么pagedown方向要设为负数?????
		
        if(dwStyle & ELVS_BKSCROLL)
        {
        	rect.top = rect.top + pLVInternalData->nHeadHeight + 1;
        	ScrollWindow (hwnd, 0, scrollHeight, &rect, NULL);
        }
        else
        	InvalidateRect (hwnd, NULL, FALSE);	
        
        if(dwStyle & WS_VSCROLL)
        	lstSetVScrollInfo (pLVInternalData);
		
		lvNotifyParentPos(hwnd);
		break;
	}
	//end add
	default:
		break;
    }

  return DefaultControlProc (hwnd, message, wParam, lParam);
}

BOOL RegisterEBListViewControl (void)
{
  WNDCLASS WndClass;

  WndClass.spClassName = CTRL_EBLISTVIEW;
  WndClass.dwStyle = WS_NONE;
  WndClass.dwExStyle = WS_EX_NONE;
  WndClass.hCursor = GetSystemCursor (0);
  WndClass.iBkColor = PIXEL_lightwhite;
  WndClass.WinProc = sListViewProc;

  return RegisterWindowClass (&WndClass);
}

void EBListViewControlCleanup (void)
{
  UnregisterWindowClass (CTRL_EBLISTVIEW);
}
