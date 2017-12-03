#ifndef __SOFTKEYWINDOW_H__
#define __SOFTKEYWINDOW_H__

void EnableSoftKeypad (BOOL e);
HWND CreateSoftKeypad (void (*cb)(BOOL SoftKeyWindowIsShown));

#endif
