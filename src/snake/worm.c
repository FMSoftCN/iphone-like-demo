/*
** $Id: worm.c 224 2007-07-03 09:38:24Z xwyan $
**
** housekeeper.c: The module of worm game for Feynman PMP solution.
**
** Copyright(C) 2007 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Current maintainer: Zhang Hua .
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "worm.h"
#include "worm_text.h"
#include "worm_level.h"

#include "../sharedbuff.h"

static BITMAP bitmap_worm[10];
static HWND hMainWnd = HWND_INVALID;


#ifndef _240x320_WORM
RECT rect_score = { 60, 190, 300, 220 };
#else
RECT rect_score = { 160, 50, 220, 200 };
RECT rect_score1 = { 160, 50, 220, 100 };
RECT rect_score2 = { 160, 150, 220, 200 };
#endif


/*static BITMAP *bitmap_worm_node;
static BITMAP *bitmap_worm_node_hide;
static BITMAP *bitmap_map_node;
static BITMAP *bitmap_boni_red;
static BITMAP *bitmap_boni_yellow;
static BITMAP *bitmap_boni_hide;*/
static int    FREQ_CLOCK;

static BOOL restart = FALSE;
static worm_list 	worm;
static boni_list 	bon;
static boni_list	bon_live;
static map_node		*array_wall, *array_wall_first;
static int 			killtimes;
static int 			count_wall;
static int 			score_number;
static BOOL 		new = TRUE;
static BOOL 		play = FALSE;
static BOOL 		begin = FALSE;
static BOOL 		end = FALSE;

static int draw_wall( HWND hWnd, HDC hdc, map_node *array_wall, int count, BITMAP *Bitmap );
static int draw_worm( HWND hWnd, HDC hdc, worm_list *worm, BITMAP *Bitmap );
static int draw_boni( HWND hWnd, HDC hdc, boni_list  *bon, BITMAP *Bitmap_red, BITMAP *Bitmap_yellow );

typedef struct _WORM_MENU_PARAMS {
    int level;
} WORM_MENU_PARAMS;

static int worm_menu_item_text_id [] = {
    IDS_PMP_WORM_START,
    IDS_PMP_WORM_SET_LEVEL,
    IDS_PMP_WORM_SCORE_INFO
};

WORM_MENU_PARAMS worm_menu_params = { 1 };

int _worm_menu_start_callback( void *context, int id, int op, char *text_buff )
{
    switch( op ) {

        case PMP_MI_OP_DEF:
            sprintf( text_buff, _(worm_menu_item_text_id[0]) );
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_ENTER:
            return PMP_MI_ST_SELECTED;
    }
    return PMP_MI_ST_NONE;
}

int _worm_menu_setlevel_callback( void *context, int id, int op, char *text_buff )
{
    WORM_MENU_PARAMS* my_params = ( WORM_MENU_PARAMS* )context;

    switch( op ) {

        case PMP_MI_OP_DEF:
            sprintf(text_buff, _(worm_menu_item_text_id[1]), my_params->level);
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_ENTER:
            return PMP_MI_ST_SELECTED;

        case PMP_MI_OP_PREV:
            if( my_params->level > 1 )
                my_params->level--;
            sprintf( text_buff, _(worm_menu_item_text_id[1]), my_params->level );
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_NEXT:
            if( my_params->level < 10 )
                my_params->level++;
            sprintf( text_buff, _(worm_menu_item_text_id[1]), my_params->level );
            return PMP_MI_ST_UPDATED;            
    }
    return PMP_MI_ST_NONE;
}

int _worm_menu_highscore_callback( void *context, int id, int op, char *text_buff )
{
    switch( op ) {
        case PMP_MI_OP_DEF:

            strcpy( text_buff, _(worm_menu_item_text_id[2]) );
            return PMP_MI_ST_UPDATED;

        case PMP_MI_OP_ENTER:
            return PMP_MI_ST_SELECTED;
    }

    return PMP_MI_ST_NONE;
}

static PMP_MENU_ITEM worm_menu[] =
{
    { 1, _worm_menu_start_callback },
    { 2, _worm_menu_setlevel_callback },
    { 3, _worm_menu_highscore_callback },
    { 0, NULL }
};

static int Init_map( void )
{
    int i;
    for (i = 0; i < ARRAY_LEN(worm_pic); i ++)
    {
        if (LoadBitmap (HDC_SCREEN, &bitmap_worm[i], worm_pic[i]))
            fprintf (stderr, "LoadBitmap %s error. \n", worm_pic[i]);
        RegisterResFromBitmap (worm_pic[i], &bitmap_worm[i]);
    }
#if 0
    bitmap_worm_node = (BITMAP *)pmp_image_res_acquire_bitmap( "../../res/bitmap_worm_node.gif" );
    bitmap_worm_node_hide = (BITMAP *)pmp_image_res_acquire_bitmap( "../../res/bitmap_worm_node_hide.gif" );
    bitmap_map_node = (BITMAP *)pmp_image_res_acquire_bitmap( "../../res/bitmap_map_node.gif" );
    bitmap_boni_red = (BITMAP *)pmp_image_res_acquire_bitmap( "../../res/bitmap_boni_red.gif" );
    bitmap_boni_yellow = (BITMAP *)pmp_image_res_acquire_bitmap( "../../res/bitmap_boni_yellow.gif" );
    bitmap_boni_hide = (BITMAP *)pmp_image_res_acquire_bitmap( "../../res/bitmap_worm_node_hide.gif" );*/
#endif
   return 0;
}

static int Unload_map( void )
{
    int i;
    for (i = 0; i < ARRAY_LEN (worm_pic)-4; i ++)
    {
        UnregisterRes (worm_pic[i]);
        //UnloadBitmap (&bitmap_worm[i]);
    }
#if 0
    pmp_image_res_release_bitmap( "../../res/worm/bitmap_worm_node.gif", bitmap_worm_node );
    bitmap_worm_node = NULL;
    pmp_image_res_release_bitmap( "../../res/worm/bitmap_worm_node_hide.gif", bitmap_worm_node_hide );
    bitmap_worm_node_hide = NULL;
    pmp_image_res_release_bitmap( "../../res/worm/bitmap_map_node.gif", bitmap_map_node );
    bitmap_map_node = NULL;
    pmp_image_res_release_bitmap( "../../res/worm/bitmap_boni_red.gif", bitmap_boni_red );
    bitmap_boni_red = NULL;
    pmp_image_res_release_bitmap( "../../res/worm/bitmap_boni_yellow.gif", bitmap_boni_yellow );
    bitmap_boni_yellow = NULL;
    pmp_image_res_release_bitmap( "../../res/worm/bitmap_worm_node_hide.gif", bitmap_boni_hide );
    bitmap_boni_hide = NULL;
#endif
    return 0;
}

static int draw_node( HWND hWnd, int x, int y, int width, int height, BITMAP *Bitmap )			// draw picture
{
    HDC hdc;
    hdc = GetClientDC( hWnd );
    FillBoxWithBitmap( hdc, x, y, width, height, Bitmap );
    ReleaseDC( hdc );
    return 0;
}

static int load_picture( HWND hWnd, int number, worm_list *worm_creat )							// draw snake
{
    int i;
    worm_node *worm_new;
    worm_new = worm_creat-> worm;
    for( i = 0; i < number; i++ ) {
        draw_node( hWnd, worm_new[i].x, worm_new[i].y, 
                NODE_WIDTH, NODE_HIGHT, (BITMAP *)RetrieveRes (worm_pic[0]));
    }
    return 0;
}

static int creat_worm( int number, worm_list *worm_creat )										// create snake.
{
    int i;
    worm_node *worm_new;
    if( worm_creat->worm != NULL )
        free( worm_creat->worm );
    worm_creat->worm = NULL;
    worm_new = ( worm_node * )malloc( sizeof( worm_node ) * number );
    worm_new[0].x = 6 * NODE_WIDTH;
    worm_new[0].y = 6*NODE_HIGHT;

    for(i = 1; i < number; i++){
        worm_new[i].x = worm_new[i - 1].x;
        worm_new[i].y = worm_new[i - 1].y - NODE_HIGHT;
    }

    worm_creat->worm 	= worm_new;
    worm_creat->number	= number;
    return 0;
}

