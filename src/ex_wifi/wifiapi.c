/*
 *Wireless Driver API
 *             
 *The Common API for Wireless
 *
 *Writed by dengkexi
 *Under the GPL licence
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "wifiapi.h"
#include "route.h"

#define DNS_CFG "/etc/resolv.conf"
//#define DNS_CFG "/home/dengkexi/resolv.conf"

static int wifi_socket_descriptor = -1;

static int dns_append_line(const char* line, int len, int* exists);
/*------------------------------------------------------------------*/
/*
 * Convert our internal representation of frequencies to a floating point.
 */
double
freq2float(iwfreq *	in)
{
  return ((double) in->m) * pow(10,in->e);
}

/*
 * Open a socket, and will talk with the driver.
 */
int wifi_socket_open (void)
{
   static const int families[] = {
     AF_INET, AF_IPX, AF_AX25, AF_APPLETALK
   };
   unsigned int i;
   int sock;

   for (i = 0; i < sizeof(families)/sizeof(int); ++i)
   {
       sock = socket (families[i], SOCK_DGRAM, 0);

       if (sock >= 0)
       {
           wifi_socket_descriptor = sock;
           return 0;
       }
   }
   return -1;
}

int wifi_socket_close (void)
{
   close (wifi_socket_descriptor);
   return 0;
}

int wifi_get_network_interface_name (char* name, int* number)
{
   struct ifconf ifc;
   struct ifreq* ifr;
   char buff[1024];
   int i;

   ifc.ifc_len = sizeof(buff);
   ifc.ifc_buf = buff;

   if (ioctl (wifi_socket_descriptor, SIOCGIFCONF, &ifc) < 0)
   {
       printf ("SIOCGIFCONF error!\n");
       return -1;
   }
   
   ifr = ifc.ifc_req;
   *number = 0;
   name[0] = '\0';

   for (i = ifc.ifc_len / sizeof (struct ifreq); --i >= 0; ifr++)
   {
       strcat (name, ifr->ifr_name);
       strcat (name, "\n");
       (*number)++;
   }
   return 0;
}

int wifi_get_wireless_interface_name (char* name, int* number)
{
   struct ifconf ifc;
   struct ifreq* ifr;
   char buff[1024];
   int i;

   ifc.ifc_len = sizeof(buff);
   ifc.ifc_buf = buff;

   if (ioctl (wifi_socket_descriptor, SIOCGIFCONF, &ifc) < 0)
   {
       printf ("WiFi SIOCGIFCONF error!\n");
       return -1;
   }
   
   ifr = ifc.ifc_req;
   *number = 0;
   name[0] = '\0';
   printf ("WiFi ifc.ifc_len / sizeof (struct ifreq) %ld\n", ifc.ifc_len / sizeof (struct ifreq));
   
   for (i = ifc.ifc_len / sizeof (struct ifreq); --i >= 0; ifr++)
   {
       struct iwreq wrq;
      
       printf ("ifr->ifr_name %s\n", ifr->ifr_name);
       strcpy (wrq.ifr_name, ifr->ifr_name);
       if (ioctl (wifi_socket_descriptor, SIOCGIWNAME, &wrq) < 0)
       continue; 
       strcat (name, ifr->ifr_name);
       strcat (name, "\n");
       (*number)++;
       return 0;
   }
   return -1;
}

int wifi_get_interface_ip (char* interface_name, char* ip)
{
   struct ifreq ifr;
   sockaddr my_addr;
   
   strcpy (ifr.ifr_name, interface_name);

   if (ioctl(wifi_socket_descriptor, SIOCGIFADDR, &ifr) < 0)
   {
        printf ("SIOCGIFADDR error\n");
        return -1;
   }
   memcpy(&my_addr, &ifr.ifr_addr, sizeof(my_addr));
   memcpy (ip, &my_addr.sa_data[2], 4);

   return 0;
}

int wifi_set_interface_ip (char* interface_name, char* ip)
{
   struct ifreq ifr;
   sockaddr my_addr;
   
   strcpy (ifr.ifr_name, interface_name);

   if (ioctl(wifi_socket_descriptor, SIOCGIFADDR, &ifr) < 0)
   {
        printf ("SIOCGIFADDR error\n");
        return -1;
   }
   memcpy (&my_addr, &ifr.ifr_addr, sizeof(my_addr));
   memcpy (&my_addr.sa_data[2], ip, 4);
   memcpy (&ifr.ifr_addr, &my_addr, sizeof(my_addr));
   if (ioctl(wifi_socket_descriptor, SIOCSIFADDR, &ifr) < 0)  
   {  
       printf ("SIOCSIFADDR error!\n");  
       return -1;  
   } 
   
   ifr.ifr_flags |= IFF_UP | IFF_RUNNING;  
   if (ioctl(wifi_socket_descriptor, SIOCSIFFLAGS, &ifr) < 0)    
   {  
       printf ("SIOCSIFFLAGS error!\n");  
       return -1;  
   } 
   return 0;
}

int wifi_get_interface_mask (char* interface_name, char* mask)
{
   struct ifreq ifr;
   sockaddr my_addr;
   
   strcpy (ifr.ifr_name, interface_name);

   if (ioctl(wifi_socket_descriptor, SIOCGIFNETMASK, &ifr) < 0)
   {
        printf ("SIOCGIFNETMASK error!\n");
        return -1;
   }
   
   memcpy (&my_addr, &ifr.ifr_netmask, sizeof(my_addr));
   memcpy (mask, &my_addr.sa_data[2], 4);
   return 0;
}

int wifi_set_interface_mask (char* interface_name, char* mask)
{
   struct ifreq ifr;
   sockaddr my_addr;
   
   strcpy (ifr.ifr_name, interface_name);

   if (ioctl(wifi_socket_descriptor, SIOCGIFNETMASK, &ifr) < 0)
   {
        printf ("SIOCGIFNETMASK error!\n");
        return -1;
   }
   
   memcpy (&my_addr, &ifr.ifr_addr, sizeof(my_addr));
   memcpy (&my_addr.sa_data[2], mask, 4);
   memcpy (&ifr.ifr_netmask, &my_addr, sizeof(my_addr));
   
   if (ioctl(wifi_socket_descriptor, SIOCSIFNETMASK, &ifr) < 0)
   {
        printf ("SIOCSIFNETMASK error!\n");
        return -1;
   }
#if 1 
   ifr.ifr_flags |= IFF_UP | IFF_RUNNING;  
   if (ioctl(wifi_socket_descriptor, SIOCSIFFLAGS, &ifr) < 0)    
   {  
       printf ("SIOCSIFFLAGS error!\n");  
       return -1;  
   } 
#endif
   return 0;
}

int wifi_get_interface_essid (char* interface_name, char* essid)
{
   struct iwreq wrq;

   //Get ESSID
   strcpy (wrq.ifr_name, interface_name);
   //wrq.u.essid.pointer = (caddr_t) info.essid;
   wrq.u.essid.pointer = (caddr_t) essid;
   wrq.u.essid.length = IW_ESSID_MAX_SIZE;
   wrq.u.essid.flags = 0;
   if(ioctl(wifi_socket_descriptor, SIOCGIWESSID, &wrq) < 0)
   {
       printf ("SIOCGIWESSID error!\n");
       return -1;
   }
   return 0;
}

int wifi_set_interface_essid (char* interface_name, char* essid)
{
   struct iwreq wrq;

   //Get ESSID
   strcpy (wrq.ifr_name, interface_name);
   //wrq.u.essid.pointer = (caddr_t) info.essid;
   wrq.u.essid.pointer = (caddr_t) essid;
   wrq.u.essid.length = strlen(essid);
   wrq.u.essid.flags = 0;
   if(ioctl(wifi_socket_descriptor, SIOCSIWESSID, &wrq) < 0)
   {
       printf ("SIOCSIWESSID error!\n");
       return -1;
   }
   return 0;
}

int wifi_get_interface_key (char* interface_name, char* key)
{
   struct iwreq wrq;

   //Get key 
   strcpy (wrq.ifr_name, interface_name);
   //wrq.u.essid.pointer = (caddr_t) info.essid;
   wrq.u.data.pointer = (caddr_t) key;
   wrq.u.data.length = IW_ENCODING_TOKEN_MAX;
   wrq.u.data.flags = 0;
   if(ioctl(wifi_socket_descriptor, SIOCGIWENCODE, &wrq) < 0)
   {
       printf ("SIOCGIWENCODE error!\n");
       return -1;
   }
   return 0;
}

int wifi_set_interface_key (char* interface_name, char* key)
{
   struct iwreq wrq;

   //Set key 
   strcpy (wrq.ifr_name, interface_name);
   //wrq.u.essid.pointer = (caddr_t) info.essid;
   wrq.u.data.pointer = (caddr_t) key;
   wrq.u.data.length = strlen (key);
   wrq.u.data.flags = 0;
   if(ioctl(wifi_socket_descriptor, SIOCGIWENCODE, &wrq) < 0)
   {
       printf ("SIOCGIWENCODE error!\n");
       return -1;
   }
   return 0;
}

int wifi_get_wireless_interface_info (char* interface_name, struct wireless_info* info)
{
   struct iwreq wrq;

   memset ((char*)info, 0, sizeof(struct wireless_info));

   //Get name
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWNAME, &wrq) < 0)
   {
       printf ("SIOCGIWNAME error!\n");
   }
   else
   {
       strcpy (info->name, wrq.u.name);
   }

   //Get network ID
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWNWID, &wrq) >= 0)
   {
       info->has_nwid = 1;
       memcpy(&(info->nwid), &(wrq.u.nwid), sizeof(iwparam));
   }
   
   //Get frequency/channel
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWFREQ, &wrq) >= 0)
   {
       info->has_freq = 1;
       info->freq = freq2float(&(wrq.u.freq));
   }

   //Get sensitivity
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWSENS, &wrq) >= 0)
   {
       info->has_sens = 1;
       memcpy(&(info->sens), &(wrq.u.sens), sizeof(iwparam));
   }

   //Get encryption information
   strcpy (wrq.ifr_name, interface_name);
   wrq.u.data.pointer = (caddr_t) info->key;
   wrq.u.data.length = 0;
   wrq.u.data.flags = 0;
   if(ioctl(wifi_socket_descriptor, SIOCGIWENCODE, &wrq) >= 0)
   {
       info->has_key = 1;
       info->key_size = wrq.u.data.length;
       info->key_flags = wrq.u.data.flags;
   }

   //Get ESSID
   strcpy (wrq.ifr_name, interface_name);
   wrq.u.essid.pointer = (caddr_t) info->essid;
   wrq.u.essid.length = 0;
   wrq.u.essid.flags = 0;
   if(ioctl(wifi_socket_descriptor, SIOCGIWESSID, &wrq) >= 0)
   {
       info->has_essid = 1;
       info->essid_on = wrq.u.data.flags;
   }
  
   //Get AP address
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWAP, &wrq) >= 0)
   {
       info->has_ap_addr = 1;
       memcpy(&(info->ap_addr), &(wrq.u.ap_addr), sizeof (sockaddr));
   }

   //Get NickName
   strcpy (wrq.ifr_name, interface_name);
   wrq.u.essid.pointer = (caddr_t) info->nickname;
   wrq.u.essid.length = 0;
   wrq.u.essid.flags = 0;
   if(ioctl(wifi_socket_descriptor, SIOCGIWNICKN, &wrq) >= 0)
       if(wrq.u.data.length > 1)
           info->has_nickname = 1;

   //Get bit rate
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWRATE, &wrq) >= 0)
   {
       info->has_bitrate = 1;
       memcpy(&(info->bitrate), &(wrq.u.bitrate), sizeof(iwparam));
   }

   //Get RTS threshold 
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWRTS, &wrq) >= 0)
   {
       info->has_rts = 1;
       memcpy(&(info->rts), &(wrq.u.rts), sizeof(iwparam));
   }

   //Get fragmentation threshold	
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWFRAG, &wrq) >= 0)
   {
       info->has_frag = 1;
       memcpy(&(info->frag), &(wrq.u.frag), sizeof(iwparam));
   }
		
   //Get operation mode
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWMODE, &wrq) >= 0)
   {
       if((wrq.u.mode < 6) && (wrq.u.mode >= 0))
           info->has_mode = 1;
       info->mode = wrq.u.mode;
   }

   //Get Power Management settings
   strcpy (wrq.ifr_name, interface_name);
   if(ioctl(wifi_socket_descriptor, SIOCGIWPOWER, &wrq) >= 0)
   {
       info->has_power = 1;
       memcpy(&(info->power), &(wrq.u.power), sizeof(iwparam));
   }
		
   return 0;
}

void wifi_using_dhcp (char* interface_name)
{
/*
   char cmd[256];
   memset (cmd, 0x0, 256);
   system ("killall udhcpc");
   sprintf (cmd, "ifconfig %s dhcp", interface_name);
   //system ("ifconfig wlan0 dhcp");
   system (cmd);
   memset (cmd, 0x0, 256);
   sprintf (cmd, "udhcpc -b -p /var/run/udhcpc.%s.pid -i %s", interface_name, interface_name);
   //system ("udhcpc -b -p /var/run/udhcpc.wlan0.pid -i wlan0");
   system (cmd);
*/
}

void wifi_set_gateway(char *gateway)
{
/*
   char * exec = NULL;
   asprintf(&exec, "route add default gw %s", gateway);
   system(exec);
   free(exec);
*/
}

void wifi_add_dns(char *dns)
{
   char *exec = NULL;
   if (asprintf(&exec, "nameserver %s", dns) < 0)
        return;
   dns_append_line(exec, strlen(exec), NULL);
   free(exec);
}

void wifi_set_search(char *search)
{
   char *exec = NULL;
   if (asprintf(&exec, "search %s", search) < 0)
        return;

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
        printf ("File permission denied!\n");
        goto EXIT;
    }

    if (fread(buf, st.st_size, 1, fp) < 1) {
        ret = -4;
        printf ("read error!\n");
        goto EXIT;
    }

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

int wifi_get_string_search (char* string, char *name, int* number)
{
    int ret = 0;
    char* buf = NULL;
    FILE* fp = NULL;
    struct stat st;
    char* pstr = NULL;
    char* pmove = NULL;
    char* penter = NULL;

    if (!string || !name)
        return 0;

    if (stat(DNS_CFG, &st))
        return -1;

    if (!(buf = (char*)calloc(st.st_size + 1, 1)))
        return -2;

    if (!(fp = fopen(DNS_CFG, "r+"))) {
        ret = -3;
        printf ("File permission denied!\n");
        goto EXIT;
    }

    if (fread(buf, st.st_size, 1, fp) < 1) {
        ret = -4;
        printf ("read error!\n");
        goto EXIT;
    }

    pmove = buf;

    while (pmove)
    {
        pstr = strstr (pmove, string);
        if (pstr == NULL)
          break;
        
        if (pstr)
        {
            pstr = pstr + strlen(string);
            penter = strstr (pstr, "\n"); 
       
            strncpy (name, pstr, penter - pstr);
            name = name + (penter - pstr);
            strcat (name, "\n");
            name++;
            (*number)++;
        }
        pmove = pstr;
    }
    
EXIT:
    if (fp)
        fclose(fp);
    if (buf)
        free(buf);
    return ret; 
    return 0;
}

int wifi_get_dns (char* name, int* number)
{
   char string[64];
   
   memset (string, 0x0, 64);
   strcpy (string, "nameserver ");
   return wifi_get_string_search (string, name, number);
}

int wifi_get_search (char* name, int* number)
{
   char string[64];
   
   memset (string, 0x0, 64);
   strcpy (string, "search ");
   return wifi_get_string_search (string, name, number);
}

int wifi_get_default_interface_info (char* name, char* gateway)
{
   return route_get_default_gateway (name, gateway);
}

static char *
wifi_get_ifname(char *	name,	/* Where to store the name */
	      int	nsize,	/* Size of name buffer */
	      char *	buf)	/* Current position in buffer */
{
  char *	end;

  /* Skip leading spaces */
  while(isspace(*buf))
    buf++;

  /* Get name up to the last ':'. Aliases may contain ':' in them,
   * but the last one should be the separator */
  end = strrchr(buf, ':');
#if 0
  /* Get name up to ": "
   * Note : we compare to ": " to make sure to process aliased interfaces
   * properly. Doesn't work on /proc/net/dev, because it doesn't guarantee
   * a ' ' after the ':'*/
  end = strstr(buf, ": ");
#endif

  if((end == NULL) || (((end - buf) + 1) > nsize))
    return(NULL);

  /* Copy */
  memcpy(name, buf, (end - buf));
  name[end - buf] = '\n';

  /* Return value currently unused, just make sure it's non-NULL */
  return(end);
}


#define PROC_NET_WIRELESS "/proc/net/wireless"
int wifi_enum_devices (char* name, int* number)
{
    char	buff[1024];
    FILE *	fh;
    
    fh = fopen(PROC_NET_WIRELESS, "r");

    if (fh != NULL) {

        if (fgets(buff, sizeof(buff), fh) == NULL) {
            fclose (fh);
            return -1;
        }
        if (fgets(buff, sizeof(buff), fh) == NULL) {
            fclose (fh);
            return -1;
        }

      /* Read each device line */
        while(fgets(buff, sizeof(buff), fh)) {
	        //char	name[IFNAMSIZ + 1];
	        char *s;

	        /* Skip empty or almost empty lines. It seems that in some
	         * cases fgets return a line with only a newline. */
	        if((buff[0] == '\0') || (buff[1] == '\0'))
	            continue;

	        /* Extract interface name */
	        s = wifi_get_ifname(name, IFNAMSIZ + 1, buff);

	        if(!s) {
	            fprintf(stderr, "Cannot parse " PROC_NET_WIRELESS "\n");
	        }
            else {
                while ((*name) != '\0') {
                    name ++;
                }
                name ++;
                (*number) ++;
            }
            
        }
        fclose (fh);
        return 0;
    }
    
    return -1;
}

#if 0 
int main (int argc, char** argv)
{
   char name[256];
   char gateway[256];
   int number = 0;

   memset (name, 0x0, 256);
   wifi_get_default_interface_info (name, gateway);
   
   printf ("name %s\n", name);
   printf ("gateway %s\n", gateway);
#if 0
   wifi_get_search (name, &number);
   //wifi_get_string_search (string, name, &number);
   printf ("search number is %d\n", number);
   printf ("search name:%s\n", name);
   
   memset (name, 0x0, 256);
   number = 0;
   
   wifi_get_dns (name, &number);
   printf ("dns number is %d\n", number);
   printf ("dns name:%s\n", name);
#endif
   return 0;
}
#endif
