#include "udp_client_server.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <map>

#define BUFFLEN 4096
using namespace udp_client_server;

void extract_string_lookup(char* buffer, int size)
{
	char *buffer_copy = new char[size];
	char *buffer_1 = new char[size];
	std::string filename="GUNS.txt";
	std::ifstream f1(filename.c_str());
	strcpy(buffer_copy, buffer);
	std::string ip;
	std::string domain_nam;
	int flag = 0;
	std::map<std::string,std::string> m1;
	if(size != 0 && buffer != NULL)
	{

		//for (char *trav_buff = strtok(buffer_copy, " "); trav_buff != NULL; trav_buff = strtok(NULL, " "))
		//	std::cout<<"String received  "<<trav_buff<<std::endl;

		if(f1.is_open())
		{
			for(int i = 0; f1.getline(buffer_1,size);i++)
			{
				flag = 0;
				for (char *trav_buff = strtok(buffer_1, " "); trav_buff != NULL; trav_buff = strtok(NULL, " "))
				{
					if(!flag)
						domain_nam = trav_buff;
					else
						ip = trav_buff;
					flag++;

				}
				m1[domain_nam] = ip;
			}
		}
		
		std::cout<<"map ke ander "<<m1[std::string("www.google.com")]<<std::endl;

	}
	delete []buffer_copy;
	delete []buffer_1;
}

char * send_dns_ip()
{
	
	char *buffer = new char[BUFFLEN];
	char *buffer1 = new char[BUFFLEN];
	std::string filename="GUNS.txt";
	std::ifstream f1(filename.c_str());
	for(int i = 0; f1.getline(buffer, BUFFLEN);i++)
	{
		strcat(buffer1, " ");
		strcat(buffer1, buffer);
	}
	return buffer1;
}

int main()
{

	char buffer[BUFFLEN];
	char *buff = NULL;
	memset(buffer,'\0',BUFFLEN);
	int n=0;
 	struct sockaddr_in si_other;
 	socklen_t slen;
 	
	udp_server *u1=new udp_server("10.201.29.104",10000);
	while(1)
	{
		u1->myrecvfrom(buffer,BUFFLEN,&si_other,&slen);
		if(!strcmp(buffer,"JOIN"))
		{
			buff = send_dns_ip();
			u1->mysendto(buff,strlen(buff),&si_other,slen);
		}

        std::cout<<"Received "<<buffer<<std::endl;
      	u1->mysendto(buffer,strlen(buffer),&si_other,slen);
	}	
	extract_string_lookup(buffer, BUFFLEN);
}