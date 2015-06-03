/* A server to take order from different client "Ravintola Step1"  */
#include <stdio.h>
#include <stdlib.h> //exit
#include <string.h> //memset
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    int sockfd, newsockfd, portno, clilen, child, pid, ppid;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    int n = 1;

    if (argc < 2) {
	fprintf(stderr,"ERROR, no port provided\n");
	exit(1);
    }
    portno = atoi(argv[1]);

    if ( portno < 5000 ) {
	fprintf(stderr,"Enter port number >= 5000 \n");
	exit(1);
    }
	
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
	error("ERROR opening socket");
    }
    memset((char *) &serv_addr ,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    int a = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(int));
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
	     sizeof(serv_addr)) < 0) 
	error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (1){
	newsockfd = accept(sockfd, 
			   (struct sockaddr *) &cli_addr, 
			   &clilen);
	if (newsockfd < 0) {
	    error("ERROR on accept");
	}
	
	child = fork();
	if (child == 0) {
	    pid = getpid();
	    ppid = getppid();
	    printf("pid as %d; ppid as %d\n", pid, ppid);
	    while (n > 0 ) {
		memset(buffer,0,256);
		n = read(newsockfd,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		if ( n == 0 ) close(newsockfd);
		if ( strcmp(buffer, "client") > 0 ) {
		    printf("\nOrder from %s",buffer); 
		}
	    }
	    return EXIT_SUCCESS; 
	} else if (child == -1 ){
	    perror("ERROR on fork");
	    return 1;
	} 
    }
} 

