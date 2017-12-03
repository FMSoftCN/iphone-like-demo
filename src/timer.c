/*
 * timer manager
 */

#include "timer.h"

//#define  DEBUG 1
//#define FUN_DEBUG
//#define VAL_DEBUG
#include "my_debug.h"
extern HDC g_hMainDC;

static _total_animates=0;

static HDC _mem_dc;

#define AF_REMOVE 0x1
#define AF_AUTODEL 0x02


animate_obj_link* _animate_priorities[4];
ANIMATE_BKCLEAN* _animate_bkclean=NULL;
#define MAX_PRIORITY  4

#ifdef USE_OS_TIMER
#include <time.h>
#include <sys/time.h>
#include <signal.h>

static void (*old_handler)(int) =NULL;
static int timer_intval=0;
static int timer_count = 0;
static void set_system_timer(int ms, void (*handle)(int))
{
    struct sigaction siga;
	struct itimerval itv,oldtv;

    sigaction (SIGALRM, NULL, &siga);
	old_handler = siga.sa_handler;
    siga.sa_handler = handle;
    siga.sa_flags = SA_RESTART;
    sigaction (SIGALRM, &siga, NULL);

	timer_intval = ms/10;
	timer_count = 0;

	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 10*1000;
	itv.it_value.tv_sec = itv.it_interval.tv_sec;
	itv.it_value.tv_usec = itv.it_interval.tv_usec;

	setitimer(ITIMER_REAL,&itv,&oldtv);
}

static void on_system_timer(int sig)
{
#if DEBUG
	static struct timeval tv1;
	struct timeval tv2;
	gettimeofday(&tv2,NULL);
	printf("%s:%d:time=%d usec\n", __FUNCTION__,__LINE__,
		(tv2.tv_sec-tv1.tv_sec)*1000000+tv2.tv_usec-tv1.tv_usec);
	tv1 = tv2;
#endif
	if(old_handler)
		(*old_handler)(sig);
	timer_count ++;
	if(timer_count >= timer_intval){
		PostMessage(g_hMainWnd, MSG_MYTIMER, 1, 0);
		timer_count = 0;
	}
}
#endif

void ResetTimerInterval(int ten_ms)
{
	//timer_intval = ms/10;
    ResetTimer (g_hMainWnd, 100, ten_ms);
    printf("reset timer = %d\n", ten_ms);
}

POnAllAnimateFinished OnAllAnimateFinished = NULL;

BOOL InitAnimateTimer()
{

	_mem_dc = CreateCompatibleDC(HDC_SCREEN);
#ifdef USE_OS_TIMER
	//set_system_timer(200, on_system_timer);
#endif
	return TRUE;
}

/*
void DeleteAllAnimates (void)
{
	int i;
	for(i=0; i<sizeof(_animate_priorities)/sizeof(animate_obj_link*); i++)
	{
		
		animate_obj_link* lk = (animate_obj_link*)_animate_priorities[i];
		animate_obj_link* prev = lk;
		while (lk)
        {
            animate_obj_link *lktmp = lk;
            _total_animates --;
            if(lk->flag & AF_AUTODEL)
                AmFree(lk->obj);

            lk = lk->next;
            if(prev == lktmp)
            {
                _animate_priorities[i] = lk;
                prev = lk;
            }
            else {
                prev->next = lk;
            }
            free(lktmp);
        }
	}
	if(_total_animates>0 && OnAllAnimateFinished)
	{
		(*OnAllAnimateFinished)();
	}
	_total_animates = 0;
}
*/

static void removeAnimates(int remove_cnt)
{
	int i;
	for(i=0; i<sizeof(_animate_priorities)/sizeof(animate_obj_link*)&& remove_cnt>0; i++)
	{
		
		animate_obj_link* lk = (animate_obj_link*)_animate_priorities[i];
		animate_obj_link* prev = lk;
		while(lk && remove_cnt>0)
		{
			if(lk->flag & AF_REMOVE)
			{
				remove_cnt --;
				_total_animates --;
                //TEST_VAL (_total_animates, %d);
				if(lk->flag & AF_AUTODEL)
					AmFree(lk->obj);

				animate_obj_link *lktmp = lk;
				lk = lk->next;
				if(prev == lktmp)
				{
					_animate_priorities[i] = lk;
					prev = lk;
				}
				else {
					prev->next = lk;
				}
				free(lktmp);
			}
			else{
				prev = lk;
				lk = lk->next;
			}
		}
	}
}
static volatile int timer_lock; // spin lock for timer proc.
void TimerCallback(int timerId)
{
//	struct timeval tv;
//	struct timeval tv2;
//	gettimeofday(&tv,NULL);
    
    if (timer_lock) return;
    timer_lock = 1;

	if(_total_animates > 0)
	{
		BEGIN_TIME()
		//clear bkground
		RECT rt={0,0,1,1};
		SelectClipRect(_mem_dc,&rt);
		ANIMATE_BKCLEAN * ambk = _animate_bkclean;
		while(ambk)
		{
			IncludeClipRect(_mem_dc,&ambk->rtBk);			
			(*ambk->clearBkground)(_mem_dc,&ambk->rtBk,ambk->user_data);
			ambk = ambk->next;
		}

		//draw animate
		int i;
		int remove_cnt = 0;
		for(i=0;i<sizeof(_animate_priorities)/sizeof(animate_obj_link*); i++)
		{
			animate_obj_link* lk = (animate_obj_link*)_animate_priorities[i];
			while(lk)
			{
				if(lk->flag & AF_REMOVE){
					lk = lk->next;
					continue;
				}

				if(AmIsEnd(lk->obj))
				{
                    /*wxg */
					//lk->flag &= AF_REMOVE;
					lk->flag |= AF_REMOVE;
					remove_cnt ++;
				}
				else{
					AmShowCurFrame(lk->obj, _mem_dc);
					AmNextFrame(lk->obj);
				}
				lk = lk->next;
			}
		}
		if(remove_cnt>0)
			removeAnimates(remove_cnt);

		BitBlt(_mem_dc,0,0,g_rcScr.right,g_rcScr.bottom, g_hMainDC,0,0,0);
		if(_total_animates<=0 && OnAllAnimateFinished)
		{
			(*OnAllAnimateFinished)();
		}
		END_TIME()
	}
    timer_lock = 0;
}

