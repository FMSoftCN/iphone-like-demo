/* 
** $Id$
**
** Listing 22.1
**
** Copyright (C) 2004 ~ 2006 Feynman Software.
**
** License: GPL
*/

#include "render.h"

#define IDC_CLASSIC_BTN        200
#define IDC_FASHION_BTN        201
#define IDC_TINY_BTN           202
#define IDC_FLAT_BTN           203
#define IDC_SKIN_BTN           204
#define IDC_COLOR_BTN          205


#define IDC_TRACKBAR           210
#define IDC_PROGRESSBAR        211
#define IDC_TREEVIEW           212
#define IDC_CHECK_BTN          213
#define IDC_NONCHECK_BTN       214
#define IDC_AUTOCHECK_BTN      215

#define TIMER_PROGRESS        1000

#define contents               "Contents"   


#define MAX_COLOR_NUM       4 
#define DEFAULT_WIDTH       240
#define DEFAULT_HEIGHT      320

#define IDC_LABEL_ID    300 

#define IDC_SELECT_ID   400 

#define IDC_OK                 500
#define IDC_CANCEL             501

static HWND pbt[MAX_COLOR_NUM];
static HWND plb[MAX_COLOR_NUM];
static DWORD g_dwCFG [7];
static DWORD g_dwDefaultCFG [7];
static DWORD g_dwSetCFG [7];
static HWND g_hMainWnd = 0;


static EM_STYLE_TYPE g_emStyleType = EM_TYPE_SKIN;
static EM_STYLE_TYPE g_emTmpStyleType = EM_TYPE_SKIN;


static char* g_pcStyle[] = {
  "classic",
  "flat",
  "skin"
};

static const char *chapter[] =
{
    "Static Control",
    "Button Control",
    "Edit Control",
    "Listbox Control",
    "Treeview Control",
};

static const char *section[] =
{
    "Styles of Control",
    "Messages of Control",
    "Sample Program"
};

static TVITEMINFO bookInfo =
{
   contents 
};

static DLGTEMPLATE ColorDisDialog =
{
   WS_VISIBLE | WS_BORDER | WS_CAPTION,WS_EX_NONE,
   0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT,
   "color setting",
   0,0,
   1,NULL,
   0
};

static CTRLDATA ColorControl[]=
{
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE,
        10, 10, 130, 30,
        IDC_LABEL_ID,
        "0xFFFFFFFF",
        0,0
    },
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE,
        10, 45, 130, 30,
        IDC_LABEL_ID+1,
        "0xFFFFFFFF",
        0,0
    },
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE,
        10, 80, 130, 30,
        IDC_LABEL_ID+2,
        "0xFFFFFFFF",
        0,0
    },
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE,
        10, 115, 130, 30,
        IDC_LABEL_ID+3,
        "0xFFFFFFFF",
        0,0
    },
#if 0
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE,
        10, 150, 130, 30,
        IDC_LABEL_ID+4,
        "0xFFFFFFFF",
        0,0
    },
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE,
        10, 185, 130, 30,
        IDC_LABEL_ID+5,
        "0xFFFFFFFF",
        0,0
    },
    {
        CTRL_STATIC,
        WS_CHILD | WS_VISIBLE,
        10, 220, 130, 30,
        IDC_LABEL_ID+6,
        "0xFFFFFFFF",
        0,0
    },
#endif

    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE,
        150, 10, 70, 30,
        IDC_SELECT_ID+0,
        "BGCAACap",
        0,0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE,
        150, 45, 70, 30,
        IDC_SELECT_ID+1,
        "BGCBACap",
        0,0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE,
        150, 80, 70, 30,
        IDC_SELECT_ID+2,
        "BGCItem",
        0,0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE,
        150, 115, 70, 30,
        IDC_SELECT_ID+3,
        "MBody",
        0,0
    },
#if 0
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE,
        150, 150, 70, 30,
        IDC_SELECT_ID+4,
        "WinBGC",
        0,0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE,
        150, 185, 70, 30,
        IDC_SELECT_ID+5,
        "MainBody",
        0,0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE,
        150, 220, 70, 30,
        IDC_SELECT_ID+6,
        "Other",
        0,0
    },
