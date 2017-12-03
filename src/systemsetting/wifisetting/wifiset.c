/*
 ** $ld: wifiset.c 
 *
 *
 *
 ** Copyright (C) 2004 ~ 2007 Feynman Software.
 *
 ** License: GPL
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "wifiset.h"
#include "ipset.h"

#define MODE_320_240            0x00
#define MODE_240_320            0x01
#define WIFI_NETWORK_OPNE       0x01
#define WIFI_NETWORK_CLOSE      0x10
#define IDC_BTN_WIFI_OK         0x100
#define IDC_BTN_WIFI_CANCEL     0x101
#define IDC_STATIC_BAR          0x102
#define IDC_STATIC_WF_WIFI      0x105     //Wi-Fi
#define IDC_STATIC_WF_AUTOCNT   0x106     //自动连接到网络
#define IDC_BTN_WF_WIFI         0x200     //Wi-Fi

#define IDC_BTN_WF_AUTOCNT      0x210
#define IDC_LIST_WF_NET         0x401

#define IDC_PASS_SAVE           500
#define IDC_PASS_CANCEL         501
#define IDC_PASS_EDIT           502
#define IDC_PASS_OK             503

#define MSG_MOVE_WIFI          3000
#define MSG_SET_WIFI           3001
#define MSG_EXIT_PWD           3002
#define MAX_LIST_SIZE          4096 

static BOOL g_bWiFiEraseBKG;

typedef struct _WiFi_List
{
    char* pEssid;
    BOOL  bHaveKey;
    struct _WiFi_List* pNext;
}WiFi_List;

typedef WiFi_List* WiFi_List_Pt;
WiFi_List_Pt g_pWiFiListHead;
WiFi_List_Pt g_pCurWiFi;
char* g_pList;
static char g_cEssid [32];
static int g_nTotalNum = 0;
static WifiParam g_WiFiParam;
static RECT g_rcOpenCloseWiFi;
static RECT g_rcAutoConnect;

// wifi network ip parameter
static IpParam IpsetStaticIp;
static BITMAP g_WiFiBitmap;
static BITMAP g_OpenCloseBKG;
static BITMAP g_Thumber;
static BOOL g_bOpenCloseWiFi = FALSE;
static BOOL g_bOpenCloseAUTO = FALSE;
static BITMAP g_WiFiLock;
static BITMAP g_bPWDBKG; 
static BITMAP g_bPWDErr;
static BITMAP g_bPWDOK;
static BITMAP g_bPWDSAVE;
static PLOGFONT logfont;
static BITMAP g_UnselectIcon,g_SelectIcon,btnsig_png,btnarrow_png, wifi_bg_bmp,unselect_single_bar,select_single_bar;
static BITMAP wifi_ok_png, wifi_cancel_png,wifi_open_png,wifi_wifi,wifi_auto_cnt;
static BITMAP g_SelectListTop;
static BITMAP g_SelectListMid;
static BITMAP g_SelectListBtm;
static BITMAP g_UnselectListTop;
static BITMAP g_UnselectListMid;
static BITMAP g_UnselectListBtm;
static PBITMAP g_bitmap[] = {&g_SelectListTop, &g_SelectListBtm, &g_SelectListBtm};
static PBITMAP g_unslct_tip[] = {&g_UnselectIcon, &g_UnselectIcon, &g_UnselectIcon};
static HDC g_hdcBKG;
static HDC g_hPWDBKG;
char* exec;
static BITMAP g_WiFiBitmap;
static BITMAP g_PWDBitmap;
static HWND g_hPWDWnd;

static EBLVEXTDATA _listData1 =
{
    32,                   //nItemHeight            
    0,                    //nItemGap               
    0xb4a7,               //nSelectBKColor     0xb4a7,
    0x0000,               //nSelectTextColor    
    0xffff,               //nItemBKColor       
    0x8978,               //nItemTextColor     
    0xffff,               //nMouseOverBkColor   
    0x0,                  //nMouseOverTextColor
    (DWORD)0,// (DWORD)plistitemBkbmp,//nItemBKImage 
    (DWORD)0,// (DWORD)&listBkbmp     //nBKImage 
};

#if 0
static DLGTEMPLATE IDD_PASSWORD =
{
    WS_NONE,
    WS_EX_NONE,
    //0, 0, DELETE_W, DELETE_H,
    0, 0, 240, 320,
    "Dialog Password",
    0, 0,
    0, NULL,
    0
};

static CTRLDATA IDC_PASSWORD[] =
{ 
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        4, 21, 42, 21,
        IDC_PASS_SAVE,
        0,
        (DWORD)&wifi_ok_png,
    },
    {
        MGD_BUTTON,
        WS_CHILD | WS_VISIBLE, 
        196, 21, 42, 21,
        IDC_PASS_CANCEL,
        0,
        (DWORD)&wifi_cancel_png,
    },
    {
        CTRL_SLEDIT,
        WS_CHILD | WS_VISIBLE | ES_PASSWORD, 
        83, 90, 110, 20,
        IDC_PASS_EDIT,
        WS_EX_TRANSPARENT,
        0,
    },

};
#endif

static HWND AddOneMemoList(HWND hList, int nItem)
{
    EBLVSUBITEM stListViewSubItem;
    EBLVITEM stListViewItem;
    char cServiceName[256];

    memset (&cServiceName, 0, sizeof(cServiceName));
    memset(&stListViewSubItem, 0, sizeof(EBLVSUBITEM));

    stListViewItem.nItem = nItem;
    SendMessage (hList, ELVM_ADDITEM, 0, (LPARAM) &stListViewItem);	
    stListViewSubItem.nItem = nItem;

    stListViewSubItem.mask = ELV_TEXT; 

    stListViewSubItem.mask = ELV_BITMAP;//ELV_ICON; 
    stListViewSubItem.subItem = 1;
    stListViewSubItem.pszText = NULL;
    stListViewSubItem.cchTextMax = 0;
    stListViewSubItem.iImage = (DWORD)g_bitmap;//iconlist;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);


    stListViewSubItem.subItem = 2;
    stListViewSubItem.pszText = (char *)&cServiceName;
    stListViewSubItem.cchTextMax = strlen(cServiceName);
    stListViewSubItem.wordtype = NULL;
    stListViewSubItem.iImage = 0;
    stListViewSubItem.lparam = 0;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    //SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);
    stListViewSubItem.mask = ELV_BITMAP; 
    stListViewSubItem.subItem = 3;
    stListViewSubItem.pszText = NULL;
    stListViewSubItem.cchTextMax = 0;
    stListViewSubItem.iImage = (DWORD)g_bitmap;//siglist;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    stListViewSubItem.mask = ELV_BITMAP;//ELV_ICON; 
    stListViewSubItem.subItem = 4;
    stListViewSubItem.pszText = NULL;
    stListViewSubItem.cchTextMax = 0;
    stListViewSubItem.iImage = (DWORD)g_bitmap;//iconlist;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    return hList;
}


static HWND AddColumn(HWND hList, char *pname, int nItem)
{
    EBLVSUBITEM stListViewSubItem;
    EBLVITEM stListViewItem;
 
    memset(&stListViewSubItem, 0, sizeof(EBLVSUBITEM));

    stListViewItem.nItem = nItem;
    SendMessage (hList, ELVM_ADDITEM, 0, (LPARAM) &stListViewItem);	
    stListViewSubItem.nItem = nItem;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    stListViewSubItem.mask = ELV_BITMAP; 
    stListViewSubItem.subItem = 1;
    stListViewSubItem.pszText = NULL;
    stListViewSubItem.cchTextMax = 0;
    stListViewSubItem.iImage = (DWORD)g_unslct_tip;//siglist;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    stListViewSubItem.mask = ELV_TEXT; 
    stListViewSubItem.subItem = 2;
    stListViewSubItem.pszText = (char *)pname;
    stListViewSubItem.cchTextMax = strlen(pname);
    stListViewSubItem.wordtype = NULL;
    stListViewSubItem.iImage = 0;
    stListViewSubItem.lparam = 0;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    stListViewSubItem.mask = ELV_BITMAP; 
    stListViewSubItem.subItem = 3;
    stListViewSubItem.pszText = NULL;
    stListViewSubItem.cchTextMax = 0;
    stListViewSubItem.iImage = (DWORD)g_bitmap;//siglist;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    stListViewSubItem.mask = ELV_BITMAP; 
    stListViewSubItem.subItem = 4;
    stListViewSubItem.pszText = NULL;
    stListViewSubItem.cchTextMax = 0;
    stListViewSubItem.iImage = (DWORD)g_bitmap;//iconlist;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

    stListViewSubItem.mask = ELV_BITMAP; 
    stListViewSubItem.subItem = 5;
    stListViewSubItem.pszText = NULL;
    stListViewSubItem.cchTextMax = 0;
    stListViewSubItem.iImage = (DWORD)g_bitmap;//iconlist;
    SendMessage (hList, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);

   return hList;
}


char* GetWiFiInterfaceEssid (const char* pSub)
{
    if (!pSub)
        return NULL;

    char* pStart = strstr (pSub, "ESSID:\"");

    if (!pStart)
        return NULL;

    pStart = pStart + strlen ("ESSID:\"");

    char* pEnd = strstr (pStart, "\"");
    int nLen = pEnd - pStart;
    char* pValue = (char*) malloc (sizeof(char) * nLen + 1);
    
    memcpy (pValue, pStart, nLen);
    pValue[nLen] = '\0';

    return pValue;
}

BOOL GetWiFiInterfaceKey (const char* pSub)
{
    if (!pSub)
        return FALSE;

    char* pTmp = strstr (pSub, "Encryption key:");

    if (!pTmp)
        return FALSE;

    if (pTmp [strlen ("Encryption key:") + 2] == 'f')
        return FALSE;
    else
        return TRUE;
}

int GetWiFiInterfaceAP (WiFi_List** pWiFiList, const char* pStr)
{
    WiFi_List* pstTmp = NULL;
    char* pSub = NULL;
    
    if (!g_pList)
        return -1;

    pstTmp = *pWiFiList;

    if (strlen (pStr) >= 4)
        pSub = strstr (pStr, "Cell");

    if (pSub == NULL)
    {
        fprintf (stderr, "pSub is NULL, pStr %s\n", pStr);
        return 0;
    }

    while (pSub)
    {
        char* pEssid = GetWiFiInterfaceEssid (pSub);

        if (pEssid)
        {
            WiFi_List* pstWiFiTmp = (WiFi_List*) malloc (sizeof (WiFi_List));

            memset (pstWiFiTmp, 0x0, sizeof (WiFi_List));
            pstWiFiTmp->pEssid = strdup (pEssid);
            pstWiFiTmp->bHaveKey = GetWiFiInterfaceKey (pSub);

            if (!pstTmp)
                pstTmp = *pWiFiList = pstWiFiTmp;
            else
                pstTmp->pNext = pstWiFiTmp;

            pstTmp = pstWiFiTmp;

        }
        pSub = strstr (pSub + strlen ("Cell"), "Cell");
    }
    return 1;
}

void* GetWiFiInterfaceList ()
{
    int fds[2];
	int rtn;
	pid_t _childpid;
	//fix: just for ipaq
	char * proc = "/sbin/iwlist";
	struct stat procstats;

    if (stat(proc, &procstats) == -1) {
		printf("not find the iwlist process\n");
		return NULL;
	}

    if (pipe(fds))
        return NULL;

    _childpid = fork();
    // childpid is -1, if the fork failed, so print out an error message
	if (_childpid == -1) {
		printf("fork failed. the iwlist will not using\n");
		return NULL;
	}

	// childpid is a positive integer, if we are the parent, and
	// fork() worked
	if (_childpid > 0) {
        char* buf;
        int len = 0;

        close(fds[1]); /* Close unused write end */
        buf = (char*)calloc(MAX_LIST_SIZE+1, 1);
        while ((read(fds[0], &buf[len], 1) > 0) && (len<MAX_LIST_SIZE))
           len++;

        wait(&rtn);
	g_pList = buf;
    	return buf;
	} else {
        close(1);
        dup2(fds[1], 1);
        close(fds[0]);
        //system ("/home/dengkexi/Work/WiFiAPI/WiFi/wifi_test");
        system("iwlist wlan0 scan");
        _exit(EXIT_SUCCESS);
    }

    return NULL;
}

