/*
 ** $ld: netcardset.c 
 *
 *
 *
 ** Copyright (C) 2004 ~ 2007 Feynman Software.
 *
 ** License: GPL
 */

#include "netcardset.h"
/******************************  Netcard Setting ***************** */
/* define cancel and ok button */
#define IDC_BTN_NETCARD_OK              0x100
#define IDC_BTN_NETCARD_CANCEL          0x101
//#define IDC_STATIC_NETCARD_BAR          0x102

/*define necard set button */
#define IDC_BTN_NETCARD_DHCP     0x501   
#define IDC_BTN_NETCARD_BOOTP    0x502
#define IDC_BTN_NETCARD_STATIC   0x503


/* define netcard static */
#define IDC_STATIC_NETCARD_IPADDR   0x601   //ip add
#define IDC_STATIC_NETCARD_SUBM     0x602   //subnet mask
#define IDC_STATIC_NETCARD_GATEWAY   0x603   //getway
#define IDC_STATIC_NETCARD_DNS      0x604   //dns
#define IDC_STATIC_NETCARD_DOM      0x605   //dns
#define IDC_STATIC_NETCARD_CID      0x606   //dns


/* define netcard edit */
#define IDC_SLEDIT_NETCARD_IPADDR  0x701
#define IDC_SLEDIT_NETCARD_SUBM    0x702
#define IDC_SLEDIT_NETCARD_GATEWAY  0x703
#define IDC_SLEDIT_NETCARD_DNS     0x704
#define IDC_SLEDIT_NETCARD_DOM     0x705   //dns
#define IDC_SLEDIT_NETCARD_CID     0x706   //dns



/* define the three netcard type: hcdp,static */
#define TYPE_DHCP       0x800
#define TYPE_STATIC     0x801
#define TYPE_BOOTP      0x802

typedef struct _NetcardParam{
    int cur_ip_type; 
    char ipaddr[20];       //192.168.111.124
    char submask[20];
    char gateway[20];
    char dns[256];
    char domain[256];
    char cid[256];
    int dnsnum;
    int domnum;
}NetcardParam;

static NetcardParam g_netcard_ip_param;
static NetcardParam g_static_param;
static NetcardParam g_dhcp_param;
static HDC g_hdcBKG;
static BITMAP g_DHCPBitmap;
static BITMAP g_STATICBitmap;

static PLOGFONT netcard_font;
static NetcardParam ipparam_test[2] ={
    {TYPE_DHCP,"  0.  0.  0.  0","  0.  0.  0.  0","  0.  0.  0.  0","  0.  0.  0.  0"},
    {TYPE_DHCP,"  0.  0.  0.  0","  0.  0.  0.  0","  0.  0.  0.  0","  0.  0.  0.  0"},
    };

static BITMAP bg_bmp, bar_bmp, bootp_bmp,list_bmp;
static BITMAP nc_ok_bmp,nc_cancel_bmp,nc_fgn,nc_ipaddr,nc_dns,nc_submask,nc_gateway,nc_domain,nc_clientid;


static void IpparamToString (unsigned char *ipparam, char *ipparam_str)
{
   // char param0, param1, param2, param3;
  //  char str0[4], str1[4], str2[4], str3[4];
    unsigned char tmpstr[20] = {0};
    unsigned char str_len = 0;

    sprintf (tmpstr, "%3d.%3d.%3d.%3d", (int)ipparam[0], (int)ipparam[1], (int)ipparam[2], (int)ipparam[3]);
    memcpy(ipparam_str, tmpstr, sizeof(tmpstr)); 
}

static void InitParamByDefault (void)
{
#if 0 
    g_netcard_ip_param.cur_ip_type = TYPE_STATIC;
    g_netcard_ip_param.ipaddr[20] = '192.168.1.112';
    g_netcard_ip_param.submask[20] = '255.255.255.0';
    g_netcard_ip_param.gateway[20] = '192.168.1.1';
    g_netcard_ip_param.dns[20] = '192.168.1.1';
    g_netcard_ip_param.domain[20] = 'domain';
    g_netcard_ip_param.cid[20] = '110';
    g_netcard_ip_param.dnsnum = 1;
    g_netcard_ip_param.domnum = 1;
#endif
    memcpy (&g_netcard_ip_param, &ipparam_test[0], sizeof (g_netcard_ip_param));
}


