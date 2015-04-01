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
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>
#include<math.h>

#define buffer_size 1048576 
/* 1MB=1048576 Bytes , 10MB=10485760 Bytes, 20MB=20971520 Bytes */
/* 60MB=62914560 Bytes */

   
int main(int argc, char **argv)
{
	int sockfd,n,sockopt;
	int recv_buffer_size= buffer_size;
	struct sockaddr_in dest;
	
//	http request string
	char send_string[]="GET /test_060m.zip HTTP/1.1\r\nHost: speed.hinet.net\r\nReferer: http://speed.hinet.net/index_test01.htm\r\n\r\n";
	
	time_t time_1,time_2,time_3;
	int ct=0; // count of the loop
	unsigned int recv_size=0,file_size=0,accu_size=0; // accumulated file size
	char *buffer =  malloc(buffer_size);
	char *buffer_initial_addr = buffer;

//	create socket 
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("Socket creation error!");
		return 0;
	}
//	initialize value in dest 
	memset(&dest,0,sizeof(dest));
	dest.sin_family = PF_INET;
	dest.sin_port = htons(80);
	
//	http://tpdb.speed2.hinet.net
	inet_aton("210.61.132.1", &dest.sin_addr);


	time_1=time(NULL); // get current time
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
	time_2=time(NULL); // get current time
	printf("\nStart time : %s",ctime(&time_1));
	printf("End   time : %s",ctime(&time_2));
	printf("Elapsed time : %d seconds\n",time_2-time_1);
	printf("Data transfer rate : %.2f KBytes/S\n",(accu_size/difftime(time_2,time_1))/1024.0);
	

//	Close connection
	close(sockfd);
	free(buffer_initial_addr);
	return 0;
}
