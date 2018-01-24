#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<minigui/common.h>
#undef _USE_MINIGUIENTRY
#include<minigui/minigui.h>
#include<minigui/gdi.h>
#include<minigui/window.h>
#include<minigui/control.h>

#include "ebcontrol.h"
#include "eblistview.h"

#include "../sharedbuff.h" 

#define MSG_SHAREBUFFER_READY    5000

#define  NAME_LEN             20
#define  DESC_LEN             128
#define  CITY_NUM             5

#define  SPACE_HEIGHT           20
#define  SPACE_WIDTH            10

#define  RIM_WEIGHT             5

#define  COLOR_WHITE_R        255
#define  COLOR_WHITE_G        255
#define  COLOR_WHITE_B        255

#define  COLOR_FIRST_R        65 
#define  COLOR_FIRST_G        40
#define  COLOR_FIRST_B        62

#define  COLOR_SECOND_R       56
#define  COLOR_SECOND_G       33
#define  COLOR_SECOND_B       51

#define  IDC_LISTVIEW_OB     0x1005
#define  RESOURCE_PATH         "./res/"

typedef  enum{
    MODE_240_320=0,
    MODE_320_240
}DIS_MODE_E;

typedef enum{
   LG_CHINA = 0,
   LG_JAPANESE,
   LG_HONGKONG,
   LG_ENGLISH,
   LG_ARABIC,
   LG_MAX   
}LANGUAGE_E;

typedef enum{
   WEATHER_SUNNY = 0,
   WEATHER_SUNCLOUDY,
   WEATHER_LIGHTENING,
   WEATHER_HAIL,
   WEATHER_RAIN,
   WEATHER_MAX
}WEATHER_E;

typedef  enum{
    BK_INDEX=0,
    ICON_INDEX,
    CITY_INDEX,
    DES_INDEX,
    SRC_INDEX,
    MAX_INDEX
}INDEX_E;

typedef  struct _MG_CITY{
    char           cName[NAME_LEN];
    char           cDescription[DESC_LEN];
    char           cDescription2[DESC_LEN];
    char           cDescription3[DESC_LEN];
    
    int            nMinrate;    
    int            nMaxrate;
    
    WEATHER_E      Weather;
    LANGUAGE_E     Language;
}MG_CITY,*PMG_CITY;

static int  LoadBitmapNeeded(void);

//static  PMG_CITY  g_pCurCity;
//gb2312 euc-jp utf-8 *  iso-8859-6iso-8859-6iso-8859-6iso-8859-6iso-8859-6iso-8859-6iso-8859-6iso-8859-6iso-8859-6 
static MG_CITY g_City[CITY_NUM]={
	    {"北京",
         "北京天气预报：","7月29日，晴","气温17～10度，微风",
         10,17,WEATHER_SUNNY,LG_CHINA},
        {{0x00,0x23,0x43},
         "東京都の天気予報"," "," ",
         8,22,WEATHER_SUNCLOUDY,LG_JAPANESE},
        {"台北","台北天气預報：","7月29日，有短時雷電","气溫9～21度，風力9～11級",
            9,21,WEATHER_LIGHTENING,LG_HONGKONG},
        {"New York",
         "NewYork weather:","29 Jul snow, temp. 6 ~23","centigrade, breeze."
             ,8,22,WEATHER_HAIL,LG_ENGLISH},
        {"deheilan","tianqi","2","3",
        8,24,WEATHER_RAIN,LG_ARABIC}
};
 
//static PMG_CITY g_pCurCity=&(g_City[0]);
//const  PMG_CITY g_pFirstCity=&(g_City[0]),g_pEndCity=&(g_City[CITY_NUM-1]);
static int selected_item =1;
static DIS_MODE_E  g_mode=-1;