#endif
    
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE,
        50, 260, 40, 30,
        IDC_OK,
        "OK",
        0,0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE,
        120, 260, 40, 30,
        IDC_CANCEL,
        "Cancel",
        0,0
    },

};
#if 0
static CTRLDATA RenderControl [] =
{ 
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 65, 25,
        IDC_CLASSIC_BTN, 
        "CLASSIC",
        0,
        0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 25, 65, 25,
        IDC_FASHION_BTN, 
        "FASHION",
        0,
        0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 50, 65, 25,
        IDC_TINY_BTN, 
        "TINY",
        0,
        0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 75, 65, 25,
        IDC_FLAT_BTN, 
        "FLAT",
        0,
        0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 100, 65, 25,
        IDC_SKIN_BTN, 
        "SKIN",
        0,
        0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 125, 65, 25,
        IDC_COLOR_BTN, 
        "COLOR",
        0,
        0
    },

    {
        CTRL_TRACKBAR,
        WS_VISIBLE | TBS_NOTIFY,
        72, 1, 158, 50,
        IDC_TRACKBAR, 
        "TRACKBAR",
        0,
        0
    },
    
    {
        "progressbar",
        WS_VISIBLE | PBS_VERTICAL,
        3, 152, 40, 135,
        IDC_PROGRESSBAR, 
        "PROGRESSBAR",
        0,
        0
    },
    {
        "treeview",
        WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
        50, 152, 180, 135,
        IDC_TREEVIEW,
        "treeview control",
        (LPARAM)&bookInfo
    },
    
    {
        CTRL_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_TABSTOP | WS_GROUP,
        70, 50, 150, 30,
        IDC_CHECK_BTN,
        "RADIO CHECK",
        0
    },
    
    {
        CTRL_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON,
        70, 80, 150, 30,
        IDC_NONCHECK_BTN,
        "NON RADIO CHECK",
        0
    },
    {
        CTRL_BUTTON,
        WS_VISIBLE | BS_AUTOCHECKBOX,
        70, 110, 150, 30,
        IDC_AUTOCHECK_BTN,
        "AUTO CHECK BOX",
        0
    },
};
#endif

static void ShowColorBox(HWND hwnd,int index, int r, int g, int b)
{
    char cString [32];

    memset (cString, 0, sizeof(cString));

    SetWindowBkColor (plb[index], RGB2Pixel (HDC_SCREEN, r, g, b));
   
    sprintf (cString, "R:%x G:%x B:%x", r, g, b);
    SetWindowCaption (plb[index], cString); 
    g_dwSetCFG [index] = 0xFF000000 | (r & 0x000000FF) |
        ((g & 0x000000FF) << 8) | ((b & 0x000000FF) << 16);

    InvalidateRect (hwnd, NULL, TRUE);
}

void SetWindowStyleType (EM_STYLE_TYPE emType)
{
    g_emStyleType = emType;
    g_emTmpStyleType = emType;
}

void SelectColor (HWND hosting, COLORRET *color);