/* get wall number.*/
static int creat_map( char *level[] )
{
    char 	str[41];
    int 	i, j;
    int		count;
    count = 0;
    for( i = 0; i < MAP_ROWS; i++ ) {
        sprintf( str, "%s", level[i] );
        for( j = 0; j < MAP_COLS; j++ )
        {
            if( str[j] == 'p' ) {
                count++;
            }
        } 
    }
    return count;
}

static void save_seed( char *cfgfile, int seed )
{
    FILE	*fp;
    fp = fopen( cfgfile, "w" );
    if( fp == NULL )
    {
        _MY_PRINTF( "Can not open the file of %s to write something\n", cfgfile );
    }
    else
    {
        fprintf( fp, "%s: the configuration file for program worm.\n\n", cfgfile );
        fprintf( fp, "[all]\n" );
        fprintf( fp, "num=%d\n\n", 1 );
        fprintf( fp, "[%d]\n", 0 );
        fprintf( fp, "seed=%d\n", seed );
        fclose( fp );
    }		
}

static int get_seed(void)
{
    char charindex[5];
    int buffer;

    sprintf(charindex, "%d", 0);
    if(GetIntValueFromEtcFile( SEED_CFG, charindex, "seed", &buffer) < 0) {
        _MY_PRINTF( "open etc file error\n" );
        buffer = 599; 
    }
    return buffer;
}

static int move_change( worm_list *worm, int direction, int *x, int *y )
{
    int i, j;
    worm_node *worm_move;
    worm_move = worm->worm;
    i = direction;
    *x = worm_move[worm->number - 1].x;
    *y = worm_move[worm->number - 1].y;
    for( j = worm->number - 1; j > 0; j-- ){
        worm_move[j].x = worm_move[j - 1].x;
        worm_move[j].y = worm_move[j - 1].y;
    }
    switch( i ){
        case UP:
            worm_move[0].x = worm_move[0].x;
            worm_move[0].y = worm_move[0].y - NODE_WIDTH;
            break;
        case RIGHT:
            worm_move[0].x = worm_move[0].x + NODE_WIDTH;
            worm_move[0].y = worm_move[0].y;
            break;
        case DOWN:
            worm_move[0].x = worm_move[0].x;
            worm_move[0].y = worm_move[0].y + NODE_WIDTH;
            break;
        case LEFT:
            worm_move[0].x = worm_move[0].x - NODE_WIDTH;
            worm_move[0].y = worm_move[0].y;
            break;
    }
    return 0;
}

static int right_direction( worm_list *worm )
{
    int i, j;
    worm_node *worm_move;
    worm_move = worm->worm;

    i = worm_move[0].x - worm_move[1].x;
    j = worm_move[0].y - worm_move[1].y;

    if( ( i == NODE_WIDTH ) && ( j == 0 ) ) {
        worm->direction = RIGHT;
        return NOLEFT;
    }

    if( ( i == 0 ) && ( j == NODE_WIDTH ) ) {
        worm->direction = DOWN;
        return NOUP;
    }	

    if( ( i == ( 0 - NODE_WIDTH ) ) && ( j == 0 ) ) {
        worm->direction = LEFT;
        return NORIGHT;
    }	

    if( ( i == 0 ) && ( j == ( 0 - NODE_WIDTH ) ) ) {
        worm->direction = UP;
        return NODOWN;
    }
    return 9;
}

static int move( worm_list *worm )
{
    int i;
    worm_node *worm_move;
    worm_move = worm->worm;

    for( i = worm->number - 1; i > 0; i-- ) {
        worm_move[i].x = worm_move[i - 1].x;
        worm_move[i].y = worm_move[i - 1].y;
    }

    right_direction( worm );

    switch( worm->direction )	{
        case UP:
            worm_move[0].x = worm_move[0].x;
            worm_move[0].y = worm_move[0].y - NODE_WIDTH;
            break;

        case RIGHT:
            worm_move[0].x = worm_move[0].x + NODE_WIDTH;
            worm_move[0].y = worm_move[0].y;
            break;

        case DOWN:
            worm_move[0].x = worm_move[0].x;
            worm_move[0].y = worm_move[0].y + NODE_WIDTH;
            break;

        case LEFT:
            worm_move[0].x = worm_move[0].x - NODE_WIDTH;
            worm_move[0].y = worm_move[0].y;
            break;
    }
    return 0;
}

static int moveback( worm_list *worm, int x, int y )
{
    int i;
    worm_node *worm_move;
    worm_move = worm->worm;

    for( i = 0; i < worm->number - 1; i++ ) {
        worm_move[i].x = worm_move[i + 1].x;
        worm_move[i].y = worm_move[i + 1].y;
    }

    worm_move[worm->number - 1].x = x;
    worm_move[worm->number - 1].y = y;

    return 0;
}

static int tail_direction( worm_list *worm )	
{
    int	j, k;
    int number;
    worm_node 	*worm_move;
    number = worm->number;
    worm_move = worm->worm;

    j = worm_move[number - 2].x - worm_move[number - 1].x;
    k = worm_move[number - 2].y - worm_move[number - 1].y;

    if( ( j == -NODE_WIDTH ) && ( k == 0 ) ) {
        worm->tail_direction = RIGHT;
        return RIGHT;
    }

    if( ( j == 0 ) && ( k == ( 0 - NODE_WIDTH ) ) ) {
        worm->tail_direction = DOWN;
        return DOWN;
    }

    if( ( j == NODE_WIDTH ) && ( k == 0 ) ) {
        worm->tail_direction = LEFT;
        return LEFT;
    }

    if( ( j == 0 ) && ( k == NODE_WIDTH ) ) {
        worm->tail_direction = UP;
        return UP;
    }

    return 9;
}

static int add_node( worm_list *worm , int number )
{
    int new_number;
    int	i;
    int direction;

    worm_node 	*worm_move;
    worm_node   *worm_new_move;
    worm_new_move = ( worm_node *) malloc( sizeof( worm_node )* number );

    worm_move = worm->worm;
    new_number = number;
    direction = tail_direction(worm);
 
    for( i = 0; i < new_number; i++ ){

        if( i < worm->number ) {
            worm_new_move[i].x = worm_move[i].x;
            worm_new_move[i].y = worm_move[i].y;
        } else {
            switch( direction ) {

                case RIGHT:
                    {
                        worm_new_move[i].x = worm_new_move[i - 1].x +  NODE_WIDTH;
                        worm_new_move[i].y = worm_new_move[i - 1].y;
                    }
                    break;

                case DOWN:
                    {
                        worm_new_move[i].x = worm_new_move[i - 1].x;
                        worm_new_move[i].y = worm_new_move[i - 1].y + NODE_WIDTH;
                    }
                    break;

                case LEFT:
                    {
                        worm_new_move[i].x = worm_new_move[i - 1].x - NODE_WIDTH;
                        worm_new_move[i].y = worm_new_move[i - 1].y;
                    }
                    break;

                case UP:
                    {
                        worm_new_move[i].x = worm_new_move[i - 1].x;
                        worm_new_move[i].y = worm_new_move[i - 1].y - NODE_WIDTH;
                    }
                    break;
            }
        }
    }
    if( worm->worm != NULL )
        free( worm->worm );
    worm->worm = NULL;
    worm->worm = worm_new_move;
    worm->number = new_number;

    return 0;
}

static int delet_node( HWND hWnd, worm_list *worm )
{
    int i;
    worm_node 	*worm_move;
    worm_node	*worm_new_move;
    worm_move = worm->worm;

    if( worm->number == begin_NUMBER ) {
        worm_new_move = NULL;
    } else {
        worm_new_move = ( worm_node * ) malloc( sizeof( worm_node ) * ( worm->number - 4 ) );

        for( i =0; i < worm->number - 4; i++ ) {
            worm_new_move[i].x = worm_move[i].x;
            worm_new_move[i].y = worm_move[i].y;
        }
    }
    for( i = worm->number - 4; i < worm->number; i++ ) {
        draw_node( hWnd, worm_move[i].x, worm_move[i].y, NODE_WIDTH,
                NODE_HIGHT, (BITMAP *)RetrieveRes (worm_pic[1]) );
    }
    if( worm->worm != NULL )
        free( worm->worm );
    worm->worm = NULL;
    worm->number = worm->number - 4;
    worm->worm	= worm_new_move;

    return 0;
}