void StartSearchAPThread ()
{
    pthread_attr_t pThreadAttr;
    pthread_t tid;
    int ret;

    ret = pthread_attr_init (&pThreadAttr);
    pthread_attr_setstacksize (&pThreadAttr, 256*1024);
    ret = pthread_create (&tid, &pThreadAttr, GetWiFiInterfaceList, NULL);

}

int InitWifiInterface(void)
{
    char cName [256];
    char *pStart = NULL;
    char *pEnd = NULL;
    char gateway[32];
    char cWiFiName [16];
    int number = 0;
    int i;
    WiFi_List_Pt pLink = NULL;

    memset (cWiFiName, 0x0, sizeof (cWiFiName));
    memset (g_cEssid, 0x0, sizeof (g_cEssid));
    sprintf (cWiFiName, "wlan0");
    wifi_socket_open();
    wifi_get_interface_essid (cWiFiName, g_cEssid);
    wifi_socket_close();
    g_WiFiParam.wifi_is_open = TRUE;
    g_WiFiParam.is_auto_cnt = FALSE;
    g_WiFiParam.count_of_net = 0;//sizeof(net)/sizeof(Network);
    g_WiFiParam.cur_net = 0;
    g_pWiFiListHead = NULL;
    //g_pWiFiListHead->pEssid = NULL;
    
    StartSearchAPThread ();
    sleep (3);
    
    return 0;
}


