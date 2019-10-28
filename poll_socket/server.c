#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <poll.h>
#include <limits.h>		/*for OPEN_MAX*/
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


#ifndef OPEN_MAX
#define OPEN_MAX 1024
#endif

#ifndef INFTIM
#define INFTIM -1
#endif

#define PORT 8888
#define MAX_LINE 2048
#define LISTENQ 20


int main(int argc , char **argv)
{
	int i, maxi, listenfd, connfd, sockfd;

	int nready;
	
	ssize_t n, ret;
		
	struct pollfd client[OPEN_MAX];

	char buf[MAX_LINE];

	socklen_t clilen;

	struct sockaddr_in servaddr , cliaddr;

	/*(1) Get listener descriptor*/
	listenfd = socket(AF_INET , SOCK_STREAM , 0);

	/*(2) Binding socket*/
	bzero(&servaddr , sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	bind(listenfd , (struct sockaddr *)&servaddr , sizeof(servaddr));

	/*(3) MONITOR*/
	listen(listenfd , LISTENQ);

	/*(4) Setting poll*/
	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for(i=1 ; i<OPEN_MAX ; ++i)
	{
		client[i].fd = -1;
	}//for
	maxi = 0;

	/*(5) Enter the server to receive the request infinite loop*/
	while(1)
	{
		nready = poll(client , maxi+1 , INFTIM);
		
		if(client[0].revents & POLLRDNORM)
		{
			/*Receive client requests*/
			clilen = sizeof(cliaddr);

			printf("\naccpet connection~\n");

			if((connfd = accept(listenfd , (struct sockaddr *)&cliaddr , &clilen)) < 0)
			{
				perror("accept error.\n");
				exit(1);
			}//if		

			printf("accpet a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr) , cliaddr.sin_port);
			
			/*Add a client link socket descriptor to an array*/
			for(i=1 ; i<OPEN_MAX ; ++i)
			{
				if(client[i].fd < 0)
				{
					client[i].fd = connfd;
					break;
				}//if
			}//for

			if(OPEN_MAX == i)
			{
				perror("too many connection.\n");
				exit(1);
			}//if
	
			/*The event that the descriptor is waiting for*/
			client[i].events = POLLRDNORM;
			if(i > maxi)
				maxi = i;

			if(--nready < 0)
				continue;
		}//if
			
		for(i=1; i<=maxi ; ++i)
		{
			if((sockfd = client[i].fd) < 0)
				continue;
			/*The actual event of the link descriptor*/
			if(client[i].revents & (POLLRDNORM | POLLERR))
			{
				/*Handling customer requests*/
				printf("\nreading the socket~~~ \n");
				
				bzero(buf , MAX_LINE);
				if((n = read(sockfd , buf , MAX_LINE)) <= 0)
				{
					close(sockfd);				
					client[i].fd = -1;
				}//if
				else{
					printf("clint[%d] send message: %s\n", i , buf);
					if((ret = write(sockfd , buf , n)) != n)	
					{
						printf("error writing to the sockfd!\n");
						break;
					}//if
				}//else
				if(--nready <= 0)
					break;
			}//if
		}//for
	}//while
	exit(0);
}
