#include "udp_client_server.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <map>
#include <arpa/inet.h>
#include <cstdlib>
#include <algorithm>
#include <sys/time.h>

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
	char received_buffer_copy[size];
	strcpy(received_buffer_copy, buffer);
	bool flag_lookup = 0;
	
	struct timeval tv;
	struct timeval tv1;
	char *ip = new char[BUFFLEN];
	char *domain_nam = new char[BUFFLEN];
	struct timeval tv2;
	struct timeval tv3;
	int position_lkp = 0;
	char *trav1_buff;
	bool flag_1 = 0;
	bool return_f;
	char *command = new char[100];
	char *get_line_from_file = new char[BUFFLEN];
	std::string filename = "sd1.txt";
	std::ifstream f2(filename.c_str());
	memset(lookup_resp_buff,'\0',sizeof(lookup_resp_buff));

	if(size != 0 && buffer != NULL)
	{
		//std::cout<<"received_buffer_copy"<<received_buffer_copy<<std::endl;
		/* Seperating the URLs present in the request*/
		for (trav1_buff = strtok(received_buffer_copy, " "); trav1_buff != NULL; trav1_buff = strtok(NULL, " "))
		{
		//	std::cout<<"received_buffer_copy in loop"<<received_buffer_copy<<" "<<trav1_buff<<std::endl;
			if(strcmp(trav1_buff, "LOOKUP") && flag_lookup == 0)
				throw -1;		
			else if(!strcmp(trav1_buff, "LOOKUP"))
			{
				flag_lookup = 1;
				continue;
			}
		//	std::cout<<"Buffer to search "<<trav1_buff<<std::endl;
		//	trav1_buff = changetodot(trav_buff);
		//	std::cout<<"String received  "<<trav_buff<<std::endl;
			if(m1.find(trav1_buff) != m1.end())
			{
				strcat(lookup_resp_buff, trav1_buff);
				strcat(lookup_resp_buff, " ");
				gettimeofday (&tv, NULL);
				
				strcat(lookup_resp_buff, (m1.find(trav1_buff)->second).c_str());
				gettimeofday (&tv1, NULL);
			//	std::cout<<"time before lookup "<<tv.tv_sec<<"s "<<tv.tv_usec<<"us "<<std::endl;
			//	std::cout<<"time after lookup "<<tv1.tv_sec<<"s "<<tv1.tv_usec<<"us "<<std::endl;
		//		std::cout<<"Buffer found in hash. Time for lookup "<<(tv1.tv_sec - tv.tv_sec)<<"s "<<(tv1.tv_usec - tv.tv_usec)<<"us "<<std::endl;
				strcat(lookup_resp_buff, "\n");
			}
			else
			{

			//	std::cout<<"time to search in global stream"<<std::endl;
				sprintf(command, "sh global_str.sh %s",trav1_buff);
				system(command);
				if(return_f = f2.is_open())
				{
					for(int j = 0; f2.getline(get_line_from_file,size,',') ;j++)
					{
						//	std::cout<<" Line: "<<get_line_from_file<<std::endl;

							if(flag_1 == 0)
							{
								std::string trv_buf = get_line_from_file;
								position_lkp = trv_buf.find("key");
								if(position_lkp == SIZE_MAX)
									continue;
								if(position_lkp >= 0)
								{
									position_lkp = trv_buf.find(":");

								//	std::cout<<"position of key "<<position<<std::endl;
									int k = 0;
									position_lkp = position_lkp + 3;
									while(get_line_from_file[position_lkp] != '"')
									{
										//std::cout<<trav_buff[position]<<std::endl;
										domain_nam[k] = get_line_from_file[position_lkp];
										position_lkp++;
										k++;
									}
									domain_nam[k] = '\0';
							//		std::cout<<"domain name "<<domain_nam<<std::endl;
								}
								flag_1 = 1;
							}
							else if(flag_1 == 1)
							{
								std::string trv1_buf = get_line_from_file;
								position_lkp = trv1_buf.find("data");
								if(position_lkp >= 0)
								{
									position_lkp = trv1_buf.find(":");
								//	std::cout<<"position of data "<<position<<std::endl;
									int k = 0;
									position_lkp = position_lkp + 3;
									while(get_line_from_file[position_lkp] != '"')
									{
										ip[k] = get_line_from_file[position_lkp];
										position_lkp++;
										k++;
									}
									ip[k] = '\0';
							//		std::cout<<"ip is "<<ip<<std::endl;
								}
								flag_1 = 0;
							}

						
					
				}		// get line
				f2.close();
			//	std::cout<<"no fault"<<std::endl;
				std::string doman_nam = domain_nam;
				std::string ip_nam = ip;
				
				m1[doman_nam] = ip_nam;
			//	std::cout<<"fault"<<std::endl;
				strcat(lookup_resp_buff, domain_nam);
				strcat(lookup_resp_buff, " ");
				gettimeofday (&tv, NULL);						
				if(m1.find(trav1_buff)!=m1.end())
				strcat(lookup_resp_buff, (m1.find(trav1_buff)->second).c_str());
				gettimeofday (&tv1, NULL);
			//	std::cout<<"time before lookup "<<tv.tv_sec<<"s "<<tv.tv_usec<<"us "<<std::endl;
			//	std::cout<<"time after lookup "<<tv1.tv_sec<<"s "<<tv1.tv_usec<<"us "<<std::endl;
			//	std::cout<<"Time for lookup "<<(tv1.tv_sec - tv.tv_sec)<<"s "<<(tv1.tv_usec - tv.tv_usec)<<"us "<<std::endl;
			//	std::cout<<"Buffer added to hash "<<trav1_buff<<std::endl;
				strcat(lookup_resp_buff, "\n");
			//	std::cout<<"received_buffer_copy end loop"<<received_buffer_copy<<std::endl;
				
			} // end of f2
			else
			{
				std::cout<<trav1_buff<<" "<<"Domain name missing in global stream"<<std::endl;
			}
		//	std::cout<<"end of global"<<std::endl;
			}

		}
	
		std::cout<<"Lookup result "<<lookup_resp_buff<<std::endl;
		//delete []received_buffer_copy;
		return lookup_resp_buff;
	}
	else
	{
		//delete []received_buffer_copy;
		return NULL;
	}
}



