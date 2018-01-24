#include <minigui/common.h>
#undef _USE_MINIGUIENTRY
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#if defined(__GNUC__)
#ifdef _DEBUG_MSG
#   define _MY_PRINTF(fmt...) fprintf (stderr, fmt)
#else
#   define _MY_PRINTF(fmt...)
#endif
#else /* __GNUC__ */
    static inline void _MY_PRINTF(const char* fmt, ...)
    {
#ifdef _DEBUG_MSG
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        va_end(ap);
#endif
    }
#endif /* __GNUC__ */

#define MAP_ROWS                20
#define MAP_COLS                39

#define BLOCK_WIDTH             8
#define BLOCK_HIGHT             8
#define UP                      1
#define RIGHT                   2
#define DOWN                    3
#define LEFT                    4
#define NOUP                    5
#define NORIGHT                 6
#define NODOWN                  7
#define NOLEFT                  8
#define NODE_WIDTH              4
#define NODE_HIGHT              4
#define ADD_NUMBER              4
#define RED                     1
#define YELLOW                  2
#define BONI_WIDTH              4	
#define BONI_HIGHT              4
#define MEET                    1
#define UNMEET                  0
#define MEET_BONI_RED           -1
#define MEET_BONI_YELLOW        0
#define EAT_SELF                1
#define IN_WALL                 1
#define UNIN_WALL               0
#define begin_NUMBER            4
#define TIMER_CLOCK1            100
#define FREQ_CLOCK1             30 
#define WORM_SCOREPATH          (PMP_CFG_PATH "worm_score.conf")
#define SEED_CFG                (PMP_CFG_PATH "seed")

#define _240x320_WORM

#ifndef _240x320_WORM
#define INFO_X                  110
#define INFO_Y                  80
#else
#define INFO_X                  40
#define INFO_Y                  150
#endif

#ifndef _240x320_WORM
#define MAINWINDOW_W         320
#define MAINWINDOW_H         240
#else
#define MAINWINDOW_W         240
#define MAINWINDOW_H         320
#endif

#define PMP_MI_OP_DEF        0x00
#define PMP_MI_OP_PREV       0x01
#define PMP_MI_OP_NEXT       0x02
#define PMP_MI_OP_ENTER      0x03
#define PMP_MI_OP_CANCEL     0x04

#define PMP_MI_ST_NONE       0x10
#define PMP_MI_ST_UPDATED    0x11
#define PMP_MI_ST_SELECTED   0x12
#define PMP_MI_ST_UPDATED_OTHERS    0x13

#define LEN_ITEM_TEXT        60
#define PMP_CFG_PATH                "/media/cfg/"

#define LEN_RES_TOP_DIR     126
#define LEN_RES_FILE_NAME   126

#define _(id)  cur_text_domain->text_array[ id - cur_text_domain->id_base]
#define ARRAY_LEN(array) \
    (sizeof(array)/sizeof(array[0]))

/* [0]:bitmap_worm_node
 * [1]:bitmap_worm_node_hide
 * [2]:bitmap_map_node
 * [3]:bitmap_boni_red
 * [4]:bitmap_boni_yellow
 * [5]:bitmap_boni_hide
 * [6]:menu_top
 * [7]:menu_captbg
 * [8]:menu_mid
 * [9]:menu_btm
 */
static char* worm_pic[] = {
    "./res/worm_node.gif",
    "./res/worm_node_hide.gif",
    "./res/worm_map_node.gif",
    "./res/worm_boni_red.gif",
    "./res/worm_boni_yellow.gif",
    "./res/worm_boni_hide.gif",
    "./res/worm_menu_top.gif",
    "./res/worm_menu_captbg.gif",
    "./res/worm_menu_mid.gif",
    "./res/worm_menu_btm.gif"
};

typedef struct tagworm_node
{
    int x;
    int y;
} worm_node;

typedef struct tagworm_list
{
    struct tagworm_node *worm;
    int direction;
    int number;
    int tail_direction;
    int score[5];
    int level;
    int seed;
    char strText[50];
} worm_list;

typedef struct tagmap_node
{
    int x;
    int y;
} map_node;

typedef struct tagboni
{
    int x;
    int y;
    int color;
} boni;

typedef struct tagbonus_list
{
    struct tagboni *bonus;
    int number;
    int boni_num;
} boni_list;



typedef int (*CB_MENU_ITEM) (void * context, int id, int op, char *text_buff);
    
typedef struct _PMP_MENU_ITEM{
    int id; 
    CB_MENU_ITEM cb_menu_item;
    BOOL dirty;
} PMP_MENU_ITEM;


typedef struct _PMP_RES_HASH_UNIT
{
    const char* key;
    const unsigned char* data;
    unsigned int data_size;
    BITMAP *bmp; 
    int ref_count;
} PMP_RES_HASH_UNIT;

int pmp_show_menu (HWND parent, const char* title, PMP_MENU_ITEM *items, void *context);
int pmp_show_info(HWND h_parent_wnd, const char* title, const char* msg, DWORD dwStyle);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
