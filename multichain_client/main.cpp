#include "udp_client_server.h"
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <time.h>
#include <sys/time.h>
#include <iostream>

using namespace std;
using namespace udp_client_server;

#define DNS_Servers "DNS.txt"
#define Search_Entries "DNS_Responses.txt"
#define UserKeyFile "UserKey.txt"
#define MAXBUFSIZE 4096
#define NUM_ENTRIES 9
#define NUM_DNS_QUERIES	4
#define RECV_TIMEOUT_MS	1500
#define PUBLISH_RECV_TIMEOUT_MS	60000
#define NUM_SERVERS 6


void readServers(char *servIP, char *port);
void createlist(char *list);
udp_client* getServerIP(char *servIP, char *port);

int main(int argc,char *argv[])
{

	char PortNum[25];
	char serverIP[25];
	int done = 0;
	char buf[MAXBUFSIZE]="";
	udp_client *client = NULL;
	int res = 0;
	printf("argc is %d\n", (int)argc);
	if(strcmp(argv[1],"join")==0)
	{
		while(1)
		{
			bzero(serverIP, sizeof(serverIP));
			bzero(PortNum, sizeof(PortNum));
			bzero(buf, sizeof(buf));
			client = getServerIP(serverIP, PortNum);
			fflush(stdout);
			strcpy(buf,"PUBLISH:www.nba.com:98765432:");
			client->send(buf, (strlen(buf)));
			bzero(buf, sizeof(buf));
			if((res = (client->timed_recv(buf, MAXBUFSIZE, RECV_TIMEOUT_MS))) < 0)
			{
				printf("\r\nFailed Server");
				delete client;
			}
			else
			{
				printf("\r\nFound Server");
				done = 1;
				printf("\r\nResult:%s\n\n",buf);
				FILE *fp;
				fp = fopen(Search_Entries,"w+");
				fwrite("         \n",1,10,fp);
				int count = 0;
				while(1)
				{
					char url[100]="";
					char ip[100]="";
					char temp[200]="";
					sscanf(buf,"%s %s %[^\n]",url,ip,buf);
					count++;
					printf("url is %s\n", url);
					printf("ip is %s\n", ip);
					printf("buf is %s\n", buf);
					sprintf(temp,"%d. %s %s\n",count,url,ip);
					fwrite(temp,1,strlen(temp),fp);
					if(strncmp(buf,url,6)==0)
						break;
				}
				fseek ( fp , 0, SEEK_SET );
				char temp[10]="";
				sprintf(temp,"#%d#",count);
				fwrite(temp,1,strlen(temp),fp);
				fclose(fp);
				break;
			}		
		}
	}
	else if(strcmp(argv[1],"lookup")==0)
	{
		struct timeval t1,t2;
		while(1)
		{
			// if(argc<2)
			// {
			// 	printf("Usage main <url>\n");
			// 	exit(1);
			// }
			bzero(serverIP, sizeof(serverIP));
			bzero(PortNum, sizeof(PortNum));
			udp_client *client = getServerIP(serverIP, PortNum);
			char url[100]="";
			if(argc)
				strcpy(url,argv[argc]);
			bzero(buf, sizeof(buf));
			FILE *fp;
			fp = fopen(Search_Entries,"r+");
			if(fp == NULL)
			{
				printf("File %s open error\n", Search_Entries);
				exit(1);
			}
			fread(buf,1,MAXBUFSIZE,fp);
			char *t;
			if((t = strstr(buf,url)) != NULL)
			{
				char ip[100]="";
				sscanf(t,"%s %s", url,ip);
				printf("\nurl is %s and ip is %s\n", url,ip);
				exit(0);
			}
			fclose(fp);
			bzero(buf, sizeof(buf));
			strcpy(buf,"LOOKUP ");
			strcat(buf,url);
			strcat(buf," ");
			createlist(buf+strlen(buf));
			// strcpy(buf,"LOOKUP www.google.com www.cmu.edu www.restaurants.com www.colorado.edu www.yahoo.com");
			printf("\r\nLIST: %s", buf);
			// int i = 0;
			// for(;i<5;i++)
			// 	system("time -p dig +short www.yahoo.com");
			gettimeofday(&t1,NULL);
			client->send(buf, (strlen(buf)));
			bzero(buf, sizeof(buf));
			if((res = (client->timed_recv(buf, MAXBUFSIZE, RECV_TIMEOUT_MS))) < 0)
			{
				printf("\r\nFailed Server");
				delete client;
				bzero(serverIP, sizeof(serverIP));
				bzero(PortNum, sizeof(PortNum));
				udp_client *client = getServerIP(serverIP, PortNum);
				continue;
			}
			else
			{
				printf("\nbuf received is \n%s\n", buf);
				gettimeofday(&t2,NULL);
				char cnt[50]="";
				FILE *fp;
				fp = fopen(Search_Entries,"r+");
				if(fp == NULL)
				{
					printf("File %s open error\n", Search_Entries);
					exit(1);
				}
				fseek ( fp , 0, SEEK_SET );
				int count = 0;
				fscanf(fp,"%s",cnt);
				printf("\ncnt is %s\n", cnt);
				sscanf(cnt,"#%d#",&count);
				printf("\ncount is %d\n", count);
				char ip[100]="";
				char *temp;
				temp = strstr(buf, url);
				if(temp == NULL)
				{
					printf("\n\n\nCouldn't find %s!!!!!!!!!!!!!!!!!!!!\n\n\n", url);
					exit(1);
				}
				sscanf(temp,"%s %s %[^\n]",url,ip,buf);
				count++;
				fseek ( fp , 0, SEEK_SET );
				sprintf(cnt,"#%d#",count);
				fwrite(cnt,1,strlen(cnt),fp);
				printf("\nurl is %s\n", url);
				printf("\nip is %s\n", ip);
				printf("\nbuf is %s\n", buf);
				bzero(buf, sizeof(buf));
				sprintf(buf,"%d. %s %s\n",count,url,ip);
				fseek ( fp , 0, SEEK_END );
				fwrite(buf,1,strlen(buf),fp);
				fclose(fp);
				break;
			}
		}
		cout<<"\nTime for query is " << (t2.tv_sec - t1.tv_sec)<<"s "<< (t2.tv_usec - t1.tv_usec)<<"us\n";
	}
	else if(strcmp(argv[1],"publish")==0)
	{
		if(strcmp(argv[2],"add")==0)
		{
			while(1)
			{
				bzero(serverIP, sizeof(serverIP));
				bzero(PortNum, sizeof(PortNum));
				bzero(buf, sizeof(buf));
				client = getServerIP(serverIP, PortNum);
				fflush(stdout);
				sprintf(buf,"PUBLISH:ADD:%s:%s:",argv[3],argv[4]);
				printf("sending buf %s\n", buf);
				client->send(buf, (strlen(buf)));
				bzero(buf, sizeof(buf));
				if((res = (client->timed_recv(buf, MAXBUFSIZE, PUBLISH_RECV_TIMEOUT_MS))) < 0)
				{
					printf("\r\nFailed Server");
					delete client;
				}
				else
				{
					printf("\r\nFound Server");
					done = 1;
					printf("\r\nResult:%s\n\n",buf);
					sscanf(buf,"SUCCESS %s",buf);
					if(strcmp(buf,"DUPLICATE")==0)
					{
						printf("Duplicate Domain!!!\n\nBYEBYE!!!!\n");
						break;
					}
					FILE *fp;
					fp = fopen(UserKeyFile,"w");
					printf("\nbytes written is %d\n",(int)fwrite(buf,1,strlen(buf),fp));
					fclose(fp);
					break;
				}		
			}
		}
		else if(strcmp(argv[2],"append")==0)
		{
			while(1)
			{
				bzero(serverIP, sizeof(serverIP));
				bzero(PortNum, sizeof(PortNum));
				bzero(buf, sizeof(buf));
				client = getServerIP(serverIP, PortNum);
				fflush(stdout);
				char key[100]="";
				FILE *fp;
				fp = fopen(UserKeyFile,"r");
				fread(key,1,100,fp);
				sprintf(buf,"PUBLISH:APPEND:%s:%s:%s:",argv[3],argv[4],key);
				client->send(buf, (strlen(buf)));
				bzero(buf, sizeof(buf));
				if((res = (client->timed_recv(buf, MAXBUFSIZE, PUBLISH_RECV_TIMEOUT_MS))) < 0)
				{
					printf("\r\nFailed Server");
					delete client;
				}
				else
				{
					printf("\r\nFound Server");
					done = 1;
					printf("\r\nResult:%s\n\n",buf);
					sscanf(buf,"SUCCESS %s",buf);
					if(strcmp(buf,"BAD")==0)
					{
						printf("Bad Domain!!!\n\nBYEBYE!!!!\n");
						break;
					}
					FILE *fp;
					fp = fopen(UserKeyFile,"w");
					fwrite(buf,1,strlen(buf),fp);
					fclose(fp);
					break;
				}		
			}
		}
	}
	return 0;
}

