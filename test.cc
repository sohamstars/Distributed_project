#include "udp_client_server.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <map>

#define BUFFLEN 4096
using namespace udp_client_server;

class Lookup
{
	public:
		Lookup(std::string filename);
		char *extract_string_Lookup(char *buffer, int size);
		int populate_map_from_stream(int size);

	private:
		std::map<std::string,std::string> m1;
		std::string filename;
		char *lookup_resp_buff;

};

Lookup::Lookup(std::string filename)
{
	this->filename = filename;
	this->lookup_resp_buff = new char[BUFFLEN];
}

char * Lookup::extract_string_Lookup(char* buffer, int size)
{
	char * received_buffer_copy = new char[size];
	strcpy(received_buffer_copy, buffer);
	bool flag_lookup = 0;
	
	if(size != 0 && buffer != NULL)
	{
		/* Seperating the URLs present in the request*/
		for (char *trav_buff = strtok(received_buffer_copy, " "); trav_buff != NULL; trav_buff = strtok(NULL, " "))
		{
			if(strcmp(trav_buff, "LOOKUP") && flag_lookup == 0)
				throw -1;		
			std::cout<<"String received  "<<trav_buff<<std::endl;
			if(m1.find(trav_buff) != m1.end())
			{
				strcat(lookup_resp_buff, (m1.find(trav_buff)->second).c_str());
				strcat(lookup_resp_buff, " ");
			}
			else
			{
				// check the secure copy of the global stream
			}
		}
		std::cout<<"Lookup result "<<lookup_resp_buff<<std::endl;
		delete []received_buffer_copy;
		return lookup_resp_buff;
	}
	delete []received_buffer_copy;
	return NULL;
}


int Lookup::populate_map_from_stream(int size)
{
		/* Building the map out of the stream text File */
		std::ifstream f1(filename.c_str());
		std::string ip;
		std::string domain_nam;
		bool flag = 0;
		char *get_line_from_file = new char[BUFFLEN];
		if(f1.is_open())
		{
			for(int i = 0; f1.getline(get_line_from_file,size);i++)
			{
				flag = 0;
				for (char *trav_buff = strtok(get_line_from_file, " "); trav_buff != NULL; trav_buff = strtok(NULL, " "))
				{
					if(!flag)
						domain_nam = trav_buff;
					else
						ip = trav_buff;
					flag++;

				}
				m1[domain_nam] = ip;
			}
			delete []get_line_from_file;
			std::cout<<"map ke ander "<<m1[std::string("www.google.com")]<<std::endl;
			return 0;
		}
		else
			return -1;
		
}

/* Join Response */
char * send_dns_ip_for_join()
{
	
	char *buffer = new char[BUFFLEN];
	char *buffer1 = new char[BUFFLEN];
	std::string filename="GUNS.txt";
	std::ifstream f1(filename.c_str());
	if(f1.is_open())
	{
		for(int i = 0; f1.getline(buffer, BUFFLEN);i++)
		{
			strcat(buffer1, buffer);
			strcat(buffer1, " ");
		}
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
 	
	udp_server *u1=new udp_server("10.201.20.51",10000);
	Lookup *l1 = new Lookup("GUNS.txt");
	while(1)
	{
		u1->myrecvfrom(buffer,BUFFLEN,&si_other,&slen);
		// Join Message Response
		if(!strcmp(buffer,"JOIN"))
		{
			buff = send_dns_ip_for_join();
			u1->mysendto(buff,strlen(buff),&si_other,slen);
		}
		else
		{
	        std::cout<<"Received "<<buffer<<std::endl;
	      	u1->mysendto(buffer,strlen(buffer),&si_other,slen);
	    }
	}	
	try {
		l1->extract_string_Lookup(buffer, BUFFLEN);
	}
	catch(int i)
	{
		std::cout<<"LOOKUP key word missing"<<std::endl;
	}

}