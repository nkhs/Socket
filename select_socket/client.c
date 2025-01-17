#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#define PORT 8888
#define MAX_LINE 2048

int max(int a , int b)
{
	return a > b ? a : b;
}

/*readline function implementation*/
ssize_t readline(int fd, char *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = read(fd, &c,1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

/*Ordinary client message processing function*/
void str_cli(int sockfd)
{
	/*Send and receive buffers*/
	char sendline[MAX_LINE] , recvline[MAX_LINE];
	while(fgets(sendline , MAX_LINE , stdin) != NULL)	
	{
		write(sockfd , sendline , strlen(sendline));

		bzero(recvline , MAX_LINE);
		if(readline(sockfd , recvline , MAX_LINE) == 0)
		{
			perror("server terminated prematurely");
			exit(1);
		}//if

		if(fputs(recvline , stdout) == EOF)
		{
			perror("fputs error");
			exit(1);
		}//if

		bzero(sendline , MAX_LINE);
	}//while
}

/*采用select的客户端消息处理函数*/
void str_cli2(FILE* fp , int sockfd)
{
	int maxfd;
	fd_set rset;
	/*Send and receive buffers*/
	char sendline[MAX_LINE] , recvline[MAX_LINE];

	FD_ZERO(&rset);
	while(1)
	{
		/*将文件描述符和套接字描述符添加到rset描述符集*/
		FD_SET(fileno(fp) , &rset);	
		FD_SET(sockfd , &rset);
		maxfd = max(fileno(fp) , sockfd) + 1;
		select(maxfd , &rset , NULL , NULL , NULL);
		
		if(FD_ISSET(fileno(fp) , &rset))
		{
			if(fgets(sendline , MAX_LINE , fp) == NULL)
			{
				printf("read nothing~\n");
				close(sockfd); /*all done*/
				return ;
			}//if
			sendline[strlen(sendline) - 1] = '\0';
			write(sockfd , sendline , strlen(sendline));
		}//if

		if(FD_ISSET(sockfd , &rset))
		{
			if(readline(sockfd , recvline , MAX_LINE) == 0)
			{
				
				perror("handleMsg: server terminated prematurely.\n");
				exit(1);			
			}//if
			
			if(fputs(recvline , stdout) == EOF)
			{
				perror("fputs error");
				exit(1);
			}//if
		}//if	
	}//while
}

int main(int argc , char **argv)
{
	/*Declare socket and link server address*/
    int sockfd;
    struct sockaddr_in servaddr;

    /*Determine if it is a legal input*/
    if(argc != 2)
    {
        perror("usage:tcpcli <IPaddress>");
        exit(1);
    }//if

    /*(1) Create a socket*/
    if((sockfd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    {
        perror("socket error");
        exit(1);
    }//if

    /*(2) Set the link server address structure*/
    bzero(&servaddr , sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if(inet_pton(AF_INET , argv[1] , &servaddr.sin_addr) < 0)
    {
        printf("inet_pton error for %s\n",argv[1]);
        exit(1);
    }//if

    /*(3) SEND LINK SERVER REQUEST*/
    if(connect(sockfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
    {
        perror("connect error");
        exit(1);
    }//if

	/*调用普通消息处理函数*/
	str_cli(sockfd);	
	/*调用采用select技术的消息处理函数*/
	//str_cli2(stdin , sockfd);
	exit(0);
}
