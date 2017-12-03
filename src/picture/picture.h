#ifndef PICTURE_H
#define PICTURE_H

#ifdef  __cplusplus
extern "C" {
#endif

//#include "config.h"

#define VIEW_PATH  "view/"
typedef struct _BMP_NODE{
    BITMAP icon;
    int row;
    int col;
    int index;
    struct _BMP_NODE *prev;
    struct _BMP_NODE *next;
}BMP_NODE;

typedef struct _NODE_LIST {
    BOOL isViewList;
    int rowOffset;
    int count;
    int maxRowOffset;
    int boxX;
    int boxY;
    int boxOffX;
    int boxOffY;
    unsigned int scale;
    struct _BMP_NODE *beginNode;
    struct _BMP_NODE *curNode;
}NODE_LIST;

typedef struct _MOUSE_OP {
    unsigned int time;
    int originalX;
    int originalY;
    int downX;
    int downY;
}MOUSE_OP;

#ifdef  ENABLE_LANDSCAPE 
#define PIC_BMP_BG  "res/picture_bg.png"
#define PIC_MAIN_ROW 6
#define PIC_MAIN_COL 8
#else
#define PIC_BMP_BG  "res/picture_bg_pt.png"
#define PIC_MAIN_ROW 8
#define PIC_MAIN_COL 6
#endif

#define MAIN_BORDER 2

#define MAX_PICTURES 10

#ifdef  __cplusplus
}
#endif

#endif /* PICTURE_H */

