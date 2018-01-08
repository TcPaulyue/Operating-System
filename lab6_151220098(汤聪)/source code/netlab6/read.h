#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/thernet.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>

struct icmp *icmp;
struct ip *ip;
#define MAX_ROUTE_INFO 100
#define MAX_ARP_SIZE 100
#define MAX_DEVICE 100
typedef struct route_item
{
	unsigned char destination[4];
	unsigned char gateway[4];
	unsigned char netmask[4];
	char interface[6];
}ROUTE_INFO;
ROUTE_INFO route_info[MAX_ROUTE_INFO];
int route_item_index = 0;

typedef struct arp_table_item
{
	unsigned char ip_addr[4];
	unsigned char mac_addr[6];
}ARP_TABLE;
ARP_TABLE arp_table[MAX_ARP_SIZE];
int arp_item_index = 0;

typedef struct device_item
{
	char interface[6];
	unsigned char mac_addr[6];
}DEVICE;
DEVICE device[MAX_DEVICE];
int device_index = 0;
typedef struct mac_head
{
	unsigned char desmac[6];
	unsigned char sourcemac[6];
	unsigned short type:16;
}MAC_HEAD;
#define routetable "route"
#define arptable "arp"
#define devicetable "device"
void initRouteTable();
void initArpTable();
void initDeviceTable();