static int redraw_worm( HWND hWnd, worm_list *worm, int x, int y )
{
    worm_node	*worm_move;
    worm_move = worm->worm;

    draw_node( hWnd, x, y, NODE_WIDTH, NODE_HIGHT, (BITMAP *)RetrieveRes (worm_pic[1]));
    draw_node( hWnd, worm_move[0].x, worm_move[0].y, NODE_WIDTH, NODE_HIGHT, (BITMAP *)RetrieveRes (worm_pic[0]));
    return 0;
}

static int creat_bonus_xy( int *x, int *y, worm_list *worm )
{
    double i, j;

    srand( worm->seed );
    worm->seed = worm->seed + 100;
    if( worm->seed > 30000 )
        worm->seed = worm->seed % 100;
#ifndef _240x320_WORM
    i = random();
    j = i/( double )( RAND_MAX );
    *x = j * MAP_COLS * BLOCK_WIDTH;
    i = random();
    j = i/( double )( RAND_MAX );
    *y = j * MAP_ROWS * BLOCK_HIGHT;
#else
    i = random();
    j = i/( double )( RAND_MAX );
    *x = j * MAP_ROWS * BLOCK_WIDTH;
    i = random();
    j = i/( double )( RAND_MAX );
    *y = j * MAP_COLS * BLOCK_HIGHT;
#endif
    *x = *x - *x % NODE_WIDTH;
    *y = *y - *y % NODE_HIGHT;

    return 0;
}

static int boni_wall( int x, int y, int count, worm_list *worm, map_node *array )
{
    int i;
    int node_y;
    int direction;
    int tail_x, tail_y;
    worm_node *worm_move;
    node_y = NODE_HIGHT;
    worm_move = worm->worm;
    tail_x = worm_move[worm->number - 1].x;
    tail_y = worm_move[worm->number - 1].y;
    direction = tail_direction( worm );
    switch( direction ) {
        case RIGHT:
            for( i = 0; i < ADD_NUMBER; i++ ) {
                tail_x = tail_x + NODE_WIDTH;
                if( x == tail_x && y == tail_y )
                    return IN_WALL;
            }
            break;
        case DOWN:
            for( i = 0; i < ADD_NUMBER; i++ ) {
                tail_y = tail_y + NODE_HIGHT;
                if( x == tail_x && y == tail_y )
                    return IN_WALL;
            }
            break;
        case LEFT:
            for( i = 0; i < ADD_NUMBER; i++ ) {
                tail_x = tail_x - NODE_WIDTH;
                if( x == tail_x && y == tail_y )
                    return IN_WALL;
            }
            break;
        case UP:
            for( i = 0; i < ADD_NUMBER; i++ ) {
                tail_y = tail_y - NODE_HIGHT;
                if( x == tail_x && y == tail_y )
                    return IN_WALL;
            }
            break;
    }
    for( i = 0; i < worm->number; i++ )
        if( x == worm_move[i].x && y == worm_move[i].y )
            return IN_WALL;
    for( i = 0; i < begin_NUMBER - 1; i++ ) {
        if( x == 3 * NODE_WIDTH && y == node_y )
            return IN_WALL;
        node_y = node_y + NODE_HIGHT;
    }
    for( i = 0; i < count * 4; i++ )
        if( x + BONI_WIDTH/2 == array[i].x && y + BONI_HIGHT/2 == array[i].y )
            return IN_WALL;

    return UNIN_WALL;
}

static int tail_wall( worm_list *worm, map_node *array, int count_wall, int x, int y, boni_list *bon_live )
{
    int i;
    int x1, y1, x2, y2, x3, y3;
    worm_node *worm_move;
    worm_move = worm->worm;

    x1 = worm_move[worm->number - 1].x + NODE_WIDTH/2;
    y1 = worm_move[worm->number - 1].y + NODE_HIGHT/2;
    x2 = worm_move[worm->number - 2].x + NODE_WIDTH/2;
    y2 = worm_move[worm->number - 2].y + NODE_HIGHT/2;
    x3 = worm_move[worm->number - 3].x + NODE_WIDTH/2;
    y3 = worm_move[worm->number - 3].y + NODE_HIGHT/2;
    x = x + NODE_WIDTH / 2;
    y = y + NODE_HIGHT / 2;

    for( i = 0; i < count_wall * 4; i++ ) {
        if( ( x1 == array[i].x && y1 == array[i].y )
                || ( x2 == array[i].x && y2 == array[i].y )
                || ( x3 == array[i].x && y3 == array[i].y )
                || ( x == array[i].x && y == array[i].y ) )
            return IN_WALL;
    }

    x1 = worm_move[worm->number - 1].x;
    y1 = worm_move[worm->number - 1].y;
    x2 = worm_move[worm->number - 2].x;
    y2 = worm_move[worm->number - 2].y;
    x3 = worm_move[worm->number - 3].x;
    y3 = worm_move[worm->number - 3].y;
    x = x - NODE_WIDTH/2;
    y = y - NODE_HIGHT/2;

    for( i = 0; i < bon_live->boni_num; i++ ) {
        if( ( x1 == bon_live->bonus[i].x && y1 == bon_live->bonus[i].y )
                || ( x2 == bon_live->bonus[i].x && y2 == bon_live->bonus[i].y )
                || (x3 == bon_live->bonus[i].x && y3 == bon_live->bonus[i].y)
                || (x == bon_live->bonus[i].x && y == bon_live->bonus[i].y ) )
            return IN_WALL;
    }

    for( i = 0; i < worm->number - 3; i++ ) {
        if( ( x1 == worm_move[i].x && y1 == worm_move[i].y )
                || ( x2 == worm_move[i].x && y2 == worm_move[i].y )
                || ( x3 == worm_move[i].x && y3 == worm_move[i].y )
                || ( x == worm_move[i].x && y == worm_move[i].y ) )
            return IN_WALL;
    }
    return UNIN_WALL;
}

static int num_inwall( worm_list *worm, map_node *array, int count_wall, boni_list *bon_live )
{
    int j;
    worm_node *worm_move;
    worm_move = worm->worm;
    for( j = 0; j < count_wall * 4; j++ ) {
        if( worm_move[worm->number - 1 - 0].x + NODE_WIDTH/2 == array[j].x &&
                worm_move[worm->number - 1 - 0].y + NODE_HIGHT/2 == array[j].y )
            return 1;
        if( worm_move[worm->number - 1 - 1].x + NODE_WIDTH/2 == array[j].x &&
                worm_move[worm->number - 1 - 1].y + NODE_HIGHT/2 == array[j].y )
            return 2;
        if( worm_move[worm->number - 1 - 2].x + NODE_WIDTH/2 == array[j].x &&
                worm_move[worm->number - 1 - 2].y + NODE_HIGHT/2 == array[j].y )
            return 3;
        if( worm_move[worm->number - 1 - 3].x + NODE_WIDTH/2 == array[j].x &&
                worm_move[worm->number - 1 - 3].y + NODE_HIGHT/2 == array[j].y )
            return 4;
    }
    for( j = 0; j < bon_live->boni_num; j++ ) {
        if( worm_move[worm->number - 1 - 0].x  == bon_live->bonus[j].x &&
                worm_move[worm->number - 1 - 0].y == bon_live->bonus[j].y )
            return 1;
        if( worm_move[worm->number - 1 - 1].x == bon_live->bonus[j].x &&
                worm_move[worm->number - 1 - 1].y == bon_live->bonus[j].y )
            return 2;
        if( worm_move[worm->number - 1 - 2].x == bon_live->bonus[j].x &&
                worm_move[worm->number - 1 - 2].y == bon_live->bonus[j].y )
            return 3;
        if( worm_move[worm->number - 1 - 3].x  == bon_live->bonus[j].x &&
                worm_move[worm->number - 1 - 3].y == bon_live->bonus[j].y )
            return 4;
    }

    for( j = 0; j < worm->number - 3; j++ ) {
        if( worm_move[worm->number - 1 - 0].x == worm_move[j].x &&
                worm_move[worm->number - 1 - 0].y == worm_move[j].y )
            return 1;
        if( worm_move[worm->number - 1 - 1].x == worm_move[j].x &&
                worm_move[worm->number - 1 - 1].y == worm_move[j].y )
            return 2;
        if( worm_move[worm->number - 1 - 2].x == worm_move[j].x &&
                worm_move[worm->number - 1 - 2].y == worm_move[j].y )
            return 3;
    }
    return 0;
}

