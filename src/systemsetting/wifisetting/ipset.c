/* 
** $Id: helloworld.c 481 2008-02-15 06:31:50Z wangjian $
**
** Listing 2.1
**
** helloworld.c: Sample program for MiniGUI Programming Guide
**      The first MiniGUI application.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/

#include "wifiset.h"
#include "ipset.h"

/*****************************  IP SET **************************/
/* define ip set state */
#define WIFI_IPSET_OPEN       0x01
#define WIFI_IPSET_CLOSE      0x10

/*define ip set button */
#define IDC_BUTTON_IPSET_DHCP    0x501   
#define IDC_BUTTON_IPSET_BOOTP   0x503
#define IDC_BUTTON_IPSET_STATIC   0x505
#define IDC_BUTTON_IPSET_OK        0x506
#define IDC_BUTTON_IPSET_CANCEL    0x507
/* define ipset static */
#define IDC_STATIC_IPSET_FGN     0x601  //Forget this Network
#define IDC_STATIC_IPSET_IPADD     0x602  //IP Address

#define IDC_STATIC_IPSET_IPADDR   0x603  //ip add
#define IDC_STATIC_IPSET_SUBM    0x604  //subnet mask
#define IDC_STATIC_IPSET_ROUT    0x605  //router
#define IDC_STATIC_IPSET_DNS     0x606  //dns
#define IDC_STATIC_IPSET_DOM     0x607  //search domains
#define IDC_STATIC_IPSET_CID     0x608  //client id

/* define ipset edit */
#define IDC_SLEDIT_IPSET_IPADDR  0x701
#define IDC_SLEDIT_IPSET_SUBM   0x702
#define IDC_SLEDIT_IPSET_ROUT   0x703
#define IDC_SLEDIT_IPSET_DNS    0x704
#define IDC_SLEDIT_IPSET_DOM    0x705
#define IDC_SLEDIT_IPSET_CID    0x706

/******************input key box************************/
#define IDC_BTN_INPUTBOX_OK         0x801
#define IDC_BTN_INPUTBOX_CANCEL     0x802
#define IDC_EDIT_INPUTBOX_KEY       0x803
#define IDC_BTN_MSG_OK              0x804

static Network g_wifi_net;
static IpParam g_dhcp_param,g_static_param;

static BITMAP g_WiFiDHCP, g_WiFiSTATIC;
static BITMAP ipset_bg_bmp, bootp_close_bmp;
static BITMAP ipset_ok_png,ipset_cancel_png,ipset_list_bmp,ipset_bar,ipset_fgn;
static BITMAP inputbox_bg_bmp,inputbox_ok_bmp,inputbox_cancel_bmp,ipset_ipaddr,ipset_dns,ipset_submask,ipset_gateway,ipset_domain,ipset_clientid;;
static PLOGFONT inputbox_font,ipset_font, ipfont;
static BITMAP msg_bg_bmp,msg_ok_bmp;
static HDC g_hdcBKG, g_hdcpsw,  g_hdcmsgbox;

static void InitMsgboxBitmap(void)
{
    LoadBitmap(HDC_SCREEN, &msg_ok_bmp, WIFISETTING_RES"PasswordOK.png");
}

static int MsgboxWinProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case MSG_CREATE:
           CreateWindow ("MGD_BUTTON","",
                    WS_CHILD | WS_VISIBLE,
                    IDC_BTN_MSG_OK,
                    41, 178, 157, 25,
                    hwnd, 
                    (int)(&msg_ok_bmp));
         break;
        case MSG_COMMAND:
        case BUTTON_KEYDOWN:
         {
            int id_btn;
            id_btn = LOWORD(wParam);
            switch(id_btn)
            {
                case IDC_BTN_MSG_OK:
                    SendMessage(hwnd, MSG_CLOSE, 0, 0);
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

          if (hdc == 0)
          {
             hdc = GetClientDC (hwnd);
             fGetDC = TRUE;
          }
          if (clip) 
          {
             rcTemp = *clip;
             ScreenToClient (hwnd, &rcTemp.left, &rcTemp.top);
             ScreenToClient (hwnd, &rcTemp.right, &rcTemp.bottom);
             IncludeClipRect (hdc, &rcTemp);
             BitBlt (g_hdcmsgbox, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, hdc, rcTemp.left, rcTemp.top, 0 );
          }
 
          if (fGetDC)
             ReleaseDC (hdc);
          return 0;
          
        }
        case MSG_CLOSE:
            UnloadBitmap(&msg_ok_bmp);
            DeleteCompatibleDC (g_hdcmsgbox);
            DestroyMainWindow (hwnd);
            return 0;
    }
     return DefaultMainWinProc (hwnd, message, wParam, lParam);
}