static int InitWiFiEBListView (HWND hList)
{
    int i = 0;
    EBLVCOLOUM stListViewColumn;
    
    stListViewColumn.nCols = 1;
    stListViewColumn.pszHeadText = "none test";
    stListViewColumn.width = 24;
    SendMessage (hList, ELVM_ADDCOLUMN, 0, (LPARAM)&stListViewColumn);

    stListViewColumn.nCols = 2;
    stListViewColumn.pszHeadText = "none test";
    stListViewColumn.width = 138;
    SendMessage (hList, ELVM_ADDCOLUMN, 0, (LPARAM)&stListViewColumn);
    for (i = 3; i < 6; i++) {
        stListViewColumn.nCols = i;
        stListViewColumn.pszHeadText = "none test";
        stListViewColumn.width = 24;
        SendMessage (hList, ELVM_ADDCOLUMN, 0, (LPARAM)&stListViewColumn);
    }

#if 1 
    for(i = 0; i < g_nTotalNum; i++)
    {
        AddColumn(hList, "AddList", i + 1);
    }
#endif
    //SendMessage (hList, ELVM_SETITEMSELECTED, 0, 1);
    //SetFocus (hList); 

    return 1;
}

    
static void WiFiDrawEBListViewItem (HWND hWnd, HDC hdc, void* context)
{
    ITEM_DRAW_CON* drawcontext =(ITEM_DRAW_CON *)context;
    RECT  rect;
    WiFi_List_Pt pWiFiItem;
    int i = 0;

    if(!drawcontext)
        return ;

    rect =drawcontext->paint_area;
    SetBkMode(hdc,BM_TRANSPARENT);

    if(1 == drawcontext->row && 1 ==drawcontext->total_rows){
        if(ROWSTATE_SELECTED == drawcontext->state)
        {
            FillBoxWithBitmap (hdc, rect.left, rect.top, 234, 32, &select_single_bar);
            if (!strcmp (g_cEssid, g_pWiFiListHead->pEssid))
                FillBoxWithBitmap (hdc, rect.left + 6, rect.top + 12, 14, 11, &g_SelectIcon);
        }
        else
        {
            FillBoxWithBitmap (hdc, rect.left, rect.top, 234, 32, &unselect_single_bar);
            if (!strcmp (g_cEssid, g_pWiFiListHead->pEssid))
                FillBoxWithBitmap (hdc, rect.left + 6, rect.top + 12, 14, 11, &g_UnselectIcon);
        }
        if (g_pWiFiListHead->bHaveKey)
            FillBoxWithBitmap (hdc, rect.left + 162, rect.top + 8, 11, 14, &g_WiFiLock);
        FillBoxWithBitmap (hdc, rect.left + 185, rect.top + 8, 16, 13, &btnsig_png);
        FillBoxWithBitmap (hdc, rect.left + 205, rect.top + 5, 24, 24, &btnarrow_png);
        TextOut(hdc, rect.left + 27, rect.top + 13, g_pWiFiListHead->pEssid);//just only one net
    }
    else if(1 == drawcontext->row)
    {
        if(ROWSTATE_SELECTED == drawcontext->state)
        {
            FillBoxWithBitmap (hdc, rect.left, rect.top, 234, 32, &g_SelectListTop);
            if (!strcmp (g_cEssid, g_pWiFiListHead->pEssid))
                FillBoxWithBitmap (hdc, rect.left + 6, rect.top + 12, 14, 11, &g_SelectIcon);
        }
        else
        {
            FillBoxWithBitmap (hdc, rect.left, rect.top, 234, 32, &g_UnselectListTop);
            if (!strcmp (g_cEssid, g_pWiFiListHead->pEssid))
                FillBoxWithBitmap (hdc, rect.left + 6, rect.top + 12, 14, 11, &g_UnselectIcon);
        }

        if (g_pWiFiListHead->bHaveKey)
            FillBoxWithBitmap (hdc, rect.left + 162, rect.top + 8, 11, 14, &g_WiFiLock);
        FillBoxWithBitmap (hdc, rect.left + 185, rect.top + 8, 16, 13, &btnsig_png);
        FillBoxWithBitmap (hdc, rect.left + 205, rect.top + 5, 24, 24, &btnarrow_png);
        TextOut(hdc, rect.left + 27, rect.top + 13, g_pWiFiListHead->pEssid);//just only one net
    }
    else if(drawcontext->row < drawcontext->total_rows){
#if 1 
        pWiFiItem = g_pWiFiListHead->pNext;
        for (i = 2; i <= drawcontext->row; i++) 
            pWiFiItem = pWiFiItem->pNext;
#endif
        if(ROWSTATE_SELECTED == drawcontext->state)
        {
            FillBoxWithBitmap (hdc, rect.left, rect.top, 234, 32, &g_SelectListMid);
            if (!strcmp (g_cEssid, pWiFiItem->pEssid))
                FillBoxWithBitmap (hdc, rect.left + 6, rect.top + 12, 14, 11, &g_SelectIcon);
        }
        else
        {
            FillBoxWithBitmap (hdc, rect.left, rect.top, 234, 32, &g_UnselectListMid);
            if (!strcmp (g_cEssid, pWiFiItem->pEssid))
                FillBoxWithBitmap (hdc, rect.left + 6, rect.top + 12, 14, 11, &g_UnselectIcon);
        }
        if (pWiFiItem->bHaveKey)
            FillBoxWithBitmap (hdc, rect.left + 162, rect.top + 8, 11, 14, &g_WiFiLock);
        FillBoxWithBitmap (hdc, rect.left + 185, rect.top + 8, 16, 13, &btnsig_png);
        FillBoxWithBitmap (hdc, rect.left + 205, rect.top + 5, 24, 24, &btnarrow_png);
        TextOut(hdc, rect.left + 27, rect.top + 13, pWiFiItem->pEssid);//just only one net
   }
    else if(drawcontext->row == drawcontext->total_rows){
#if 1 
        pWiFiItem = g_pWiFiListHead->pNext;
        
        while (pWiFiItem->pNext)
            pWiFiItem = pWiFiItem->pNext;
#endif
        if(ROWSTATE_MOUSEOVER == drawcontext->state)
        {
            FillBoxWithBitmap (hdc, rect.left, rect.top, 234, 32, &g_SelectListBtm);
            if (!strcmp (g_cEssid, pWiFiItem->pEssid))
                FillBoxWithBitmap (hdc, rect.left + 6, rect.top + 12, 14, 11, &g_SelectIcon);
        }
        else
        {
            FillBoxWithBitmap (hdc, rect.left, rect.top, 234, 32, &g_UnselectListBtm);
            if (!strcmp (g_cEssid, pWiFiItem->pEssid))
                FillBoxWithBitmap (hdc, rect.left + 6, rect.top + 12, 14, 11, &g_UnselectIcon);
        }
        if (pWiFiItem->bHaveKey)
            FillBoxWithBitmap (hdc, rect.left + 162, rect.top + 8, 11, 14, &g_WiFiLock);
        FillBoxWithBitmap (hdc, rect.left + 185, rect.top + 8, 16, 13, &btnsig_png);
        FillBoxWithBitmap (hdc, rect.left + 205, rect.top + 5, 24, 24, &btnarrow_png);
        TextOut(hdc, rect.left + 27, rect.top + 13, pWiFiItem->pEssid);//just only one net
   }
}


