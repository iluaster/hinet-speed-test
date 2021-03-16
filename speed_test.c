/*
# Licensed under the GNU General Public License, version 2.0 (GPLv2)
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
# NON INFRINGEMENT.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>
#include<math.h>
#include<assert.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<getopt.h>

#define buffer_size 1048576 
/* 1MB=1048576 Bytes , 10MB=10485760 Bytes, 20MB=20971520 Bytes */
/* 60MB=62914560 Bytes */

char *file_list[]={
		"test_010m.zip",
		"test_015m.zip",
		"test_020m.zip",
		"test_040m.zip",
		"test_060m.zip",
		"test_100m.zip",
		"test_200m.zip",
		"test_250m.zip",
		"test_400m.zip"
};

void show_file_list(){
	int i;
	printf("No_of_file-size : 0-8\n");
	for(i=0;i<(sizeof(file_list)/sizeof(*file_list));i++)
	{
		printf("%d -> %s\n",i,file_list[i]);
	}

}

 unsigned int get_file(char *filesize){
	int sockfd,n,sockopt;
	int recv_buffer_size= buffer_size;
	struct sockaddr_in dest;

	struct hostent *hp;
	struct in_addr ip_addr;
	char send_string[]="GET /test_000m.zip HTTP/1.1\r\nHost: speed.hinet.net\r\nReferer: http://speed.hinet.net/index_test01.htm\r\n\r\n";
	int modify_index;
	for(modify_index=5;modify_index<=13;modify_index++)
		send_string[modify_index]=filesize[modify_index-5];
	char *host= "tpdb.speed2.hinet.net";	
	int ct=0; // count of the loop
	unsigned int recv_size=0,file_size=0,accu_size=0; // accumulated file size
	char *buffer =  (char *)malloc(buffer_size);
	assert(buffer); // if malloc fail, then terminate program
	char *buffer_initial_addr = buffer;
	//	initialize value in dest 
	memset(&dest,0,sizeof(dest));
	dest.sin_family = PF_INET;
	dest.sin_port = htons(80);

	//	http://tpdb.speed2.hinet.net
	hp = gethostbyname(host);
	ip_addr= *(struct in_addr *)(hp->h_addr);
	inet_aton(inet_ntoa(ip_addr), &dest.sin_addr);

	//	create socket 
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("Socket creation error!");
		return -1;
	}

	//	Connecting to server 
	connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));

	/* Receive message from the server and print to screen */
	// flush the buffer content with zero
	memset(buffer,0,buffer_size);
	write(sockfd, send_string, sizeof(send_string));

	while((n=read(sockfd, buffer, buffer_size)) > 0)

	{
		ct++; // the count of loop
		accu_size+=n;
		printf("#");
	}


	//	Close connection
	close(sockfd);
	free(buffer_initial_addr);
	buffer_initial_addr=NULL; //avoid doubly free
	return accu_size;

}
int main(int argc, char **argv)
{


	unsigned int accu_size=0;
	time_t time_1,time_2;
	int opt=0;
	unsigned int file_index=1,no_of_thread=1;


	if (argc == 1){
		fprintf(stderr, "Usage: %s -s [No_of_file-size] -t [thread]\n",argv[0]);
		show_file_list();
		return -1;
	}

	while ((opt = getopt(argc, argv, "s:t:")) != -1) {
		switch (opt) {
			case 's':
				file_index = atoi(optarg);
				break;
			case 't':
				no_of_thread = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s -s [No_of_file-size] -t [thread]\n",argv[0]);
				show_file_list();
				return -1;
		}
	}
	//VLA
	pthread_t t[no_of_thread];
	unsigned int ret_val_of_thrd[no_of_thread];

	time_1=time(NULL); // get current time

	unsigned int k=0 ;
	for(k=0;k<no_of_thread;k++)
	{
	  	pthread_create(&t[k],NULL,get_file,file_list[file_index]);
	}

	for(k=0;k<no_of_thread;k++)
	{
		pthread_join(t[k],&ret_val_of_thrd[k]);
		accu_size+=ret_val_of_thrd[k];
	}

	time_2=time(NULL); // get current time
	printf("\nStart time : %s",ctime(&time_1));
	printf("End   time : %s",ctime(&time_2));
	printf("Elapsed time : %jd seconds\n",time_2-time_1);
	printf("File name : %s\n",file_list[file_index]);
	printf("The No. of thread : %d\n",no_of_thread);
	printf("Data transfer rate : %.2f KBytes/S\n",(accu_size/difftime(time_2,time_1))/1024.0);
	return 0;
}