static void InitMsgboxCreateInfo(HWND hWnd, PMAINWINCREATE createInfo)
{
    createInfo->dwStyle = WS_NONE;
    createInfo->dwExStyle = WS_EX_NONE;
    createInfo->spCaption = "";
    createInfo->hMenu = 0;
    createInfo->hCursor = GetSystemCursor (0);
    createInfo->hIcon = 0;
    createInfo->MainWindowProc = MsgboxWinProc;
    createInfo->lx = 0;
    createInfo->ty = 0;
    createInfo->rx = 240;
    createInfo->by = 320;

    createInfo->iBkColor = PIXEL_lightwhite;
    createInfo->dwAddData = 0;
    createInfo->hHosting = HWND_DESKTOP;
}

static int CreateMsgboxWindow(HWND hOwner)
{
    MAINWINCREATE CreateInfo;
    HWND hMainwnd;
    MSG Msg;

    InitMsgboxCreateInfo(hOwner, &CreateInfo);
    InitMsgboxBitmap();
    LoadBitmap(HDC_SCREEN, &msg_bg_bmp, WIFISETTING_RES"PasswordErrorBKG.png");
    g_hdcmsgbox= CreateCompatibleDC (HDC_SCREEN);
    FillBoxWithBitmap (g_hdcmsgbox, 0,0,240,320,&msg_bg_bmp);
    hMainwnd = CreateMainWindow(&CreateInfo);
    if (hMainwnd == HWND_INVALID)
        return -1;
     ShowWindow (hMainwnd, SW_SHOWNORMAL);
     while (GetMessage (&Msg, hMainwnd)){
         TranslateMessage (&Msg);
         DispatchMessage (&Msg);
   }
    MainWindowThreadCleanup(hMainwnd);
    return 0;
}


static void InitInputBoxBitmap(void)
{
    LoadBitmap(HDC_SCREEN, &inputbox_ok_bmp, SYSTEM_RES"SaveButton.png");
    LoadBitmap(HDC_SCREEN, &inputbox_cancel_bmp, SYSTEM_RES"CancelButton.png");
}