#if 0

void* SetWiFiInterfaceList ()
{
    int fd[2];
	int rtn;
	pid_t _childpid;
	//fix: just for ipaq
	char * proc = "/sbin/iwconfig";
	struct stat procstats;
    
    if (stat(proc, &procstats) == -1) {
		printf("not find the iwlist process\n");
		return NULL;
	}

    if (pipe(fd))
        return NULL;

    _childpid = fork();
    // childpid is -1, if the fork failed, so print out an error message
	if (_childpid == -1) {
		printf("fork failed. the iwlist will not using\n");
		return NULL;
	}

	// childpid is a positive integer, if we are the parent, and
	// fork() worked
	if (_childpid > 0) {
        char* buf;
        int len = 0;

        close(fd[1]); /* Close unused write end */
        buf = (char*)calloc(MAX_LIST_SIZE+1, 1);
        while ((read(fd[0], &buf[len], 1) > 0) && (len<MAX_LIST_SIZE))
           len++;

        wait(&rtn);
        if (len)
		    g_pSetWiFi = buf;
        else
            free (buf);
        
        fprintf (stderr, "g_pSetWiFi %s, len %d\n", g_pSetWiFi, len);

		return buf;
	} else {
        close(1);
        dup2(fd[1], 1);
        close(fd[0]);
        system ("iwconfig wlan0 essid SZFM key 8618203940");
        //system (exec);
        _exit(EXIT_SUCCESS);
    }

    return NULL;
}


void StartSetAPThread ()
{
    pthread_attr_t pThreadAttr;
    pthread_t tid;
    int ret;

    ret = pthread_attr_init (&pThreadAttr);
    pthread_attr_setstacksize (&pThreadAttr, 256*1024);
    ret = pthread_create (&tid, &pThreadAttr, SetWiFiInterfaceList, NULL);

}
#endif
static int PassWordErrorProc (HWND hWnd, int nMessage, WPARAM wParam, LPARAM lParam)
{
    switch (nMessage)
    {
        case MSG_CREATE:
            {
                 CreateWindow ("MGD_BUTTON","",
                              WS_CHILD | WS_VISIBLE,
                              IDC_PASS_OK,
                              41, 178, 157, 25,
                              hWnd, 
                              (DWORD)&g_bPWDOK);
            }
            break;
        case MSG_COMMAND:
        case BUTTON_KEYDOWN:
            {
                switch (LOWORD(wParam))
                {
                    case IDC_PASS_OK:
                        SendMessage (hWnd, MSG_CLOSE, 0, 0);
                        break;
                    default:
                        break;
                }
            }
            return 0;
        case MSG_CLOSE:
             DestroyMainWindow (hWnd);
             return 0;
        case MSG_ERASEBKGND:
            {
                HDC hdc = (HDC)wParam;
                const RECT* clip = (const RECT*) lParam;
                BOOL fGetDC = FALSE;
                RECT rcTemp;

                if (hdc == 0)
                {
                    //hdc = GetClientDC (hWnd);
                    hdc = GetSecondaryClientDC (hWnd);
                    fGetDC = TRUE;
                }
                if (clip) 
                {
                    rcTemp = *clip;
                    ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
                    ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
                    IncludeClipRect (hdc, &rcTemp);

                    //BitBlt (g_hPWDBKG, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, hdc, rcTemp.left, rcTemp.top, 0 );
                }
                FillBoxWithBitmap (hdc, 0, 0, 240, 320, &g_bPWDSAVE);
                FillBoxWithBitmap (hdc, 0, 0, 240, 320, &g_bPWDErr);

                if (fGetDC) {
                    //ReleaseDC (hdc);
                    ReleaseSecondaryDC(hWnd, hdc);
                }
                return 0;

            }
    }
    return DefaultMainWinProc (hWnd, nMessage, wParam, lParam);
}

static void InitPassWordErrorCreateInfo(PMAINWINCREATE createInfo)
{
    createInfo->dwStyle = WS_NONE;
    createInfo->dwExStyle = WS_EX_NONE;
    createInfo->spCaption = "";
    createInfo->hMenu = 0;
    createInfo->hCursor = GetSystemCursor (0);
    createInfo->hIcon = 0;
    createInfo->MainWindowProc = PassWordErrorProc;
    createInfo->lx = 0;
    createInfo->ty = 0;
    createInfo->rx = 240;
    createInfo->by = 320;

    createInfo->iBkColor = PIXEL_lightwhite;
    createInfo->dwAddData = 0;
    createInfo->hHosting = HWND_DESKTOP;
}

int CreatePassWordErrorWindow (void)
{
    MAINWINCREATE CreateInfo;
    HWND hMainwnd;
    MSG Msg;

    InitPassWordErrorCreateInfo(&CreateInfo);
 
    hMainwnd = CreateMainWindow(&CreateInfo);
    if (hMainwnd == HWND_INVALID)
        return -1;
     ShowWindow (hMainwnd, SW_SHOWNORMAL);
     while (GetMessage (&Msg, hMainwnd)){
         TranslateMessage (&Msg);
         DispatchMessage (&Msg);
    }
    //DeleteCompatibleDC (g_hdcpsw);
    MainWindowThreadCleanup(hMainwnd);
    return 0;
}

