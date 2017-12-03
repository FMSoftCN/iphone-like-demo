typedef enum
{
    IDS_PMP_HOUSEKEEPER_TITLE = 1000,
    IDS_PMP_HOUSEKEEPER_TEXT,
    IDS_PMP_HOUSEKEEPER_REFILE_SHOW,
    IDS_PMP_HOUSEKEEPER_WRFILE_SHOW,
    IDS_PMP_HOUSEKEEPER_UNDO,
    IDS_PMP_HOUSEKEEPER_RESTART,
    IDS_PMP_HOUSEKEEPER_SET_LEVEL,
    IDS_PMP_HOUSEKEEPER_SET_RANK,
    IDS_PMP_HOUSEKEEPER_SCORE_INFO,
    IDS_PMP_HOUSEKEEPER_DISPLAY,
    IDS_PMP_HOUSEKEEPER_SHOW_SCORE,
    IDS_PMP_HOUSEKEEPER_PREV,
    IDS_PMP_HOUSEKEEPER_NEXT,
} HK_MSG;       

typedef struct _HK_TEXT_DOMAIN_INFO
{
    char*   name;
    int     id_base;
    char**   text_array ;
} HK_TEXT_DOMAIN_INFO;

static char *hk_text[] =
{
    /*housekeeper msg*/
    "HOUSEKEEPER",
    "WARNING",
    "The document of /media/cfg/housekeeper_score.conf nonexistent or the user can't read!\nPlease disposal!",
    "The user can not open the file of /media/cfg/housekeeper_score.conf or don't have the power of creat file in the dir of /media/cfg!\nPlease disposal!",
    "UNDO",
    "RESTART",
    "Set Level \t %s",
    "Set rank \t %s",
    "High Score",
    "LEVEL:%s RANK:%d SCORE:%d",
    "First Score: %d\nSecond Score: %d\nThird Score: %d\nFourth Score: %d\nFifth Score: %d\n",
    "PREV",
    "NEXT",
};

HK_TEXT_DOMAIN_INFO _hk_text_domain =  {"en", 1000, hk_text};
static HK_TEXT_DOMAIN_INFO *cur_text_domain = &_hk_text_domain;