static BOOL CompareRouterkey(char *sourcekey, char *newkey, int srckeylen, int newkeylen)
{
    int len;
    
    if(srckeylen != newkeylen)
        return FALSE;
    len = srckeylen;
   // for(i = 0; i < srckeylen; i++)
   while(len-- != 0)
        if(*sourcekey++ != *newkey++)
            return FALSE;
   return TRUE;
}
static void edit_notify_proc(HWND hwnd, int id, int nc, DWORD add_data)
{
    unsigned char  buff[16] = {0};

    if(nc == EN_CHANGE)
    {
        SendMessage (hwnd, MSG_GETTEXT, (WPARAM)sizeof(buff),(LPARAM)buff);
        SetWindowText (hwnd, buff);//sznew);
        SetCaretPos(hwnd, strlen(buff)*GetSysCharWidth(), 0);
       // SendMessage (hwnd, MSG_SETTEXT, 0, (DWORD)&buff[0]);
       // SendMessage (hwnd, EM_SETCARETPOS, (WPARAM)&line_pos, (LPARAM)&char_pos);
        // len = strlen(buff);

      // SetWindowText (hwnd, '*')
       //SendMessage (hwnd, EM_GETCHARETPOS, 0 ,0);
      //  SendMessage (hwnd, MSG_CHAR, '*', 0L);
    
    }

}
static int InputBoxWinProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    HWND  okbtnwnd, cancelbtnwnd, editwnd;
    char* exec = NULL;
 
    switch (message)
    {
        case MSG_CREATE:
           CreateWindow ("MGD_BUTTON","",
                    WS_CHILD | WS_VISIBLE,
                    IDC_BTN_INPUTBOX_CANCEL,
                    4, 21, 42, 21,
                    hwnd, 
                    (int)(&inputbox_cancel_bmp));
             CreateWindow ("MGD_BUTTON", "",
                    WS_CHILD | WS_VISIBLE,
                    IDC_BTN_INPUTBOX_OK,
                    196, 21,40, 21,
                    hwnd, 
                    (int)(&inputbox_ok_bmp));
  
           editwnd= CreateWindowEx (CTRL_SLEDIT, "",
                    WS_CHILD | WS_VISIBLE | ES_PASSWORD,
                    WS_EX_TRANSPARENT,
                    IDC_EDIT_INPUTBOX_KEY,
                    83,90,110, 20,
                    hwnd, 0);
            SetNotificationCallback (editwnd, edit_notify_proc);
            inputbox_font = CreateLogFont (NULL, "Times", "GB2312",
                    FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                    FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                    14,0);
            break;
         case MSG_COMMAND:
         case BUTTON_KEYDOWN:
         {
            int i,id_btn,len;
            char buff[10];
            char routerkey[10];
            BOOL pswvalid;
            id_btn = LOWORD(wParam);
            switch(id_btn)
            {
                case IDC_BTN_INPUTBOX_OK:
                   //CreateMsgboxWindow();
                   // SendMessage (GetDlgItem(hwnd, IDC_EDIT_INPUTBOX_KEY), EM_GETPASSWORDCHAR, routerkey, 0L);
                   //SendMessage (hwnd, EM_GETPASSWORDCHAR, buff, 0L);
                   memset (buff, 0x0, sizeof (buff));
                   GetWindowText(GetDlgItem(hwnd, IDC_EDIT_INPUTBOX_KEY), buff, sizeof(buff));               
                   len = strlen(buff);
#if 1 
                   for(i=0; i< len; i++)
                       g_wifi_net.ip_param.router_key[i] = buff[len-1-i];
#endif
                   asprintf (&exec, "iwconfig wlan0 key %s", buff); 
                   system (exec);
                   free (exec);
                   //printf("key:%s \n",routerkey);//123456->654321
                   //if (-1 == wifi_set_interface_key ("wlan0", "8618203940")) 
                   //    printf ("wifi set interface essid error!\n");
                   //CreateMsgboxWindow(hwnd);
                   //pswvalid = CompareRouterkey(g_wifi_net.ip_param.router_key, routerkey, strlen(g_wifi_net.ip_param.router_key), len);
                   //if (FALSE == pswvalid)
                  // {
                        
                       //wifi_set_interface_essid ("wlan0", "SZFM");
                       //wifi_set_interface_key ("wlan0", "8618203940");
                   //}
                   //memcpy(g_wifi_net.ip_param.router_key, routerkey, strlen(routerkey));
                   SendMessage(hwnd, MSG_CLOSE, 0, 0);
                   break;
                case IDC_BTN_INPUTBOX_CANCEL:
                    CreateMsgboxWindow(hwnd);
                    SendMessage(hwnd, MSG_CLOSE, 0, 0);
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
             
             if (hdc == 0)
             {
                 hdc = GetClientDC (hwnd);
                 fGetDC = TRUE;
             }
             if (clip) 
             {
                 rcTemp = *clip;
                 ScreenToClient (hwnd, &rcTemp.left, &rcTemp.top);
                 ScreenToClient (hwnd, &rcTemp.right, &rcTemp.bottom);
                 IncludeClipRect (hdc, &rcTemp);
                 BitBlt (g_hdcpsw, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, hdc, rcTemp.left, rcTemp.top, 0 );
             }
           // FillBoxWithBitmap (g_hdcpsw, 0,0,240,320, &inputbox_bg_bmp);
            if (fGetDC)
                ReleaseDC (hdc);
            return 0;
         }
         case MSG_CLOSE:
             UnloadBitmap(&inputbox_ok_bmp);
             UnloadBitmap(&inputbox_cancel_bmp); 
             DestroyMainWindow (hwnd);
             return 0;
    }
     return DefaultMainWinProc (hwnd, message, wParam, lParam);
}

static void InitInputBoxCreateInfo(HWND hWnd, PMAINWINCREATE createInfo)
{
    createInfo->dwStyle = WS_NONE;
    createInfo->dwExStyle = WS_EX_NONE;
    createInfo->spCaption = "";
    createInfo->hMenu = 0;
    createInfo->hCursor = GetSystemCursor (0);
    createInfo->hIcon = 0;
    createInfo->MainWindowProc = InputBoxWinProc;
    createInfo->lx = 0;
    createInfo->ty = 0;
    createInfo->rx = 240;
    createInfo->by = 320;

    createInfo->iBkColor = PIXEL_lightwhite;
    createInfo->dwAddData = 0;
    createInfo->hHosting = HWND_DESKTOP;
}

