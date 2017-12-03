


#ifndef WIFISET_H
#define WIFISET_H  

#include <stdlib.h>
//#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "./../sysmain.h"

HWND g_hWiFiNetSetting;
HWND g_hWiFiMainMenu;

typedef struct _IpParam{
  int dnsnum; 
  char ipaddr[16];       //192.168.111.124
  char submask[16];
  char router[32];
  char router_key[8];
  char dns[256];
  char domain[256];
  char cid[16];
}IpParam;

typedef struct _Network{  
    char netname[32];
    int cur_ip_type;        //current type
    IpParam ip_param;       //dhcp
}Network;
typedef struct _WifiParam{
    BOOL wifi_is_open;        //wifi is open?
    BOOL is_auto_cnt;         //wifi is auto connect?
    int  count_of_net;        //count of net;
    int  cur_net;             //current net;
    Network net[16];          //net
}WifiParam;
//int CreateWifisetWindow (HWND hWnd);
void SetWifiNetParam(Network *network);
void GetWifiNetParam(Network *network);
HWND CreateWIFIWindow (void);
HWND CreateWiFiManualWindow (void);

#endif