static int PassWordProc (HWND hWnd, int nMessage, WPARAM wParam, LPARAM lParam)
{
    switch (nMessage)
    {
        case MSG_CREATE:
             CreateWindow ("MGD_BUTTON","",
                    WS_CHILD | WS_VISIBLE,
                    IDC_PASS_CANCEL,
                    4, 21, 42, 21,
                    hWnd, 
                    (DWORD)&wifi_cancel_png);
             CreateWindow ("MGD_BUTTON", "",
                    WS_CHILD | WS_VISIBLE,
                    IDC_PASS_SAVE,
                    196, 21,40, 21,
                    hWnd, 
                    (DWORD)&wifi_ok_png);
  
             CreateWindowEx (CTRL_SLEDIT, "",
                    WS_CHILD | WS_VISIBLE | ES_PASSWORD,
                    WS_EX_TRANSPARENT,
                    IDC_PASS_EDIT,
                    83,90,110, 20,
                    hWnd, 0);
            //SetNotificationCallback (editwnd, edit_notify_proc);
            return 0;
        case MSG_COMMAND:
        case BUTTON_KEYDOWN:
            {
                switch (LOWORD(wParam))
                {
                    case IDC_PASS_SAVE:
                        {
                            char buff [32];
                            int m;

                            memset (buff, 0x0, sizeof (buff));
                            GetWindowText(GetDlgItem(hWnd, IDC_PASS_EDIT), buff, sizeof(buff));         
                            asprintf (&exec, "iwconfig wlan0 essid %s key %s", g_pCurWiFi->pEssid, buff); 
#if 1 
                            m = system (exec);
                            free (exec);
                            fprintf (stderr, "m is %d\n", m);
#endif
                            if (m != 0)
                            {
                                HDC hdc;
                                //hdc = GetClientDC (hWnd);
                                hdc = GetSecondaryClientDC (hWnd);
                                GetBitmapFromDC (hdc, 0, 0, 240, 320, &g_bPWDSAVE);
                                //ReleaseDC (hdc);
                                ReleaseSecondaryDC (hWnd, hdc);
                                CreatePassWordErrorWindow ();
                                UnloadBitmap (&g_bPWDSAVE);
                            }
                            else
                            {
                                SendMessage(hWnd, MSG_CLOSE, 0, 0);
                            }
                         }      
                        break;
                    case IDC_PASS_CANCEL:
                        SendMessage(hWnd, MSG_CLOSE, 0, 0);
                        break;
                    default:
                        break;
                }
            }
            break;
        
        case MSG_CLOSE:
            CloseSYSIMEMethod (FALSE);
            SendMessage (g_hWiFiMainMenu, MSG_EXIT_PWD, 0, 0);
            return 0;
        case MSG_SYS_CLOSE:
             DestroyMainWindow (hWnd);
             return 0;
        case MSG_ERASEBKGND:
            {
                HDC hdc = (HDC)wParam;
                const RECT* clip = (const RECT*) lParam;
                BOOL fGetDC = FALSE;
                RECT rcTemp;

                if (hdc == 0)
                {
                    //hdc = BeginPaint (hWnd);//GetClientDC (hWnd);
                    hdc = GetSecondaryClientDC (hWnd);
                    fGetDC = TRUE;
                }
                if (clip) 
                {
                    rcTemp = *clip;
                    ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
                    ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
                    IncludeClipRect (hdc, &rcTemp);
                }
                BitBlt (g_hPWDBKG, 0, 0, 0, 0, hdc, 0, 0, 0 );

                if (fGetDC)
                    //EndPaint (hWnd, hdc);//ReleaseDC (hdc);
                    ReleaseSecondaryDC(hWnd, hdc);
                return 0;

            }
    }
    return DefaultMainWinProc (hWnd, nMessage, wParam, lParam);
}

static void InitPassWordCreateInfo(PMAINWINCREATE createInfo)
{
    createInfo->dwStyle = WS_NONE;
    createInfo->dwExStyle = WS_EX_AUTOSECONDARYDC;
    createInfo->spCaption = "";
    createInfo->hMenu = 0;
    createInfo->hCursor = GetSystemCursor (0);
    createInfo->hIcon = 0;
    createInfo->MainWindowProc = PassWordProc;
    createInfo->lx = 0;
    createInfo->ty = 0;
    createInfo->rx = 240;
    createInfo->by = 320;

    createInfo->iBkColor = PIXEL_lightwhite;
    createInfo->dwAddData = 0;
    createInfo->hHosting = HWND_DESKTOP;
}

static int CreatePassWordWindow (void)
{
    MAINWINCREATE CreateInfo;
    HWND hMainwnd;
    MSG Msg;

    InitPassWordCreateInfo(&CreateInfo);
 
    return CreateMainWindow(&CreateInfo);
#if 0 
    ShowWindow (hMainwnd, SW_SHOWNORMAL);
     while (GetMessage (&Msg, hMainwnd)){
         TranslateMessage (&Msg);
         DispatchMessage (&Msg);
    }
    //DeleteCompatibleDC (g_hdcpsw);
    MainWindowThreadCleanup(hMainwnd);
    return 0;
#endif
}
static void WiFiEBListViewProc (HWND hwnd, int id, int code, DWORD addData)
{
    PEBLSTVWDATA pListdata = (PEBLSTVWDATA)addData;
    PEBITEMDATA pCell = NULL;
    static int count = 0;
    if(pListdata){
        pCell = pListdata->pItemSelected;
        if(5 == pCell->nCols)
        {
            WiFi_List_Pt pWiFiItem;
            int i;

            g_WiFiParam.cur_net = pCell->nRows - 1;

            if (pCell->nRows == 1)
                pWiFiItem = g_pWiFiListHead;
            else
            {
                pWiFiItem = g_pWiFiListHead;
                for (i = 2; i <= pCell->nRows; i++)
                    pWiFiItem = pWiFiItem->pNext;
            }
            if (pWiFiItem->bHaveKey)
            {
                g_pCurWiFi = pWiFiItem;
                g_hPWDWnd = CreatePassWordWindow ();
                {
                    HDC hdc;
                    hdc = GetSecondaryDC (g_hPWDWnd);
                    SetSecondaryDC(g_hPWDWnd, hdc, DoubleBufferProc);
                    UpdateAll (g_hPWDWnd, TRUE);
                    GetBitmapFromDC (hdc, 0, 0, 240, 320, &g_PWDBitmap);
                    SetSecondaryDC(g_hPWDWnd, hdc, NULL);
                    GetBitmapFromDC (HDC_SCREEN, 0, 0, 240, 320, &g_WiFiBitmap);
                }
	        PushPullBitmap (g_hDoubleBuffer, &g_rcSysSetting, &g_PWDBitmap, &g_WiFiBitmap, 5, FALSE);
           //     ShowWindow (g_hPWDWnd, SW_SHOWNORMAL);
               IncludeWindowStyle (g_hPWDWnd, WS_VISIBLE);
               SendMessage (HWND_DESKTOP, MSG_MOVETOTOPMOST, (WPARAM)g_hPWDWnd, 0);
               AnimateMoveBackUp (g_hPWDWnd);
                UnloadBitmap (&g_PWDBitmap);
            }
            else
            {
                int m;
                asprintf (&exec, "iwconfig wlan0 essid %s", pWiFiItem->pEssid); 
                m = system (exec);
                free (exec);
                fprintf (stderr, "save m is %d\n", m);
                SendMessage (g_hWiFiMainMenu, MSG_EXIT_PWD, 0, 0);
            }
        }
    }
}