static BOOL GetNetcardParam (void)
{
    char netname[32] = {0};
    char dns[256] ;
    char gateway[6],submask[6], ipaddr[6], search[32];
    int i,count,net_num,dns_num, dom_num;
    int temp1,temp2;
    BOOL success = TRUE;

    memset(&g_netcard_ip_param, 0x0, sizeof(NetcardParam));
    g_netcard_ip_param.cur_ip_type = TYPE_STATIC;
    
    net_num = 0;
    // memset(search, 0x0, sizeof(search));
    memset(dns, 0x0, 256);//sizeof(dns));
    // memset(submask, 0x0, sizeof(submask));
    
    
#if 1
    if(-1 == wifi_socket_open())
        return FALSE;
    //wifi_get_network_interface_name(netname, &net_num)
    if(-1 == wifi_get_default_interface_info (netname, gateway))
    {
       return FALSE;
    }
    memcpy (g_netcard_ip_param.gateway, gateway, strlen(gateway));  //save gateway

    //sprintf (netname, "eth0");
    if(-1 == wifi_get_interface_ip (netname, ipaddr))
        success =  FALSE;
    IpparamToString (ipaddr, g_netcard_ip_param.ipaddr);
   
    if(-1 == wifi_get_interface_mask (netname, submask))
        success =  FALSE;
    IpparamToString (submask, g_netcard_ip_param.submask);
#if 0   
   char name[40];
   int number = 0;
   memset (name, 0x0, 40);
   number = 0;
   wifi_get_dns (name, &number);
   printf ("dns number is %d\n", number);
   printf ("dns name:%s\n", name);
#endif
#if 1
    /************* get dns ********************/
    dns_num = 0;
    wifi_get_dns (dns, &dns_num);
    g_netcard_ip_param.dnsnum = dns_num;
    i = 0;

    for(count = 0; count < dns_num; count++)
    {
        while(dns[i] != '\n')
           i++;
       // count--;
        if(count < dns_num)
            dns[i] = ';';
    }
    if(dns_num > 0)
        memcpy (g_netcard_ip_param.dns, dns, sizeof (dns));

#endif
    /************* get search domain **********/
    dom_num = 0;
    memset (search, 0x0, sizeof(search));
    wifi_get_search (search, &dom_num);
    if(dom_num > 0)
        memcpy (g_netcard_ip_param.domain, search, strlen(search)-1);


   if( -1 == wifi_socket_close())
    {
        printf("socket close error");
        return FALSE;
    }
    return success;

#endif
}

static BOOL  SetNetcardParam(HWND hwnd, int cur_ip_type)//, char *netname)
{
    char netname[32],ipaddr[20],submask[20],dns[40],getway[20],search[20],mp[20];
    int i,j,k;
    
    j = 0;
    k = 0;
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_IPADDR), ipaddr, sizeof(ipaddr));
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_SUBM), submask, sizeof(submask));
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_GATEWAY), getway, sizeof(getway));
    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_DNS), dns, sizeof(dns));
    memcpy(g_netcard_ip_param.ipaddr, ipaddr, strlen(ipaddr));
    memcpy(g_netcard_ip_param.submask, submask, strlen(submask));
 
   /* while(dns[j] != '\n')
    {
        if(dns[j] == ';')
             tmp[k++] = '\n';
         tmp[k++] = dns[j++];
     }*/
    memcpy(g_netcard_ip_param.dns, dns, strlen(dns));
    g_netcard_ip_param.cur_ip_type = cur_ip_type;

    //set wifi network
#if 1
    if(-1 == wifi_socket_open())
        return FALSE;
    if(-1 == wifi_set_interface_ip(netname, ipaddr))
    {
        printf ("wifi set interface ip error!\n");
    }
    wifi_using_dhcp (netname);

    if(-1 ==  wifi_set_interface_mask(netname, submask))
    {
        printf ("wifi set interface mask error!\n");
    }
    wifi_set_gateway(submask);//no getway
    wifi_add_dns(dns);
    wifi_set_search(search);
    if(-1 == wifi_socket_close())
    {
        printf("socket close error!");
        return FALSE;
    }
    return TRUE;
