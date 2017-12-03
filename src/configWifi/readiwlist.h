#ifndef WIFI_READ_H 
#define WIFI_READ_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _wireless_ap
{
	char *essid;
	int haveKey;
	struct _wireless_ap *next;
} WIRELESS_AP;

int get_all_ap(WIRELESS_AP **ap_list, const char* str);
void free_all_ap(WIRELESS_AP * ap);

#endif
