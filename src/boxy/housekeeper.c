/*
** $Id: housekeeper.c 224 2007-07-03 09:38:24Z xwyan $
**
** housekeeper.c: The module of housekeeper for Feynman PMP solution.
**
** Copyright (C) 2007 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: Zhang Hua .
*/

#undef _DEBUG_MSG

#include "housekeeper.h"
#include "housekeeper_map.h"
#include "housekeeper_text.h"

#include "../sharedbuff.h" 

#define HOUSEKEEPER_SCORE_FILENAME  "housekeeper_score.conf"

static HWND hMainWnd=HWND_INVALID;
ptagLevelCollection pTempColl;

static int HousekeeperProc ( HWND hWnd, int message, WPARAM wParam, LPARAM lParam );
static void UpdateHighScore ( int ScoreNumber );
static void _housekeeper_init_create_info ( PMAINWINCREATE pCreateInfo );

static BITMAP bitmapAll[10];
static int HighScore[5];
static int ScoreNumber = 0;

static char *level_set_str[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" };
static int rank_set_str[] = { IDS_PMP_HOUSEKEEPER_PREV, IDS_PMP_HOUSEKEEPER_NEXT };

typedef struct _HOUSEKEEPER_MENU_PARAMS {
    int rank;
    int level;
} HOUSEKEEPER_MENU_PARAMS;

static int housekeeper_menu_item_text_id [] = {
    IDS_PMP_HOUSEKEEPER_UNDO,
    IDS_PMP_HOUSEKEEPER_RESTART,    
    IDS_PMP_HOUSEKEEPER_SET_RANK,
    IDS_PMP_HOUSEKEEPER_SET_LEVEL,
    IDS_PMP_HOUSEKEEPER_SCORE_INFO
};

HOUSEKEEPER_MENU_PARAMS housekeeper_menu_params = {0, 0};

int _housekeeper_menu_undo_callback ( void *context, int id, int op, char *text_buff )
{
    switch ( op ) {

        case PMP_MI_OP_DEF:
            sprintf ( text_buff, _(housekeeper_menu_item_text_id[0]) );
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_ENTER:
            return PMP_MI_ST_SELECTED;
    }
    return PMP_MI_ST_NONE;
}

int _housekeeper_menu_restart_callback ( void *context, int id, int op, char *text_buff )
{
    switch ( op ) {

        case PMP_MI_OP_DEF:
            sprintf ( text_buff, _(housekeeper_menu_item_text_id[1]) );
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_ENTER:
            return PMP_MI_ST_SELECTED;
    }
    return PMP_MI_ST_NONE;
}

int _housekeeper_menu_setrank_callback ( void *context, int id, int op, char *text_buff )
{
    HOUSEKEEPER_MENU_PARAMS* my_params = ( HOUSEKEEPER_MENU_PARAMS* )context;

    switch ( op ) {

        case PMP_MI_OP_DEF:
            sprintf ( text_buff, _(housekeeper_menu_item_text_id[2]), _(rank_set_str[ my_params->rank ]) );
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_ENTER:
            return PMP_MI_ST_SELECTED;

        case PMP_MI_OP_PREV:
            if ( my_params->rank > 0 )
                my_params->rank = 0;
            sprintf ( text_buff, _(housekeeper_menu_item_text_id[2]), _(rank_set_str[ my_params->rank ]) );
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_NEXT:
            if ( my_params->rank < 1 )
                my_params->rank = 1;
            sprintf ( text_buff, _(housekeeper_menu_item_text_id[2]), _(rank_set_str[ my_params->rank ]) );
            return PMP_MI_ST_UPDATED;            
    }

    return PMP_MI_ST_NONE;
}

int _housekeeper_menu_setlevel_callback ( void *context, int id, int op, char *text_buff )
{
    HOUSEKEEPER_MENU_PARAMS* my_params = ( HOUSEKEEPER_MENU_PARAMS* ) context;

    switch ( op ) {

        case PMP_MI_OP_DEF:
            sprintf ( text_buff, _(housekeeper_menu_item_text_id[3]), level_set_str[my_params->level ] );
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_ENTER:
            return PMP_MI_ST_SELECTED;

        case PMP_MI_OP_PREV:
            if ( my_params->level > 0)
                my_params->level -- ;
            sprintf ( text_buff, _(housekeeper_menu_item_text_id[3]), level_set_str[ my_params->level]) ;
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_NEXT:
            if ( my_params->level < 9)
                my_params->level ++ ;
            sprintf ( text_buff, _(housekeeper_menu_item_text_id[3]), level_set_str[ my_params->level ] );
            return PMP_MI_ST_UPDATED;
    }
    return PMP_MI_ST_NONE;

}

int _housekeeper_menu_highscore_callback ( void *context, int id, int op, char *text_buff )
{
    switch ( op ) {
        case PMP_MI_OP_DEF:

            strcpy ( text_buff, _(housekeeper_menu_item_text_id[4]) );
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_ENTER:
            return PMP_MI_ST_SELECTED;
    }

    return PMP_MI_ST_NONE;
}

static PMP_MENU_ITEM housekeeper_menu[] =
{
    {1, _housekeeper_menu_undo_callback},
    {2, _housekeeper_menu_restart_callback},
    {3, _housekeeper_menu_setrank_callback},
    {4, _housekeeper_menu_setlevel_callback},
    {5, _housekeeper_menu_highscore_callback},
    {0, NULL}
};

static void UpdateHighScore( int ScoreNumber )
{
    int i,j;
    for ( i = 0; i < 5; i++ )
        if ( ScoreNumber > HighScore[ i ] )
            break;
    if ( i == 5 ){
        return;
    }
    for ( j = 4; j > i; j-- ){
        HighScore[j] = HighScore[ j - 1 ];
    }		
    HighScore[ i ] = ScoreNumber;
}


static void OnDraw ( HWND hWnd, HDC hDC ) 
{
    RECT rect;
    POINT poStart1;
    int iSideLength;
    ptagLevel cur_level;
    char strText[ 50 ];
    int iRow;
    int iCol;
    int i, j;
    int iBmp;

    GetClientRect ( hWnd, &rect );
    SetBkColor ( hDC, RGB2Pixel ( HDC_SCREEN, 102, 102, 102 ) );
    SetTextColor ( hDC, COLOR_green );	
    SetPenColor ( hDC,COLOR_black );
    poStart1.x = rect.left;
    poStart1.y = rect.top;

    cur_level = theMap->currentLevel;
    snprintf ( strText, 50, _(IDS_PMP_HOUSEKEEPER_DISPLAY), theMap->current->strName, cur_level->iNo, ScoreNumber );	
    TextOut( hDC, rect.left + 10, rect.bottom - 20, strText );
    rect.bottom -= 20;	
    if (rect.bottom - rect.top < 100)
        return;
    iRow = cur_level->row;
    iCol = cur_level->col;

    i = RECTH( rect )/iRow;
    j = RECTW( rect )/iCol;
    if ( i <= j ) 
        iSideLength = i;
    else 	
        iSideLength = j;	

    for ( i = 0; i < iRow; i++ ) {
        for ( j = 0; j < iCol; j++ ) {
            iBmp = cur_level->data [ i * iCol + j ];
            if ( iBmp < B_NOTHING ) {
                FillBoxWithBitmap ( hDC, poStart1.x + iSideLength * j \
                        , poStart1.y + iSideLength * i, iSideLength, iSideLength, (BITMAP *)RetrieveRes(housekeeper_pic[iBmp]));
            }
        }
    }
}

void Housekeeper_DrawALittleBlock(HWND hWnd, int x, int y, int itype)	
{
	HDC hdc;
	RECT rect;
	int iSideLength;
	ptagLevel cur_level;
	int iRow;
	int iCol;
	int i, j;

	GetClientRect(hWnd,&rect);
	rect.bottom -= 20;	
	if ( rect.bottom - rect.top < 100 )
		return;
	cur_level = theMap->currentLevel;
	iRow = cur_level->row;
	iCol = cur_level->col;
	i = RECTH(rect)/iRow;
	j = RECTW(rect)/iCol;
	if ( i <= j ) 
		iSideLength = i;
	else 	
		iSideLength = j;	

	i = x;
	j = y;

	x = rect.left + i * iSideLength;
	y = rect.top  + j * iSideLength;

	hdc = GetClientDC(hWnd);
	if ( itype <= B_NOTHING ) {
		if ( itype == B_NOTHING ) {
			SetBrushColor ( hdc, RGB2Pixel ( HDC_SCREEN, 102, 102, 102 ) );
			FillBox ( hdc, x, y, iSideLength, iSideLength );
		}
		else {
			FillBoxWithBitmap ( hdc, x, y, iSideLength, iSideLength, (BITMAP *)RetrieveRes (housekeeper_pic[itype]));
        }

	}
	ReleaseDC ( hdc );
}

void Housekeeper_Register (void)
{
    int i;
    for (i = 0; i < ARRAY_LEN(housekeeper_pic); i++)
    {
        if(LoadBitmap (HDC_SCREEN, &bitmapAll[i], housekeeper_pic[i]))
            fprintf (stderr, "LoadBitmap %s error. \n", housekeeper_pic[i]);
        RegisterResFromBitmap (housekeeper_pic[i], &bitmapAll[i]);
    }

    Housekeeper_InitMap ();
}

static void Housekeeper_Unregister( HWND hWnd )
{
    int i;
    for (i = 0; i < ARRAY_LEN (housekeeper_pic); i++)
    {
        UnregisterRes (housekeeper_pic[i]);
    }
    ScoreNumber = 0;

    Housekeeper_DestroyMap();
    DestroyMainWindow ( hWnd );
    MainWindowCleanup ( hWnd );
    //pmp_app_notify_closed (PMP_APP_HOUSEKEEPER);
}


static int HousekeeperProc ( HWND hWnd, int message, WPARAM wParam, LPARAM lParam )
{
    HDC hdc;
    char st[6][20];
    char str[100];
    FILE *stream;
    int iDir = 0;
    switch(message){
        case MSG_CREATE:
            Housekeeper_Register ();
            stream = fopen (PMP_CFG_PATH HOUSEKEEPER_SCORE_FILENAME, "r+" );
            if ( stream == NULL )
            {
                _MY_PRINTF ("Can not open the file of %s to read something", 
                        PMP_CFG_PATH HOUSEKEEPER_SCORE_FILENAME);
                HighScore[0] = 0;
                HighScore[1] = 0;
                HighScore[2] = 0;
                HighScore[3] = 0;
                HighScore[4] = 0;
            }
            else
            {
                fgets ( st[0], 100, stream );
                HighScore[0] = strtol ( st[0], NULL, 10 );
                fgets ( st[1], 100, stream );
                HighScore[1] = strtol ( st[1], NULL, 10 );
                fgets ( st[2], 100, stream );
                HighScore[2] = strtol ( st[2], NULL, 10 );
                fgets ( st[3], 100, stream );
                HighScore[3] = strtol ( st[3], NULL, 10 );
                fgets ( st[4], 100, stream );
                HighScore[4] = strtol ( st[4], NULL, 10 );
                fclose ( stream );
            }
            break;

        case MSG_PAINT:
            hdc = BeginPaint ( hWnd );
            OnDraw ( hWnd, hdc );
            EndPaint ( hWnd, hdc );
            return 0;

        case MSG_KEYDOWN:
            switch ( wParam ) {
                case SCANCODE_CURSORBLOCKUP:     
                    iDir = DIR_UP;
                    break;
                case SCANCODE_CURSORBLOCKLEFT: 
                    iDir = DIR_LEFT;
                    break;
                case SCANCODE_CURSORBLOCKRIGHT:      
                    iDir = DIR_RIGHT;
                    break;
                case SCANCODE_CURSORBLOCKDOWN:        
                    iDir = DIR_DOWN;
                    break;
                case SCANCODE_F1:
                   switch ( pmp_show_menu ( hWnd, _(IDS_PMP_HOUSEKEEPER_TITLE), housekeeper_menu, &housekeeper_menu_params ) ) {

                        case 0:
                            break;
                        case 1:
                            Housekeeper_PlayUndo ( hMainWnd );
                            break;
                        case 2:
                            Housekeeper_PlayRestart ( ); 
                            InvalidateRect ( hMainWnd, NULL, TRUE );
                            break;
                        case 3:
                            if ( 0 == housekeeper_menu_params.rank ){
                                theMap->current->current = theMap->current->current->prev;
                                Housekeeper_PlayRestart();
                                InvalidateRect(hMainWnd, NULL, TRUE);
                            }
                            else{
                                theMap->current->current = theMap->current->current->next;
                                Housekeeper_PlayRestart();
                                InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            break;
                        case 4:
                            if ( housekeeper_menu_params.level == 0 ) {
                                pTempColl = theMap->head;
                                if ( Housekeeper_GotoCollection ( pTempColl ) ) 
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            else if ( housekeeper_menu_params.level == 1 ){
                                pTempColl = theMap->head->next;
                                if ( Housekeeper_GotoCollection ( pTempColl ) ) 
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            else if ( housekeeper_menu_params.level == 2 )
                            {
                                pTempColl = theMap->head->next->next;
                                if ( Housekeeper_GotoCollection ( pTempColl ) )
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            else if ( housekeeper_menu_params.level == 3 )
                            {
                                pTempColl = theMap->head->next->next->next;
                                if ( Housekeeper_GotoCollection ( pTempColl ) )
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            else if ( housekeeper_menu_params.level == 4 )
                            {
                                pTempColl = theMap->head->next->next->next->next;
                                if ( Housekeeper_GotoCollection ( pTempColl ) )
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            else if ( housekeeper_menu_params.level == 5 )
                            {
                                pTempColl = theMap->head->next->next->next->next->next;
                                if ( Housekeeper_GotoCollection ( pTempColl ) ) 
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            else if ( housekeeper_menu_params.level == 6 )
                            {
                                pTempColl = theMap->tail->prev->prev->prev;
                                if ( Housekeeper_GotoCollection ( pTempColl ) ) 
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            else if ( housekeeper_menu_params.level == 7 )
                            {
                                pTempColl = theMap->tail->prev->prev;
                                if ( Housekeeper_GotoCollection ( pTempColl ) ) 
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            else if ( housekeeper_menu_params.level == 8 )
                            {
                                pTempColl = theMap->tail->prev;
                                if ( Housekeeper_GotoCollection ( pTempColl ) ) 
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }
                            else if ( housekeeper_menu_params.level == 9 )
                            {
                                pTempColl = theMap->tail;
                                if ( Housekeeper_GotoCollection ( pTempColl ) )
                                    InvalidateRect ( hMainWnd, NULL, TRUE );
                            }

                            break;
                        case 5:
                            sprintf ( str, _(IDS_PMP_HOUSEKEEPER_SHOW_SCORE), HighScore[0], HighScore[1], HighScore[2], HighScore[3], HighScore[4]);
                            pmp_show_info ( hWnd, _(IDS_PMP_HOUSEKEEPER_SCORE_INFO), str, MB_ICONINFORMATION );
                            break; 
                    }
                default:
                    iDir = -1;
            }

            if ( ( iDir != -1 ) && ( Housekeeper_PlayKeyboard ( hWnd, iDir ) == STEP_TYPE_PUSH ) ){
                if ( Housekeeper_CheckMissionComplete ( ) ) {
                    theMap->current->current = theMap->current->current->next;
                    ScoreNumber = ScoreNumber + 100;
                    Housekeeper_PlayRestart();
                    InvalidateRect ( hWnd, NULL, TRUE );
                }
                return 0;
            }

            if ( wParam == SCANCODE_ESCAPE )
            {
                UpdateHighScore ( ScoreNumber );
                stream = fopen (PMP_CFG_PATH HOUSEKEEPER_SCORE_FILENAME, "w+" );
                if ( stream == NULL )
                {
                    _MY_PRINTF ("Can not open the file of %s to write someting", 
                            PMP_CFG_PATH HOUSEKEEPER_SCORE_FILENAME);
                    pmp_show_info ( hWnd, _(IDS_PMP_HOUSEKEEPER_TEXT), _(IDS_PMP_HOUSEKEEPER_WRFILE_SHOW), MB_ICONINFORMATION );
                }
                else
                {
                    sprintf ( str, "%d\n", HighScore[ 0 ] );
                    fputs ( str, stream );
                    sprintf ( str, "%d\n", HighScore[ 1 ] );
                    fputs ( str, stream );
                    sprintf ( str, "%d\n", HighScore[ 2 ] );
                    fputs ( str, stream );
                    sprintf ( str, "%d\n", HighScore[ 3 ] );
                    fputs ( str, stream );
                    sprintf ( str, "%d\n", HighScore[ 4 ] );
                    fputs ( str, stream );
                    fclose ( stream );
                }
                Housekeeper_Unregister ( hWnd );
            }
            return 0;			
        case MSG_TIMER:
            return 0;			
        case MSG_SETFOCUS:
            //pmp_set_captionbar_title ( _(IDS_PMP_HOUSEKEEPER_TITLE) );
            break;            

        case MSG_DESTROY:
            DestroyAllControls ( hWnd );
            hMainWnd = HWND_INVALID;
            return 0; 
        case MSG_CLOSE:
            Housekeeper_Unregister ( hWnd );
            return 0;
       }
    return DefaultMainWinProc ( hWnd, message, wParam, lParam );
}

static void _housekeeper_init_create_info ( PMAINWINCREATE pCreateInfo )
{
    pCreateInfo->dwStyle = WS_NONE;
    pCreateInfo->dwExStyle = WS_EX_AUTOSECONDARYDC;
    pCreateInfo->spCaption = _(IDS_PMP_HOUSEKEEPER_TITLE);
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = HousekeeperProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = MAINWINDOW_W;
    pCreateInfo->by = MAINWINDOW_H;
    pCreateInfo->iBkColor = RGB2Pixel ( HDC_SCREEN, 102, 102, 102 ); 
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int MiniGUIMain (int args, const char* arg[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer (NAME_DEF_LAYER, "housekeeper", 0, 0);
#endif

    _housekeeper_init_create_info ( &CreateInfo );
    hMainWnd = CreateMainWindow(&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

#if 0
    ShowWindow (hMainWnd, SW_SHOWNORMAL);
#else
    ShowWindowUsingShareBuffer (hMainWnd);
#endif

    while( GetMessage(&Msg, hMainWnd) ) {
        TranslateMessage (&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup(hMainWnd);
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif



