#ifndef __SFKBD_CLIENT_H
#define __SFKBD_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define SFKBD_REQID    (MAX_SYS_REQID + 9)

#define SFKBD_GETSTATE      3

BOOL SFKBDIsShown(void);
int SFKBDShow (BOOL);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