static void UnloadWiFiBitmap (void)
{
    UnloadBitmap (&g_SelectListTop);
    UnloadBitmap (&g_UnselectListTop);
    UnloadBitmap (&g_SelectListMid);
    UnloadBitmap (&g_UnselectListMid);
    UnloadBitmap (&g_SelectListBtm);
    UnloadBitmap (&g_UnselectListBtm);
    UnloadBitmap (&g_WiFiBitmap);
    UnloadBitmap (&wifi_bg_bmp);
    UnloadBitmap (&wifi_wifi);
    UnloadBitmap (&wifi_auto_cnt);
    UnloadBitmap (&g_OpenCloseBKG);
    UnloadBitmap (&g_Thumber);
    UnloadBitmap (&wifi_ok_png);
    UnloadBitmap (&wifi_cancel_png);
    UnloadBitmap (&btnarrow_png);
    UnloadBitmap (&btnsig_png);
    UnloadBitmap (&g_UnselectIcon);
    UnloadBitmap (&g_SelectIcon);
    UnloadBitmap (&unselect_single_bar);
    UnloadBitmap (&select_single_bar);
    UnloadBitmap (&wifi_open_png);
    UnloadBitmap (&g_WiFiLock);
    UnloadBitmap (&g_bPWDBKG);
    UnloadBitmap (&g_bPWDErr);
    UnloadBitmap (&g_bPWDOK);
}

int GetSearchAPNum (void)
{
    return g_nTotalNum;
}