static LRESULT ColorDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
        case MSG_INITDIALOG:
            {
                int   i;
                int   r, g, b;
                char  cString [32];

                g_dwCFG [0] = GetWindowElementAttr (g_hMainWnd, WE_BGCA_ACTIVE_CAPTION);
                g_dwCFG [1] = GetWindowElementAttr (g_hMainWnd, WE_BGCB_ACTIVE_CAPTION);
                g_dwCFG [2] = GetWindowElementAttr (g_hMainWnd, WE_BGC_HIGHLIGHT_ITEM);//WE_BGC_WINDOW);
                g_dwCFG [3] = GetWindowElementAttr (g_hMainWnd, WE_MAINC_THREED_BODY);

                g_dwSetCFG [0] = g_dwCFG[0];
                g_dwSetCFG [1] = g_dwCFG[1];
                g_dwSetCFG [2] = g_dwCFG[2];
                g_dwSetCFG [3] = g_dwCFG[3];

                for (i = 0; i < MAX_COLOR_NUM; i++)
                {
                    pbt[i] = GetDlgItem (hWnd, IDC_SELECT_ID + i);
                    plb[i] = GetDlgItem (hWnd, IDC_LABEL_ID + i);

                    r = g_dwCFG[i] & 0x000000FF;
                    g = (g_dwCFG[i] & 0x0000FF00) >> 8;
                    b = (g_dwCFG[i] & 0x00FF0000) >> 16;

                    sprintf (cString, "R:%x G:%x B:%x", r, g, b);
                    SetWindowCaption (plb[i], cString); 
                    SetWindowBkColor (plb[i], RGB2Pixel(HDC_SCREEN,r,g,b));
                }
                InvalidateRect (hWnd, NULL, TRUE);
                break;
            }
        case MSG_COMMAND:
            {
                int id = LOWORD(wParam);
                int nc = HIWORD(wParam);

                COLORRET pcdd = {0};

                if (nc == BN_CLICKED 
                        && (id >= IDC_SELECT_ID 
                            && id < (IDC_SELECT_ID + MAX_COLOR_NUM)))
                {
                    SelectColor (hWnd, &pcdd);
                    //ShowColorBox (GetParent(hWnd),id-IDC_SELECT_ID, pcdd.r, pcdd.g, pcdd.b);
                    ShowColorBox (hWnd, id-IDC_SELECT_ID, pcdd.r, pcdd.g, pcdd.b);
                }
                else if (id == IDC_OK) {
                    EndDialog (hWnd, IDC_OK);
                }
                else if (id == IDC_CANCEL) {
                    EndDialog (hWnd, IDC_CANCEL);
                }
                break;
            }
        case MSG_CLOSE:
            {
                EndDialog (hWnd, 0);
                break;
            }
    }
    return DefaultDialogProc (hWnd, message, wParam, lParam);
}


static int g_nProgress = 0;
static int g_nReturn = 0;

void InvalidateAllElement (HWND hWnd)
{
    InvalidateRect (hWnd, NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_CLASSIC_BTN), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_FASHION_BTN), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_TINY_BTN), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_FLAT_BTN), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_COLOR_BTN), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_TRACKBAR), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_PROGRESSBAR), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_TREEVIEW), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_CHECK_BTN), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_NONCHECK_BTN), NULL, TRUE);
    InvalidateRect (GetDlgItem (hWnd, IDC_AUTOCHECK_BTN), NULL, TRUE);
}

void SetWindowElementerDefault (HWND hControl)
{
    SetWindowElementAttr (hControl, WE_BGCA_ACTIVE_CAPTION, g_dwDefaultCFG [0]);
    SetWindowElementAttr (hControl, WE_BGCB_ACTIVE_CAPTION, g_dwDefaultCFG [1]);
    SetWindowElementAttr (hControl, /*WE_BGC_WINDOW*/WE_BGC_HIGHLIGHT_ITEM, g_dwDefaultCFG [2]);
    SetWindowElementAttr (hControl, WE_MAINC_THREED_BODY, g_dwDefaultCFG[3]);
}

void SetWindowElementer (HWND hControl)
{
    SetWindowElementAttr (hControl, WE_BGCA_ACTIVE_CAPTION, g_dwSetCFG [0]);
    SetWindowElementAttr (hControl, WE_BGCB_ACTIVE_CAPTION, g_dwSetCFG [1]);
    SetWindowElementAttr (hControl, /*WE_BGC_WINDOW*/WE_BGC_HIGHLIGHT_ITEM, g_dwSetCFG [2]);
    SetWindowElementAttr (hControl, WE_MAINC_THREED_BODY, g_dwSetCFG[3]);
}

