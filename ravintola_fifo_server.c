/* Ravintola step 2
   A server to take order from different clients and write to a file
   * Lisää pääohjelmalle alkuun yksi lapsi forkkaamalla.
   * Tästä tulee se varsinainen tilausten käsittelijä. Tämä lapsi luo fifon
     kuten fifo esimerkissä eli fifo suljetaan ja avataan jokaisen viestin
     jälkeen ja luettu viesti tulostetaan.
   * Muuta client handlerit (eli lapset jotka saavat connection socketin)
     printf:n tilalta kirjoittamaan fifoon.
*/

/* 
  $ gcc -o exec_s2 ravintola_fifo_server.c  -Wall
  $ ./exec_s2 5000
*/

#include <stdio.h>
#include <poll.h>
#include <stdlib.h> //exit
#include <string.h> //memset
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h> 

#define LOGFILE "order_list.txt"
#define FIFOFILE "fifofile"

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, flag; 

    pid_t child, pid, ppid;
    char buffer[256];
    struct sockaddr_in serv_addr;
    FILE *fp_log;
    struct pollfd fifo[1]; 
    int a = 1;
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
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &a, 
	       sizeof(int));
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
	     sizeof(serv_addr)) < 0) 
	error("ERROR on binding");
    listen(sockfd,5);

    /* create child to handle following tasks
     * - read from FIFO file by poll 
     * - write to logfile
     * - print out from stdio
     */
    child = fork(); 
    if (child == 0) {
	pid = getpid();
	ppid = getppid();
	printf("\nPrint/Read child: pid as %d; ppid as %d\n", 
	       pid, ppid);
	fp_log = fopen(LOGFILE,"a");  
	fprintf(fp_log, 
		"\n=== Order list (ravintola_socket_*.c) ===\n");
	fflush(fp_log); 

	n = mkfifo(FIFOFILE,0777);
	fifo[0].fd = open(FIFOFILE, O_RDONLY, 0); 
	fifo[0].events = POLLIN; 
	flag = fcntl(fifo[0].fd,F_GETFL);
	flag = flag + O_NONBLOCK;
	fcntl(fifo[0].fd, F_SETFL, flag);

	while(1) {
	    memset(buffer,0,255);
	    n = poll(fifo, 1, -1);
	    if (n > -1) {
		if (fifo[0].revents & POLLIN) {	
		    n = read(fifo[0].fd,buffer,255);
		    printf("\n%s",buffer);
		    buffer[n] = '\0';
                    fputs(buffer, fp_log);
		    fflush(fp_log); 
		}
	    } else {
		printf("No input \n");
	    }
	}
	close(fifo[0].fd); 
	fclose(fp_log);
	return EXIT_SUCCESS;
    } else if (child == -1 ){
	perror("ERROR on fork1");
        exit(EXIT_FAILURE);
    } 
    return EXIT_SUCCESS;
} 