static int CreateInputBoxWindow(HWND hOwner)
{
    MAINWINCREATE CreateInfo;
    HWND hMainwnd;
    MSG Msg;

    InitInputBoxCreateInfo(hOwner, &CreateInfo);
    InitInputBoxBitmap();
    LoadBitmap(HDC_SCREEN, &inputbox_bg_bmp, WIFISETTING_RES"PasswordBKG.png");
    g_hdcpsw= CreateCompatibleDC (HDC_SCREEN);
    FillBoxWithBitmap (g_hdcpsw, 0,0,240,320, &inputbox_bg_bmp);
 
    hMainwnd = CreateMainWindow(&CreateInfo);
    if (hMainwnd == HWND_INVALID)
        return -1;
     ShowWindow (hMainwnd, SW_SHOWNORMAL);
     while (GetMessage (&Msg, hMainwnd)){
         TranslateMessage (&Msg);
         DispatchMessage (&Msg);
   }
    DeleteCompatibleDC (g_hdcpsw);
    MainWindowThreadCleanup(hMainwnd);
    return 0;
}

void InitIpset(void)
{
    GetWifiNetParam(&g_wifi_net);

}
static void InitIpsetBitmap(void)
{ 
    //LoadBitmap(HDC_SCREEN, &bg_bmp, "./res/background_240*320.png");
    LoadBitmap(HDC_SCREEN, &g_WiFiSTATIC, LOCALNETWORK_RES"STATIC.png");
    LoadBitmap(HDC_SCREEN, &g_WiFiDHCP, LOCALNETWORK_RES"DHCP.png");
    
    LoadBitmap(HDC_SCREEN, &bootp_close_bmp, LOCALNETWORK_RES"CloseBOOTP.png");
    LoadBitmap(HDC_SCREEN, &ipset_list_bmp, LOCALNETWORK_RES"IPSettingList.png");
    LoadBitmap(HDC_SCREEN, &ipset_ok_png, SYSTEM_RES"SaveButton.png");
    LoadBitmap(HDC_SCREEN, &ipset_cancel_png, SYSTEM_RES"CancelButton.png");
 
    LoadBitmap(HDC_SCREEN, &ipset_ipaddr, LOCALNETWORK_RES"IPAddr.png");
    LoadBitmap(HDC_SCREEN, &ipset_submask, LOCALNETWORK_RES"SubnetMask.png");
    LoadBitmap(HDC_SCREEN, &ipset_dns, LOCALNETWORK_RES"DNS.png");
    LoadBitmap(HDC_SCREEN, &ipset_domain, LOCALNETWORK_RES"SearchDomains.png");
    LoadBitmap(HDC_SCREEN, &ipset_clientid, LOCALNETWORK_RES"ClientID.png");
    LoadBitmap(HDC_SCREEN, &ipset_gateway, WIFISETTING_RES"Router.png");
}
static void  GetIpparamFromWindow(HWND hwnd, int curnet, int cur_ip_type, char *netname)
{
    char ipaddr[20],submask[20],dns[40],router[20],dom[16],cid[16],tmp[20];
    Network net;
    int i,j,k;
   
    j = 0;
    k = 0;
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_IPADDR), ipaddr, sizeof(ipaddr));
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_SUBM), submask, sizeof(submask));
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DNS), dns, sizeof(dns));
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_ROUT), router, sizeof(router));
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DOM), dom, sizeof(dom));
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_CID), cid, sizeof(cid));
   // SetWifiNet(Network *net)
    //router??   
    //wifi_set_interface_key(netname, key);

}           
static void SetStaticIpparam (HWND hwnd)//, NetcardParam *ip_param)
{
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_IPADDR),MSG_SETTEXT, 0, (DWORD)(&(g_static_param.ipaddr[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_SUBM), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.submask[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DNS), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.dns[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_ROUT), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.router[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DOM), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.domain[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_CID), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.cid[0])));
}

