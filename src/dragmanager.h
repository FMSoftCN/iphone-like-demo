/*
 * drag manager, try to translate mouse information into drag info
 *
 */

#ifndef DRAG_MANAGER_H
#define DRAG_MANAGER_H

/*
 */


#define DRAG_MSG_BEGIN LAY_MSG_MOUSE_BEGIN + 10 
#define DRAG_MSG_END  DRAG_MSG_BEGIN + 5

/*
 * wParam = 0
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 */
#define MSG_DRAG_ACTIVE DRAG_MSG_BEGIN + 1

#endif