static BITMAP  bg_bitmap;
static BITMAP  lv_bk[3]; 
static BITMAP  weather_fi[CITY_NUM],weather_se[CITY_NUM];
//static BITMAP  icon_bitmap[CITY_NUM];
//static BITMAP  temperature_bitmap[CITY_NUM]; 
//static BITMAP  hightlight_bitmap;
static PLOGFONT  pfont[LG_MAX]={NULL};
static PLOGFONT  tinyfont[LG_MAX]={NULL};
/*const  static char pstr_bg_path[][100]={"res/bg_220x280.png",
                                            "res/bg_300x200.png"
                                            };
const  static char pstr_header_path[][100]={"res/header_sun.png",
                                                "res/header_suncloudy.png",
                                                "res/header_lightening.png",
                                                "res/header_hail.png",
                                                "res/header_sun_rain.png"
                                            };
const  static char pstr_icon_path[][30] = {"res/mini_sun.png",
                                            "res/mini_sun_cloudy.png",
                                            "res/mini_lightening.png",
                                            "res/mini_hail.png",
                                            "res/mini_sun_rain.png"
                                            };
const  static char pstr_temperature_path[][30]={"res/t_17x10.png",
                                                "res/t_22x8.png",
                                                "res/t_21x9.png",
                                                "res/t_22x8.png",
                                                "res/t_24x8.png"
                                                };*/
//const  static char   plight_str[][30]={"res/light_240x320.png","res/light_320x240.png"};
//const  static char   plv_str[][30]={"res/lv_240x320.png","res/lv_320x240.png"};
/*
 * 背景图坐标，ICON坐标，城市坐标，城市天气描述坐标，信息来源坐标
 */
const  static RECT   prect[]={{0,0,220,280},{90,0,130,30},{4,90+3,215,90+3+32},
                                {15,30+35,210,90},{42,283,210,315},
                                {0,0,300,200},{130,0,170,30},{5,30+3,5+210,30+3+28},
                                {5+210+3,35,5+210+80,200-5},{5+10,200-5-20+5,300-5-80,200-5}
                            };

static FILE *fp = NULL;
static char *japastr[4] ={NULL};
static char *arabicstr[4] ={NULL};
static char linebuf[512];
static BOOL g_bMainToSub;

static LRESULT WeatherForecastProc(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);
HWND       g_hwnd;
int MiniGUIMain (int argc, const char* argv[])
{
   MSG Msg;
   HWND hMainWnd;
   MAINWINCREATE CreateInfo;
   char  coldpath[256];
   char  cnewpath[256];
   if (getcwd(coldpath,255) == NULL)
        return -1;

   strcpy(cnewpath,coldpath);
   strcat(cnewpath,"/weatherforecast/");
   //chdir(cnewpath);
#ifdef _MGRM_PROCESSES
    int i;
    const char* layer = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp (argv[i], "-layer") == 0) {
            layer = argv[i+1];
          if(g_mode != -1)
            break;
        	     }
        if(strcmp(argv[i],"-mode") == 0){
        	 g_mode=atoi(argv[i+1]);
        	  if(strcmp(layer,"") != 0){
        	      break;
        	            }
        	     }
            }
    GetLayerInfo (layer, NULL, NULL, NULL);
    if (JoinLayer (layer, argv[0], 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        exit (1);
    }
#endif
     g_mode = 0;
   if(g_mode == MODE_240_320){
        g_mode=MODE_240_320;
        CreateInfo.rx = 240;
        CreateInfo.by = 320;
       }
   else{
       g_mode=MODE_320_240;
       CreateInfo.rx = 320;
       CreateInfo.by = 240;
   } 

   if(LoadBitmapNeeded())
   {
       printf("load bitmap error!\n");
       return 1;
   }
   
   CreateInfo.dwStyle   = WS_NONE;
   CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC;
   CreateInfo.spCaption = "";
   CreateInfo.hMenu   = 0;
   CreateInfo.hCursor = GetSystemCursor(0);
   CreateInfo.hIcon   = 0;
   CreateInfo.MainWindowProc = WeatherForecastProc;
   CreateInfo.lx = 0;
   CreateInfo.ty = 0;
   CreateInfo.iBkColor  = PIXEL_darkgray;
   CreateInfo.dwAddData = 0;
   CreateInfo.hHosting  = HWND_DESKTOP;

   RegisterEBListViewControl();

   g_hwnd = hMainWnd = CreateMainWindow (&CreateInfo);

   if (hMainWnd == HWND_INVALID)
       return 1;

#if 0
   ShowWindow (hMainWnd, SW_SHOWNORMAL);
#else
    ShowWindowUsingShareBuffer (hMainWnd);
#endif

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    EBListViewControlCleanup();
    MainWindowThreadCleanup (hMainWnd);

    if (chdir(coldpath)) {
        return 1;
    }

    return 0;
}

