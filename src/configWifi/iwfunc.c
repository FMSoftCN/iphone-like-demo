#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "iwfunc.h"

#define DNS_CFG "/etc/resolv.conf"
#define MAX_LIST_SIZE 4096

void free_ap_list(char* buf)
{
    if (buf)
        free(buf);
}

char * ap_list_buf = NULL;
void* start_get_ap_list_str();

char *get_ap_list_str()
{
	char * tmp = ap_list_buf;
	ap_list_buf = NULL;
	return tmp;
}
void get_ap_list()
{
	pthread_attr_t tattr;
	pthread_t tid;
	int ret;
	ret = pthread_attr_init(&tattr);
	pthread_attr_setstacksize (&tattr, 256 * 1024);
	/* default behavior specified*/
	ret = pthread_create(&tid, &tattr, start_get_ap_list_str, NULL);

}

void* start_get_ap_list_str()
{
    int fds[2];
	int rtn;
	pid_t _childpid;
	//fix: just for ipaq
	char * proc = "/sbin/iwlist";
	struct stat procstats;

    if (stat(proc, &procstats) == -1) {
		printf("not find the iwlist process\n");
		return NULL;
	}

    if (pipe(fds))
        return NULL;

    _childpid = fork();
    // childpid is -1, if the fork failed, so print out an error message
	if (_childpid == -1) {
		printf("fork failed. the iwlist will not using\n");
		return NULL;
	}

	// childpid is a positive integer, if we are the parent, and
	// fork() worked
	if (_childpid > 0) {
        char* buf;
        int len = 0;

        close(fds[1]); /* Close unused write end */
        buf = (char*)calloc(MAX_LIST_SIZE+1, 1);
        while ((read(fds[0], &buf[len], 1) > 0) && (len<MAX_LIST_SIZE))
           len++;

        wait(&rtn);

		fprintf(stderr,"\n\n %s \n\n", buf);
		ap_list_buf = buf;
		return buf;
	} else {
        close(1);
        dup2(fds[1], 1);
        close(fds[0]);
        system("iwlist scanning");
        _exit(EXIT_SUCCESS);
    }

    return NULL;
}

void ap_set_ssid(char* arg)
{
	if (!arg)
		return;
	printf("ssid %s \n", arg);
	char * exec = NULL;
	asprintf(&exec, "iwconfig wlan0 essid %s", arg);
	system(exec);
	free(exec);
}

void ap_set_key(char* arg)
{
	if (!arg)
		return;
	char * exec = NULL;
	asprintf(&exec, "iwconfig wlan0 key %s", arg);
	system(exec);
	free(exec);
}


int go_ping(const char *svrip)
{
	pid_t pid;
	if ((pid = vfork()) < 0) {
		printf("vfork error");
		return -1;
	} else if (pid == 0) {
		/*
		char * exec = NULL;
		asprintf(&exec, "ping -c 2 %s ", svrip);
		int ret = system(exec);
		free(exec);
*/
		if (execlp("ping", "ping", "-c", "3", svrip, (char*)0) < 0) {
//		if (ret < 0) {
			printf("execlp error\n");
			_exit(-1);
		}
		printf("execlp ok\n");
		_exit(EXIT_SUCCESS);
	} 
	//	else
	   	{
		int stat;
		waitpid(pid, &stat, 0);

		if (stat == 0)
			return 0;
	}

    return -1;
}

//-3 is unset
int check_net_res = -3;
char *ip = NULL;
int  get_check_network()
{
	int tmp = check_net_res;
	check_net_res = -3;
	return tmp;
}
/* 0:OK
 * -1:invalid parameter
 * -2:invalid local ip
 * -3:invalid gateway 
 * -4:invalid host 
 * -5:invalid dns
 * -6:unknow error
 */
void* exec_check_network()
{
	if (!ip)
	{
		check_net_res = -1;
		return -1;
	}
	printf("will exec check\n");

	printf("will exec check %s \n", ip);
	if (ip && go_ping(ip))
	{
		check_net_res = -2;
		return -2;
	}

	check_net_res = 0;
	return 0;
}

void check_network(const char* arg)
{
	if (!arg)
	{
		check_net_res = -1;
		return ;
	}

	{
		if (ip)
			free(ip);
		ip = (char*) malloc(sizeof(char)* strlen(arg)+1);
		memcpy(ip, arg, strlen(arg));
		ip[strlen(arg)] = '\0';
	}
	pthread_attr_t tattr;
	pthread_t tid;
	int ret;
	ret = pthread_attr_init(&tattr);
	pthread_attr_setstacksize (&tattr, 256 * 1024);
	/* default behavior specified*/
	ret = pthread_create(&tid, &tattr, exec_check_network, NULL);
	return ret;
}
#if 0
int check_network(const char* ip, const char* gw, const char* host, const char* dns)
{
    if (!(ip && gw && host && dns))
        return -1;

    if (ip && go_ping(ip))
        return -2;

    if (gw && go_ping(gw))
        return -3;
    
    if (host && go_ping(host))
        return -4;

    if (host && go_ping(dns))
        return -5;

    return 0;
}
#endif

void if_using_dhcp()
{
	system("killall udhcpc");
	system("ifconfig wlan0 dhcp");
	system("udhcpc -b -p /var/run/udhcpc.wlan0.pid -i wlan0");
}

void if_set_ip(char *ip)
{
	char * exec = NULL;
	asprintf(&exec, "ifconfig wlan0  %s", ip);
	system(exec);
	free(exec);
}

void if_set_netmask(char *netmask)
{
	char * exec = NULL;
	asprintf(&exec, "ifconfig wlan0 netmask %s", netmask);
	system(exec);
	free(exec);
}

void if_set_gateway(char *gateway)
{
	char * exec = NULL;
	asprintf(&exec, "route add default gw %s", gateway);
	system(exec);
	free(exec);
}

static int dns_append_line(const char* line, int len, int* exists);
void if_set_dns(char *dns)
{
	char *exec = NULL;
	asprintf(&exec, "nameserver %s", dns);
	dns_append_line(exec, strlen(exec), NULL);
	free(exec);
}

void if_set_search(char *arg)
{
	char *exec = NULL;
	asprintf(&exec, "search %s", arg);
	dns_append_line(exec, strlen(exec), NULL);
	free(exec);
}

static int dns_append_line(const char* line, int len, int* exists)
{
    int ret = 0;
    char* buf = NULL;
    FILE* fp = NULL;
    char* substr = NULL;
    struct stat st;
    char eol = '\n';

    if (!line || !len)
        return 0;

    if (stat(DNS_CFG, &st))
        return -1;

    if (!(buf = (char*)calloc(st.st_size + 1, 1)))
        return -2;

    if (!(fp = fopen(DNS_CFG, "r+"))) {
        ret = -3;
        goto EXIT;
    }

    fread(buf, st.st_size, 1, fp);
    substr = strcasestr(buf, line);
    if (substr) {
        ret = substr - buf;
        if (exists)
            *exists = 1;
        goto EXIT;
    }

    fseek(fp, 0, SEEK_END);
    fwrite(line, len, 1, fp);
    fwrite(&eol, sizeof(eol), 1, fp);

EXIT:
    if (fp)
        fclose(fp);
    if (buf)
        free(buf);
    return ret; 
}
