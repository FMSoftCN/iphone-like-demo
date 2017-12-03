#ifndef ICON_LAYOUT_H
#define ICON_LAYOUT_H
typedef struct icon_cell_t{
	LAYOUT_CELL_INTF *intf;
//	int x, y, width, height;
	PHONE_ICON icon;
}icon_cell_t;

LAYOUT_CELL_OBJ* loadIcons(const char* etcFile,const char*sectName, void *data);

#define MSG_SPACECELL MSG_USER + 50

#endif