#endif
}


static void SetStaticIpparam (HWND hwnd)//, NetcardParam *ip_param)
{
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_IPADDR),MSG_SETTEXT, 0, (DWORD)(&(g_static_param.ipaddr[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_SUBM), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.submask[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_DNS), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.dns[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_GATEWAY), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.gateway[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_DOM), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.domain[0])));
    SendMessage(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_CID), MSG_SETTEXT, 0, (DWORD)(&(g_static_param.cid[0])));
}

static void GetStaticIpparam (HWND hwnd)
{


}

static void InitNetcardBitmap(void)
{
    LoadBitmap (HDC_SCREEN, &nc_ok_bmp, SYSTEM_RES"SaveButton.png");
    LoadBitmap (HDC_SCREEN, &nc_cancel_bmp, SYSTEM_RES"CancelButton.png");
    LoadBitmap (HDC_SCREEN, &bootp_bmp, LOCALNETWORK_RES"CloseBOOTP.png");
    LoadBitmap (HDC_SCREEN, &bar_bmp, LOCALNETWORK_RES"FNStringBKG.png");
    LoadBitmap (HDC_SCREEN, &list_bmp, LOCALNETWORK_RES"IPSettingList.png");
    LoadBitmap (HDC_SCREEN, &nc_ipaddr, LOCALNETWORK_RES"IPAddr.png");
    LoadBitmap (HDC_SCREEN, &nc_submask, LOCALNETWORK_RES"SubnetMask.png");
    LoadBitmap (HDC_SCREEN, &nc_gateway, LOCALNETWORK_RES"Gateway.png");
    LoadBitmap (HDC_SCREEN, &nc_dns, LOCALNETWORK_RES"DNS.png");
    LoadBitmap (HDC_SCREEN, &nc_domain, LOCALNETWORK_RES"SearchDomains.png");
    LoadBitmap (HDC_SCREEN, &nc_clientid, LOCALNETWORK_RES"ClientID.png");
    LoadBitmap (HDC_SCREEN, &bg_bmp, LOCALNETWORK_RES"IPSettingBKG.png");
    LoadBitmap (HDC_SCREEN, &nc_fgn, LOCALNETWORK_RES"FNString.png") ;
    LoadBitmap (HDC_SCREEN, &g_DHCPBitmap, LOCALNETWORK_RES"DHCP.png");
    LoadBitmap (HDC_SCREEN, &g_STATICBitmap, LOCALNETWORK_RES"STATIC.png");
}

static void UnloadResource (void) 
{
    UnloadBitmap (&nc_ok_bmp);
    UnloadBitmap (&nc_cancel_bmp);
    UnloadBitmap (&bootp_bmp);
    UnloadBitmap (&bar_bmp);
    UnloadBitmap (&list_bmp);
    UnloadBitmap (&nc_ipaddr);
    UnloadBitmap (&nc_submask);
    UnloadBitmap (&nc_gateway);
    UnloadBitmap (&nc_dns);
    UnloadBitmap (&nc_domain);
    UnloadBitmap (&nc_clientid);
    UnloadBitmap (&bg_bmp);
    UnloadBitmap (&nc_fgn);
    UnloadBitmap (&g_DHCPBitmap);
    UnloadBitmap (&g_STATICBitmap);
}


