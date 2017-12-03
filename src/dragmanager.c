/*
 * drag manager inplemention
 *
 */

#include <sys/time.h>
#include <time.h>
#include "minigui/common.h"
#include "dragmanager.h"
#include "sence.h"

#define NORMAL  0
#define LDOWNED 1
#define DRAGED  2

#define DIFF_TIME_USEC(later_tv, early_tv) \
    (((later_tv).tv_sec - (early_tv).tv_sec) * 10000000 \
    + (later_tv).tv_usec - (early_tv).tv_usec)

static LPARAM ldown_xy;
static struct timeval ldown_time = { 0, 0 };
static struct timeval cur_time;
static HWND _trace_hwnd  = 0;
void TranslateDragMessage(MSG* pmsg)
{
    switch (pmsg->message)
    {
        case MSG_TIMER:
            if (ldown_time.tv_usec != 0 ||
                    ldown_time.tv_sec != 0){
	            gettimeofday (&cur_time, NULL);
				if(DIFF_TIME_USEC (cur_time, ldown_time) > 2000000) {
	                PostMessage (_trace_hwnd, MSG_DRAG_ACTIVE, 0, ldown_xy);
	                ldown_time.tv_usec = 0;
	                ldown_time.tv_sec = 0;
				}
            }
            break;

        case MSG_LBUTTONDOWN:
            gettimeofday (&ldown_time, NULL);
            _trace_hwnd = pmsg->hwnd;
            ldown_xy = pmsg->lParam;
            break;

        case MSG_MOUSEMOVE:
            ldown_xy = pmsg->lParam;
            break;

        case MSG_LBUTTONUP:
            ldown_time.tv_usec = -1;
            if (LOSWORD(ldown_xy) > 210)
		        SenceProc(g_curSence, LAY_MSG_INFO, pmsg->wParam, pmsg->lParam);
            break;
    }

}
