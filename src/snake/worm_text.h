typedef enum
{
    /*worm msgid*/
    IDS_PMP_WORM_TITLE = 1300,
    IDS_PMP_WORM_TEXT,
    IDS_PMP_WORM_REFILE_SHOW,
    IDS_PMP_WORM_WRFILE_SHOW,
    IDS_PMP_WORM_START,
    IDS_PMP_WORM_SET_LEVEL,
    IDS_PMP_WORM_SCORE_INFO,
    IDS_PMP_WORM_SCORE,
    IDS_PMP_WORM_LEVEL,
    IDS_PMP_WORM_RESTART_GAME,
    IDS_PMP_WORM_SHOW_SCORE,        
} WORM_MSG;       

typedef struct _WORM_TEXT_DOMAIN_INFO
{
    char*   name;
    int     id_base;
    char**   text_array ;
} WORM_TEXT_DOMAIN_INFO;

static char *worm_text[] =
{
    /*worm msg*/
    "WORM",
    "WARNING",
    "The document of /media/cfg/worm_score.conf nonexistent or the user can't read!\nPlease disposal!",
    "The user can not open the file of /media/cfg/worm_score.conf or don't have the power of creat file in the dir of /media/cfg!\nPlease disposal!",
    "START",
    "Set Level \t %d",
    "High Score",
#ifndef _240x320_WORM
    "SCORE:%d",
    "LEVEL:%d",
#else
    "SCORE:\n %d",
    "LEVEL:\n %d",
#endif
    "Game Over!",
    "First Score: %d\nSecond Score: %d\nThird Score: %d\nFourth Score: %d\nFifth Score: %d\n",
};

WORM_TEXT_DOMAIN_INFO _worm_text_domain =  {"en", 1300, worm_text};
static WORM_TEXT_DOMAIN_INFO *cur_text_domain = &_worm_text_domain;


