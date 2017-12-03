
#ifndef _QUICK_VIEW_H_
#define _QUICK_VIEW_H_


void Init_QuickView();

void Show_QuickView(HDC dc);
void Hide_QuickView(HDC dc, BOOL draw_once);

void Switch_QuickView(const char * name, const char * command);

#endif /* _QUICK_VIEW_H_ */
