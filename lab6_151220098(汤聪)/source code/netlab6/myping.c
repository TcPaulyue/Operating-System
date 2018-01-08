#include "struct.h"
#include "read.h"
#define DEFAULT_LEN 56
#define BUFSIZE 1500
char sendbuf[BUFSIZE];
char recvbuf[BUFSIZE];
int nsend = 0;
pid_t pid;
void macheadfill(unsigned char *destmac,unsigned char *sourcemac)
{
	MAC_HEAD *m=(MAC_HEAD *)sendbuf;
	int i=0;
	for(;i<6;i++)
	{
		m->desmac[i]=destmac[i];
	}
	i=0;
	for(;i<6;i++)
	{
		m->sourcemac[i]=sourcemac[i];
	}
	m->type = 0x0008;
}
unsigned checksum(unsigned char *buf,int len)
{
	unsigned int sum=0;
	unsigned short *cbuf;
	cbuf = (unsigned short *)buf;
	while(len>1)
	{
		sum+=*cbuf++;
		len-=2;
	}
	if(len)
		sum+=*(unsigned char *)cbuf;
	sum = (sum>>16)+(sum&0xffff);
	sum+=(sum>>16);
	return ~sum;
}
void ipheadfill(unsigned char *destip,unsigned char *sourceip)
{
	IP_HEADER *ip_hdr;
	ip_hdr = (IP_HEADER *)(sendbuf+MAC_HSIZE);
	ip_hdr->ver_hlen = (sizeof(IP_HEADER)>>2)+(4<<4);
	ip_hdr->tos=0;
	ip_hdr->data_length = IP_HSIZE + ICMP_HSIZE + DEFAULT_LEN;
	ip_hdr->ident=0;
	ip_hdr->flags=0;
	ip_hdr->offset=0;
	ip_hdr->proto=IPPROTO_ICMP;
	ip_hdr->ttl = 255;
	ip_hdr->destip[0] = destip[0];
	ip_hdr->destip[1] = destip[1];
	ip_hdr->destip[2] = destip[2];
	ip_hdr->destip[3] = destip[3];
	ip_hdr->sourceip[0] = sourceip[0];
	ip_hdr->sourceip[1] = sourceip[1];
	ip_hdr->sourceip[2] = sourceip[2];
	ip_hdr->sourceip[3] = sourceip[3];
}
void icmpfill()
{
	ICMP_HEAD *icmp_hdr;
	icmp_hdr = (ICMP_HEAD *)(sendbuf+MAC_HSIZE+IP_HSIZE);
	icmp_hdr->type = 8;
	icmp_hdr->code = 0;
	icmp_hdr->icmp_id = pid;
	icmp_hdr->icmp_seq = nsend++;
	memset(icmp_hdr->data,0xff,DEFAULT_LEN);
	icmp_hdr->checksum = 0;
	icmp_hdr->checksum = checksum((unsigned char *)icmp_hdr,IP_HSIZE+ICMP_HSIZE+DEFAULT_LEN);
}

unsigned char *desmac(unsigned char *destip)
{
	unsigned char *gateway;
	for(int i=0;i<route_item_index;i++)
	{
		if(route_info[i].destination[0]==destip[0]&&route_info[i].destination[1]==destip[1]&&route_info[i].destination[2]==destip[2]&&route_info[i].destination[3]==destip[3])
		{
			gateway = route_info[i].gateway;
			break;
		}
	}
	for(int i=0;i<arp_item_index;i++)
	{
		if(arp_table[i].ip_addr[0]==gateway[0]&&arp_table[i].ip_addr[1]==gateway[1]&&arp_table[i].ip_addr[2]==gateway[2]&&arp_table[i].ip_addr[3]==gateway[3])
		{
			return arp_table[i].mac_addr;
		}
	}
}
struct sockaddr_in dest;
int main(int argc,char *argv[])
{
	pid=getpid();
	if(argc!=2)
	{
		printf("unknow ip\n");
		return -1;
	}
	else
	{
		char dest1[20];
		strcpy(dest1,argv[1]);
		unsigned char destip[4];
		destip[0] = atoi(strtok(dest1,"."));
		destip[1] = atoi(strtok(NULL,"."));
		destip[2] = atoi(strtok(NULL,"."));
		destip[3] = atoi(strtok(NULL,"."));
		initRouteTable();
		initArpTable();
		initDeviceTable();
		macheadfill(desmac(destip),device[0].mac_addr);
		ipheadfill(destip,arp_table[0].ip_addr);
		icmpfill();
		int sockfd;
		if((sockfd=socket(PF_PACKET,SOCK_RAW,IPPROTO_ICMP))<0)
		{
			printf("RAW socket created error");
			exit(1);
		}
		
		dest.sin_addr.s_addr = destip[0]+(destip[1]<<8)+(destip[2]<<16)+(destip[3]<<24);
		dest.sin_port = ntohs(0);
		dest.sin_family = PF_PACKET;
		sendto(sockfd,sendbuf,MAC_HSIZE+IP_HSIZE+ICMP_HSIZE+DEFAULT_LEN,0,(struct sockaddr *)&dest,sizeof(dest));
	}
	return 0;
}
