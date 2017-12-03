#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "config.h"
#include "mlskbd.h"
#include "mlssize.h"

static HWND ghwnd;
static int gmsg;
static WPARAM gwParam;
static LPARAM glParam;
static BOOL pressed = FALSE;
static SRVEVTHOOK hook_cb = NULL;	

#define  MLS_KEY_PRESSED	1
#define  MLS_KEY_RELEASED	0
typedef struct _mls_key_t {

	/* the corrosponding rect of the key pad */
	RECT bound;

	/*style of current key, the value is MLS_KEY_PRESSED or MLS_KEY_RELEASED*/
	int state;

	/* the correcponging scancode of this key pad.
	 * such as SCANCODE_F1~F3,BACKSPACE,ENTER,UP,DOWN,LEFT,RIGHT...
	 */
	int scan_code; 

	/* private data for method update */
	void * data;

	/* this function is used to update the rect of key,
	 * which decided by key_pad. 
	 */
	void (*update)(struct _mls_key_t * key, HDC hdc);

} mls_key_t;

typedef struct _mls_kbd_t {
	/* rect of whole keyboard*/
	RECT bound;

	/* rect to monitor message*/
	RECT mrc;

	/* pointer to keys[] */
	mls_key_t* key;

	/* the number of the keys[] */
	int key_num;

	/*slave dc of MLShadow*/
	HDC hdc;

	/* value is : 
	 * MLS_KBD_SHOWN
	 * MLS_KBD_ACTIVE
	 */
	int state;

	/*id of timer*/
	int tid;

	/*time to live*/
	int ttl;

	/*count of time ticks*/
	int tc;

	/* private data for method update */
	void * data;

} mls_kbd_t;

#define KEYNUM(keys) (sizeof(keys)/sizeof(mls_key_t))

static mls_kbd_t* kbd;
#ifndef _MGRM_THREADS
static SRVEVTHOOK oldhook;
#elif !defined(_MGRM_PROCESSES)
static MSGHOOK oldhook;
#endif

static void mls_update(RECT* rect);

static void mls_key_update(mls_key_t* key, HDC hdc)
{
	int color;
	if(key->state == MLS_KEY_RELEASED) {
		mls_update(&key->bound);
		mlsEnableSlaveScreen(kbd->hdc, FALSE);
		mlsEnableSlaveScreen(kbd->hdc, TRUE);
	} else {
		color = SetBrushColor(kbd->hdc, RGB2Pixel(kbd->hdc, 255, 0, 0));
		FillBox(hdc, key->bound.left, 
				key->bound.top, RECTW(key->bound), RECTH(key->bound));
		SetBrushColor(kbd->hdc, color); 
		mlsEnableSlaveScreen(kbd->hdc, FALSE);
		mlsEnableSlaveScreen(kbd->hdc, TRUE);
	}
}

#define SCANCODE_HIDE (SCANCODE_USER + 1)
static mls_key_t key_pads[] = {
#if 1
    { RECT_MLS_KEY_1,  0, SCANCODE_CURSORBLOCKLEFT,  NULL, mls_key_update},
    { RECT_MLS_KEY_2,  0, SCANCODE_CURSORBLOCKDOWN,  NULL, mls_key_update},
    { RECT_MLS_KEY_3,  0, SCANCODE_CURSORBLOCKRIGHT, NULL, mls_key_update},
    { RECT_MLS_KEY_4,  0, SCANCODE_CURSORBLOCKUP,    NULL, mls_key_update},
    { RECT_MLS_KEY_5,  0, SCANCODE_ENTER,            NULL, mls_key_update},
    //{ RECT_MLS_KEY_6,  0, SCANCODE_ESCAPE,           NULL, mls_key_update},
    { RECT_MLS_KEY_6,  0, SCANCODE_M,           NULL, mls_key_update},
    { RECT_MLS_KEY_7,  0, SCANCODE_HIDE,             NULL, mls_key_update},
    { RECT_MLS_KEY_8,  0, SCANCODE_F1,               NULL, mls_key_update},
    { RECT_MLS_KEY_9,  0, SCANCODE_F2,               NULL, mls_key_update},
    { RECT_MLS_KEY_10, 0, SCANCODE_F3,               NULL, mls_key_update},
#endif
};

static BOOL mls_kbd_timeout(HWND hwnd, int id, DWORD time)
{
	if(id != kbd->tid)
		return FALSE;

	if(kbd->state & MLS_KBD_SHOWN) {
		kbd->tc ++;

		if(kbd->tc >= kbd->ttl)
			MLSKeyboardShow(FALSE);
	}

	return TRUE;
}

/*this function is used to update the MLShadow keyboard*/
static void mls_update(RECT* rect)
{
	PBITMAP bmp = (PBITMAP)kbd->data;

	SelectClipRect(kbd->hdc, rect);
	FillBoxWithBitmap (kbd->hdc, 0, 0, bmp->bmWidth, bmp->bmHeight, bmp);
	SelectClipRect(kbd->hdc, NULL);
}

mls_key_t* mls_get_key(mls_kbd_t* kbd, POINT p)
{
	int i;
	for(i=0; i<kbd->key_num; i++) {
		if(PtInRect(&kbd->key[i].bound, p.x, p.y))
			return &kbd->key[i];
	}

	return NULL;
}

void mls_send_msg(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	ghwnd = hwnd;
	gmsg = message;
	gwParam = wParam;
	glParam = lParam;
#if defined(_MGRM_PROCESSES) && !defined(_STAND_ALONE)
	Send2ActiveWindow (mgTopmostLayer, gmsg, gwParam, glParam);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
	PostMessage(ghwnd, gmsg, gwParam, glParam);
#endif
}

void mls_loca_trans(mls_kbd_t* kbd, POINT* p)
{
	p->x -= kbd->bound.left;
	p->y -= kbd->bound.top;
}

static BOOL repeat_msg(HWND hwnd, int id, DWORD time)
{
	mls_send_msg(ghwnd, gmsg, gwParam, glParam);
}

static int mls_kbd_proceed_hit(HWND hwnd, int message, POINT p) 
{
	static mls_key_t* key = NULL;
	static mls_key_t* old_key = NULL;

	key = mls_get_key(kbd, p);

	switch (message) {
		case MSG_LBUTTONDOWN:
		{
			if(key == NULL) 
				return HOOK_STOP;

			old_key = key;
			key->state |= MLS_KEY_PRESSED;
			key->update(key, kbd->hdc);
			mls_send_msg(hwnd, MSG_KEYDOWN, key->scan_code, 0);

			if(key->scan_code != SCANCODE_HIDE) {
				SetTimerEx(HWND_NULL, 44, 10, repeat_msg);
				KillTimer(HWND_NULL, kbd->tid);
			}
			return HOOK_STOP;
		}
		case MSG_LBUTTONUP:
		{
			KillTimer(HWND_NULL, 44);
			if(key == NULL)
				return HOOK_STOP;

			key->state &= ~MLS_KEY_PRESSED;
			key->update(key, kbd->hdc);

			if(key->scan_code == SCANCODE_HIDE) {
				MLSKeyboardShow(FALSE);
			} else {
#ifdef _MGRM_PROCESSES
				if(key->scan_code == SCANCODE_ESCAPE && hook_cb) {
				//if(hook_cb) {
					MSG msg;
					msg.message = MSG_KEYUP;
					msg.wParam = key->scan_code;
					msg.lParam = 0;
					return hook_cb(&msg);
				}
#endif
				mls_send_msg(hwnd, MSG_KEYUP, key->scan_code, 0);
				SetTimerEx(HWND_NULL, kbd->tid, 100, mls_kbd_timeout);
			}
			return HOOK_STOP;
		}
		case MSG_MOUSEMOVE:
		{
			if(!pressed)
				return HOOK_STOP;

			if(old_key == key)
				return HOOK_STOP;

			if(old_key) { 
				old_key->state &= ~MLS_KEY_PRESSED;
				old_key->update(old_key, kbd->hdc);
				KillTimer(HWND_NULL, 44);
				SetTimerEx(HWND_NULL, kbd->tid, 100, mls_kbd_timeout);
			}

			if(key != NULL) {
				key->state |= MLS_KEY_PRESSED;
				key->update(key, kbd->hdc);
			}

			old_key = key;
			return HOOK_STOP;
		}
	}

	return HOOK_GOON;
}

#ifndef _MGRM_PROCESSES
static int event_hook(void* context, HWND dst_wnd, int message, WPARAM wParam, LPARAM lParam)
{
#else
static int event_hook (PMSG msg)
{
	HWND dst_wnd = msg->hwnd;
	int message = msg->message;
	int wParam = msg->wParam;
	int lParam = msg->lParam;
#endif
	POINT p;
	switch(message) {
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
		case MSG_MOUSEMOVE:
		{
			if(message == MSG_LBUTTONDOWN)
				pressed = TRUE;
			if(message == MSG_LBUTTONUP)
				pressed = FALSE;

			if(!(kbd->state & MLS_KBD_ACTIVE))
				return HOOK_GOON;

			p.x = LOSWORD (lParam);
			p.y = HISWORD (lParam);

			if(!(kbd->state & MLS_KBD_SHOWN)) { 
				if(PtInRect(&kbd->mrc, p.x, p.y)) {
					MLSKeyboardShow(TRUE);
					return HOOK_STOP;
				}

				return HOOK_GOON;
			}

			if(PtInRect(&kbd->bound, p.x, p.y)) {
				kbd->tc = 0;
				mls_loca_trans(kbd, &p);
				return mls_kbd_proceed_hit(dst_wnd, message, p);
			} else {
				if(message != MSG_MOUSEMOVE)
					MLSKeyboardShow(FALSE);	
				return HOOK_GOON;
			}

			return HOOK_GOON;
		}

#ifdef _MGRM_PROCESSES
		case MSG_KEYUP:
		case MSG_KEYDOWN:
		{
			if(hook_cb)
				return hook_cb(msg);	
			return HOOK_GOON;
		}
#endif
	}
}

int MLSKeyboardInit(RECT bound, RECT mrc, int colorkey, 
		int alpha, int ttl, SRVEVTHOOK hook )
{
	char buff[16] = {0};
	PBITMAP bmp;

	kbd = (mls_kbd_t*) calloc (1, sizeof(mls_kbd_t));
	kbd->data = calloc (1, sizeof(BITMAP));

	kbd->bound	= bound;
	kbd->mrc	= mrc;
	kbd->ttl	= ttl;
	kbd->tid	= 43;
	kbd->tc		= 0;

	kbd->key		= key_pads;
	kbd->key_num	= KEYNUM(key_pads);
	
	sprintf(buff, "%dx%d-%dbpp", RECTW(kbd->bound), 
			RECTH(kbd->bound), 16);

   	if ((kbd->hdc = InitSlaveScreen ("mlshadow", buff)) == HDC_INVALID) {
		fprintf(stderr, "Creat slave_screen faild!\n");
		return -1;
	}

	if (LoadBitmapFromFile (kbd->hdc, (PBITMAP)kbd->data,
				"./softkeypad.gif")) {
		fprintf(stderr, "Load bitmap faild !\n");
		return -1;
	}

	mlsSetSlaveScreenInfo (kbd->hdc, 
			MLS_INFOMASK_OFFSET | MLS_INFOMASK_BLEND, 
			kbd->bound.left, kbd->bound.top, MLS_BLENDMODE_ALPHA,
			colorkey, alpha, 0);

	bmp = (PBITMAP)kbd->data;
	FillBoxWithBitmap (kbd->hdc, 0, 0, bmp->bmWidth, bmp->bmHeight, bmp);

	oldhook = NULL;

	MLSKeyboardShow(FALSE);
	MLSKeyboardSetActive(TRUE);

#ifndef _MGRM_PROCESSES
	oldhook = RegisterMouseMsgHook(NULL, event_hook);
#else
	hook_cb = hook;
	oldhook = SetServerEventHook (event_hook);
#endif

	return 0;
}

int MLSKeyboardFini()
{
	UnloadBitmap((PBITMAP)kbd->data);		
	free(kbd->data);

#ifndef _MGRM_PROCESSES
	RegisterMouseMsgHook(NULL, oldhook);
#else
	SetServerEventHook (oldhook);
#endif

	TerminateSlaveScreen (kbd->hdc);
	free(kbd);

	return 0;
}

void MLSKeyboardShow(BOOL show)
{
	int ret;
	if(show) {// && !(kbd->state & MLS_KBD_SHOWN)) {
		mlsEnableSlaveScreen (kbd->hdc, show);
		kbd->state |= MLS_KBD_SHOWN;	
		kbd->tc = 0;
		ret = SetTimerEx(HWND_NULL, kbd->tid, 100, mls_kbd_timeout);
	} else if (!show) {// && (kbd->state & MLS_KBD_SHOWN)) {
		mlsEnableSlaveScreen (kbd->hdc, show);
		kbd->state &= ~MLS_KBD_SHOWN;	
		KillTimer(HWND_NULL, kbd->tid);
		KillTimer(HWND_NULL, 44);
	}
}

void MLSKeyboardSetActive(BOOL active)
{
	if(active && !(kbd->state & MLS_KBD_ACTIVE)) {
		kbd->state |= MLS_KBD_ACTIVE;
#ifndef _MGRM_PROCESSES
		RegisterMouseMsgHook(NULL, oldhook);
#else
		SetServerEventHook (oldhook);
#endif
	} else if (!active && (kbd->state & MLS_KBD_ACTIVE)) {
		kbd->state &= ~MLS_KBD_ACTIVE;	
#ifndef _MGRM_PROCESSES
		oldhook = RegisterMouseMsgHook(NULL, event_hook);
#else
		oldhook = SetServerEventHook (event_hook);
#endif
	}
}

/*return state of mls keyboard.
 * return value is MLS_KBD_SHOWN, MLS_KBD_ACTIVE etc.
 * */
int MLSKeyboardGetState()
{
	return kbd->state;
}

void MLSKeyboardSetState(int state)
{
	kbd->state = state;
}

void MLSKeyboardSetMonitorRect(RECT* rect)
{
	if(rect == NULL) {
		kbd->mrc = g_rcScr;
		return;
	}

	kbd->mrc = *rect;
}
