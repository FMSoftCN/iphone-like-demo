
#ifndef MG_PHONE_H
#define MG_PHONE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "animate/animate.h"

typedef struct _PHONE_ICON {
    BITMAP l_slant_bmp;
    BITMAP r_slant_bmp;
    BITMAP normal_bmp;
	BITMAP bmpSwitch;
    /*the program name used by fork*/
    char* name;
    char* command;
    
    //BOOL is_app_launched;
	pid_t pid;

	int x,y;
	
	ANIMATE *a;

}PHONE_ICON;

/*if the mouse on the icon*/
BOOL is_hitted_icon (PHONE_ICON* ph_icon, int x, int y);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