BOOL InsertAnimate(int priority, ANIMATE_OBJ* aobj, BOOL autoDelete)
{
	if(priority < TIME_PRI_LOW) 
		priority = TIME_PRI_LOW;
	else if(priority > TIME_PRI_VERY_HIG)
		priority = TIME_PRI_VERY_HIG;
	
	animate_obj_link * lk = (animate_obj_link*)calloc(sizeof(animate_obj_link),1);
	if(autoDelete)
		lk->flag &= AF_AUTODEL;
	
	lk->obj = aobj;

	lk->next = _animate_priorities[priority];
	_animate_priorities[priority] = lk;
    _total_animates++;
    //TEST_VAL (_total_animates, %d);
    return TRUE;
}


BOOL DeleteAnimate(ANIMATE_OBJ* obj)
{
	int i;
	for(i=0; i<sizeof(_animate_priorities)/sizeof(animate_obj_link*); i++)
	{
		
		animate_obj_link* lk = (animate_obj_link*)_animate_priorities[i];
		animate_obj_link* prev = lk;
		while(lk)
		{
			if(lk->obj == obj)
			{
				_total_animates --;
                //TEST_VAL (_total_animates, %d);
				if(lk->flag & AF_AUTODEL)
					AmFree(lk->obj);

				animate_obj_link *lktmp = lk;
				lk = lk->next;
				if(prev == lktmp)
				{
					_animate_priorities[i] = lk;
				}
				else {
					prev->next = lk;
				}
				free(lktmp);
				break;
			}
			else{
				prev = lk;
				lk = lk->next;
			}
		}
	}

	return TRUE;
}


BOOL InsertAnimateBkClean(ANIMATE_BKCLEAN* bkclean)
{
	bkclean->next = _animate_bkclean;
	_animate_bkclean = bkclean;
	return TRUE;
}

BOOL DeleteAnimateBkClean(ANIMATE_BKCLEAN* bkclean)
{
	if(bkclean == _animate_bkclean)
	{
		_animate_bkclean = bkclean->next;
	}
	else{
		ANIMATE_BKCLEAN* bkc = _animate_bkclean;
		while(bkc->next && bkc->next != bkclean)
			bkc = bkc->next;
		if(bkc->next)
		{
			bkc->next = bkclean->next;
		}
	}
	return TRUE;
}


static ANIMATE_OBJ* findAnimateInList(void* key, animate_obj_link* plink)
{
	while(plink){
		if(AmEqual(plink->obj,key))
			return plink->obj;
		plink = plink->next;
	}
	return NULL;
}

ANIMATE_OBJ* FindAnimateObj(void* key, int priority)
{
	if(priority>=0 && priority<sizeof(_animate_priorities)/sizeof(animate_obj_link*))
	{
		return findAnimateInList(key,_animate_priorities[priority]);
	}
	else
    {
		int i;	
		for(i=0; i<sizeof(_animate_priorities)/sizeof(animate_obj_link*); i++)
		{
			ANIMATE_OBJ* obj = findAnimateInList(key,_animate_priorities[i]);
			if(obj)return obj;
		}
    }
}

BOOL ChangePriority(void* key, int new_priority)
{
	int i;
	animate_obj_link *obj_change = NULL;
	if(new_priority<0 || new_priority>=MAX_PRIORITY)
		return FALSE;

	for(i=0; i<MAX_PRIORITY; i++)
	{
        /*
		if(new_priority==i)
			continue;
            */
		
		animate_obj_link * lk = _animate_priorities[i];
		if(!lk) continue;

		if(AmEqual(lk->obj, key))
		{
			obj_change = lk;
			_animate_priorities[i] = lk->next;

            if (new_priority == i)
                return TRUE;

			break;
		}
		else
		{
			while(lk->next)
			{
				if(AmEqual(lk->next->obj,key))
                {
                    obj_change = lk->next;
                    lk->next = obj_change->next;

                    if (new_priority == i)
                        return TRUE;

                    goto FOUND;
                }
				lk = lk->next;
			}
		}	
	}

	if(obj_change == NULL)
		return FALSE;
FOUND:
	obj_change->next = _animate_priorities[new_priority];
	_animate_priorities[new_priority] = obj_change;
	return TRUE;
}