/*PBITMAP iconlist1[CITY_NUM][3] = {
    {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0} 
};*/

/*PBITMAP iconlist2[CITY_NUM][3] = {
    {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
};*/


static char* read_one_line(BOOL* read_over)
{
    int i = 0;
    char c = 0;

    if (fp == NULL){
        *read_over = TRUE;
        return NULL;
    }
    memset (linebuf, 0, 512);
    while(!(*read_over = feof(fp))) {
        c = fgetc(fp);
        if (c == 0xa || c == 0xd) {
            break;
        }
        linebuf[i] = c;
        i++;
    }
    if (i > 0)
        return linebuf;
    else
        return NULL;
}

static int readjapanese(void)
{
    char* pline = NULL;
    int i = 0, len = 0;
    BOOL read_over = FALSE;

    while(1){
        pline = read_one_line(&read_over);
        if (read_over) break;
        if (!pline) continue;

        len =strlen(pline);
        japastr[i] =calloc(1,len+1);
        memcpy(japastr[i],pline,len);
        i++;
    }

    return 0;
}

static int readarabic(void)
{
    char* pline = NULL;
    int i = 0, len = 0;
    BOOL read_over = FALSE;

    while(1){
        pline = read_one_line(&read_over);
        if (read_over) break;
        if (!pline) continue;

        len =strlen(pline);
        arabicstr[i] =calloc(1,len+1);
        memcpy(arabicstr[i],pline,len);
        i++;
    }

    return 0;
}


static void lv_notify_process (HWND hwnd, int id, int code, DWORD addData)
{
    PEBLSTVWDATA pListdata = (PEBLSTVWDATA)addData;
    //PEBITEMDATA pCell = NULL;
    if (pListdata) {
        //RECT rc = {0,0,240,122};
        RECT rc = {0,0,240,122};
        selected_item = pListdata->nItemSelected;
                //g_pCurCity = g_pFirstCity;
                //pCell = pListdata->pItemSelected;
                //g_pCurCity += (pCell->nRows-1);
        //printf ("selected_item %d\n", selected_item);
        InvalidateRect (g_hwnd, &rc, FALSE);
    }
}

static void   InitListViewObject(HWND hWnd)
{
    int i = 0;
    EBLVCOLOUM stListViewColumn;
    EBLVSUBITEM stListViewSubItem;
    EBLVITEM stListViewItem;
    //HDC  hdc; 
   for (i = 1; i < 4; i++) {
        stListViewColumn.nCols = i;
        stListViewColumn.pszHeadText = "";
        stListViewColumn.width = 70+(i-1)*4;
        SendMessage(hWnd,ELVM_ADDCOLUMN, 0, (LPARAM)&stListViewColumn);
     }
    for (i = 1; i < 6; i++) {
      //printf("%s %s %d %d \n",__FILE__,__FUNCTION__,__LINE__,i);
      memset(&stListViewItem, 0, sizeof(EBLVITEM));
      stListViewItem.nItem = i;
      //stListViewItem.nItem = 
      SendMessage (hWnd, ELVM_ADDITEM, 0, (LPARAM) &stListViewItem);
     
      memset(&stListViewSubItem, 0, sizeof(EBLVSUBITEM));
      stListViewSubItem.nItem = i;
      //city name
      //hdc = GetClientDC(hWnd);
      //SelectFont(hdc,pfont[g_City[i-1].Language]);
      //SetTextColor(hdc,RGB2Pixel(hdc,COLOR_WHITE_R,COLOR_WHITE_G,COLOR_WHITE_B));
      stListViewSubItem.mask = ELV_TEXT;
      stListViewSubItem.subItem = 1;
      stListViewSubItem.pszText = g_City[i-1].cName;
      stListViewSubItem.cchTextMax = strlen(g_City[i-1].cName);
      stListViewSubItem.wordtype = NULL;
      stListViewSubItem.iImage = 0;
      stListViewSubItem.lparam = 0;
      SendMessage (hWnd, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);
      //ReleaseDC(hdc);
     //city weather bitmap
     
      stListViewSubItem.mask = ELV_TEXT;
      stListViewSubItem.subItem = 2;

      /*iconlist1[i-1][0] = &icon_bitmap[i-1];
      iconlist1[i-1][1] = &icon_bitmap[i-1];
      iconlist1[i-1][2] = &icon_bitmap[i-1];*/

      stListViewSubItem.pszText = "3";
      stListViewSubItem.cchTextMax = 1;
      stListViewSubItem.wordtype = NULL;
      //printf("%s %s %d %d %p\n",__FILE__,__FUNCTION__,__LINE__,i,iconlist[0]);
      stListViewSubItem.iImage =(DWORD)NULL;    // (DWORD)iconlist1[i-1];
      stListViewSubItem.lparam = 0;
      SendMessage (hWnd, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);
     //city temperature

      stListViewSubItem.mask = ELV_TEXT;
      stListViewSubItem.subItem = 3;
      /*iconlist2[i-1][0] = &temperature_bitmap[i-1];
      iconlist2[i-1][1] = &temperature_bitmap[i-1];
      iconlist2[i-1][2] = &temperature_bitmap[i-1];*/

      stListViewSubItem.pszText = "2";
      stListViewSubItem.cchTextMax = 1;
      stListViewSubItem.wordtype = NULL;
      stListViewSubItem.iImage = (DWORD)0;    //(DWORD)iconlist2[i-1];
      stListViewSubItem.lparam = 0;
      SendMessage (hWnd, ELVM_FILLSUBITEM, 0, (LPARAM) &stListViewSubItem);
   }
    SendMessage (hWnd, ELVM_SETITEMSELECTED, 0, 1);
    SetFocus (hWnd);
}