static void GetStaticIpparam (HWND hwnd)
{
#if 0
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_IPADDR),MSG_SETTEXT, 0, (DWORD)(&ipParam->ipaddr[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_SUBM), MSG_SETTEXT, 0, (DWORD)(&ipParam->submask[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DNS), MSG_SETTEXT, 0, (DWORD)(&ipParam->dns[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_ROUT), MSG_SETTEXT, 0, (DWORD)(&ipParam->router[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DOM), MSG_SETTEXT, 0, (DWORD)(&ipParam->domain[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_CID), MSG_SETTEXT, 0, (DWORD)(&ipParam->cid[0]));
#endif
    GetWindowText (GetDlgItem (hwnd, IDC_SLEDIT_IPSET_ROUT), g_static_param.router, 32);
    GetWindowText (GetDlgItem (hwnd, IDC_SLEDIT_IPSET_ROUT), g_static_param.ipaddr, 32);
    GetWindowText (GetDlgItem (hwnd, IDC_SLEDIT_IPSET_ROUT), g_static_param.submask, 32);
}     
static void SetIpparamToWindow(HWND hwnd, IpParam *ipParam)
{   
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_IPADDR),MSG_SETTEXT, 0, (DWORD)(&ipParam->ipaddr[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_SUBM), MSG_SETTEXT, 0, (DWORD)(&ipParam->submask[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DNS), MSG_SETTEXT, 0, (DWORD)(&ipParam->dns[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_ROUT), MSG_SETTEXT, 0, (DWORD)(&ipParam->router[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DOM), MSG_SETTEXT, 0, (DWORD)(&ipParam->domain[0]));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_CID), MSG_SETTEXT, 0, (DWORD)(&ipParam->cid[0]));

}
static void SetIpparamNone(HWND hwnd)
{
   
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_IPADDR),MSG_SETTEXT, 0, (DWORD)"");
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_SUBM), MSG_SETTEXT, 0, (DWORD)"");
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DNS), MSG_SETTEXT, 0,(DWORD)"");
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_ROUT), MSG_SETTEXT, 0 ,(DWORD)"");
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DOM), MSG_SETTEXT, 0, (DWORD)"");
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_CID), MSG_SETTEXT, 0, (DWORD)"");

}
static int IpsetWinProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    HWND okbtnwnd, cancelbtnwnd, dhcpwnd, bootpwnd, statwnd;
    HWND ipeditwnd, subeditwnd,gweditwnd,dnseditwnd,dmeditwnd, cideditwnd;
    HWND ipstawnd,  substawnd, gwstawnd, dnsstawnd, dmstawnd, cidstawnd;
    char bufftest[] = {"fsfsfs"};
    char netname[20];
    static int  cur_type;
  
    switch (message)
    {
        case MSG_CREATE:
        {
            GetWifiNetParam(&g_wifi_net);//(InitIpset();
            cur_type = TYPE_DHCP; //g_wifi_net.cur_ip_type;
            okbtnwnd = CreateWindow ("MGD_BUTTON", "",
                    WS_CHILD | WS_VISIBLE,
                    IDC_BUTTON_IPSET_OK,
                    196, 6, 40, 21,
                    hwnd, 
                    (int)(&ipset_ok_png));
            cancelbtnwnd = CreateWindow ("MGD_BUTTON","",
                    WS_CHILD | WS_VISIBLE,
                    IDC_BUTTON_IPSET_CANCEL,
                    4, 6, 42, 21,
                    hwnd, 
                    (int)(&ipset_cancel_png));
            if(cur_type == TYPE_DHCP)
            {
                dhcpwnd = CreateWindow ("MGD_BUTTON", "",
                        WS_CHILD | WS_VISIBLE | MGDBUTTON_2STATE | MGDBUTTON_ANTISTATE,
                        IDC_BUTTON_IPSET_DHCP,
                        3, 87, 78, 30,
                        hwnd, (DWORD)(&g_WiFiDHCP));
                statwnd = CreateWindow ("MGD_BUTTON", "",
                        WS_CHILD | WS_VISIBLE | MGDBUTTON_2STATE,
                        IDC_BUTTON_IPSET_STATIC,
                        160, 87, 77, 30,
                        hwnd, (DWORD)(&g_WiFiSTATIC));
            }else {
                dhcpwnd = CreateWindow ("MGD_BUTTON", "",
                        WS_CHILD | WS_VISIBLE | MGDBUTTON_2STATE,
                        IDC_BUTTON_IPSET_DHCP,
                        3, 87, 78, 30,
                        hwnd, (DWORD)(&g_WiFiDHCP));
                statwnd = CreateWindow ("MGD_BUTTON", "",
                        WS_CHILD | WS_VISIBLE | MGDBUTTON_2STATE | MGDBUTTON_ANTISTATE,
                        IDC_BUTTON_IPSET_STATIC,
                        160, 87, 77, 30,
                        hwnd, (DWORD)(&g_WiFiSTATIC));
            }
            bootpwnd = CreateWindow ("MGD_BUTTON", "",
                        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                        IDC_BUTTON_IPSET_BOOTP,
                        81, 87, 79, 30,
                        hwnd, (int)&bootp_close_bmp);

             ipfont = CreateLogFont (NULL, "Times", "GB2312",
                     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                     16,0);
             memcpy(&g_dhcp_param, &g_wifi_net.ip_param, sizeof(IpParam));
             memcpy(&g_static_param, &g_wifi_net.ip_param, sizeof(IpParam));
             
             ipstawnd = CreateWindowEx(CTRL_STATIC, g_dhcp_param.ipaddr,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_IPSET_IPADDR,
                     110, 132, 110, 20,
                     hwnd, 0);
             SetWindowFont(ipstawnd, ipfont);
             substawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.submask,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_IPSET_SUBM,
                     110, 164, 110, 20,
                     hwnd, 0);
             SetWindowFont(substawnd, ipfont);
             gwstawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.router,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_IPSET_ROUT,
                     110, 196, 110, 20,
                     hwnd, 0);
             SetWindowFont(gwstawnd, ipfont);
             dnsstawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.dns,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_IPSET_DNS,
                     110, 228, 110, 20,
                     hwnd, 0);
             SetWindowFont(dnsstawnd, ipfont);
             dmstawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.domain,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_IPSET_DOM,
                     110, 260, 110, 20,
                     hwnd, 0);             
             SetWindowFont(dmstawnd, ipfont);
             cidstawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.cid,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_IPSET_CID,
                     110, 292, 110, 20,
                     hwnd, 0);          
             SetWindowFont(dmstawnd, ipfont);
             ipeditwnd = CreateWindowEx(CTRL_SLEDIT, "",
                        WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_IPSET_IPADDR,
                     110, 132, 110, 20,
                     hwnd, 0);
             SetWindowFont(ipeditwnd, ipfont);
             subeditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_IPSET_SUBM,
                     110, 164, 110, 20,
                     hwnd, 0);
              SetWindowFont(subeditwnd, ipfont);
              gweditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_IPSET_ROUT,
                     110, 196, 110, 20,
                     hwnd, 0);
             SetWindowFont(gweditwnd, ipfont);
             dnseditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_IPSET_DNS,
                     110, 228, 110, 20,
                     hwnd, 0);
             SetWindowFont(dnseditwnd, ipfont);
             dmeditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_IPSET_DOM,
                     110, 260, 110, 20,
                     hwnd, 0);              
             SetWindowFont(dmeditwnd, ipfont);
             cideditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_IPSET_CID,
                     110, 292, 110, 20,
                     hwnd, 0);        
             SetWindowFont(cideditwnd, ipfont);
             if(TYPE_STATIC == cur_type){
                    ShowWindow (ipstawnd, SW_HIDE);  
                    ShowWindow (substawnd, SW_HIDE);
                    ShowWindow (gwstawnd, SW_HIDE);
                    ShowWindow (dnsstawnd, SW_HIDE);
                    ShowWindow (dmstawnd, SW_HIDE);
                    ShowWindow (cidstawnd, SW_HIDE);
                    SetStaticIpparam (hwnd);
            }else if (TYPE_DHCP == cur_type){
                    ShowWindow (ipeditwnd, SW_HIDE);  
                    ShowWindow (subeditwnd, SW_HIDE);
                    ShowWindow (gweditwnd, SW_HIDE);
                    ShowWindow (dnseditwnd, SW_HIDE);
                    ShowWindow (dmeditwnd, SW_HIDE);
                    ShowWindow (cideditwnd, SW_HIDE);
            }
            ShowWindow (hwnd, SW_HIDE);
            return 0;
        }
        case MSG_COMMAND:
        case BUTTON_KEYDOWN:
        {
            int id_btn;
            id_btn = LOWORD(wParam);
            switch(id_btn)
            {
                case IDC_BUTTON_IPSET_DHCP:
                    if(TYPE_STATIC == cur_type)                   
                    {
                       // SetIpparamNone(hwnd);//Set

                        SendMessage (GetDlgItem(hwnd, IDC_BUTTON_IPSET_DHCP), BUTTON_ANTISTATE, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BUTTON_IPSET_DHCP), BUTTON_NORMAL, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BUTTON_IPSET_STATIC), BUTTON_ANTISTATE, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BUTTON_IPSET_STATIC), BUTTON_NORMAL, 0, 0);
                        
                        ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_IPADDR), SW_HIDE);  
                        ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_SUBM), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_ROUT), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DNS), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DOM), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_CID), SW_HIDE);
                    }
                    ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_IPADDR), SW_SHOW);  
                    ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_SUBM), SW_SHOW);
                    ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_ROUT), SW_SHOW);
                    ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_DNS), SW_SHOW);
                    ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_DOM), SW_SHOW);
                    ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_CID), SW_SHOW);
                    cur_type = TYPE_DHCP;
                    break;

                case IDC_BUTTON_IPSET_BOOTP:
                    cur_type = TYPE_BOOTP;
                    break;

                case IDC_BUTTON_IPSET_STATIC:
                    if(TYPE_DHCP == cur_type)
                    {
                        //SetStaticIpparam(hwnd);
                        SendMessage (GetDlgItem(hwnd, IDC_BUTTON_IPSET_STATIC), BUTTON_ANTISTATE, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BUTTON_IPSET_STATIC), BUTTON_NORMAL, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BUTTON_IPSET_DHCP), BUTTON_ANTISTATE, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BUTTON_IPSET_DHCP), BUTTON_NORMAL, 0, 0);
                        
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_IPADDR), SW_HIDE);  
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_SUBM), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_ROUT), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_DNS), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_DOM), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_IPSET_CID), SW_HIDE);                    
                    }
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_IPADDR), SW_SHOWNORMAL);  
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_SUBM), SW_SHOWNORMAL);
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_ROUT), SW_SHOWNORMAL);
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DNS), SW_SHOWNORMAL);
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DOM), SW_SHOWNORMAL);
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_IPSET_CID), SW_SHOWNORMAL);
                    SetStaticIpparam(hwnd);
                    
                    cur_type = TYPE_STATIC;
                    break;
                case IDC_BUTTON_IPSET_OK:
                    g_wifi_net.cur_ip_type = cur_type; 
                    if (cur_type == TYPE_STATIC)
                    {
                        char cName [32];
                        char cString [32];
                        char* exec;
                        GetStaticIpparam (hwnd);
                        memset (cName, 0x0, sizeof (cName));

                        asprintf (&exec, "iwconfig wlan0 essid %s", g_static_param.router); 
                        system (exec);
                        free (exec);
                        asprintf (&exec, "ifconfig wlan0 %s", g_static_param.ipaddr); 
                        system (exec);
                        free (exec);
                        asprintf (&exec, "ifconfig wlan0 netmask %s", g_static_param.submask); 
                        system (exec);
                        free (exec);
                        CreateInputBoxWindow(hwnd);
                       //sprintf (cName, "wlan0");
#if 0 
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_IPADDR),MSG_SETTEXT, 0, (DWORD)(&(g_static_param.ipaddr[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_SUBM), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.submask[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DNS), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.dns[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_ROUT), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.router[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_DOM), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.domain[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_IPSET_CID), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.cid[0])));
#endif                   
    //router