static LRESULT RenderDialogProc (HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{

    switch (nMessage) {
    
      case MSG_CREATE:
      {
        TVITEMINFO tvItemInfo;
        int item;
        int i, j;
        g_hMainWnd = hWnd;
        
        CreateWindow (CTRL_BUTTON, "CLASSIC",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      IDC_CLASSIC_BTN, 
                      0, 0, 65, 35,
                      hWnd, 0);
#if 0 
        CreateWindow (CTRL_BUTTON, "FASHION",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      IDC_FASHION_BTN, 
                      0, 25, 65, 25,
                      hWnd, 0);
        
        CreateWindow (CTRL_BUTTON, "TINY",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      IDC_TINY_BTN, 
                      0, 50, 65, 25,
                      hWnd, 0);
#endif
        CreateWindow (CTRL_BUTTON, "FLAT",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      IDC_FLAT_BTN, 
                      0, 37, 65, 35,
                      hWnd, 0);

        CreateWindow (CTRL_BUTTON, "SKIN",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      IDC_SKIN_BTN, 
                      0, 74, 65, 35,
                      hWnd, 0);
        
        CreateWindow (CTRL_BUTTON, "COLOR",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      IDC_COLOR_BTN, 
                      0, 111, 65, 35,
                      hWnd, 0);
        
        CreateWindow (CTRL_TRACKBAR, "TRACKBAR",
                      WS_VISIBLE | TBS_NOTIFY,
                      IDC_TRACKBAR, 
                      72, 1, 158, 50,
                      hWnd, 0);
        
        CreateWindow ("progressbar", "PROGRESSBAR",
                      WS_VISIBLE | PBS_VERTICAL,
                      IDC_PROGRESSBAR, 
                      3, 152, 40, 135,
                      hWnd, 0);
        
        CreateWindow ("treeview", "TREEVIEW",
                      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
                      IDC_TREEVIEW, 
                      50, 152, 180, 135,
                      hWnd, (LPARAM)&bookInfo);

        CreateWindow (CTRL_BUTTON, "RADIO CHECK",
                      WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_TABSTOP | WS_GROUP,
                      IDC_CHECK_BTN,
                      70, 50, 150, 30,
                      hWnd, 0);
        
        CreateWindow (CTRL_BUTTON, "NON RADIO CHECK",
                      WS_VISIBLE | BS_AUTORADIOBUTTON,
                      IDC_NONCHECK_BTN,
                      70, 80, 150, 30,
                      hWnd, 0);
        
        CreateWindow (CTRL_BUTTON, "AUTO CHECK BOX",
                      WS_VISIBLE | BS_AUTOCHECKBOX,
                      IDC_NONCHECK_BTN,
                      70, 110, 150, 30,
                      hWnd, 0);

        if (g_emStyleType == EM_TYPE_SKIN)
            EnableWindow (GetDlgItem (hWnd, IDC_COLOR_BTN), FALSE);

        for (i = 0; i < 5; i++) {
            tvItemInfo.text = (char*)chapter[i];
            item = SendMessage (GetDlgItem(hWnd, IDC_TREEVIEW), TVM_ADDITEM, 
                                0, (LPARAM)&tvItemInfo);
            for (j = 0; j < 3; j++) {
                tvItemInfo.text = (char*)section[j];
                SendMessage (GetDlgItem(hWnd, IDC_TREEVIEW), TVM_ADDITEM, 
                            item, (LPARAM)&tvItemInfo);
            }
        }
        SendDlgItemMessage (hWnd, IDC_PROGRESSBAR, PBM_SETRANGE, 0, 500);
        //SetTimer (hWnd, TIMER_PROGRESS, 1);
        //SendDlgItemMessage (hWnd, IDC_PROGRESSBAR, PBM_SETPOS, 80, 0L);
        g_dwDefaultCFG [0] = GetWindowElementAttr (g_hMainWnd, WE_BGCA_ACTIVE_CAPTION);
        g_dwDefaultCFG [1] = GetWindowElementAttr (g_hMainWnd, WE_BGCB_ACTIVE_CAPTION);
        g_dwDefaultCFG [2] = GetWindowElementAttr (g_hMainWnd, WE_BGC_HIGHLIGHT_ITEM);//WE_BGC_WINDOW);
        g_dwDefaultCFG [3] = GetWindowElementAttr (g_hMainWnd, WE_MAINC_THREED_BODY);
        
        g_nProgress = 0;
        SetTimer (hWnd, TIMER_PROGRESS, 10);
      }
      return 0;

    case MSG_TIMER:
      {
          if (!IsWindowVisible(hWnd))
              return 0;
          g_nProgress += 5;
          SendDlgItemMessage (hWnd, IDC_PROGRESSBAR, PBM_SETPOS, g_nProgress, 0L);
          g_nProgress = g_nProgress % 500;
      }
      break;

    case MSG_COMMAND:
      {
          switch (wParam)
          {
              case IDC_CLASSIC_BTN:
                  if (g_emStyleType == EM_TYPE_CLASSIC)
                    break;
                  g_emTmpStyleType = g_emStyleType;
                  g_emStyleType = EM_TYPE_CLASSIC;
                  SendMessage (g_hSysMain, MSG_RENDER_COLOR, g_emTmpStyleType, g_emStyleType);
                  //SendMessage (hWnd, MSG_CLOSE, g_emStyleType, 0);
                  //SetDefaultWindowElementRenderer (g_pcStyle[g_emStyleType - 1]);
                  //InvalidateRect (hWnd, NULL, TRUE);
                  break;
#if 0
              case IDC_FASHION_BTN:
                  if (g_emStyleType == EM_TYPE_FASHION)
                    break;
                  g_emTmpStyleType = g_emStyleType;
                  g_emStyleType = EM_TYPE_FASHION;
                  SendMessage (g_hSysMain, MSG_RENDER_COLOR, g_emTmpStyleType, g_emStyleType);
                  //SetDefaultWindowElementRenderer (g_pcStyle[g_emStyleType - 1]);
                  //InvalidateRect (hWnd, NULL, TRUE);
                  //SendMessage (hWnd, MSG_CLOSE, g_emStyleType, 0);
                  break;
              case IDC_TINY_BTN:
                  if (g_emStyleType == EM_TYPE_TINY)
                    break;
                  g_emTmpStyleType = g_emStyleType;
                  g_emStyleType = EM_TYPE_TINY;
                  SendMessage (g_hSysMain, MSG_RENDER_COLOR, g_emTmpStyleType, g_emStyleType);
                  //SendMessage (hWnd, MSG_CLOSE, g_emStyleType, 0);
                  break;
#endif
              case IDC_FLAT_BTN:
                  if (g_emStyleType == EM_TYPE_FLAT)
                    break;
                  g_emTmpStyleType = g_emStyleType;
                  g_emStyleType = EM_TYPE_FLAT;
                  SendMessage (g_hSysMain, MSG_RENDER_COLOR, g_emTmpStyleType, g_emStyleType);
                  //SendMessage (hWnd, MSG_CLOSE, g_emStyleType, 0);
                  break;
              case IDC_SKIN_BTN:
                  if (g_emStyleType == EM_TYPE_SKIN)
                    break;
                  g_emTmpStyleType = g_emStyleType;
                  g_emStyleType = EM_TYPE_SKIN;
                  SendMessage (g_hSysMain, MSG_RENDER_COLOR, g_emTmpStyleType, g_emStyleType);
                  //SendMessage (hWnd, MSG_CLOSE, g_emStyleType, 0);
                  break;
              case IDC_COLOR_BTN:
                  if (g_emStyleType == EM_TYPE_SKIN)
                    break;

                  ColorDisDialog.controls = ColorControl;
                  ColorDisDialog.controlnr =  sizeof (ColorControl) / sizeof (ColorControl[0]);
                  g_nReturn = DialogBoxIndirectParam (&ColorDisDialog, HWND_DESKTOP, ColorDialogProc, 0L);

                  if (g_nReturn == IDC_OK)
                  {
                     HWND hControl;
#if 1
                     SetWindowElementer (hWnd);
#if 0
                     SetWindowElementAttr (hWnd, WE_BGCA_ACTIVE_CAPTION, g_dwSetCFG [0]);
                     SetWindowElementAttr (hWnd, WE_BGCB_ACTIVE_CAPTION, g_dwSetCFG [1]);
                     SetWindowElementAttr (hWnd, WE_BGC_WINDOW, g_dwSetCFG [2]);
                     SetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY, g_dwSetCFG[3]);
#endif               
                     hControl = GetDlgItem (hWnd, IDC_CLASSIC_BTN);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_FASHION_BTN);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_TINY_BTN);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_FLAT_BTN);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_SKIN_BTN);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_COLOR_BTN);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_TRACKBAR);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_PROGRESSBAR);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_TREEVIEW);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_CHECK_BTN);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_NONCHECK_BTN);
                     SetWindowElementer (hControl);
                     
                     hControl = GetDlgItem (hWnd, IDC_AUTOCHECK_BTN);
                     SetWindowElementer (hControl);
                     //InvalidateAllElement (hWnd);