int Lookup::populate_map_from_stream(int size)
{
	/* Building the map out of the stream text File */
	std::string filename = "sd.txt";
	std::ifstream f1(filename.c_str());
	char *ip = new char[BUFFLEN];
	char *domain_nam = new char[BUFFLEN];
	struct timeval tv2;
	struct timeval tv3;
	int position = 0;
	bool flag = 0;
	char *get_line_from_file = new char[BUFFLEN];
	gettimeofday (&tv2, NULL);
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
				//		std::cout<<"domain name "<<domain_nam<<std::endl;
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
				//		std::cout<<"ip is "<<ip<<std::endl;
					}
					flag = 0;
				}

			}
			std::string doman_nam = domain_nam;
			std::string ip_nam = ip;
			m1[doman_nam] = ip_nam;
		}
		gettimeofday (&tv3, NULL);
		std::cout<<"Time for populating "<<(tv3.tv_sec - tv2.tv_sec)<<"s "<<(tv3.tv_usec - tv2.tv_usec)<<"us "<<std::endl;
		delete []get_line_from_file;
		f1.close();
		//std::cout<<"map ke ander "<<m1[std::string("wwwdotgoogledotcom")]<<std::endl;
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
		f1.close();
	}
	
	delete []buffer;
	return buffer1;
}

void send_dns_key_for_publish(char *domain_name, char *ip)
{
	char command[1024];
	char exec_command[1024];
	char exec_command_local[1024];
	bool return_f;

	sprintf(command, "sh global_str.sh %s",domain_name);
        system(command);
	 std::string filename = "sd.txt";
        std::ifstream f2(filename.c_str());
	size_t position_lkp;
char *get_line_from_file = new char[BUFFLEN];
		
	 if(return_f = f2.is_open())
         {
		f2.getline(get_line_from_file,BUFFLEN,',');

                       std::string trv_buf = get_line_from_file;
                       position_lkp = trv_buf.find("key");
                        if(position_lkp >= 0)
			{
				std::cout<<"Sstring found, returning without adding"<<std::endl;
				f2.close();
				return;
			}
	}
		
	std::cout<<"Publish"<<std::endl;
	strcpy(exec_command,"multichain-cli chain1 publish globalstream ");
	 strcpy(exec_command_local,"multichain-cli chain1 publish localstream ");
	
	
	strcat(exec_command,domain_name);
	 strcat(exec_command_local,domain_name);
	strcat(exec_command_local," ");
	strcat(exec_command_local,ip);
	strcat(exec_command," ");
	strcat(exec_command,ip);
std::cout<<"EXEC "<< exec_command<<std::endl;	
	system(exec_command);
	system(exec_command_local);
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
 	char *COMMAND=NULL;
	char *COMMAND_AMP=NULL;	//Add modify publish
	char domain_name[1024];
	memset(domain_name,0,1024);
	char new_ip[10];
	memset(new_ip,0,10);
	char *old_ip=NULL;
	char *token=NULL;
	
	udp_server *u1=new udp_server("172.31.3.44",10000);
	Lookup *l1 = new Lookup();//"GUNS.txt");
	l1->populate_map_from_stream(BUFFLEN);
	while(1)
	{
		u1->myrecvfrom(buffer,BUFFLEN,&si_other,&slen);
		std::cout<<"Received "<<buffer<<std::endl;
		// Join Message Response
		
		token=strtok(buffer,":");
		
		if(!strcmp(token,"JOIN"))
		{
			buff = send_dns_ip_for_join();
                        //std::cout<<"buffer sent "<<buffer_to_send<<" length "<<strlen(buffer_to_send)<<std::endl;
                        if(u1->mysendto(buff,BUFFLEN,&si_other,slen)<0)
                                perror("Error in send");
                        delete []buff;
		}
		else if(!strcmp(token,"PUBLISH"))
		{
			std::cout<<"In publish"<<token<<std::endl;
			token=strtok(NULL,":");
			std::cout<<"Token"<<token<<std::endl;
			if(!strcmp(token,"ADD"))
			{
				std::cout<<"ADD"<<std::endl;
				token=strtok(NULL,":");
				strcpy(domain_name,token);
				token=strtok(NULL,":");
				strcpy(new_ip,token);	
				std::cout<<"Domain name"<<domain_name<<" Ip "<<new_ip<<std::endl;	
				send_dns_key_for_publish(domain_name,new_ip);
				
			}	
			buff=new char[100];
			strcpy(buff,"key1234");
			if(u1->mysendto(buff,BUFFLEN,&si_other,slen)<0)
				perror("Error in send");
			delete []buff;	
				
		}
		
		
		//std::cout<<"Received from"<<inet_ntoa(si_other.sin_addr)<<":"<<ntohs(si_other.sin_port)<<std::endl;
		else if(!strcmp(buffer,"JOIN"))
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
	
