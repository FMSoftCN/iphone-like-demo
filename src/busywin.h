#ifndef __BUSYWIN_H
#define __BUSYWIN_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define BUSYWIN_REQID    (MAX_SYS_REQID + 7)

HWND CreateBusyWindow (HWND hosting);

int ShowBusyWindow (BOOL bShow);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __BUSYWIN_H */

