#include <minigui/common.h>
#include <minigui/minigui.h>

#ifdef _MGRM_PROCESSES

#include "sfkbd_client.h"
BOOL SFKBDIsShown(void)
{
    REQUEST req;
	int data = SFKBD_GETSTATE; 
    int reply;

    req.id = SFKBD_REQID;
    req.data = &data;
    req.len_data = sizeof (data);

    ClientRequest (&req, &reply, sizeof (reply));

    return reply;
}

int SFKBDShow (BOOL show)
{
    REQUEST req;
	int data = show;
    int reply;

    req.id = SFKBD_REQID;
    req.data = &data;
    req.len_data = sizeof (data);

    ClientRequest (&req, &reply, sizeof (reply));

    return reply;
}

#else /* !_MGRM_PROCESSES */

#endif /*! _MGRM_PROCESSES */