#endif
                  }
                  break;
              default:
                  break;
          }
          
      }
      break;

    case MSG_KEYDOWN:
      {
          if ((LOWORD(wParam)) == SCANCODE_X)
          {
              g_emStyleType = EM_TYPE_NONE;
              SendMessage (hWnd, MSG_CLOSE, g_emStyleType, 0);
          }
      }
      break;
    case MSG_CLOSE:
      SendMessage (g_hSysMain, MSG_CLOSE_APP, g_emStyleType, 6);
      return 0;

    case MSG_SYS_CLOSE:
      {
        HWND hControl;
        SetWindowElementerDefault (hWnd);
        hControl = GetDlgItem (hWnd, IDC_CLASSIC_BTN);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_FASHION_BTN);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_TINY_BTN);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_FLAT_BTN);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_SKIN_BTN);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_COLOR_BTN);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_TRACKBAR);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_PROGRESSBAR);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_TREEVIEW);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_CHECK_BTN);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_NONCHECK_BTN);
        SetWindowElementerDefault (hControl);
        hControl = GetDlgItem (hWnd, IDC_AUTOCHECK_BTN);
        SetWindowElementerDefault (hControl);
        KillTimer (hWnd, TIMER_PROGRESS);
        DestroyAllControls (hWnd);
        DestroyMainWindow (hWnd);
      }
      return 0;
    case MSG_PAINT:
      {
          HDC hdc;
          hdc = BeginPaint (hWnd);
          EndPaint (hWnd, hdc);
      }
      break;
        
    default:
        break;
    }
    
    return DefaultMainWinProc(hWnd, nMessage, wParam, lParam);
    //return DefaultDialogProc (hWnd, nMessage, wParam, lParam);
}