void createlist(char *list){
	int count = 0;
	char search[25];
	char Buffer[MAXBUFSIZE];
	char URL[MAXBUFSIZE];
	FILE *Fp;
	Fp = fopen(Search_Entries,"r");
	char *ptr;
	int res;
	if((res = fread(Buffer, 1, MAXBUFSIZE, Fp)) < 0)
	{
		perror("Error");
		exit(1);
	}
	srand (time(NULL));
	while(count < NUM_DNS_QUERIES)
	{
		 
		int index = 1 + rand()%NUM_ENTRIES;
		printf("INDEX:%d\n",index);
		sprintf(search,"%d. ",index);
		ptr = strstr(Buffer,search);
		ptr = ptr + strlen(search);
		sscanf(ptr,"%s",URL);
		strcat(list,URL);
		strcat(list," ");
		count++;
	}

}


void readServers(char *servIP, char *port)
{
	ifstream file;
	file.open(DNS_Servers);
	char buffer[MAXBUFSIZE];
	char *ptr;
	char port1[25];
	char servipp[50];
	file.getline(buffer, MAXBUFSIZE);
	static int ServNum = 1;
	if((ptr = strstr(buffer, "Servers:")) == NULL)
	{
		printf("\r\n Error");
		exit(1);
	}
	int localServerCount = 1;
	while(!file.eof())
	{
		
		file.getline(buffer, MAXBUFSIZE);
	
		//sscanf(buffer,"%s %s", servIP, port);	
		if(localServerCount == ServNum)
		{
			//printf("\r\nBuffer:%s",buffer);			
			sscanf(buffer,"%s %s", servIP, port);
			//printf("\r\nServerIP:%s",servIP);
			//printf("\r\nPort:%s",port);
			ServNum++;
			break;
		}
		localServerCount++;
	}
	if(file.eof())
	{
#undef NUM_SERVERS
#define NUM_SERVERS localServerCount
	}
	if(ServNum > NUM_SERVERS)
		ServNum = 1;
	//printf("\r\njdksakdakd");	

}

udp_client* getServerIP(char *servIP, char *port)
{
	readServers(servIP, port);
	printf("\r\nIP:%s, Port:%d\n", servIP, atoi(port));
	string serv_IP(servIP);
	udp_client *client = new udp_client(serv_IP, atoi(port));
	return client;
}