#define  HEIGHT_GAP    (prect[g_mode*MAX_INDEX+CITY_INDEX].bottom - \
                        prect[g_mode*MAX_INDEX+CITY_INDEX].top)

#define  WIDTH_GAP     (prect[g_mode*MAX_INDEX+CITY_INDEX].right - \
                        prect[g_mode*MAX_INDEX+CITY_INDEX].left)

static int  LoadBitmapNeeded(void)
{
    /*if(LoadBitmap (HDC_SCREEN, &bg_bitmap, RESOURCE_PATH"weather_bk.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"weather_bk.png");
        return -1;
    }*/
    
    if(LoadBitmap (HDC_SCREEN, &bg_bitmap, RESOURCE_PATH"weather_bk2.gif")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"weather_bk2.gif");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &lv_bk[0], RESOURCE_PATH"itembk1.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"itembk1.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &lv_bk[1], RESOURCE_PATH"itembk2.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"itembk2.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &lv_bk[2], RESOURCE_PATH"itembk3.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"itembk3.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_fi[0], RESOURCE_PATH"snow_fi.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"snow_fi.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_fi[1], RESOURCE_PATH"suncloudy_fi.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"suncloudy_fi.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_fi[2], RESOURCE_PATH"sun_fi.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"sun_fi.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_fi[3],RESOURCE_PATH"sunrain_fi.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"sunrain_fi.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_fi[4],RESOURCE_PATH"thunder_fi.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"thunder_fi.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_se[0],RESOURCE_PATH"snow_se.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"snow_se.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_se[1],RESOURCE_PATH"suncloudy_se.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"suncloudy_se.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_se[2],RESOURCE_PATH"sun_se.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"sun_se.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_se[3],RESOURCE_PATH"sunrain_se.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"sunrain_se.png");
        return -1;
    }

    if(LoadBitmap (HDC_SCREEN, &weather_se[4],RESOURCE_PATH"thunder_se.png")){
        printf("LoadBitmap %s error!\n",RESOURCE_PATH"thunder_se.png");
        return -1;
    }

    return 0;
}

static int InitLogFontNeeded(void)
{
    int i;

    pfont[LG_CHINA] =  CreateLogFont ("upf", "fmsong", "UTF-8",
            FONT_WEIGHT_BOLD,
            FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE,
            FONT_STRUCKOUT_NONE,
            16, 0);
    if(!pfont[LG_CHINA])
        printf("hejian test chinese error\n");

    pfont[LG_JAPANESE] = CreateLogFont ("rbf", "gothic", "JISX0208-1",
            FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            16, 0);
    if(!pfont[LG_JAPANESE])
        printf("hejian test japanese error\n");

    pfont[LG_HONGKONG] = CreateLogFont ("upf", "fmsong", "UTF-8",
            FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL, FONT_OTHER_LCDPORTRAIT,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            16, 0);
    if(!pfont[LG_HONGKONG])
        printf("hejian test hongkong error\n");

    pfont[LG_ENGLISH] =  CreateLogFont ("upf", "unifont", "GB2312",
            FONT_WEIGHT_BOLD,
            FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_OTHER_TTFNOCACHE, FONT_UNDERLINE_NONE,
            FONT_STRUCKOUT_NONE,
            16,0);
    if(!pfont[LG_ENGLISH])
        printf("hejian test english error\n");

    pfont[LG_ARABIC] = CreateLogFont ("vbf", "Naskhi18", "ISO8859-6",
            FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            18, 0);
    if(!pfont[LG_ARABIC])
        printf("hejian test arabic error\n");

    if(!pfont[LG_CHINA] || !pfont[LG_JAPANESE] || !pfont[LG_HONGKONG]
            || !pfont[LG_ENGLISH] || !pfont[LG_ARABIC])
    {
        printf("%s %s %d \n",__FILE__,__func__,__LINE__);
        for(i=0;i < CITY_NUM;i++){
            if(pfont[i])
                DestroyLogFont(pfont[i]);
        }
        return -1;
    }


    tinyfont[LG_CHINA] =  CreateLogFont ("upf", "fmsong", "UTF-8",
            FONT_WEIGHT_LIGHT,
            FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE,
            FONT_STRUCKOUT_NONE,
            14, 0);
    if(!tinyfont[LG_CHINA])
        printf("hejian test chinese tinyfont error\n");

    tinyfont[LG_JAPANESE] = CreateLogFont ("rbf", "gothic", "JISX0208-1",
            FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            14, 0);
    if(!tinyfont[LG_JAPANESE])
        printf("hejian test japanese tinyfont error\n");

    tinyfont[LG_HONGKONG] = CreateLogFont ("upf", "fmsong", "UTF-8",
            FONT_WEIGHT_LIGHT, FONT_SLANT_ROMAN,
            FONT_SETWIDTH_NORMAL, FONT_OTHER_LCDPORTRAIT,
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            14, 0);
    if(!tinyfont[LG_HONGKONG])
        printf("hejian test hongkong tinyfont error\n");

    tinyfont[LG_ENGLISH] =  CreateLogFont ("upf", "fmsong", "ISO8859-1",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_OTHER_TTFNOCACHE, FONT_UNDERLINE_NONE,
            FONT_STRUCKOUT_NONE,
            16,0);
    if(!tinyfont[LG_ENGLISH])
        printf("hejian test english tinyfont error\n");

    tinyfont[LG_ARABIC] = CreateLogFont ("vbf", "Naskhi18", "ISO8859-6",
            FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            12, 0);
    if(!tinyfont[LG_ARABIC])
        printf("hejian test arabic tinyfont error\n");

    if(!tinyfont[LG_CHINA] || !tinyfont[LG_JAPANESE] || !tinyfont[LG_HONGKONG]
            || !tinyfont[LG_ENGLISH] || !tinyfont[LG_ARABIC])
    {
        printf("%s %s %d \n",__FILE__,__func__,__LINE__);
        for(i=0;i < CITY_NUM;i++){
            if(tinyfont[i])
                DestroyLogFont(tinyfont[i]);
        }
        return -1;
    }
    return 0;
}