static int NetcardWinProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    HWND okbtnwnd, cancelbtnwnd, dhcpwnd, bootpwnd, statwnd;
    HWND ipeditwnd, subeditwnd,gweditwnd,dnseditwnd,dmeditwnd, cideditwnd;
    HWND ipstawnd,  substawnd, gwstawnd, dnsstawnd, dmstawnd, cidstawnd;
    char netname[20];
    static int cur_type;
    
    switch (message)
    {
     case MSG_CREATE:
         {
             HDC hdc;
             BOOL bRet = FALSE;
             bRet = GetNetcardParam ();
             if (!bRet)
             {
                 InitParamByDefault ();
             }
             cur_type = g_netcard_ip_param.cur_ip_type;
               
             cancelbtnwnd = CreateWindow (MGD_BUTTON,"cacel",
                     WS_VISIBLE,
                     IDC_BTN_NETCARD_CANCEL,
                     4, 6, 42, 21,
                     hwnd, 
                     (DWORD)(&nc_cancel_bmp));

             okbtnwnd = CreateWindow (MGD_BUTTON, "ok",
                     WS_VISIBLE,// | BS_DEFPUSHBUTTON,
                     IDC_BTN_NETCARD_OK,
                     196, 6, 40, 21,
                     hwnd, 
                     (DWORD)&nc_ok_bmp); 
             if(cur_type == TYPE_DHCP)
             {
                 dhcpwnd = CreateWindow ("MGD_BUTTON", "",
                     WS_CHILD | WS_VISIBLE | MGDBUTTON_2STATE | MGDBUTTON_ANTISTATE,
                     IDC_BTN_NETCARD_DHCP,
                     3, 87, 78, 30,
                     hwnd, (DWORD)(&g_DHCPBitmap));
                 statwnd = CreateWindow ("MGD_BUTTON", "",
                     WS_CHILD | WS_VISIBLE | MGDBUTTON_2STATE,
                     IDC_BTN_NETCARD_STATIC,               
                     160, 87, 77, 30,
                     hwnd, (DWORD)(&g_STATICBitmap));
             }else if(cur_type == TYPE_STATIC){
               dhcpwnd = CreateWindow ("MGD_BUTTON", "",
                     WS_CHILD | WS_VISIBLE | MGDBUTTON_2STATE,
                     IDC_BTN_NETCARD_DHCP,
                     3, 87, 78, 30,
                     hwnd, (DWORD)(&g_DHCPBitmap));
               statwnd = CreateWindow ("MGD_BUTTON", "",
                     WS_CHILD | WS_VISIBLE | MGDBUTTON_2STATE | MGDBUTTON_ANTISTATE,
                     IDC_BTN_NETCARD_STATIC,               
                     160, 87, 77, 30,
                     hwnd, (DWORD)(&g_STATICBitmap));
                 
             
             }
             bootpwnd = CreateWindow ("MGD_BUTTON", "",
                     WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                     IDC_BTN_NETCARD_BOOTP,
                     81, 87, 79, 30,
                     hwnd, (int)&bootp_bmp);
             memcpy(&g_dhcp_param, &g_netcard_ip_param, sizeof(NetcardParam));
             memcpy(&g_static_param, &g_netcard_ip_param, sizeof(NetcardParam));
             
             ipstawnd = CreateWindowEx(CTRL_STATIC, g_dhcp_param.ipaddr,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_NETCARD_IPADDR,
                     110, 136, 110, 20,
                     hwnd, 0);
             substawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.submask,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_NETCARD_SUBM,
                     110, 168, 110, 20,
                     hwnd, 0);
             gwstawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.gateway,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_NETCARD_GATEWAY,
                     110, 200, 110, 20,
                     hwnd, 0);
             dnsstawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.dns,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_NETCARD_DNS,
                     110, 232, 110, 20,
                     hwnd, 0);
             dmstawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.domain,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_NETCARD_DOM,
                     110, 264, 110, 20,
                     hwnd, 0);             
             cidstawnd = CreateWindowEx (CTRL_STATIC, g_dhcp_param.cid,
                     WS_CHILD | WS_VISIBLE | SS_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_STATIC_NETCARD_CID,
                     110, 296, 110, 20,
                     hwnd, 0);          
             ipeditwnd = CreateWindowEx(CTRL_SLEDIT, "",
                        WS_CHILD | WS_VISIBLE | ES_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_NETCARD_IPADDR,
                     110, 136, 110, 20,
                     hwnd, 0);
             subeditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | ES_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_NETCARD_SUBM,
                     110, 168, 110, 20,
                     hwnd, 0);
              gweditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | ES_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_NETCARD_GATEWAY,
                     110, 200, 110, 20,
                     hwnd, 0);
             dnseditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | ES_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_NETCARD_DNS,
                     110, 232, 110, 20,
                     hwnd, 0);
             dmeditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | ES_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_NETCARD_DOM,
                     110, 264, 110, 20,
                     hwnd, 0);              
             cideditwnd = CreateWindowEx (CTRL_SLEDIT, "",
                     WS_CHILD | WS_VISIBLE | ES_RIGHT,
                     WS_EX_TRANSPARENT,
                     IDC_SLEDIT_NETCARD_CID,
                     110, 296, 110, 20,
                     hwnd, 0);        
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
            return 0;
      } 
    case MSG_COMMAND:
#if 1 
     case BUTTON_KEYDOWN:
        {
            int id_btn;
            char buff[100];
            id_btn = LOWORD(wParam);
            switch(id_btn)
            {
                case IDC_BTN_NETCARD_DHCP:
                    CloseSYSIMEMethod (FALSE);
                    if(TYPE_STATIC == cur_type)                   
                    {
                        //SetIpparamNone(hwnd);
                        //SetWindowAdditionalData (GetDlgItem(hwnd, IDC_BTN_NETCARD_STATIC),  (DWORD)&static_close_bmp);
                        SendMessage (GetDlgItem(hwnd, IDC_BTN_NETCARD_STATIC), BUTTON_ANTISTATE, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BTN_NETCARD_STATIC), BUTTON_NORMAL, 0, 0);
                        //SetWindowAdditionalData (GetDlgItem(hwnd, IDC_BTN_NETCARD_DHCP),  (DWORD)&dhcp_open_bmp);
                        SendMessage (GetDlgItem(hwnd, IDC_BTN_NETCARD_DHCP), BUTTON_ANTISTATE, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BTN_NETCARD_DHCP), BUTTON_NORMAL, 0, 0);

                        ShowWindow (GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_IPADDR), SW_HIDE);  
                        ShowWindow (GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_SUBM), SW_HIDE);
                        ShowWindow (GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_GATEWAY), SW_HIDE);
                        ShowWindow (GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_DNS), SW_HIDE);
                        ShowWindow (GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_DOM), SW_HIDE);
                        ShowWindow (GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_CID), SW_HIDE);
                    }
#if 1 
                    ShowWindow (GetDlgItem(hwnd, IDC_STATIC_NETCARD_IPADDR), SW_SHOW);  
                    ShowWindow (GetDlgItem(hwnd, IDC_STATIC_NETCARD_SUBM), SW_SHOW);
                    ShowWindow (GetDlgItem(hwnd, IDC_STATIC_NETCARD_GATEWAY), SW_SHOW);
                    ShowWindow (GetDlgItem(hwnd, IDC_STATIC_NETCARD_DNS), SW_SHOW);
                    ShowWindow (GetDlgItem(hwnd, IDC_STATIC_NETCARD_DOM), SW_SHOW);
                    ShowWindow (GetDlgItem(hwnd, IDC_STATIC_NETCARD_CID), SW_SHOW);
#endif
                    cur_type = TYPE_DHCP;
                    break;
               case IDC_BTN_NETCARD_BOOTP:
                  //  cur_type = TYPE_BOOTP;
                    break;
               case IDC_BTN_NETCARD_STATIC:
                    if(TYPE_DHCP == cur_type)
                    {
                        SendMessage (GetDlgItem(hwnd, IDC_BTN_NETCARD_DHCP), BUTTON_ANTISTATE, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BTN_NETCARD_DHCP), BUTTON_NORMAL, 0, 0);
                        //SetWindowAdditionalData (GetDlgItem(hwnd, IDC_BTN_NETCARD_STATIC),  (DWORD)&static_open_bmp);
                        SendMessage (GetDlgItem(hwnd, IDC_BTN_NETCARD_STATIC), BUTTON_ANTISTATE, 0, 0);
                        SendMessage (GetDlgItem(hwnd, IDC_BTN_NETCARD_STATIC), BUTTON_NORMAL, 0, 0);
                        
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_NETCARD_IPADDR), SW_HIDE);  
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_NETCARD_SUBM), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_NETCARD_GATEWAY), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_NETCARD_DNS), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_NETCARD_DOM), SW_HIDE);
                        ShowWindow ( GetDlgItem(hwnd, IDC_STATIC_NETCARD_CID), SW_HIDE);                    
                    }
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_IPADDR), SW_SHOWNORMAL);  
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_SUBM), SW_SHOWNORMAL);
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_GATEWAY), SW_SHOWNORMAL);
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_DNS), SW_SHOWNORMAL);
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_DOM), SW_SHOWNORMAL);
                    ShowWindow ( GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_CID), SW_SHOWNORMAL);
                    SetStaticIpparam(hwnd);
                    cur_type = TYPE_STATIC;
                    break;
                case IDC_BTN_NETCARD_OK:
                    g_netcard_ip_param.cur_ip_type = cur_type; 
                    
                    SetNetcardParam(hwnd,cur_type);
                    GetWindowText(GetDlgItem(hwnd, IDC_SLEDIT_NETCARD_SUBM), buff, sizeof(buff));
                    SendMessage(hwnd, MSG_CLOSE, 0, 0);
                    break;
                case IDC_BTN_NETCARD_CANCEL:
                    SendMessage(hwnd, MSG_CLOSE, 0, 0);
                    break;
           }
        }