static int creat_bonus( int count, worm_list *worm, boni_list *bon,
        map_node *array )
{
    int x,y;
    boni *bonus;
    x = 0;
    y = 0;

    if( bon->bonus != NULL )
        free( bon->bonus );
    bon->bonus = NULL;
    if( bon->number % 3 != 0 ) {
        bonus = ( boni * )malloc( sizeof( boni ) * 1 );
        creat_bonus_xy( &x, &y, worm );
        while( boni_wall( x, y, count, worm, array ) == IN_WALL ) {
            creat_bonus_xy( &x, &y, worm );
        }
        bonus[0].color = RED;
        bonus[0].x = x;
        bonus[0].y = y;
        bon->bonus = bonus;
        bon->boni_num = 1;
    } else {
        bonus = ( boni * )malloc( sizeof( boni ) * 2 );
        creat_bonus_xy( &x, &y, worm );
        while( boni_wall( x, y, count, worm, array ) == IN_WALL ) {
            creat_bonus_xy( &x, &y, worm );
        }
        bonus[0].color = YELLOW;
        bonus[0].x = x;
        bonus[0].y = y;
        creat_bonus_xy( &x, &y, worm );
        while( boni_wall( x, y, count, worm, array ) == IN_WALL ) {
            creat_bonus_xy( &x, &y, worm );
        }
        bonus[1].color = RED;
        bonus[1].x = x;
        bonus[1].y = y;
        bon->bonus = bonus;
        bon->boni_num = 2;
    }
    return 0;
}

static int Init_bonlive( boni_list *bon, boni_list *bon_live )
{
    int i;
    boni *bonus;
    if( bon_live->bonus != NULL )
        free( bon_live->bonus );
    bon_live->bonus = NULL;
    bonus = ( boni * )malloc( sizeof( boni ) * bon->boni_num );
    for( i = 0; i < bon->boni_num; i++ ) {
        bonus[i].x = bon->bonus[i].x;
        bonus[i].y = bon->bonus[i].y;
        bonus[i].color = bon->bonus[i].color;
    }
    bon_live->bonus = bonus;
    bon_live->boni_num = bon->boni_num;
    return 0;
}

static int delete_worm_node( HWND hWnd, worm_list *worm )
{
    int i;
    worm_node	*worm_move;
    worm_move = worm->worm;

    draw_node( hWnd, worm_move[0].x, worm_move[0].y,
            NODE_WIDTH, NODE_HIGHT, (BITMAP *)RetrieveRes (worm_pic[1]));

    for( i = 0; i < worm->number - 1; i++ ) {
        worm_move[i].x = worm_move[i + 1].x;
        worm_move[i].y = worm_move[i + 1].y;
    }
    return 0;
}

static int creat_map_array( int count, char *level[], map_node *array,
        map_node *array_end )
{
    char 	str[41];
    int 	i, j;
    int 	countnumber = 0;
    for( i = 0; i < MAP_ROWS; i++ )
    {
        sprintf( str, "%s", level[i] );
        for( j = 0; j < MAP_COLS ; j ++ )
        { 
            if( str[j] == 'p' ) {
#ifndef _240x320_WORM                
                array[countnumber].x = BLOCK_WIDTH * j;
                array[countnumber].y = BLOCK_HIGHT * i;
#else
                array[countnumber].x = BLOCK_WIDTH * (MAP_ROWS - 1-i);
                array[countnumber].y = BLOCK_HIGHT * j;
#endif
                countnumber++;
            }
        }
    }

    for( i = 0; i < count; i++ ) {
        array_end[4*i + 0].x = array[i].x + 1*BLOCK_WIDTH/4;
        array_end[4*i + 0].y = array[i].y + 1*BLOCK_HIGHT/4;
        array_end[4*i + 1].x = array[i].x + 3*BLOCK_WIDTH/4;
        array_end[4*i + 1].y = array[i].y + 1*BLOCK_HIGHT/4;
        array_end[4*i + 2].x = array[i].x + 1*BLOCK_WIDTH/4;
        array_end[4*i + 2].y = array[i].y + 3*BLOCK_HIGHT/4;
        array_end[4*i + 3].x = array[i].x + 3*BLOCK_WIDTH/4;
        array_end[4*i + 3].y = array[i].y + 3*BLOCK_HIGHT/4;
    }

    return 0;
}

static int meet_wall( int count, worm_list *worm, map_node *array_end )
{
    int i;
    int x, y;
    worm_node *worm_move;
    worm_move = worm->worm;
    right_direction( worm );
    x = worm_move[0].x + NODE_WIDTH / 2;
    y = worm_move[0].y + NODE_HIGHT / 2;
    for( i = 0; i < count * 4; i++ )
        if( y == array_end[i].y && x == array_end[i].x )
            return MEET;
    return UNMEET;
}

static int meet_boni( worm_list *worm, boni_list *bonus )
{
    int i;
    int j;
    BOOL MEETED;
    boni *bonu;
    boni *boni_new;
    worm_node *worm_move;
    worm_move = worm->worm;
    bonu = bonus->bonus;
    j = 0;
    MEETED = FALSE;
    boni_new = NULL;

    for( i = 0; i < bonus->boni_num; i++ )
        if( worm_move[0].x == bonu[i].x && worm_move[0].y == bonu[i].y )
            MEETED = TRUE;

    if( MEETED == TRUE ) {

        boni_new = ( boni *) malloc( sizeof( boni ) * ( bonus->boni_num - 1 ) );

        for( i = 0; i < bonus->boni_num; i++ ) {
            if( bonu[i].x != worm_move[0].x || bonu[i].y != worm_move[0].y ) {
                boni_new[j].x = bonu[i].x;
                boni_new[j].y = bonu[i].y;
                boni_new[j].color = bonu[i].color;
                j++;
            }
        }

        for( i = 0; i < bonus->boni_num; i++ )
            if( ( worm_move[0].x == bonu[i].x ) && ( worm_move[0].y == bonu[i].y ) ) {
                if( bonu[i].color == RED ) {
                    if( bonus->bonus != NULL )
                        free( bonus->bonus );
                    bonus->bonus = NULL;
                    bonus->bonus = boni_new;
                    bonus->boni_num = bonus->boni_num - 1;
                    return MEET_BONI_RED;
                } else {
                    if( bonus->bonus != NULL )
                        free( bonus->bonus );
                    bonus->bonus = NULL;
                    bonus->bonus = boni_new;
                    bonus->boni_num = bonus->boni_num - 1;
                    return MEET_BONI_YELLOW;
                }
            }
    }
    return 9;
}

static int add_boni( HWND hWnd, boni_list *bon, boni_list *bon_live )
{
    int i, number;
    boni *bonu;
    bonu = NULL;
    number = bon->boni_num + bon_live->boni_num;
    bonu = ( boni * ) malloc( sizeof( boni ) * ( number ) );
    if( number % 3 == 0 && number > 1 ) {
        draw_node( hWnd, bon_live->bonus[bon_live->boni_num - 1].x,
                bon_live->bonus[bon_live->boni_num - 1].y, NODE_WIDTH, NODE_HIGHT,
                (BITMAP *)RetrieveRes (worm_pic[5]));
        number = number - 1;
    }
    for( i = 0; i < number; i++ ) {
        if( i < bon->boni_num ) {
            bonu[i].x = bon->bonus[i].x;
            bonu[i].y = bon->bonus[i].y;
            bonu[i].color = bon->bonus[i].color;
        } else {
            bonu[i].x = bon_live->bonus[i - bon->boni_num].x;
            bonu[i].y = bon_live->bonus[i - bon->boni_num].y;
            bonu[i].color = bon_live->bonus[i - bon->boni_num].color;
        }
    }
    if( bon_live->bonus != NULL )
        free( bon_live->bonus );
    bon_live->bonus = NULL;
    bon_live->bonus = bonu;
    bon_live->boni_num = number;
    return 0;
}

static int meet_self( worm_list *worm )
{
    int i;
    int x, y;
    worm_node *worm_move;
    worm_move = worm->worm;

    x = worm_move[0].x;
    y = worm_move[0].y;
    for( i = 1; i < worm->number; i++ )
        if( x == worm_move[i].x && y == worm_move[i].y ) {
            return EAT_SELF;
        }
    return 9;
}

