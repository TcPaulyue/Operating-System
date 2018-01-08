#include "struct.h"
#include "read.h"
int route_item_index = 0;
ROUTE_INFO route_info[MAX_ROUTE_INFO];

ARP_TABLE arp_table[MAX_ARP_SIZE];
int arp_item_index = 0;

DEVICE device[MAX_DEVICE];
int device_index = 0;
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
		
//		printf("route_info[%d].destination[0]=%d\n",route_item_index,route_info[route_item_index].destination[0]);
//		printf("route_info[%d].destination[1]=%d\n",route_item_index,route_info[route_item_index].destination[1]);
//		printf("route_info[%d].destination[2]=%d\n",route_item_index,route_info[route_item_index].destination[2]);
//		printf("route_info[%d].destination[3]=%d\n",route_item_index,route_info[route_item_index].destination[3]);

//		printf("route_info[%d].gateway[0]=%d\n",route_item_index,route_info[route_item_index].gateway[0]);
//		printf("route_info[%d].gateway[1]=%d\n",route_item_index,route_info[route_item_index].gateway[1]);
//		printf("route_info[%d].gateway[2]=%d\n",route_item_index,route_info[route_item_index].gateway[2]);
//		printf("route_info[%d].gateway[3]=%d\n",route_item_index,route_info[route_item_index].gateway[3]);
		
//		printf("route_info[%d].netmask[0]=%d\n",route_item_index,route_info[route_item_index].netmask[0]);
//		printf("route_info[%d].netmask[1]=%d\n",route_item_index,route_info[route_item_index].netmask[1]);
//		printf("route_info[%d].netmask[2]=%d\n",route_item_index,route_info[route_item_index].netmask[2]);
//		printf("route_info[%d].netmask[3]=%d\n",route_item_index,route_info[route_item_index].netmask[3]);
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
		

//		printf("arp_table[%d].ip_addr[0]=%d\n",arp_item_index,arp_table[arp_item_index].ip_addr[0]);
//		printf("arp_table[%d].ip_addr[1]=%d\n",arp_item_index,arp_table[arp_item_index].ip_addr[1]);
//		printf("arp_table[%d].ip_addr[2]=%d\n",arp_item_index,arp_table[arp_item_index].ip_addr[2]);
//		printf("arp_table[%d].ip_addr[3]=%d\n",arp_item_index,arp_table[arp_item_index].ip_addr[3]);

//		printf("arp_table[%d].mac_addr[0]=%d\n",arp_item_index,arp_table[arp_item_index].mac_addr[0]);
//		printf("arp_table[%d].mac_addr[1]=%d\n",arp_item_index,arp_table[arp_item_index].mac_addr[1]);
//		printf("arp_table[%d].mac_addr[2]=%d\n",arp_item_index,arp_table[arp_item_index].mac_addr[2]);
//		printf("arp_table[%d].mac_addr[3]=%d\n",arp_item_index,arp_table[arp_item_index].mac_addr[3]);
//		printf("arp_table[%d].mac_addr[4]=%d\n",arp_item_index,arp_table[arp_item_index].mac_addr[4]);
//		printf("arp_table[%d].mac_addr[5]=%d\n",arp_item_index,arp_table[arp_item_index].mac_addr[5]);
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
