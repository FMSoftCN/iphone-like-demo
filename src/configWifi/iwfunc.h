
void get_ap_list();
char *get_ap_list_str();

void ap_set_ssid(char* ssid);

void ap_set_key(char* key);

//int check_network(const char* ip, const char* gw, const char* host, const char* dns);
void check_network(const char* ip);
int  get_check_network();

void if_using_dhcp();
void if_set_ip(char *ip);
void if_set_netmask(char *netmask);
void if_set_gateway(char *gateway);


void if_set_search(char *search);
void if_set_dns(char *dns);

//BOOL set_ap_key();