static int handle_move( HWND hWnd, worm_list *worm, int count_wall,
        boni_list *bon, boni_list *bon_live, map_node *array_wall_first,
        map_node *array_wall, int killtimes, int direction )
{
    int x, y;
    int count;
    count = 0;

    move_change( worm, direction, &x, &y );
    right_direction( worm );

    if( ( meet_wall( count_wall, worm, array_wall ) == MEET ) 
            || ( meet_self( worm ) == EAT_SELF ) ) {

        HDC hdc;
        moveback( worm, x, y );
        KillTimer( hWnd, TIMER_CLOCK1 );
        sleep( 1 );
        SetTimer( hWnd, TIMER_CLOCK1, FREQ_CLOCK );
        while( count < worm->number ) {
            delete_worm_node( hWnd, worm );
            count ++;
        }
        killtimes ++;
        if( killtimes <= 2 )
        {
            if( worm->worm != NULL )
                free( worm->worm );
            worm->worm = NULL;
            creat_worm( begin_NUMBER, worm );
            right_direction( worm );
            hdc = GetClientDC( hWnd );
            draw_worm( hWnd, hdc, worm, (BITMAP *)RetrieveRes (worm_pic[0]));
            ReleaseDC( hdc );
        }
    }
    else {
        HDC hdc;
        redraw_worm( hWnd, worm, x, y );
        if( tail_wall( worm, array_wall, count_wall, x, y, bon_live ) == IN_WALL ) {
            hdc = GetClientDC( hWnd );
            draw_boni( hWnd, hdc, bon_live, (BITMAP *)RetrieveRes (worm_pic[3]), (BITMAP *)RetrieveRes (worm_pic[4]));
            draw_wall( hWnd, hdc, array_wall_first, count_wall, (BITMAP *)RetrieveRes (worm_pic[2]) );
            ReleaseDC( hdc );
        }
    }
    switch( meet_boni( worm, bon_live ) )
    {
        case MEET_BONI_RED :
            {
                HDC hdc;
                int num_wall;
                add_node( worm, ADD_NUMBER + worm->number );
                num_wall = num_inwall( worm, array_wall, count_wall, bon_live );
                load_picture( hWnd, worm->number - num_wall, worm );
                if( tail_wall( worm, array_wall, count_wall, x, y, bon_live ) == IN_WALL ) {
                    hdc = GetClientDC( hWnd );
                    draw_boni( hWnd, hdc, bon_live, (BITMAP *)RetrieveRes (worm_pic[3]), (BITMAP *)RetrieveRes (worm_pic[4]));
                    draw_wall( hWnd, hdc, array_wall_first, count_wall, (BITMAP *)RetrieveRes (worm_pic[2]));
                    if( num_wall == 0 )
                        draw_worm( hWnd, hdc, worm, (BITMAP *)RetrieveRes (worm_pic[0]));
                    ReleaseDC( hdc );
                }
                bon->number = bon->number + 1;
                if( worm->seed > 30000 )
                    worm->seed = worm->seed % 100;
                creat_bonus( count_wall, worm, bon, array_wall );
                worm->seed = worm->seed + 100;
                add_boni( hWnd, bon, bon_live );
                hdc = GetClientDC( hWnd );
                draw_boni( hWnd, hdc, bon, (BITMAP *)RetrieveRes (worm_pic[3]), (BITMAP *)RetrieveRes (worm_pic[4]));
                ReleaseDC( hdc );
                score_number = score_number + worm->number * 10;
#if 0
                InvalidateRect( hWnd,&rect_score,FALSE );
#else
                ClientToScreen (hWnd, &rect_score.left,  &rect_score.top);
                ClientToScreen (hWnd, &rect_score.right, &rect_score.bottom);
                SendAsyncMessage (hWnd, MSG_ERASEBKGND, 0,
                        (LPARAM)&rect_score);
#endif

            }
            break;
        case MEET_BONI_YELLOW:
            {
                HDC hdc;
                if( worm->number == begin_NUMBER ) {
                    KillTimer( hWnd, TIMER_CLOCK1 );
                    sleep( 1 );
                    SetTimer( hWnd, TIMER_CLOCK1, FREQ_CLOCK );
                    delet_node( hWnd, worm );
                } else {
                    delet_node( hWnd, worm );
                }
                if( worm->number == 0 ) {
                    killtimes++;
                    if( worm->worm != NULL )
                        free( worm->worm );
                    worm->worm = NULL;
                    creat_worm( begin_NUMBER, worm );
                    right_direction( worm );
                    hdc = GetClientDC( hWnd );
                    draw_worm( hWnd, hdc, worm, (BITMAP *)RetrieveRes (worm_pic[0]));
                    ReleaseDC( hdc );
                }
                bon->number = bon->number + 1;
                if( worm->seed > 30000 )
                    worm->seed = worm->seed % 100;
                creat_bonus( count_wall, worm, bon, array_wall );
                worm->seed = worm->seed + 100;
                add_boni( hWnd, bon, bon_live );
                hdc = GetClientDC( hWnd );
                draw_boni( hWnd, hdc, bon, (BITMAP *)RetrieveRes (worm_pic[3]), (BITMAP *)RetrieveRes (worm_pic[4]));
                ReleaseDC( hdc );
                score_number = score_number + worm->number * 10;
#if 0
                InvalidateRect( hWnd,&rect_score,FALSE );
#else
                ClientToScreen (hWnd, &rect_score.left,  &rect_score.top);
                ClientToScreen (hWnd, &rect_score.right, &rect_score.bottom);
                SendAsyncMessage (hWnd, MSG_ERASEBKGND, 0,
                        (LPARAM)&rect_score);
#endif

            }
            break;
        default:
            break;
    }
    return killtimes;
}

static int draw_wall( HWND hWnd, HDC hdc, map_node *array_wall, int count, BITMAP *Bitmap )
{
    int i;
    for( i = 0; i < count; i++ )
        FillBoxWithBitmap( hdc, array_wall[i].x, array_wall[i].y,
                BLOCK_WIDTH, BLOCK_HIGHT, Bitmap );
    return 0;
}

static int draw_worm( HWND hWnd, HDC hdc, worm_list *worm, BITMAP *Bitmap )
{
    int i;
    worm_node *worm_move;
    worm_move = worm->worm;
    for( i = 0; i < worm->number; i++ )
        FillBoxWithBitmap( hdc, worm_move[i].x, worm_move[i].y, NODE_WIDTH, NODE_HIGHT, Bitmap );
    return 0;
}

static int draw_boni( HWND hWnd, HDC hdc, boni_list  *bon, BITMAP *Bitmap_red, BITMAP *Bitmap_yellow )
{
    int i;
    boni *bonus;
    bonus = bon->bonus;
    for( i = 0; i < bon->boni_num; i++ )
        if( bonus[i].color == RED ) {
            FillBoxWithBitmap( hdc, bonus[i].x, bonus[i].y, BONI_WIDTH,
                    BONI_HIGHT, Bitmap_red );
        } else {
            FillBoxWithBitmap( hdc, bonus[i].x, bonus[i].y, BONI_WIDTH,
                    BONI_HIGHT, Bitmap_yellow );
        }

    return 0;
}

static void UpdateHighScore( int score_number )
{
    int i,j;
    for( i=0; i<5; i++ )
        if( score_number>worm.score[i] )
            break;
    if( i == 5 ) {
        return;
    }
    for( j = 4; j > i; j-- ){
        worm.score[j] = worm.score[j-1];
    }
    worm.score[i] =score_number;
}