static void drawitemcallback(HWND hWnd, HDC hdc, void* context)
{
    ITEM_DRAW_CON* drawcontext =(ITEM_DRAW_CON *)context;
    RECT  rect;

    if(!drawcontext)
        return ;

    rect =drawcontext->paint_area;
    SetBkMode(hdc,BM_TRANSPARENT);
    SetTextColor(hdc,RGB2Pixel(hdc,COLOR_WHITE_R,COLOR_WHITE_G,COLOR_WHITE_B));

    if(1 == drawcontext->row && 1 ==drawcontext->total_rows){
        SetBrushColor (hdc, RGBA2Pixel (hdc, 0xFF, 0x00, 0x00, 0xFF));
        FillBox (hdc, rect.left, rect.top, (rect.right -rect.left), (rect.bottom -rect.top));
        TextOut(hdc, rect.left +5,rect.top+3,"the first line, only one line");
    }
    else if(1 == drawcontext->row ||
            3 == drawcontext->row ||
            5 == drawcontext->row){
        if(ROWSTATE_SELECTED == drawcontext->state)
            FillBoxWithBitmap (hdc, rect.left, rect.top, 216, 31, &lv_bk[2]);
        else
            FillBoxWithBitmap (hdc, rect.left, rect.top, 216, 31, &lv_bk[1]);
        switch(drawcontext->row){
            case 1:
                FillBoxWithBitmap (hdc, rect.left, rect.top, 216, 31, &weather_se[2]);
                SelectFont(hdc,pfont[LG_CHINA]);
                TextOut(hdc,rect.left+20,rect.top+10,g_City[LG_CHINA].cName);
                break;
            case 3:
                FillBoxWithBitmap (hdc, rect.left, rect.top, 216, 31, &weather_se[4]);
                SelectFont(hdc,pfont[LG_HONGKONG]);
                TextOut(hdc,rect.left+20,rect.top+10,g_City[LG_HONGKONG].cName);
                break;
            case 5:
                FillBoxWithBitmap (hdc, rect.left, rect.top, 216, 31, &weather_se[3]);
                SelectFont(hdc,pfont[LG_ARABIC]);
                if(arabicstr[0])
                    TextOut(hdc,rect.left+20,rect.top+10,arabicstr[0]);
        }
    }
    else if(2 == drawcontext->row || 
            4 == drawcontext->row){
        if(ROWSTATE_SELECTED == drawcontext->state)
            FillBoxWithBitmap (hdc, rect.left, rect.top, 216, 31, &lv_bk[2]);
        else
            FillBoxWithBitmap (hdc, rect.left, rect.top, 216, 31, &lv_bk[0]);
        switch(drawcontext->row){
            case 2:
                FillBoxWithBitmap (hdc, rect.left, rect.top, 216, 31, &weather_se[1]);
                SelectFont(hdc,pfont[LG_JAPANESE]);
                if(japastr[0])
                    TextOut(hdc,rect.left+20,rect.top+10,japastr[0]);
                break;
            case 4:
                FillBoxWithBitmap (hdc, rect.left, rect.top, 216, 31, &weather_se[0]);
                SelectFont(hdc,pfont[LG_ENGLISH]);
                TextOut(hdc,rect.left+20,rect.top+10,g_City[LG_ENGLISH].cName);
        }
    }
    else
        printf("Here has an error!file:%s, func:%s\n",__FILE__,__func__);
}


