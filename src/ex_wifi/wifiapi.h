/*
 *Wireless Driver API
 *             
 *The Common API for Wireless
 *
 *Writed by dengkexi
 *Under the GPL licence
 */

#ifndef WI_API_H
#define WI_API_H

/* Start */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>		/* gethostbyname, getnetbyname */
//#include "route.h"
/* This is our header selection. Try to hide the mess and the misery :-(
 * Please choose only one of the define...
 */
/* Kernel headers 2.0.X + Glibc 2.0 - Debian 2.0, RH5
 * Kernel headers 2.2.X + Glibc 2.1 - Debian 2.2, RH6.1 */
#define GLIBC_HEADERS

/* Kernel headers 2.2.X + Glibc 2.0 - Debian 2.1 */
#undef KLUDGE_HEADERS

/* Kernel headers 2.0.X + libc5 - old systems */
#undef LIBC5_HEADERS

#ifdef KLUDGE_HEADERS
#include <socketbits.h>
#endif	/* KLUDGE_HEADERS */

#if defined(KLUDGE_HEADERS) || defined(GLIBC_HEADERS)
#include <linux/if_arp.h>	/* For ARPHRD_ETHER */
#include <linux/socket.h>	/* For AF_INET & struct sockaddr */
/* mos
#include <linux/in.h>		
*/
#endif	/* KLUDGE_HEADERS || GLIBC_HEADERS */

#ifdef LIBC5_HEADERS
#include <sys/socket.h>		/* For AF_INET & struct sockaddr & socket() */
#include <linux/if_arp.h>	/* For ARPHRD_ETHER */
#include <linux/in.h>		/* For struct sockaddr_in */
#endif	/* LIBC5_HEADERS */

/* Wireless extensions */
#include <linux/wireless.h>

#if WIRELESS_EXT < 8
#error "Wireless Extension v9 or newer required :-(\n\
Use Wireless Tools v19 or update your kernel headers"
#endif

/* Some usefull constants */
#define KILO	1e3
#define MEGA	1e6
#define GIGA	1e9

/* Shortcuts */
typedef struct iw_statistics	iwstats;
typedef struct iw_range		iwrange;
typedef struct iw_param		iwparam;
typedef struct iw_freq		iwfreq;
typedef struct iw_quality	iwqual;
typedef struct iw_priv_args	iwprivargs;
typedef struct sockaddr		sockaddr;

/* Structure for storing all wireless information for each device
 * This is a cut down version of the one above, containing only
 * the things *truly* needed to configure a card.
 * Don't add other junk, I'll remove it... */
typedef struct wireless_config
{
  char		name[IFNAMSIZ + 1];	/* Wireless/protocol name */
  int		has_nwid;
  iwparam	nwid;			/* Network ID */
  int		has_freq;
  double	freq;			/* Frequency/channel */
  int		freq_flags;
  int		has_key;
  unsigned char	key[IW_ENCODING_TOKEN_MAX];	/* Encoding key used */
  int		key_size;		/* Number of bytes */
  int		key_flags;		/* Various flags */
  int		has_essid;
  int		essid_on;
  char		essid[IW_ESSID_MAX_SIZE + 1];	/* ESSID (extended network) */
  int		has_mode;
  int		mode;			/* Operation mode */
} wireless_config;

/* Structure for storing all wireless information for each device
 * This is pretty exhaustive... */
typedef struct wireless_info
{
  char		name[IFNAMSIZ];		/* Wireless/protocol name */
  int		has_nwid;
  iwparam	nwid;			/* Network ID */
  int		has_freq;
  float		freq;			/* Frequency/channel */
  int		has_sens;
  iwparam	sens;			/* sensitivity */
  int		has_key;
  unsigned char	key[IW_ENCODING_TOKEN_MAX];	/* Encoding key used */
  int		key_size;		/* Number of bytes */
  int		key_flags;		/* Various flags */
  int		has_essid;
  int		essid_on;
  char		essid[IW_ESSID_MAX_SIZE + 1];	/* ESSID (extended network) */
  int		has_nickname;
  char		nickname[IW_ESSID_MAX_SIZE + 1]; /* NickName */
  int		has_ap_addr;
  sockaddr	ap_addr;		/* Access point address */
  int		has_bitrate;
  iwparam	bitrate;		/* Bit rate in bps */
  int		has_rts;
  iwparam	rts;			/* RTS threshold in bytes */
  int		has_frag;
  iwparam	frag;			/* Fragmentation threshold in bytes */
  int		has_mode;
  int		mode;			/* Operation mode */
  int		has_power;
  iwparam	power;			/* Power management parameters */

  /* Stats */
  iwstats	stats;
  int		has_stats;
  iwrange	range;
  int		has_range;
} wireless_info;

/* The function */
/* Open a socket */
int wifi_socket_open (void);
/* Close a socket */
int wifi_socket_close (void);
/* Get the number and name about network interface */
int wifi_get_network_interface_name (char* name, int* number);
/* Get the wifi interface name and number */
int wifi_get_wireless_interface_name (char* name, int* number);
/* Get the current interface ip */
int wifi_get_interface_ip (char* interface_name, char* ip);
/* Set the current interface ip */
int wifi_set_interface_ip (char* interface_name, char* ip);
/* Get the current interface mask */
int wifi_get_interface_mask (char* interface_name, char* mask);
/* Set the current interface mask */
int wifi_set_interface_mask (char* interface_name, char* mask);
/* Get the current interface essid */
int wifi_get_interface_essid (char* interface_name, char* essid);
/* Set the current interface essid */
int wifi_set_interface_essid (char* interface_name, char* essid);
/* Get the current interface key */
int wifi_get_interface_key (char* interface_name, char* key);
/* Set the current interface key */
int wifi_set_interface_key (char* interface_name, char* key);
/* Get the total wifi interface info */
int wifi_get_wireless_interface_info (char* interface_name, struct wireless_info* info);
/* Set DHCP */
void wifi_using_dhcp (char* interface_name);
/* Set the gateway, just for default ethernet*/
void wifi_set_gateway (char *gateway);
/* Add dns*/
void wifi_add_dns (char *dns);
/* Add search*/
void wifi_add_search (char *search);
/* Get dns */
int wifi_get_dns (char* name, int* number);
/* Get search */
int wifi_get_search (char* name, int* number);
/* Get defaulte interface name and gateway */
int wifi_get_default_interface_info (char* name, char* gateway);
/* Get wifi name and number from file */
int wifi_enum_devices (char* name, int* number);

#endif
