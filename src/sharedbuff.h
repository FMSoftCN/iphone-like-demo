#ifndef __SHAREDBUFF_H
#define __SHAREDBUFF_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define SHARED_BUFF_REQID    (MAX_SYS_REQID + 8)

#define REQ_BUFFER_READY    1

void* CreateSharedBuffer (void);

void* AttachSharedBuffer (void);
void NotifyServerSharedBufferReady (void);
void ShowWindowUsingShareBuffer (HWND hwnd);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __SHAREDBUFF_H */