#if 0
    if (-1 == wifi_set_interface_essid (cName, "SZFM")) 
        printf ("wifi set interface essid error!\n");
    //set wifi network
    if(-1 == wifi_set_interface_ip(cName, "192.168.2.101"))
        printf ("wifi set interface ip error!\n");
    //wifi_using_dhcp (netname);

    if(-1 ==  wifi_set_interface_mask(cName, "255.255.255.0"))
        printf ("wifi set interface mask error!\n");
#endif
    //wifi_set_gateway(submask);//no getway
    //wifi_add_dns(network->ip_param.dns);
    //wifi_set_search(network->ip_param.domain);
                        //SetWifiNetParam(&g_wifi_net);
                        //CreateInputBoxWindow(hwnd);
                    }
                    SendMessage(hwnd, MSG_CLOSE, 0, 0);
                    break;
                case IDC_BUTTON_IPSET_CANCEL:
                    SendMessage(hwnd, MSG_CLOSE, 0, 0);
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
            if (hdc == 0)
            {
                hdc = GetClientDC (hwnd);
                fGetDC = TRUE;
            }
            if (clip) 
            {
                rcTemp = *clip;
                ScreenToClient (hwnd, &rcTemp.left, &rcTemp.top);
                ScreenToClient (hwnd, &rcTemp.right, &rcTemp.bottom);
                IncludeClipRect (hdc, &rcTemp);
                BitBlt (g_hdcBKG, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom, hdc, rcTemp.left, rcTemp.top, 0 );
            }
            if (fGetDC)
                ReleaseDC (hdc);
            return 0;
          }
        case MSG_CLOSE:
            SendMessage (g_hWiFiMainMenu, MSG_WIFINET_CANCEL, 0, 0);
            return 0;
        
        case MSG_SYS_CLOSE:
            DeleteCompatibleDC (g_hdcBKG);
            UnloadBitmap (&bootp_close_bmp);
            UnloadBitmap (&ipset_bar);
            UnloadBitmap (&ipset_fgn);
            UnloadBitmap (&ipset_list_bmp);
            UnloadBitmap (&ipset_ok_png);
            UnloadBitmap (&ipset_cancel_png);  
            UnloadBitmap (&ipset_ipaddr);
            UnloadBitmap (&ipset_submask);
            UnloadBitmap (&ipset_gateway);
            UnloadBitmap (&ipset_dns);
            UnloadBitmap (&ipset_domain);
            UnloadBitmap (&ipset_clientid);
            UnloadBitmap (&ipset_bg_bmp);
            UnloadBitmap (&ipset_fgn);
            DestroyMainWindow (hwnd);
            return 0;
    }

    return DefaultMainWinProc (hwnd, message, wParam, lParam);
}
static void InitIpsetCreateInfo(HWND hWnd, PMAINWINCREATE createInfo)
{
    createInfo->dwStyle = WS_NONE;
    createInfo->dwExStyle = WS_EX_NONE;
    createInfo->spCaption = "";
    createInfo->hMenu = 0;
    createInfo->hCursor = GetSystemCursor (0);
    createInfo->hIcon = 0;
    createInfo->MainWindowProc = IpsetWinProc;
    createInfo->lx = 0;
    createInfo->ty = 0;
    createInfo->rx = 240;
    createInfo->by = 320;

    createInfo->iBkColor = PIXEL_lightwhite;
    createInfo->dwAddData = 0;
    createInfo->hHosting = HWND_DESKTOP;
}

