#include "readiwlist.h"

void free_wireless_ap_node(WIRELESS_AP * ap)
{
	if (!ap)
		return;

	if (ap->essid)
		free(ap->essid);
	free(ap);

	ap = NULL;
}

void free_all_ap(WIRELESS_AP * ap)
{
	WIRELESS_AP *tmp1 , *tmp2;
	tmp1 = tmp2 = ap;
	while (tmp1)
	{
		tmp2 = tmp1->next;
		free_wireless_ap_node(tmp1);
		tmp1 = tmp2;
	}
	ap = NULL;
}

void pinrtf_all_ap_node(WIRELESS_AP * ap)
{
	WIRELESS_AP * tmp = ap;
	while(tmp)
	{
		printf("the essid is [%s], haveKey %s\n", tmp->essid, tmp->haveKey?"true":"false");
		tmp = tmp->next;
	}
}

char*get_essid(const char *sub)
{
	/*	will from 
	 *	ESSID:"feynman"\n  
	 *  to get the value of "feynman"
	 *	*/
	if (!sub)
		return NULL;

	char* begin = strstr (sub, "ESSID:\"") ;
	if (!begin)
		return NULL;

	begin = begin + strlen("ESSID:\"");
	char* after = strstr (begin, "\""); 

	int len = after - begin;
	char *value = (char*)malloc (sizeof(char)*len + 1);
	memcpy(value, begin, len);
	value[len] = '\0';

	return value;
}

int	isHaveKey(const char *sub)
{
	if (!sub)
		return 0;
	char* tmp = strstr (sub, "Encryption key:");
	if (!tmp)
		return 0;

	if (tmp[strlen("Encryption key:")+2] == 'f')
		return 0;
	else
		return 1;
}

int get_all_ap(WIRELESS_AP **ap_list, const char* str)
{
	if (!str)
		return 0;
	WIRELESS_AP *tmp = NULL;
	tmp = *ap_list;
	char *sub = strstr (str, "Cell");

	while(sub)
	{
		char* essid = get_essid(sub);

		if (essid)
		{
			WIRELESS_AP * ap_tmp = (WIRELESS_AP *)malloc(sizeof(WIRELESS_AP));
			memset (ap_tmp, 0, sizeof(WIRELESS_AP));

			ap_tmp->essid = strdup(essid);
			ap_tmp->haveKey = isHaveKey(sub);

			if (!tmp)
				tmp = *ap_list = ap_tmp;
			else
				tmp->next = ap_tmp;
			tmp = ap_tmp;

			free(essid);
		}
		sub = strstr (sub + strlen("Cell"), "Cell");
	} 
	return 1;
}

#if 0
int main()
{
	WIRELESS_AP * ap_list = NULL;
	char * ap_str = "Cell 01 - Address: 00:15:E9:E1:A2:2F       "                                              "ESSID:\"feynman\""                                             
                    "Mode:Master"                                                 
                    "Channel:2"      
                    "Encryption key:on"                                           
                    "Quality:0/0  Signal level:17/255  Noise level:0/0   "        
          "Cell 02 - Address: 00:0A:EB:E2:4D:DE"                                  
                    "ESSID:\"WXXR-WIRELESS\""                                       
                    "Mode:Master"                                                 
                    "Channel:6"                                                   
                    "Encryption key:off\n"                                           
                    "Quality:0/0  Signal level:20/255  Noise level:0/0  Bit Rates:1 Mb/s; 2 Mb/s; 5.5 Mb/s; 11 Mb/s; 6 Mb/s";         

	if (!get_all_ap (&ap_list, ap_str))
		printf("no any ap\n");

	pinrtf_all_ap_node (ap_list);

	free_all_ap(ap_list);
	return 0;
}
#endif