static void new_game( HWND hWnd )
{
    if( restart == TRUE )
        restart = FALSE;
    new = FALSE;
    end = FALSE;
    score_number = 0;
    if( worm.worm != NULL )
        free( worm.worm );
    worm.worm = NULL;
    if( bon.bonus != NULL )
        free( bon.bonus );
    bon.bonus = NULL;
    if( bon_live.bonus != NULL )
        free( bon_live.bonus );
    bon_live.bonus = NULL;
    bon.boni_num = 0;
    bon_live.boni_num = 0;
    worm.number = 0;
    /* Setting the level*/
    switch( worm.level ) {
        case 1:
            count_wall = creat_map( level1 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node *) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level1, array_wall_first,
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1;
            break;
        case 2:
            count_wall = creat_map( level2 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first =( map_node * ) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level2, array_wall_first, 
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1 - 4;
            break;
        case 3:
            count_wall = creat_map( level3 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level3, array_wall_first,
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1 - 8;
            break;
        case 4:
            count_wall = creat_map( level4 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level4, array_wall_first,
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1 - 12;
            break;
        case 5:
            count_wall = creat_map( level5 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level5, array_wall_first,
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1 - 15;
            break;
        case 6:
            count_wall = creat_map( level6 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level6, array_wall_first,
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1 - 18;
            break;
        case 7:
            count_wall = creat_map( level7 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first = ( map_node *) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node * ) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level7, array_wall_first, 
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1 - 21;
            break;
        case 8:
            count_wall = creat_map( level8 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first = ( map_node *) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node *) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level8, array_wall_first,
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1 - 22;
            break;
        case 9:
            count_wall = creat_map( level9 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first = ( map_node *) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node *) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level9, array_wall_first,
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1 - 23;
            break;
        case 10:
            count_wall = creat_map( level10 );
            if( array_wall_first != NULL )
                free( array_wall_first );
            array_wall_first = NULL;
            array_wall_first =( map_node *) malloc( ( sizeof( map_node ) * count_wall ) );
            if( array_wall != NULL )
                free( array_wall );
            array_wall = NULL;
            array_wall = ( map_node *) malloc( ( sizeof( map_node ) * count_wall * 4 ) );
            creat_map_array( count_wall, level10, array_wall_first,
                    array_wall );
            FREQ_CLOCK = FREQ_CLOCK1 - 24;
            break;
        default:
            break;
    }
    SetTimer( hWnd, TIMER_CLOCK1, FREQ_CLOCK );
    creat_worm( begin_NUMBER, &worm );
    right_direction( &worm );
    if( worm.seed > 30000 )
        worm.seed = worm.seed % 100;
    creat_bonus( count_wall, &worm, &bon, array_wall );
    worm.seed = worm.seed + 100;
    Init_bonlive( &bon, &bon_live );
    InvalidateRect( hWnd, NULL, TRUE );
    killtimes = 0;
}

static LRESULT WormGameProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    char strText[50];    
    HDC hdc;
    char st[7][200];
    char str[100];
    FILE *stream;
    switch(message) 
    {
        case MSG_CREATE:
            {    
                play = TRUE;    
                begin = TRUE;
                worm.worm = NULL;
                bon.bonus = NULL;
                array_wall = NULL;
                array_wall_first = NULL;
                bon.number = 0;
                bon.boni_num = 0;
                bon_live.bonus = NULL;
                bon_live.number = 0;
                bon_live.boni_num = 0;
                worm.level = 1;
                worm_menu_params.level = 1;
                Init_map();
                worm.seed = get_seed();
                count_wall = creat_map( level1 );
                if( array_wall_first != NULL )
                    free( array_wall_first );
                array_wall_first = NULL;
                array_wall_first = ( map_node *) malloc( ( sizeof( map_node )*count_wall ) );
                if( array_wall != NULL )
                    free( array_wall );
                array_wall = NULL;
                array_wall = ( map_node *) malloc( ( sizeof( map_node )*count_wall*4 ) );
                creat_map_array( count_wall, level1, array_wall_first, 
                        array_wall );
                creat_worm( begin_NUMBER, &worm );
                right_direction( &worm );
                if( worm.seed > 30000 )
                    worm.seed = worm.seed % 100;
                creat_bonus( count_wall, &worm, &bon, array_wall );
                worm.seed = worm.seed + 100;
                Init_bonlive( &bon, &bon_live );

                stream = fopen( WORM_SCOREPATH, "r+" );
                if( stream == NULL )
                {
                    _MY_PRINTF( "Can not open the file of %s to read something", WORM_SCOREPATH );
                    worm.score[0] = 0;
                    worm.score[1] = 0;
                    worm.score[2] = 0;
                    worm.score[3] = 0;
                    worm.score[4] = 0;
                }
                else
                {
                    if (fgets( st[0], 100, stream ))
                        worm.score[0] = strtol( st[0], NULL, 10 );
                    if (fgets( st[1], 100, stream ))
                        worm.score[1] = strtol( st[1], NULL, 10 );
                    if (fgets( st[2], 100, stream ))
                        worm.score[2] = strtol( st[2], NULL, 10 );
                    if (fgets( st[3], 100, stream ))
                        worm.score[3] = strtol( st[3], NULL, 10 );
                    if (fgets( st[4], 100, stream ))
                        worm.score[4] = strtol( st[4], NULL, 10 );
                    fclose( stream );
                }
            }
            break;
        case MSG_PAINT:
            hdc = BeginPaint( hWnd );
            SetBrushColor( hdc, PIXEL_black );
            FillBox( hdc, 0, 0, BLOCK_WIDTH * MAP_COLS, BLOCK_HIGHT * MAP_ROWS );
            draw_wall( hWnd, hdc, array_wall_first, count_wall, (BITMAP *)RetrieveRes (worm_pic[2]));
            draw_worm( hWnd, hdc, &worm, (BITMAP *)RetrieveRes (worm_pic[0]));
            draw_boni( hWnd, hdc, &bon_live, (BITMAP *)RetrieveRes (worm_pic[3]), (BITMAP *)RetrieveRes (worm_pic[4]));
            if( play == TRUE )
            {
                SetBkColor( hdc, COLOR_black );
                SetTextColor( hdc, COLOR_green );
                SetPenColor( hdc, COLOR_green );
#ifndef _240x320_WORM
                snprintf( strText, 50, _(IDS_PMP_WORM_SCORE), score_number );
                TextOut( hdc, rect_score.left, rect_score.top, strText );
                snprintf( strText, 50, _(IDS_PMP_WORM_LEVEL), worm.level );
                TextOut( hdc, rect_score.left+110, rect_score.top, strText );
#else
                snprintf( strText, 50, _(IDS_PMP_WORM_SCORE), score_number );
                DrawText (hdc, strText, -1, &rect_score1, DT_LEFT);
                snprintf( strText, 50, _(IDS_PMP_WORM_LEVEL), worm.level );
                DrawText (hdc, strText, -1, &rect_score2, DT_LEFT);
#endif
            }

            EndPaint( hWnd, hdc );
            //ReleaseDC( hdc );
            break;

        case MSG_TIMER:
            {
                int x, y;
                int count;
                worm_node *worm_move;
                worm_move = worm.worm;
                count = 0;

                x = worm_move[worm.number -1].x;
                y = worm_move[worm.number -1].y;
                move( &worm );

                if( meet_wall( count_wall, &worm, array_wall ) == MEET ||
                        meet_self( &worm ) == EAT_SELF )
                {
                    score_number = score_number + worm.number * 10;
#if 0
                    InvalidateRect( hWnd,&rect_score,FALSE );
#else
                    ClientToScreen (hWnd, &rect_score.left,  &rect_score.top);
                    ClientToScreen (hWnd, &rect_score.right, &rect_score.bottom);
                    SendAsyncMessage (hWnd, MSG_ERASEBKGND, 0,
                            (LPARAM)&rect_score);
#endif
                    moveback( &worm, x, y );
                    KillTimer( hWnd, TIMER_CLOCK1 );
                    sleep( 1 );
                    SetTimer( hWnd, TIMER_CLOCK1, FREQ_CLOCK );
                    while( count < worm.number )
                    {
                        delete_worm_node( hWnd, &worm );
                        count++;
                    }
                    killtimes++;
                    if( worm.worm != NULL )
                        free( worm.worm );
                    worm.worm = NULL;
                    creat_worm( begin_NUMBER, &worm );
                    right_direction( &worm );
                    hdc = GetClientDC( hWnd );
                    draw_worm( hWnd, hdc, &worm, (BITMAP *)RetrieveRes (worm_pic[0]));
                    ReleaseDC( hdc );
                    break;
                }
                else
                {
                    redraw_worm( hWnd, &worm, x, y );
                    if( tail_wall( &worm, array_wall, count_wall, x, y, &bon_live ) == IN_WALL )
                    {
                        hdc = GetClientDC( hWnd );
                        draw_boni ( hWnd, hdc, &bon_live, (BITMAP *)RetrieveRes (worm_pic[3]),
                                (BITMAP *)RetrieveRes (worm_pic[4]));
                        draw_wall ( hWnd, hdc, array_wall_first, count_wall, 
                                (BITMAP *)RetrieveRes (worm_pic[0]));
                        ReleaseDC ( hdc);
                    }
                }

                switch( meet_boni( &worm, &bon_live ) )
                {
                    case MEET_BONI_RED:	
                        {
                            int num_wall;
                            add_node( &worm, worm.number + ADD_NUMBER );
                            num_wall = num_inwall( &worm, array_wall, count_wall, &bon_live );
                            load_picture( hWnd, worm.number - num_wall, &worm );
                            if(tail_wall( &worm, array_wall, count_wall, x, y, &bon_live ) == IN_WALL )
                            {
                                hdc = GetClientDC( hWnd );
                                draw_boni( hWnd, hdc, &bon_live, (BITMAP *)RetrieveRes (worm_pic[3]), (BITMAP *)RetrieveRes (worm_pic[4]));
                                draw_wall( hWnd, hdc, array_wall_first, count_wall, (BITMAP *)RetrieveRes (worm_pic[2]));
                                if( num_wall == 0 )
                                    draw_worm( hWnd, hdc, &worm, (BITMAP *)RetrieveRes (worm_pic[0]));
                                ReleaseDC( hdc );
                            }
                            bon.number = bon.number + 1;
                            if( worm.seed > 30000 )
                                worm.seed = worm.seed % 100;
                            creat_bonus( count_wall, &worm, &bon, array_wall );
                            worm.seed = worm.seed + 100;
                            add_boni( hWnd, &bon, &bon_live );
                            hdc = GetClientDC( hWnd );
                            draw_boni( hWnd, hdc, &bon, (BITMAP *)RetrieveRes (worm_pic[3]), (BITMAP *)RetrieveRes (worm_pic[4]));
                            ReleaseDC( hdc );
                        }
                        score_number = score_number + worm.number * 10;
#if 0
                        InvalidateRect( hWnd,&rect_score,FALSE );
#else
                        ClientToScreen (hWnd, &rect_score.left,  &rect_score.top);
                        ClientToScreen (hWnd, &rect_score.right, &rect_score.bottom);
                        SendAsyncMessage (hWnd, MSG_ERASEBKGND, 0,
                                (LPARAM)&rect_score);
#endif

                        break;
                    case MEET_BONI_YELLOW:
                        {
                            delet_node( hWnd, &worm );
                            if( worm.number == 0 )
                            {
                                killtimes++;
                                if( worm.worm != NULL )
                                    free( worm.worm );
                                worm.worm = NULL;
                                creat_worm( begin_NUMBER, &worm );
                                right_direction( &worm );
                                hdc = GetClientDC( hWnd );
                                draw_worm( hWnd, hdc, &worm, (BITMAP *)RetrieveRes (worm_pic[0]));
                                ReleaseDC( hdc );
                            }	

                            bon.number = bon.number + 1;
                            if( worm.seed > 30000 )
                                worm.seed = worm.seed % 100;
                            creat_bonus( count_wall, &worm, &bon, array_wall );
                            worm.seed = worm.seed + 100;
                            add_boni( hWnd, &bon, &bon_live );
                            hdc = GetClientDC( hWnd );
                            draw_boni( hWnd, hdc, &bon, (BITMAP *)RetrieveRes (worm_pic[3]), (BITMAP *)RetrieveRes (worm_pic[4]));
                            ReleaseDC( hdc );
                        }
                        score_number = score_number + worm.number * 10;
#if 0
                        InvalidateRect( hWnd,&rect_score,FALSE );
#else
                        ClientToScreen (hWnd, &rect_score.left,  &rect_score.top);
                        ClientToScreen (hWnd, &rect_score.right, &rect_score.bottom);
                        SendAsyncMessage (hWnd, MSG_ERASEBKGND, 0,
                                (LPARAM)&rect_score);
#endif

                        break;
                    default :
                        break;
                }

                if( killtimes == 3 )
                {
                    restart = TRUE;
                    end = TRUE;
                    KillTimer( hWnd, TIMER_CLOCK1 );
                    killtimes = 0;
                    UpdateHighScore( score_number );
                    score_number = 0;
                    //hdc = BeginPaint( hWnd );
                    hdc = GetClientDC( hWnd );
                    SetBkColor( hdc, COLOR_black );
                    SetTextColor( hdc, COLOR_green );
                    SetPenColor( hdc, COLOR_green );
                    snprintf( strText, 50, _(IDS_PMP_WORM_RESTART_GAME) );
                    TextOut( hdc, INFO_X, INFO_Y, strText );
                    //EndPaint( hWnd, hdc );
                    ReleaseDC( hdc );
                }
            }
            break;
        case MSG_KEYDOWN:
            switch( wParam )
            {
                case SCANCODE_F1:
                    {
                        KillTimer( hWnd, TIMER_CLOCK1 );
                        switch( pmp_show_menu( hWnd, _(IDS_PMP_WORM_TITLE), worm_menu, &worm_menu_params ) )
                        {
                            case 0:
                                if( play == FALSE || restart == TRUE )
                                    ;
                                else
                                    SetTimer( hWnd, TIMER_CLOCK1, FREQ_CLOCK );
                                break;
                            case 1:
                                play = TRUE;
                                new_game( hWnd );
                                break;
                            case 2:
                                worm.level=worm_menu_params.level;
                                new_game( hWnd );
                                break;
                            case 3:
                                sprintf( str, _(IDS_PMP_WORM_SHOW_SCORE),
                                        worm.score[0], worm.score[1], worm.score[2],
                                        worm.score[3], worm.score[4] );
                                pmp_show_info( hWnd, _(IDS_PMP_WORM_SCORE_INFO), str,
                                        MB_ICONINFORMATION);
                                if( play == FALSE || restart == TRUE )
                                    ;
                                else
                                    SetTimer( hWnd, TIMER_CLOCK1, FREQ_CLOCK );
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case SCANCODE_ENTER:
                    if( begin == TRUE && restart == FALSE && killtimes <= 2 && play == TRUE )
                    {
                        begin = FALSE;
                        new_game( hWnd );
                    }
                    else if( restart == FALSE && killtimes <= 2 && play == TRUE )
                    {
                        play = FALSE;
                        KillTimer( hWnd, TIMER_CLOCK1 );
                    }
                    else if(play == FALSE && restart == FALSE && killtimes <= 2)
                    {
                        play = TRUE;
                        SetTimer( hWnd, TIMER_CLOCK1, FREQ_CLOCK );
                    }
                    else 
                    {
                        play = TRUE;
                        new_game( hWnd );
                    }
                    break;
                case SCANCODE_ESCAPE:
                    UpdateHighScore( score_number );
                    restart = FALSE;
                    new = TRUE;
                    play = FALSE;
                    begin = FALSE;
                    end = FALSE;
                    score_number = 0;
                    stream = fopen( WORM_SCOREPATH, "w+" );
                    if( stream == NULL )
                    {
                        _MY_PRINTF( "Can not open the file of %s to write something", WORM_SCOREPATH );
                        play = FALSE;
                        KillTimer( hWnd, TIMER_CLOCK1 );
                        //pmp_show_info( hWnd, _(IDS_PMP_WORM_TEXT),
                          //      _(IDS_PMP_WORM_WRFILE_SHOW), MB_ICONINFORMATION );
                    }
                    else{
                        sprintf( str, "%d\n",  worm.score[0] );
                        fputs( str, stream );
                        sprintf( str, "%d\n", worm.score[1] );
                        fputs( str, stream );
                        sprintf( str, "%d\n", worm.score[2] );
                        fputs( str, stream);
                        sprintf( str, "%d\n", worm.score[3] );
                        fputs( str, stream );
                        sprintf( str, "%d\n", worm.score[4] );
                        fputs( str, stream );
                        fclose(stream);
                    }
                    DestroyMainWindow ( hWnd );
                    MainWindowCleanup ( hWnd );
                    return 0;
                case SCANCODE_CURSORBLOCKUP:
                    if( play == FALSE || end == TRUE )
                        break;

                    if( play == TRUE && end == FALSE && new == TRUE && begin == TRUE && restart == FALSE )
                        break;

                    if( right_direction( &worm ) != NOUP ) 
                    {
                        killtimes = handle_move( hWnd, &worm, count_wall, &bon, &bon_live,
                                array_wall_first, array_wall, killtimes, UP);
                        if( killtimes == 3 )
                        {
                            restart = TRUE;
                            end = TRUE;
                            KillTimer( hWnd, TIMER_CLOCK1 );
                            UpdateHighScore( score_number );
                            score_number = 0;
                            //hdc = BeginPaint( hWnd );
                            hdc = GetClientDC( hWnd );
                            SetBkColor( hdc, COLOR_black );
                            SetTextColor( hdc, COLOR_green );
                            SetPenColor( hdc, COLOR_green );
                            snprintf( strText, 50, _(IDS_PMP_WORM_RESTART_GAME) );
                            TextOut( hdc, INFO_X, INFO_Y, strText );
                            //EndPaint( hWnd, hdc );
                            ReleaseDC( hdc );
                            killtimes = 0;
                        }
                    }
                    break;
                case SCANCODE_CURSORBLOCKLEFT:
                    if( play == FALSE || end == TRUE )
                        break;

                    if(play == TRUE && end == FALSE && new == TRUE && begin == TRUE && restart == FALSE )
                        break;

                    if( right_direction( &worm ) != NOLEFT ) 
                    {
                        killtimes = handle_move( hWnd, &worm, count_wall, &bon, &bon_live,
                                array_wall_first, array_wall, killtimes, LEFT );
                        if( killtimes == 3 ) 
                        {
                            restart = TRUE;
                            end = TRUE;
                            KillTimer(hWnd, TIMER_CLOCK1 );
                            UpdateHighScore( score_number );
                            score_number = 0;
                            //hdc = BeginPaint( hWnd );
                            hdc = GetClientDC( hWnd );
                            SetBkColor( hdc, COLOR_black );
                            SetTextColor( hdc, COLOR_green );
                            SetPenColor( hdc, COLOR_green );
                            snprintf( strText, 50, _(IDS_PMP_WORM_RESTART_GAME) );
                            TextOut( hdc, INFO_X, INFO_Y, strText );
                            //EndPaint( hWnd, hdc );
                            ReleaseDC( hdc );
                            killtimes = 0;
                        }
                    }
                    break;
                case SCANCODE_CURSORBLOCKRIGHT:
                    if( play == FALSE || end == TRUE )
                        break;

                    if( play == TRUE && end == FALSE && new == TRUE &&
                            begin == TRUE && restart == FALSE )
                        break;

                    if( right_direction( &worm ) != NORIGHT ) 
                    {
                        killtimes = handle_move( hWnd, &worm, count_wall, &bon, &bon_live,
                                array_wall_first, array_wall, killtimes, RIGHT );
                        if( killtimes == 3 ) 
                        {
                            restart = TRUE;
                            end = TRUE;
                            KillTimer( hWnd, TIMER_CLOCK1 );
                            UpdateHighScore( score_number );
                            score_number = 0;
                            //hdc = BeginPaint( hWnd );
                            hdc = GetClientDC( hWnd );
                            SetBkColor( hdc, COLOR_black );
                            SetTextColor( hdc, COLOR_green );
                            SetPenColor( hdc, COLOR_green );
                            snprintf( strText, 50, _(IDS_PMP_WORM_RESTART_GAME) );
                            TextOut( hdc, INFO_X, INFO_Y, strText );
                            //EndPaint( hWnd, hdc );
                            ReleaseDC( hdc );
                            killtimes = 0;
                        }
                    }
                    break;
                case SCANCODE_CURSORBLOCKDOWN:
                    if( play == FALSE || end == TRUE )
                        break;

                    if( play == TRUE && end == FALSE && new == TRUE && begin == TRUE && restart == FALSE )
                        break;

                    if( right_direction( &worm ) != NODOWN) 
                    {
                        killtimes = handle_move( hWnd, &worm, count_wall, &bon, &bon_live,
                                array_wall_first, array_wall, killtimes, DOWN );
                        if( killtimes == 3 )
                        {
                            restart = TRUE;
                            end = TRUE;
                            KillTimer( hWnd, TIMER_CLOCK1 );
                            UpdateHighScore( score_number );
                            score_number = 0;
                            //hdc = BeginPaint( hWnd );
                            hdc = GetClientDC( hWnd );
                            SetBkColor( hdc, COLOR_black );
                            SetTextColor( hdc, COLOR_green );
                            SetPenColor( hdc, COLOR_green );
                            snprintf( strText, 50, _(IDS_PMP_WORM_RESTART_GAME) );
                            TextOut( hdc, INFO_X, INFO_Y, strText );
                            //EndPaint( hWnd, hdc );
                            ReleaseDC( hdc );
                            killtimes = 0;
                        }
                    }
                    break;
            }
            break;
        case MSG_SETFOCUS:
            //pmp_set_captionbar_title( _(IDS_PMP_WORM_TITLE) );
            break;
        case MSG_DESTROY: 
            DestroyAllControls( hWnd );
            hMainWnd = HWND_INVALID;
            break;
        case MSG_CLOSE:
            {
                if( !hWnd ) return 0;
                if( worm.seed > 30000 )
                    worm.seed = worm.seed % 100;
                save_seed( SEED_CFG, worm.seed );
                KillTimer( hWnd, TIMER_CLOCK1 );
                if( array_wall != NULL )
                    free( array_wall );
                array_wall = NULL;
                if( array_wall_first != NULL )
                    free( array_wall_first );
                array_wall_first = NULL;
                if( worm.worm != NULL )
                    free( worm.worm );
                worm.worm = NULL;
                if( bon.bonus != NULL )
                    free( bon.bonus );
                bon.bonus = NULL;
                if( bon_live.bonus != NULL )
                    free( bon_live.bonus );
                bon_live.bonus = NULL;
                Unload_map( );
                DestroyMainWindow( hWnd );
                MainWindowCleanup( hWnd );
                //pmp_app_notify_closed( PMP_APP_WORM );
                return 0;
            }
    }
    return DefaultMainWinProc( hWnd, message, wParam, lParam );
}

static void _worm_init_create_info (PMAINWINCREATE pCreateInfo )
{
    pCreateInfo->dwStyle = WS_NONE;
    //pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->dwExStyle = WS_EX_AUTOSECONDARYDC;
    pCreateInfo->spCaption = _(IDS_PMP_WORM_TITLE);
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor( IDC_ARROW );
    pCreateInfo->hIcon = GetSmallSystemIcon( IDI_APPLICATION );
    pCreateInfo->MainWindowProc = WormGameProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = MAINWINDOW_W; 
    pCreateInfo->by = MAINWINDOW_H;
    pCreateInfo->iBkColor = COLOR_black;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}
#if 0
BOOL register_worm_pic (void)
{
    int i;
    for (i = 1; i < ARRAY_LEN(worm_pic); i++)
    {
       if (RegisterResFromFile (HDC_SCREEN, worm_pic[i]) == FALSE)
       {
           fprintf (stderr, "can't register %s \n", worm_pic[i]);
           return FALSE;
       }
    }
    for (i = 0; i < ARRAY_LEN (worm_menu_pic); i ++)
    {
        if (RegisterResFromFile (HDC_SCREEN, worm_menu_pic[i]) == FALSE)
        {
            fprintf (stderr, "can't register %s \n", worm_menu_pic[i]);
            return FALSE;
        }
    }
    return TRUE;
}

void unregister_worm_pic (void)
{
    int i = 0;
    for (i  = 1; i < ARRAY_LEN (worm_pic); i ++)
        UnregisterRes (worm_pic[i]);
    for (i = 0; i < ARRAY_LEN (worm_menu_pic); i ++)
        UnregisterRes (worm_menu_pic[i]);
}
#endif

int MiniGUIMain (int args, const char* arg[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    
//    register_worm_pic ();
#ifdef _MGRM_PROCESSES
    JoinLayer (NAME_DEF_LAYER, "worm", 0, 0); 
#endif

    _worm_init_create_info( &CreateInfo );
    hMainWnd = CreateMainWindow(&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1; 

#if 0
    ShowWindow(hMainWnd,SW_SHOWNORMAL);
#else
    ShowWindowUsingShareBuffer (hMainWnd);
#endif

    while(GetMessage(&Msg, hMainWnd)) { 
        TranslateMessage (&Msg);
        DispatchMessage (&Msg);
    }
//    unregister_worm_pic ();

    MainWindowThreadCleanup(hMainWnd);
    return 0;
}


