#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/in.h>
typedef struct iphdr
{
	unsigned char ver_hlen;
	unsigned char tos;
	unsigned short data_length;
	unsigned short ident;
	unsigned int flags:3;
	unsigned int offset:13;
	unsigned char ttl;
	unsigned char proto;
	unsigned short checksum;
	unsigned char sourceip[4];
	unsigned char destip[4];
}IP_HEADER;
#define IP_HSIZE sizeof(IP_HEADER)
typedef struct machead
{
	unsigned char desmac[6];
	unsigned char sourcemac[6];
	unsigned int type:16;
}MAC_HEAD;
#define MAC_HSIZE sizeof(MAC_HEAD)
typedef struct icmpdata
{
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	union
	{
		struct
		{
			unsigned short id;
			unsigned short sequence;
		}echo;
		unsigned int gateway;
		struct
		{
			unsigned short unsed;
			unsigned short mtu;
		}frag;
	}un;
	unsigned char data[0];
	#define icmp_id un.echo.id
	#define icmp_seq un.echo.sequence
}ICMP_HEAD;
#define ICMP_HSIZE sizeof(ICMP_HEAD)