#endif
        return 0;
     case MSG_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            const RECT* clip = (const RECT*) lParam;
            BOOL fGetDC = FALSE;
            RECT rcTemp;

            if (hdc == 0){
                //hdc = BeginPaint (hwnd);//GetClientDC (hwnd);
                hdc    = GetSecondaryClientDC(hwnd);
                fGetDC = TRUE;
            }
            if (clip){
                rcTemp = *clip;
                ScreenToClient (hwnd, &rcTemp.left, &rcTemp.top);
                ScreenToClient (hwnd, &rcTemp.right, &rcTemp.bottom);
                IncludeClipRect (hdc, &rcTemp);
            }
            BitBlt (g_hdcBKG, 0, 0, 0, 0, hdc, 0, 0, 0 );
            if (fGetDC) {
                //EndPaint (hwnd, hdc);//ReleaseDC (hdc);
                ReleaseSecondaryDC(hwnd, hdc);
            }
            return 0;
        }
    case MSG_CLOSE:
        CloseSYSIMEMethod (FALSE);
        SendMessage (g_hSysMain, MSG_CLOSE_APP, 0, 2);
        return 0;

    case MSG_SYS_CLOSE:
        {
            UnloadResource ();
            DeleteCompatibleDC (g_hdcBKG);
            DestroyMainWindow (hwnd);
            //PostQuitMessage (hwnd);
            return 0;
        }
    }

    return DefaultMainWinProc (hwnd, message, wParam, lParam);
}

static void InitNetcardCreateInfo(PMAINWINCREATE createInfo)
{
    
    createInfo->dwStyle = WS_NONE;
    createInfo->dwExStyle = WS_EX_AUTOSECONDARYDC;
    createInfo->spCaption = "";
    createInfo->hMenu = 0;
    createInfo->hCursor = GetSystemCursor (0);
    createInfo->hIcon = 0;
    createInfo->MainWindowProc = NetcardWinProc;
    createInfo->lx = 0;
    createInfo->ty = 0;
    createInfo->rx = 240;
    createInfo->by = 320;

    createInfo->iBkColor = PIXEL_lightwhite;
    createInfo->dwAddData = 0;
    createInfo->hHosting = HWND_DESKTOP;
}

HWND CreateNetworkWindow (void)
{
    MAINWINCREATE createInfo;

    InitNetcardCreateInfo(&createInfo);
    InitNetcardBitmap();

    g_hdcBKG = CreateCompatibleDC (HDC_SCREEN);
    FillBoxWithBitmap (g_hdcBKG, 0,0,240,320,&bg_bmp);
    FillBoxWithBitmap (g_hdcBKG, 3,34,234,30,&bar_bmp);
    FillBoxWithBitmap (g_hdcBKG, 63,44,110,12,&nc_fgn);
    FillBoxWithBitmap (g_hdcBKG, 3,124,234,193,&list_bmp);
    FillBoxWithBitmap (g_hdcBKG, 13,136,61,10,&nc_ipaddr);
    FillBoxWithBitmap (g_hdcBKG, 13,168,74,10,&nc_submask);
    FillBoxWithBitmap (g_hdcBKG, 13,200,50,10,&nc_gateway);
    FillBoxWithBitmap (g_hdcBKG, 13,232,26,10,&nc_dns);
    FillBoxWithBitmap (g_hdcBKG, 13,264,93,10,&nc_domain);
    FillBoxWithBitmap (g_hdcBKG, 13,296,49,10,&nc_clientid);
    
    return CreateMainWindow (&createInfo);
}
