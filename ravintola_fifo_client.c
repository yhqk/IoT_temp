/* Ravintola step 2
   Client to make order and pass to FIFO file. 
   * Lisää pääohjelmalle alkuun yksi lapsi forkkaamalla.
   * Tästä tulee se varsinainen tilausten käsittelijä. Tämä lapsi luo fifon
     kuten fifo esimerkissä eli fifo suljetaan ja avataan jokaisen viestin
     jälkeen ja luettu viesti tulostetaan.
   * Muuta client handlerit (eli lapset jotka saavat connection socketin)
     printf:n tilalta kirjoittamaan fifoon.
*/
/*
  $ gcc -o exec_c2 ravintola_fifo_client.c  -Wall
  $ ./exec_c2 localhost 5000 1   
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define LOGFILE "order_list.txt"
#define FIFOFILE "fifofile"


void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n, fifofd; 
    struct sockaddr_in serv_addr;
    struct hostent *server;  // old the way.     
    char buffer[256], buffer_sent[256];
    int clientId, len; 
    char str_id[15];
    struct timeval tv;
    struct tm* ptm;
    char time_string[35]; 

    n = 1;  // in order to go to the 1st while loop
    
    if (argc < 4) {
	fprintf(stderr,
		"usage %s hostname port clientID\n", argv[0]);
	exit(0);
    }
	
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    clientId = atoi(argv[3]);
    sprintf(str_id, "client%02d", clientId); 

    if (sockfd < 0) {
	error("ERROR opening socket");
    }
    server = gethostbyname(argv[1]);
    
    if (server == NULL) {
	fprintf(stderr,"ERROR, no such host\n");
	exit(0);
    }
    memset((char *) &serv_addr, 0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr,
	   (char *)server->h_addr, 
	   server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,
		sizeof(serv_addr)) < 0) {
	error("ERROR connecting");
    }

    printf("\nPlease enter the order from %s:\n", str_id);
    while (1) {
	memset(buffer_sent,0,256);
	/* Obtain the time of day, convert it to a tm struct. */
	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);
	strftime (time_string, sizeof (time_string), 
		  "%Y-%m-%d %H:%M:%S", ptm);
	/* Add client ID and time stamp */
	strcpy(buffer_sent, str_id); 
	strcat(buffer_sent, " ");
	strcat(buffer_sent, time_string);
	strcat(buffer_sent, " ");


	/* get order */
	len = 256-strlen(buffer_sent);
	memset(buffer,0,len);
	fgets(buffer,len,stdin); 
	strcat(buffer_sent, buffer);
	fifofd = open(FIFOFILE,O_WRONLY);
	n = write(fifofd, buffer_sent, strlen(buffer_sent));
	if (n != strlen(buffer_sent) ) { 
	    error("Error in writting FIFO");
	}  
	close(fifofd);
    }
}