HWND CreateIpsetWindow (void)
{
    MAINWINCREATE CreateInfo;
    HWND hMainwnd;
    MSG Msg;

    InitIpsetCreateInfo(HWND_DESKTOP, &CreateInfo);//, caption);
    InitIpset();
    InitIpsetBitmap();
    LoadBitmap(HDC_SCREEN, &ipset_bg_bmp, LOCALNETWORK_RES"IPSettingBKG.png");
    LoadBitmap(HDC_SCREEN, &ipset_bar, LOCALNETWORK_RES"FNStringBKG.png");
    LoadBitmap(HDC_SCREEN,&ipset_fgn, LOCALNETWORK_RES"FNString.png") ;
    g_hdcBKG = CreateCompatibleDC (HDC_SCREEN);
    FillBoxWithBitmap (g_hdcBKG, 0,0,240,320,&ipset_bg_bmp);
    FillBoxWithBitmap (g_hdcBKG, 3,34,234,30,&ipset_bar);
    FillBoxWithBitmap (g_hdcBKG, 3,124,234,193,&ipset_list_bmp);
    FillBoxWithBitmap (g_hdcBKG, 63,44,110,12,&ipset_fgn);
    FillBoxWithBitmap (g_hdcBKG, 13,136,61,10,&ipset_ipaddr);
    FillBoxWithBitmap (g_hdcBKG, 13,168,74,10,&ipset_submask);
    FillBoxWithBitmap (g_hdcBKG, 13,200,50,10,&ipset_gateway);
    FillBoxWithBitmap (g_hdcBKG, 13,232,26,10,&ipset_dns);
    FillBoxWithBitmap (g_hdcBKG, 13,264,93,10,&ipset_domain);
    FillBoxWithBitmap (g_hdcBKG, 13,296,49,10,&ipset_clientid);

    return CreateMainWindow(&CreateInfo);
}