static LRESULT WeatherForecastProc(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
    HDC  hDC;
    int i;
    static HWND lv_hWnd;
    static EBLVEXTDATA _listData ;

    switch(nMessage){
        case MSG_CREATE:
            {
#if 0
                if(LoadBitmapNeeded())
                {
                    printf("load bitmap error!\n");
                    return -1;
                }
#endif

                if(InitLogFontNeeded())
                {
                    printf("Init logfont error\n");
                    return -1;
                }

                if (!fp){
                    fp = fopen(RESOURCE_PATH"japa.txt", "rb");
                    if (!fp) {
                        fprintf (stderr, "cannot open %s \n", RESOURCE_PATH"japa.txt");
                    }
                }
                if (fp){
                    fseek(fp, 0, SEEK_SET);
                    readjapanese();
                    fclose(fp);
                    fp =NULL;
                }

                if(!fp){
                    fp =fopen(RESOURCE_PATH"arabic.txt","rb");
                    if(!fp)
                        fprintf(stderr,"can't open %s \n",RESOURCE_PATH"arabic.txt");
                }
                if(fp){
                    fseek(fp,0,SEEK_SET);
                    readarabic();
                }

                _listData.nItemHeight = (g_mode ? 31:31);
                _listData.nItemGap = 0x0;
                _listData.nSelectBKColor = 0xb4a7;
                _listData.nSelectTextColor = 0xffff;
                _listData.nItemBKColor = 0xffff;
                _listData.nItemTextColor = 0x8978;
                _listData.nMouseOverBkColor = 0xffff;
                _listData.nMouseOverTextColor = 0xffff;
                _listData.nItemBKImage = 0;
                _listData.nBKImage = (DWORD)NULL;   //&lv_bitmap;

                lv_hWnd =CreateWindowEx(CTRL_EBLISTVIEW,"",
                        WS_CHILD |ELVS_TYPE3STATE | WS_VISIBLE |
                        ELVS_ITEMUSERCOLOR | ELVS_BKIMAGELEFTTOP | ELVS_BKBITMAP
                        //,WS_EX_TRANSPARENT,IDC_LISTVIEW_OB,
                        ,WS_EX_NONE,IDC_LISTVIEW_OB,
                        12,127,216,155,
                        hWnd,(DWORD)&_listData); 
                SendMessage (lv_hWnd, ELVM_SET_ITEMDRAWCALLBACK, (WPARAM)drawitemcallback,0);
                SetNotificationCallback (lv_hWnd, lv_notify_process); 
                InitListViewObject(lv_hWnd);
            }
            break;
        case MSG_PAINT:
            {
                //i=g_pCurCity-g_pFirstCity;
                hDC=BeginPaint(hWnd);
                SetBkMode(hDC,BM_TRANSPARENT);
                SetTextColor(hDC,RGB2Pixel(hDC,COLOR_WHITE_R,COLOR_WHITE_G,COLOR_WHITE_B));
                FillBoxWithBitmapPart(hDC,0,0,240,122,0,0,&bg_bitmap,0,0);
                switch(selected_item){
                    case 1:
                        FillBoxWithBitmap (hDC,0,0 , 240, 107, &weather_fi[2]);
                        SelectFont(hDC,tinyfont[LG_CHINA]);
                        TextOut(hDC,144,44,g_City[0].cDescription);
                        TextOut(hDC,156,67,g_City[0].cDescription2);
                        TextOut(hDC,112,91,g_City[0].cDescription3);
                        /*DrawText(hDC,g_City[0].cDescription,-1,
                              (void *)&prect[g_mode*MAX_INDEX+DES_INDEX],
                              DT_NOCLIP | DT_LEFT | DT_WORDBREAK);*/
                        break;
                    case 2:
                        FillBoxWithBitmap (hDC, 0,0, 240, 107, &weather_fi[1]);
                        SelectFont(hDC,tinyfont[LG_JAPANESE]);
                        /*
                         * 东京天气预报：
                         * 7月29日，晴转多云
                         * 气温 8～22度，微风
                         * */
                        if(japastr[1])
                            TextOut(hDC,126,46,japastr[1]);
                        if(japastr[2])
                            TextOut(hDC,92,66,japastr[2]);
                        if(japastr[3])
                            TextOut(hDC,46,86,japastr[3]);
                        //DrawText(hDC,japastr[1]/*g_City[1].cDescription*/,-1,
                        //      (void *)&prect[g_mode*MAX_INDEX+DES_INDEX],
                        //      DT_NOCLIP | DT_LEFT | DT_WORDBREAK);

                        break;
                    case 3:
                        FillBoxWithBitmap (hDC, 0,0, 240, 107, &weather_fi[4]);
                        SelectFont(hDC,tinyfont[LG_HONGKONG]);
                        TextOut(hDC,145,41,g_City[2].cDescription);
                        TextOut(hDC,107,69,g_City[2].cDescription2);
                        TextOut(hDC,73,96,g_City[2].cDescription3);
                        /*DrawText(hDC,g_City[2].cDescription,-1,
                              (void *)&prect[g_mode*MAX_INDEX+DES_INDEX],
                              DT_NOCLIP | DT_LEFT | DT_WORDBREAK);*/

                        break;
                    case 4:
                        FillBoxWithBitmap (hDC, 0,0, 240, 107, &weather_fi[0]);
                        SelectFont(hDC,tinyfont[LG_ENGLISH]);
                        TextOut(hDC,107,51,g_City[3].cDescription);
                        TextOut(hDC,57,71,g_City[3].cDescription2);
                        TextOut(hDC,102,91,g_City[3].cDescription3);
                        /*DrawText(hDC,g_City[3].cDescription,-1,
                              (void *)&prect[g_mode*MAX_INDEX+DES_INDEX],
                              DT_NOCLIP | DT_LEFT | DT_WORDBREAK);*/

                        break;
                    case 5:
                        FillBoxWithBitmap (hDC, 0,0, 240, 107, &weather_fi[3]);
                        SelectFont(hDC,tinyfont[LG_ARABIC]);
                        /*
                         * 德黑兰天气预报：
                         * 7月29日晴有小雨
                         * 气温9～24度，北风三级
                         * */
                        if(arabicstr[1])
                            TextOut(hDC,89,40,arabicstr[1]);
                        if(arabicstr[2])
                            TextOut(hDC,71,68,arabicstr[2]);
                        if(arabicstr[3])
                            TextOut(hDC,53,96,arabicstr[3]);

                        /*DrawText(hDC,g_City[4].cDescription,-1,
                              (void *)&prect[g_mode*MAX_INDEX+DES_INDEX],
                              DT_NOCLIP | DT_LEFT | DT_WORDBREAK);*/
                }
                //SelectFont(hDC,pfont[g_pCurCity->Language]);
                //DrawText(hDC,g_pCurCity->cDescription,-1,
                //        (void *)&prect[g_mode*MAX_INDEX+DES_INDEX],
                //        DT_NOCLIP | DT_LEFT | DT_WORDBREAK);
                SelectFont(hDC,tinyfont[LG_CHINA]);
                DrawText(hDC,"updated from yahoo",-1,
                        (void *)&prect[g_mode*MAX_INDEX+SRC_INDEX],
                        DT_NOCLIP | DT_LEFT | DT_WORDBREAK);
                EndPaint(hWnd,hDC);
                //SendMessage (lv_hWnd, ELVM_SETITEMSELECTED, 0, g_pCurCity-g_pFirstCity+1);
                //break;
                return 0;
            }
        case MSG_SHAREBUFFER_READY:
            g_bMainToSub = TRUE;
            return 0;
        case MSG_ERASEBKGND:
            {
                HDC hdc = (HDC)wParam;
                const RECT* clip = (const RECT*) lParam;
                BOOL fGetDC = FALSE;
                RECT rcTemp;

                if (g_bMainToSub) {
                    g_bMainToSub = FALSE;
                    return 0;
                }

                if (hdc == 0){
                    //hdc = GetClientDC (hWnd);
                    hdc = GetSecondaryClientDC(hWnd);
                    fGetDC = TRUE;
                }
                if (clip){
                    rcTemp = *clip;
                    ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
                    ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
                    IncludeClipRect (hdc, &rcTemp);
                }
                FillBoxWithBitmap (hdc, 0, 0, 240, 320, &bg_bitmap);
                if (fGetDC) {
                    //ReleaseDC (hdc);
                    ReleaseSecondaryDC(hWnd, hdc);
                }
            }
            return 0;
        case MSG_KEYUP:
            if(wParam == 0x6c)
            {
                /*if(g_pCurCity == g_pEndCity)
                    g_pCurCity = g_pFirstCity;
                else
                    g_pCurCity++;*/
            }
            else if(wParam == 0x67)
            {
                /*if(g_pCurCity == g_pFirstCity)
                    g_pCurCity = g_pEndCity;
                else
                    g_pCurCity--;*/
            }
            else if(wParam == 0x01)
                SendMessage(hWnd,MSG_CLOSE,0,0);
            //InvalidateRect(hWnd,NULL,TRUE);
            break;
        case MSG_CLOSE:
            {
                for(i=0;i < CITY_NUM;i++){
                    if(pfont[i])
                        DestroyLogFont(pfont[i]);
                }
                UnloadBitmap(&bg_bitmap);
                for(i=0;i<3;i++)
                    UnloadBitmap(&lv_bk[i]);
                for(i=0;i < CITY_NUM;i++){
                    UnloadBitmap(&weather_fi[i]);
                    UnloadBitmap(&weather_se[i]);
                }
                for(i=0;i<4;i++){
                    if(japastr[i]){
                        free(japastr[i]);
                        japastr[i] =NULL;
                    }
                }
                for(i=0;i<4;i++){
                    if(arabicstr[i]){
                        free(arabicstr[i]);
                        arabicstr[i] =NULL;
                    }
                }
                if(fp){
                    fclose(fp);
                    fp =NULL;
                }
                DestroyWindow(lv_hWnd);
                DestroyMainWindow(hWnd); 
                PostQuitMessage(hWnd);
                return 0;
            }
        default:
            break;
    }
    return DefaultMainWinProc(hWnd,nMessage,wParam,lParam);
}


