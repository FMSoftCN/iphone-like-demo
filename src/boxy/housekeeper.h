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

#define _240x320_SCREEM
#ifndef _240x320_SCREEM

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

static char* housekeeper_pic[] = {
    "./res/housekeeper_goal.gif",
    "./res/housekeeper_man.gif",
    "./res/housekeeper_object.gif",
    "./res/housekeeper_saveman.gif",
    "./res/housekeeper_treasure.gif",
    "./res/housekeeper_stone.gif",
    "./res/housekeeper_menu_top.gif",
    "./res/housekeeper_menu_captbg.gif",
    "./res/housekeeper_menu_mid.gif",
    "./res/housekeeper_menu_btm.gif"
};

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




