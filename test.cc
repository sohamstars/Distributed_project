#include "udp_client_server.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <map>
#include <arpa/inet.h>
#include <cstdlib>
#include <algorithm>


#define BUFFLEN 4096
using namespace udp_client_server;

class Lookup
{
	public:
		Lookup();//std::string filename);
		char *extract_string_Lookup(char *buffer, int size);
		int populate_map_from_stream(int size);

	private:
		std::map<std::string,std::string> m1;
//		std::string filename;
		char *lookup_resp_buff;

};

Lookup::Lookup()//std::string filename)
{
//	this->filename = filename;
	this->lookup_resp_buff = new char[BUFFLEN];
}


char * changetodot(char * buffertochange)
{
	int k = 0;
	char * newbuff = new char[BUFFLEN];
	bzero(newbuff,BUFFLEN);
	for(int i = 0 ; i < strlen(buffertochange);i++)
	{
		newbuff[k] = buffertochange[i];
	//	std::cout<<newbuff<<std::endl;
		k++;
		if(buffertochange[i + 1] == '.')
		{
			strcat(newbuff, "dot");
		//	std::cout<<newbuff<<std::endl;
			k=k+3;
			i++;
		}
		
	}
	newbuff[k] = '\0';
	//std::cout<<"return string: "<<newbuff<<std::endl;
	return newbuff;
}

char * Lookup::extract_string_Lookup(char* buffer, int size)
{
	char * received_buffer_copy = new char[size];
	strcpy(received_buffer_copy, buffer);
	bool flag_lookup = 0;
	char *trav1_buff;
	
	if(size != 0 && buffer != NULL)
	{
		/* Seperating the URLs present in the request*/
		for (char *trav_buff = strtok(received_buffer_copy, " "); trav_buff != NULL; trav_buff = strtok(NULL, " "))
		{
			if(strcmp(trav_buff, "LOOKUP") && flag_lookup == 0)
				throw -1;		
			else if(!strcmp(trav_buff, "LOOKUP"))
			{
				flag_lookup = 1;
				continue;
			}
			trav1_buff = changetodot(trav_buff);
		//	std::cout<<"String received  "<<trav_buff<<std::endl;
			if(m1.find(trav1_buff) != m1.end())
			{
				strcat(lookup_resp_buff, trav_buff);
				strcat(lookup_resp_buff, " ");
				strcat(lookup_resp_buff, (m1.find(trav1_buff)->second).c_str());
				strcat(lookup_resp_buff, "\n");
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
	std::string filename = "sd.txt";
	std::ifstream f1(filename.c_str());
	char *ip = new char[BUFFLEN];
	char *domain_nam = new char[BUFFLEN];
	
	int position = 0;
	bool flag = 0;
	char *get_line_from_file = new char[BUFFLEN];
	system("sh multi_c.sh");
	if(f1.is_open())
	{
		for(int i = 0; f1.getline(get_line_from_file,size,',');i++)
		{
			//	std::cout<<" Line: "<<get_line_from_file<<std::endl;

			for (char *trav_buff = strtok(get_line_from_file, ","); trav_buff != NULL; trav_buff = strtok(NULL, ","))
			{
				//std::cout<<" string: "<<trav_buff<<std::endl;
				if(flag == 0)
				{
					std::string trv_buff = trav_buff;
					position = trv_buff.find("key");
					if(position >= 0)
					{
						position = trv_buff.find(":");

					//	std::cout<<"position of key "<<position<<std::endl;
						int k = 0;
						position = position + 3;
						while(trav_buff[position] != '"')
						{
							//std::cout<<trav_buff[position]<<std::endl;
							domain_nam[k] = trav_buff[position];
							position++;
							k++;
						}
						domain_nam[k] = '\0';
						std::cout<<"domain name "<<domain_nam<<std::endl;
					}
					flag = 1;
				}
				else if(flag == 1)
				{
					std::string trv1_buff = trav_buff;
					position = trv1_buff.find("data");
					if(position >= 0)
					{
						position = trv1_buff.find(":");
					//	std::cout<<"position of data "<<position<<std::endl;
						int k = 0;
						position = position + 3;
						while(trav_buff[position] != '"')
						{
							ip[k] = trav_buff[position];
							position++;
							k++;
						}
						ip[k] = '\0';
						std::cout<<"ip is "<<ip<<std::endl;
					}
					flag = 0;
				}

			}
			std::string doman_nam = domain_nam;
			std::string ip_nam = ip;
			m1[doman_nam] = ip_nam;
		}
		delete []get_line_from_file;
		std::cout<<"map ke ander "<<m1[std::string("wwwdotgoogledotcom")]<<std::endl;
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
	memset(buffer, '\0', BUFFLEN);
	memset(buffer1, '\0', BUFFLEN);
	if(f1.is_open())
	{
		for(int i = 0; f1.getline(buffer, BUFFLEN);i++)
		{
			strcat(buffer1, buffer);
			strcat(buffer1, " ");
		}
	}
	delete []buffer;
	return buffer1;
}


int main()
{

	char buffer[BUFFLEN];
	char buffer_to_send[BUFFLEN];
	char *lookup_resp;
	char *buff = NULL;
	memset(buffer,'\0',BUFFLEN);
	memset(buffer_to_send, '\0', BUFFLEN);
	int n=0;
 	struct sockaddr_in si_other;
 	socklen_t slen = sizeof(si_other);
 	size_t length;
 	
	udp_server *u1=new udp_server("10.0.0.188",10000);
	Lookup *l1 = new Lookup();//"GUNS.txt");
	l1->populate_map_from_stream(BUFFLEN);
	while(1)
	{
		u1->myrecvfrom(buffer,BUFFLEN,&si_other,&slen);
		std::cout<<"Received "<<buffer<<std::endl;
		// Join Message Response

		//std::cout<<"Received from"<<inet_ntoa(si_other.sin_addr)<<":"<<ntohs(si_other.sin_port)<<std::endl;
		if(!strcmp(buffer,"JOIN"))
		{
			buff = send_dns_ip_for_join();
			//std::cout<<"buffer sent "<<buffer_to_send<<" length "<<strlen(buffer_to_send)<<std::endl;
			if(u1->mysendto(buff,BUFFLEN,&si_other,slen)<0)
				perror("Error in send");
			delete []buff;
		}
		else
		{
			try {
				lookup_resp = l1->extract_string_Lookup(buffer, BUFFLEN);
				std::cout<<"Lookup request"<<std::endl;
				u1->mysendto(lookup_resp,BUFFLEN,&si_other,slen);
			}
			catch(int i)
			{
				std::cout<<"LOOKUP key word missing"<<std::endl;
			}
	        
	      	
	    }
	}

}	
	