static int WifiWinProc(HWND hWnd, int nMessage, WPARAM wParam, LPARAM lParam)
{
    HWND wifibtnwnd, autocntbtnwnd,okbtnwnd, cancelbtnwnd,hListview;  

    switch (nMessage){
    
      case MSG_CREATE:
          g_hWiFiMainMenu = hWnd;
          cancelbtnwnd = CreateWindow (MGD_BUTTON, "",
              WS_VISIBLE ,//| BS_DEFPUSHBUTTON,
              IDC_BTN_WIFI_CANCEL,
              4, 6, 42, 21,
              hWnd, 
              (DWORD)(&wifi_cancel_png));
          okbtnwnd = CreateWindow (MGD_BUTTON, "",
              WS_CHILD | WS_VISIBLE,// | BS_DEFPUSHBUTTON,
              IDC_BTN_WIFI_OK,
              196, 6, 40 , 21,
              hWnd,
              (DWORD)(&wifi_ok_png));

          {
              WiFi_List_Pt pLink;
              g_pWiFiListHead = NULL;
              int nRet = GetWiFiInterfaceAP (&g_pWiFiListHead, g_pList);
              g_nTotalNum = 0;

              if (nRet > 0)
              {
                  if (g_pWiFiListHead) 
                      g_nTotalNum = 1;

                  pLink = g_pWiFiListHead;
                  pLink = pLink->pNext;

                  while (pLink)
                  {
                      g_nTotalNum ++;
                      pLink = pLink->pNext;
                  }
              }
              
              if (g_nTotalNum == 0)
                  SendMessage (hWnd, MSG_SYS_CLOSE, 0, 0);
          }
          SetTimer (hWnd, MSG_MOVE_WIFI, 5);
          g_bWiFiEraseBKG = FALSE;
          //g_nTotalNum = 4;
          hListview = CreateWindowEx (CTRL_EBLISTVIEW, "wifinetlist",
                    WS_CHILD | WS_VISIBLE | ELVS_TYPE3STATE | ELVS_ITEMUSERCOLOR
                    | ELVS_BKIMAGELEFTTOP | ELVS_BKBITMAP | ELVS_BKALPHA, 
                    WS_EX_TRANSPARENT, IDC_LIST_WF_NET, 3, 96, 234, 128, 
                    hWnd, (DWORD)&_listData1);
          SendMessage (hListview, ELVM_SET_ITEMDRAWCALLBACK, (WPARAM)WiFiDrawEBListViewItem, 0);//item draw,hejian
          SetWindowBkColor(hListview, COLOR_lightwhite);
          SetNotificationCallback (hListview, WiFiEBListViewProc);
          InitWiFiEBListView (hListview);
          return 0;
      case MSG_EXIT_PWD:
          {
              char cWiFiName [64];
              
              memset (cWiFiName, 0x0, sizeof (cWiFiName));
              memset (g_cEssid, 0x0, sizeof (g_cEssid));
              sprintf (cWiFiName, "wlan0");
              wifi_socket_open();
              wifi_get_interface_essid (cWiFiName, g_cEssid);
              wifi_socket_close();
              SendMessage (GetDlgItem (hWnd, IDC_LIST_WF_NET), MSG_PAINT, 0, 0);
          }
          GetBitmapFromDC (HDC_SCREEN, 0, 0, 240, 320, &g_PWDBitmap);
	  PushPullBitmap (g_hDoubleBuffer, &g_rcSysSetting, &g_WiFiBitmap, &g_PWDBitmap, 5, TRUE);
          g_bWiFiEraseBKG = TRUE;
          ShowWindow (g_hPWDWnd, SW_HIDE);
          UnloadBitmap (&g_PWDBitmap);
          UnloadBitmap (&g_WiFiBitmap);
          SendMessage (g_hPWDWnd, MSG_SYS_CLOSE, 0, 0);
          return 0;
      case MSG_PAINT:
          {
              HDC hdc;
              hdc = BeginPaint (hWnd);
              if (g_WiFiParam.wifi_is_open)
              {
                  g_rcOpenCloseWiFi.left  = 192;
                  g_rcOpenCloseWiFi.top   = 39;
                  g_rcOpenCloseWiFi.right = g_rcOpenCloseWiFi.left + 36;
                  g_rcOpenCloseWiFi.bottom = g_rcOpenCloseWiFi.top + 19;
                  
                  FillBoxWithBitmap (hdc, g_rcOpenCloseWiFi.left, g_rcOpenCloseWiFi.top, 36, 19, &g_Thumber);
              }
              else
              {
                  g_rcOpenCloseWiFi.left  = 160;
                  g_rcOpenCloseWiFi.top   = 39;
                  g_rcOpenCloseWiFi.right = g_rcOpenCloseWiFi.left + 36;
                  g_rcOpenCloseWiFi.bottom = g_rcOpenCloseWiFi.top + 19;
                  FillBoxWithBitmap (hdc, g_rcOpenCloseWiFi.left, g_rcOpenCloseWiFi.top, 36, 19, &g_Thumber);
              }
              
              if (g_WiFiParam.is_auto_cnt)
              {
                  g_rcAutoConnect.left  = 192;
                  g_rcAutoConnect.top   = 240;
                  g_rcAutoConnect.right = g_rcAutoConnect.left + 36;
                  g_rcAutoConnect.bottom = g_rcAutoConnect.top + 19;
                  
                  FillBoxWithBitmap (hdc, g_rcAutoConnect.left, g_rcAutoConnect.top, 36, 19, &g_Thumber);
              }
              else
              {
                  g_rcAutoConnect.left  = 160;
                  g_rcAutoConnect.top   = 240;
                  g_rcAutoConnect.right = g_rcAutoConnect.left + 36;
                  g_rcAutoConnect.bottom = g_rcAutoConnect.top + 19;
                  FillBoxWithBitmap (hdc, g_rcAutoConnect.left, g_rcAutoConnect.top, 36, 19, &g_Thumber);
              }



              EndPaint (hWnd, hdc);
          }
          return 0;
      case MSG_LBUTTONDOWN:
          {
             int x = LOSWORD (lParam); 
             int y = HISWORD (lParam);
             
             if (PtInRect (&g_rcOpenCloseWiFi, x, y) & g_WiFiParam.wifi_is_open)
             {
                 g_bOpenCloseWiFi = TRUE;
                 g_WiFiParam.wifi_is_open = FALSE;
             }
             else if(PtInRect (&g_rcOpenCloseWiFi, x, y) & (!g_WiFiParam.wifi_is_open))
             {
                 g_bOpenCloseWiFi = TRUE;
                 g_WiFiParam.wifi_is_open = TRUE;
             }

             if (PtInRect (&g_rcAutoConnect, x, y) & g_WiFiParam.is_auto_cnt)
             {
                 g_bOpenCloseAUTO = TRUE;
                 g_WiFiParam.is_auto_cnt = FALSE;
             }
             else if(PtInRect (&g_rcAutoConnect, x, y) & (!g_WiFiParam.is_auto_cnt))
             {
                 g_bOpenCloseAUTO = TRUE;
                 g_WiFiParam.is_auto_cnt = TRUE;
             }

          }
          break;
      case MSG_TIMER:
          {
              if (g_bOpenCloseWiFi)
              {
                 HDC hdc;
                 hdc = GetClientDC (hWnd);
                 if (g_WiFiParam.wifi_is_open) 
                 {
                     BitBlt (g_hdcBKG, g_rcOpenCloseWiFi.left, 
                             g_rcOpenCloseWiFi.top, 4, 
                             19, hdc, g_rcOpenCloseWiFi.left,
                             g_rcOpenCloseWiFi.top, 0);
                     
                     g_rcOpenCloseWiFi.left = g_rcOpenCloseWiFi.left + 2;
                     g_rcOpenCloseWiFi.right = g_rcOpenCloseWiFi.right + 2;

                     FillBoxWithBitmap (hdc, g_rcOpenCloseWiFi.left, g_rcOpenCloseWiFi.top, 36, 19, &g_Thumber);
                     if (g_rcOpenCloseWiFi.left == 192)
                     {
                         g_bOpenCloseWiFi = FALSE;
                     }
                 }
                 else
                 {
                     BitBlt (g_hdcBKG, g_rcOpenCloseWiFi.right - 4, 
                             g_rcOpenCloseWiFi.top, 4, 
                             19, hdc, g_rcOpenCloseWiFi.right - 4,
                             g_rcOpenCloseWiFi.top, 0);
                     
                     g_rcOpenCloseWiFi.left = g_rcOpenCloseWiFi.left - 2;
                     g_rcOpenCloseWiFi.right = g_rcOpenCloseWiFi.right - 2;

                     FillBoxWithBitmap (hdc, g_rcOpenCloseWiFi.left, g_rcOpenCloseWiFi.top, 36, 19, &g_Thumber);
                     if (g_rcOpenCloseWiFi.left == 160)
                     {
                         g_bOpenCloseWiFi = FALSE;
                     }
                 }
                 ReleaseDC (hdc);
              }

              if (g_bOpenCloseAUTO)
              {
                 HDC hdc;
                 hdc = GetClientDC (hWnd);
                 if (g_WiFiParam.is_auto_cnt) 
                 {
                     BitBlt (g_hdcBKG, g_rcAutoConnect.left, 
                             g_rcAutoConnect.top, 4, 
                             19, hdc, g_rcAutoConnect.left,
                             g_rcAutoConnect.top, 0);
                     
                     g_rcAutoConnect.left = g_rcAutoConnect.left + 2;
                     g_rcAutoConnect.right = g_rcAutoConnect.right + 2;

                     FillBoxWithBitmap (hdc, g_rcAutoConnect.left, g_rcAutoConnect.top, 36, 19, &g_Thumber);
                     if (g_rcAutoConnect.left == 192)
                     {
                         g_bOpenCloseAUTO = FALSE;
                     }
                 }
                 else
                 {
                     BitBlt (g_hdcBKG, g_rcAutoConnect.right - 4, 
                             g_rcAutoConnect.top, 4, 
                             19, hdc, g_rcAutoConnect.right - 4,
                             g_rcAutoConnect.top, 0);
                     
                     g_rcAutoConnect.left = g_rcAutoConnect.left - 2;
                     g_rcAutoConnect.right = g_rcAutoConnect.right - 2;

                     FillBoxWithBitmap (hdc, g_rcAutoConnect.left, g_rcAutoConnect.top, 36, 19, &g_Thumber);
                     if (g_rcAutoConnect.left == 160)
                     {
                         g_bOpenCloseAUTO = FALSE;
                     }
                 }
                 ReleaseDC (hdc);
              }
          }
          break;
#if 0
      case MSG_SHOW_PWD:
          {
              IDD_PASSWORD.controls = IDC_PASSWORD;
              IDD_PASSWORD.controlnr = sizeof (IDC_PASSWORD) / sizeof (IDC_PASSWORD[0]);

              DialogBoxIndirectParam (&IDD_PASSWORD, HWND_DESKTOP, (WNDPROC)PassWordProc, 0L);
          }
          return 0;
#endif
      case MSG_COMMAND:
      case BUTTON_KEYDOWN: 
          {
              int id;
              id = LOWORD(wParam);
              switch(id)
              {
                  case IDC_BTN_WIFI_OK:

                        SendMessage(hWnd, MSG_CLOSE, 0, 0);
                        break;
                  case IDC_BTN_WIFI_CANCEL:

                        SendMessage(hWnd, MSG_CLOSE, 0,0);
                        break;
                  default:

                        break;
              }
              return 0;
            }
     case MSG_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            const RECT* clip = (const RECT*) lParam;
            BOOL fGetDC = FALSE;
            RECT rcTemp;

            if (g_bWiFiEraseBKG)
            {
                g_bWiFiEraseBKG = FALSE;
                return 0;
            }

            if (hdc == 0){
                //hdc = GetClientDC (hWnd);
                hdc = GetSecondaryClientDC (hWnd);
                fGetDC = TRUE;
            }
            if (clip){
                rcTemp = *clip;
                ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
                ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
                IncludeClipRect (hdc, &rcTemp);
            }
            BitBlt (g_hdcBKG, 0, 0, 0, 0, hdc, 0, 0, 0 );
            if (fGetDC)
                //ReleaseDC (hdc);
                ReleaseSecondaryDC (hWnd, hdc);
            return 0;
        }
      case MSG_CLOSE:
        SendMessage (g_hSysMain, MSG_CLOSE_APP, 0, 1);
        return 0;
      case MSG_SYS_CLOSE:
        UnloadWiFiBitmap ();
        DeleteCompatibleDC (g_hdcBKG);
        DeleteCompatibleDC (g_hPWDBKG);
        free (g_pList);
        KillTimer (hWnd, MSG_MOVE_WIFI);
        DestroyMainWindow (hWnd);
    
        return 0; 
    }
    return DefaultMainWinProc (hWnd, nMessage, wParam, lParam);
}


