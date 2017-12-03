
#include "layouthlp.h"

struct str_dword_map{
	const char* strName;
	DWORD value;
} ;
static struct str_dword_map bkground_maps[] = {
	{"white",0xffffff},
	{"black",0x0},
	{"lightgray",0xc0c0c0},
	{"gray",0x808080},
	{"darkgray",0x404040},
	{"red",0xff0000},
	{"pink",0xffafaf},
	{"orange",0xffc800},
	{"yellow",0xffff00},
	{"green",0x00ff00},
	{"magenta",0xff00ff},
	{"cyan",0x00ffff},
	{"blue",0x0000ff}
};

#define CHAR2HEX(c)  (((c)>='0' && (c)<='9')?(c)-'0' \
	:( ((c)>='A'&&(c)<='F')?(c)-'A'+10:((c)>='a'&&(c)<='f')?(c)-'a'+10:0) )
//static int ParseBkground(const char* szValue, LAYOUT_BKGROUND *pbkg)
int ParseBkground(const char* szValue, LAYOUT_BKGROUND *pbkg)
{
	/*  serval color and type
	 */

	int value = 0;
	if(szValue[0]=='#')
	{
		//hex text like #RRGGBB
		int i;
		for(i=1;i<=6&&szValue[i];i++)
		{
			value<<=4;
			value|=(CHAR2HEX(szValue[i]));
		}
		pbkg->type = lbtColor;
		pbkg->data.bkcolor = value;
	}
	else if(strchr(szValue,'.')!=NULL) // is file
	{
		//may be leak memory
		PBITMAP pbmp = (PBITMAP)malloc(sizeof(BITMAP));
		LoadBitmapFromFile(HDC_SCREEN,pbmp,szValue);
		if(pbmp!=NULL)
		{
			pbkg->type = lbtBmpBrush;
			pbkg->data.tilBmp = pbmp;
		}
	}
	else
	{
		int i;
		for(i=0;i<sizeof(bkground_maps)/sizeof(struct str_dword_map);i++)
		{
			if(strcmp(bkground_maps[i].strName,szValue)==0){
				value = bkground_maps[i].value;
				break;
			}
		}
		pbkg->type = lbtColor;
		pbkg->data.bkcolor = value;
	}
	return 1;
}

static int ParseUnit(const char* szValue, int* type, int *log_size){
	if(szValue == 0) return 0;
	*log_size = atoi(szValue);
	while(*szValue && *szValue!='%') szValue ++;
	if(*szValue == '%') 
		*type = 1;
	else
		*type = 0;
	return 1;
}

static LAYOUT_UNIT* BuildUnits(const char* etcFile,const char* szSectName, const char* keyFormat, int n)
{
	int i;
	char szKey[32];
	char szValue[32];
	LAYOUT_UNIT* units = NULL;
	for(i=0;i<n;i++)
	{
		sprintf(szKey,keyFormat,i);
		if(GetValueFromEtcFile(etcFile, szSectName, szKey, szValue,sizeof(szValue)-1) == ETC_OK)
		{
			int type, log_size;
			if(ParseUnit(szValue,&type,&log_size))
			{
				if(units == NULL)
					units = (LAYOUT_UNIT*)calloc(n,sizeof(LAYOUT_UNIT));
				units[i].type = type==0?lutPixel:lutPercent;
				units[i].logic_size = log_size;
			}
		}
	}
	return units;
}

GRID_LAYOUT* LoadGridLayoutFromEtcFile(const char* etcFile, const char* name, PGridLayoutProc glProc, void* user_data,
	LAYOUT_CELL_OBJ* (*loadCell)(const char* etcFile, const char* sectName, void* data), void* data)
{
	char szValue[256];
	char szKey[50];

	//test is a type of gridlayout?
	if(GetValueFromEtcFile(etcFile, name, "type", szValue, sizeof(szValue)-1) != ETC_OK)
		return NULL;
	if(strcmp(szValue,"gridlayout") != 0)
		return NULL;
	
	int cols = 0;
	int rows = 0;
	
	GetIntValueFromEtcFile(etcFile, name, "cols", &cols);
	GetIntValueFromEtcFile(etcFile, name, "rows", &rows);
	if(cols <= 0 || rows <= 0)
		return NULL;
	
	GRID_LAYOUT *gl = CreateGridLayout(rows,cols, glProc,user_data);

	if(!gl)
		return NULL;

	gl->row_height = BuildUnits(etcFile, name, "row%d", rows);
	gl->col_width = BuildUnits(etcFile, name, "col%d", cols);

	//get bkground
	if(GetValueFromEtcFile(etcFile,name,"bkground",szValue,sizeof(szValue)-1) == ETC_OK)
	{
		ParseBkground(szValue, &gl->bkground);
	}

	int col;
	for(col=0; col<cols; col++)
	{
		int row;
		for(row=0; row<rows; row++)
		{
			sprintf(szKey,"cell%d-%d",row,col);
			if(GetValueFromEtcFile(etcFile, name, szKey, szValue, sizeof(szValue)-1)==ETC_OK)
			{
				LAYOUT_CELL_OBJ * obj = NULL;
				if(GetValueFromEtcFile(etcFile,szValue,"type",szKey,sizeof(szKey)-1)==ETC_OK)
				{
					if(strcmp(szKey,"gridlayout") == 0)
					{
						obj = (LAYOUT_CELL_OBJ*)LoadGridLayoutFromEtcFile(etcFile,szValue,glProc,user_data,loadCell,user_data);
					}
					else 
						goto LOAD_CALLBACK;
				}
				else{
LOAD_CALLBACK:
					if(loadCell){
						obj = (*loadCell)(etcFile,szValue, data);
					}
				}
				gl->cells[col][row].obj = obj;
			}
		}
	}

	return gl;
}

//////////////////////////////////////////
//wnidow layout

static int wndLayoutDispatchMessage(WND_LAYOUT* wl, int message, WPARAM wParam, LPARAM lParam)
{
	if(message == LAY_MSG_MOVETO)
	{
		PRECT prt = (PRECT)lParam;
		MoveWindow(wl->hwnd,prt->left,prt->top,
			prt->right-prt->left,
			prt->bottom-prt->top,
			TRUE);
	}
	else {
		return	SendMessage(wl->hwnd,message,wParam,lParam);
	}
	return 0;
}

static int wndLayoutFree(WND_LAYOUT* wl)
{
	if(wl)
		DestroyWindow(wl->hwnd);
}

static LAYOUT_CELL_INTF wnd_cell_intf={
	wndLayoutDispatchMessage,
	wndLayoutFree
};

WND_LAYOUT * CreateWndLayout(HWND hwnd)
{
	WND_LAYOUT* wl = (WND_LAYOUT*)malloc(sizeof(WND_LAYOUT));
	wl->hwnd = hwnd;
	wl->intf = &wnd_cell_intf;
}