HWND CreateRenderWindow (void)
{
    MAINWINCREATE CreateInfo;
    SetDefaultWindowElementRenderer (g_pcStyle[g_emStyleType - 1]);
    
    CreateInfo.dwStyle = WS_NONE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC;
    CreateInfo.spCaption = "Look&Feel Renderer";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = RenderDialogProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 240;
    CreateInfo.by = 320;
    CreateInfo.iBkColor = GetWindowElementPixel (HWND_DESKTOP, WE_MAINC_THREED_BODY);//PIXEL_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    return CreateMainWindow (&CreateInfo);
#if 0
static DLGTEMPLATE RenderDialog =
{
    WS_VISIBLE | WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 
    WS_EX_NONE,
    0, 0, 240, 320, 
    "Look&Feel Render",
    0, 0,
    1, NULL,
    0
};

    RenderDialog.controls = RenderControl;
    RenderDialog.controlnr = sizeof (RenderControl) / sizeof (RenderControl[0]);
    while ((g_emStyleType != EM_TYPE_NONE) && (nRet != 0))
    {
        SetDefaultWindowElementRenderer (g_pcStyle[g_emStyleType - 1]);
        nRet = DialogBoxIndirectParam (&RenderDialog, HWND_DESKTOP, RenderDialogProc, 0L);
         
    }
#endif
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