static void InitWifiBitmap(void)
{
    LoadBitmap (HDC_SCREEN, &g_WiFiBitmap, ANIMATE_RES"LocalNetwork.bmp");
    LoadBitmap (HDC_SCREEN, &wifi_bg_bmp, WIFISETTING_RES"WiFiBKG.png");
    LoadBitmap (HDC_SCREEN, &wifi_wifi, WIFISETTING_RES"WiFiString.png");
    LoadBitmap (HDC_SCREEN, &wifi_auto_cnt, WIFISETTING_RES"WiFiAutoCNet.png");  
    LoadBitmap (HDC_SCREEN, &g_OpenCloseBKG, WIFISETTING_RES"OpenCloseBKG.png");
    LoadBitmap (HDC_SCREEN, &g_Thumber, WIFISETTING_RES"Thumber.bmp");
    LoadBitmap (HDC_SCREEN, &wifi_ok_png, SYSTEM_RES"SaveButton.png");
    LoadBitmap (HDC_SCREEN, &wifi_cancel_png, SYSTEM_RES"CancelButton.png");
    LoadBitmap (HDC_SCREEN, &btnarrow_png, WIFISETTING_RES"WiFiArrow.png");
    LoadBitmap (HDC_SCREEN, &btnsig_png, WIFISETTING_RES"WifiSelectSignal_ONE.png");
    LoadBitmap (HDC_SCREEN, &g_UnselectIcon, WIFISETTING_RES"WiFiUnselectIcon.png");
    LoadBitmap (HDC_SCREEN, &g_SelectIcon, WIFISETTING_RES"WiFiSelectIcon.png");
    LoadBitmap (HDC_SCREEN, &unselect_single_bar, WIFISETTING_RES"Unselect_Single.png");
    LoadBitmap (HDC_SCREEN, &select_single_bar, WIFISETTING_RES"Select_Single.png");
    LoadBitmap (HDC_SCREEN, &wifi_open_png, WIFISETTING_RES"WiFiOpen.png");
    LoadBitmap (HDC_SCREEN, &g_SelectListTop, WIFISETTING_RES"SelectListTop.png");
    LoadBitmap (HDC_SCREEN, &g_UnselectListTop, WIFISETTING_RES"UnselectListTop.png");
    LoadBitmap (HDC_SCREEN, &g_SelectListMid, WIFISETTING_RES"SelectListMiddle.png");
    LoadBitmap (HDC_SCREEN, &g_UnselectListMid, WIFISETTING_RES"UnselectListMiddle.png");
    LoadBitmap (HDC_SCREEN, &g_SelectListBtm, WIFISETTING_RES"SelectListBottom.png");
    LoadBitmap (HDC_SCREEN, &g_UnselectListBtm, WIFISETTING_RES"UnselectListBottom.png");
    LoadBitmap (HDC_SCREEN, &g_WiFiLock, WIFISETTING_RES"WiFiLock.png");
    LoadBitmap (HDC_SCREEN, &g_bPWDBKG, WIFISETTING_RES"PasswordBKG.png");
    LoadBitmap (HDC_SCREEN, &g_bPWDErr, WIFISETTING_RES"PasswordErrorBKG.png");
    LoadBitmap (HDC_SCREEN, &g_bPWDOK, WIFISETTING_RES"PasswordOK.png");
}


static void InitWifiCreateInfo(PMAINWINCREATE createInfo)
{
    createInfo->dwStyle = WS_NONE;
    createInfo->dwExStyle = WS_EX_AUTOSECONDARYDC;
    createInfo->spCaption = "";
    createInfo->hMenu = 0;
    createInfo->hCursor = GetSystemCursor (0);
    createInfo->hIcon = 0;
    createInfo->MainWindowProc = WifiWinProc;
    createInfo->lx = 0;
    createInfo->ty = 0;
    createInfo->rx = 240;
    createInfo->by = 320;
    createInfo->iBkColor = PIXEL_lightwhite;
    createInfo->dwAddData = 0;
    createInfo->hHosting = HWND_DESKTOP;
}

HWND CreateWIFIWindow (void)
{
    MAINWINCREATE createInfo;
    
    InitWifiCreateInfo(&createInfo);
    InitWifiInterface(); 
    InitWifiBitmap();


    g_Thumber.bmType = BMP_TYPE_COLORKEY;
    g_Thumber.bmColorKey = GetPixelInBitmap (&g_Thumber, 0, 0);
    
    g_hdcBKG = CreateCompatibleDC (HDC_SCREEN);
    g_hPWDBKG = CreateCompatibleDC (HDC_SCREEN);
    FillBoxWithBitmap (g_hPWDBKG, 0, 0, 240, 320, &g_bPWDBKG);
    
    FillBoxWithBitmap (g_hdcBKG, 0, 0, 240, 320, &wifi_bg_bmp);
    FillBoxWithBitmap (g_hdcBKG, 3, 34, 234, 32, &unselect_single_bar);
    FillBoxWithBitmap (g_hdcBKG, 3, 234, 234, 32, &unselect_single_bar);
    FillBoxWithBitmap (g_hdcBKG, 14, 45, 37, 10, &wifi_wifi);
    FillBoxWithBitmap (g_hdcBKG, 11, 238, 119, 22, &wifi_auto_cnt);
    FillBoxWithBitmap (g_hdcBKG, 160, 39, 68, 19, &g_OpenCloseBKG);
    FillBoxWithBitmap (g_hdcBKG, 160, 240, 68, 19, &g_OpenCloseBKG);

    return CreateMainWindow (&createInfo);
}
#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif
