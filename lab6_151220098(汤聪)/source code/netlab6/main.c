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
#include <net/ethernet.h>
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

void initRouteTable()
{
	FILE *fp=NULL;
	fp=fopen(routetable,"r");
	char read[256];
	while(fgets(read,256,fp)!=NULL)
	{
		char temp[20];
		int i = 0;
		int j = 0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];	
		}
		temp[j++]='\0';
		route_info[route_item_index].destination[0] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		route_info[route_item_index].destination[1] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		route_info[route_item_index].destination[2] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!=' ';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		route_info[route_item_index].destination[3] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];	
		}
		temp[j++]='\0';
		route_info[route_item_index].gateway[0] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		route_info[route_item_index].gateway[1] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		route_info[route_item_index].gateway[2] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!=' ';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		route_info[route_item_index].gateway[3] = atoi(temp);
		i++;
		j=0;

		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];	
		}
		temp[j++]='\0';
		route_info[route_item_index].netmask[0] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		route_info[route_item_index].netmask[1] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		route_info[route_item_index].netmask[2] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!=' ';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		route_info[route_item_index].netmask[3] = atoi(temp);
		i++;
		j=0;

		strcpy(&read[i],route_info[route_item_index].interface);
		route_item_index++;
	}
	fclose(fp);
}

void initArpTable()
{
	FILE *fp=NULL;
	fp=fopen(arptable,"r");
	char read[256];
	while(fgets(read,256,fp)!=NULL)
	{
		char temp[20];
		int i = 0;
		int j = 0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];	
		}
		temp[j++]='\0';
		arp_table[arp_item_index].ip_addr[0] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		arp_table[arp_item_index].ip_addr[1] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!='.';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		arp_table[arp_item_index].ip_addr[2] = atoi(temp);
		i++;
		j=0;
		for(;read[i]!=' ';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		arp_table[arp_item_index].ip_addr[3] = atoi(temp);
		i++;
		j=0;
		

		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];	
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&arp_table[arp_item_index].mac_addr[0]);
		i++;
		j=0;
		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&arp_table[arp_item_index].mac_addr[1]);
		i++;
		j=0;
		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&arp_table[arp_item_index].mac_addr[2]);
		i++;
		j=0;
		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&arp_table[arp_item_index].mac_addr[3]);
		i++;
		j=0;
		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&arp_table[arp_item_index].mac_addr[4]);
		i++;
		j=0;	
		for(;read[i]!='\0';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&arp_table[arp_item_index].mac_addr[5]);
		i++;
		j=0;
		arp_item_index++;
	}
	fclose(fp);
	
}
void initDeviceTable()
{
	FILE *fp=NULL;
	fp=fopen(devicetable,"r");
	char read[256];
	while(fgets(read,256,fp)!=NULL)
	{
		char temp[20];
		int i = 0;
		int j = 0;
		for(;read[i]!=' ';i++)
		{
			temp[j++]=read[i];
		}
		temp[j++]='\0';
		i++;
		j=0;
		strcpy(device[device_index].interface,temp);
		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];	
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&device[device_index].mac_addr[0]);
		i++;
		j=0;
		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&device[device_index].mac_addr[1]);
		i++;
		j=0;
		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&device[device_index].mac_addr[2]);
		i++;
		j=0;
		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&device[device_index].mac_addr[3]);
		i++;
		j=0;
		for(;read[i]!=':';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&device[device_index].mac_addr[4]);
		i++;
		j=0;	
		for(;read[i]!='\0';i++)
		{
			temp[j++] = read[i];
		}
		temp[j++]='\0';
		sscanf(temp,"%02X",(unsigned int*)&device[device_index].mac_addr[5]);
		i++;
		j=0;
		device_index++;
	}
}

#define IP_HSIZE sizeof(struct ip)
#define MAC_HSIZE sizeof(MAC_HEAD)
#define ICMP_HSIZE sizeof(struct icmp)
char buffer[4096];
int sockfd;
struct sockaddr_ll des_addr;
struct ifreq ifrq0;
struct ifreq ifrq1;
pid_t pid;
unsigned short checksum(unsigned char *buf,int len)
{
	unsigned int sum=0;
	unsigned short *buf1;
	buf1=(unsigned short *)buf;
	while(len>1)
	{
		sum+=*buf1++;
		len-=2;
	}
	if(len)
		sum+=*(unsigned char *)buf1;
	sum=(sum>>16)+(sum&0xffff);
	sum+=(sum>>16);
	return ~sum;
}
int compare_mac(char *mac1,char *mac2)
{
if((mac1[0]==mac2[0])&&(mac1[1]==mac2[1])&&(mac1[2]==mac2[2])&&(mac1[3]==mac2[3])
&&(mac1[4]==mac2[4])&&(mac1[5]==mac2[5]))
return 1;
else return 0;
}
int compare_ip(char *ip1,char *ip2)
{
if((ip1[0]==ip2[0])&&(ip1[1]==ip2[1])&&(ip1[2]==ip2[2])&&(ip1[3]==ip2[3]))
return 1;
else return 0;
}
int check_mac(char *mac)
{
int i;
for(i=0;i<=device_index;i++)
{
	if(compare_mac(mac,device[i].mac_addr)==0)
		return 0;
}
return 1;
}
int check_ip(char *ip)
{
int i;
for(i=0;i<device_index;i++)
{
	char *selfip;
	int j=0;
	for(;j<arp_item_index;i++)
	{
		if(compare_mac(arp_table[j].mac_addr,device[i].mac_addr)==1)
			selfip=arp_table[j].ip_addr;
	}
	if(compare_ip(selfip,ip)==1)
		return 1;
}
return 0;
}
void fillmac(char *destmac,char *sourcemac)
{
	MAC_HEAD *m=(MAC_HEAD *)buffer;
	int i;
	for(i=0;i<6;i++)
	{
		m->desmac[i]=destmac[i];
		m->sourcemac[i]=sourcemac[i];
		m->type=0x08;
	}
}
void fillip(char *destip)
{
	struct ip *ip_hdr;
	ip_hdr = (struct ip *)(buffer+MAC_HSIZE);
	ip_hdr->ip_hl = (IP_HSIZE>>2)+(4<<4);
	ip_hdr->ip_tos=0;
	ip_hdr->ip_len=IP_HSIZE+ICMP_HSIZE+56;
	ip_hdr->ip_id=0;
	//ip_hdr->flags = 0;
	ip_hdr->ip_off=0;
	ip_hdr->ip_p=IPPROTO_ICMP;
	ip_hdr->ip_ttl=255;
	//char destiptemp[16];
	//strcpy(destiptemp,destip);
	//ip_hdr->destip[0]=atoi(strtok(destiptemp,"."));
	//ip_hdr->destip[1]=atoi(strtok(NULL,"."));
	//ip_hdr->destip[2]=atoi(strtok(NULL,"."));
	//ip_hdr->destip[3]=atoi(strtok(NULL,"."));
	int i=0;
	for(;i<4;i++)
	ip_hdr->ip_dst[i]=destip[i];		
	unsigned char sourceip[4];
	//char *si = findip(device[0].mac_addr);
	//int i=0;
	for(;i<arp_item_index;i++)
	{
		if(compare_mac(arp_table[i].mac_addr,device[0].mac_addr)==1)
			{
				int i1=0;
				for(;i1<4;i1++)
					sourceip[i1]=arp_table[i1].ip_addr[i1];
			}
	}
	int j=0;	
	for(;j<4;j++)
		ip_hdr->ip_src[j]=sourceip[j];	
	//	sourceip[j]=si[j];
	//ip_hdr->sourceip[0] =atoi(strtok(sourceip,"."));
	//ip_hdr->sourceip[1] =atoi(strtok(NULL,"."));
	//ip_hdr->sourceip[2] =atoi(strtok(NULL,"."));
	//ip_hdr->sourceip[3] =atoi(strtok(NULL,"."));
	
}
int nsent=0;
void fillicmp()
{
	//icmp=(struct icmp *)sendpacket;
	struct icmp *icmp=(struct icmp *)(buffer+MAC_HSIZE+IP_HSIZE);	
	icmp->icmp_type=8;
	icmp->icmp_code=0;
	icmp->icmp_id=pid;
	icmp->icmp_seq=nsent++;
	memset(icmp->icmp_data,0xff,56);
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = checksum((unsigned char *)icmp,64);

}
char *findnextmac(char *ip)
{
	unsigned char temp[4];
	int i=0;
	for(;i<route_item_index;i++)
	{
		if(compare_ip(route_info[i].destination,ip)==1)
			{
				int j=0;
				for(;j<4;j++)
					temp[j]=route_info[j].gateway[j];
				break;			
			}
	}
	for(;i<arp_item_index;i++)
	{
		if(compare_ip(arp_table[i].ip_addr,temp)==1)
			return arp_table[i].mac_addr;
	}
}
char *findip(char *mac)
{

}
void forward(int ethindex, char *ip)
{
	MAC_HEAD *m=(MAC_HEAD*)buffer;
	char *nextmac=findnextmac(ip);
	if(nextmac==NULL)
		return;
	int i=0;
	for(;i<6;i++)
		m->desmac[i]=nextmac[i];
	if(ethindex==1)
	{
		strcpy(ifrq0.ifr_name,"eth1");
		ioctl(sockfd,SIOCGIFINDEX,&ifrq0);
		des_addr.sll_ifindex=ifrq0.ifr_ifindex;
		des_addr.sll_family=PF_PACKET;
		if((sendto(sockfd,buffer,MAC_HSIZE+IP_HSIZE+ICMP_HSIZE+56,0,(struct sockaddr *)&des_addr,sizeof(des_addr)))<0)
		{
			perror("resend data error");
		}
	}
	else
	{
		strcpy(ifrq0.ifr_name,"eth0");
		ioctl(sockfd,SIOCGIFINDEX,&ifrq0);
		des_addr.sll_ifindex = ifrq0.ifr_ifindex;
		des_addr.sll_family=PF_PACKET;
		if((sendto(sockfd,buffer,MAC_HSIZE+IP_HSIZE+ICMP_HSIZE+56,0,(struct sockaddr *)&des_addr,sizeof(des_addr)))<0)
		{
			perror("resend data error");
		}
	}
}
int main(int argc,char *argv[])
{
	pid=getpid();
	int state = -1;
	int nread;
	if(argc==2)
		state=1;
	else
		state=0;

	int size = 4096;
	if((sockfd=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP)))<0)
	{
		perror("create socket error");
		exit(1);
	}

	setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));
	initRouteTable();
	initArpTable();
	initDeviceTable();
	while(1)
	{
		if(state)//发送ping消息
		{
			fillmac(findnextmac(argv[1]),device[0].mac_addr);
			fillip(argv[1]);
			fillicmp();
			strcpy(ifrq0.ifr_name,"eth0");
			ioctl(sockfd,SIOCGIFINDEX,&ifrq0);
			des_addr.sll_ifindex = ifrq0.ifr_ifindex;
			des_addr.sll_family=PF_PACKET;
			if((sendto(sockfd,buffer,MAC_HSIZE+IP_HSIZE+ICMP_HSIZE+56,0,(struct sockaddr *)&des_addr,sizeof(des_addr)))<0)
			{
				perror("resend data error");
			}
			sleep(1);
		}
		else//接收或转发ping消息
		{
			nread=recvfrom(sockfd,buffer,sizeof(buffer),0,NULL,NULL);
			if(nread<42)
			{
				printf("error when recv msg\n");
				continue;
			}
			MAC_HEAD *m = (MAC_HEAD*)buffer;
			int ethindex = -1;
			if((ethindex=check_mac(m->desmac))!=0)//检查到mac是否是本机的mac
			{
				//IP_HEADER *ip_hdr = (IP_HEADER *)(buffer+MAC_HSIZE);
					struct ip *ip=(struct ip *)(buffer+MAC_HSIZE);				
					forward(ethindex,ip_hdr->ip_dst);
					printf("success\n");
			}
		}
	}
}
