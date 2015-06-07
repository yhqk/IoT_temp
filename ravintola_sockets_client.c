/* Ravintola step 1
  A client who makes orders  
  * Tehdään ravintolan tilauspalvelu, joka toimii internetin yli.
  * Luo socket, joka odottaa esim. portissa 5000 yhteyksiä. 
    Kun yhteys on luoto, forkkaa lapsi ja anna uus yhdistetty 
    socket (accept():n paluuarvo) sille. Lapsi hoitaa yhteyden 
    clienttiin siten, että se lukee mitä clientillä on 
    sanottavaa ja kirjottaa sen ulos. Kun read()-funktion 
    paluuarvo on 0, sulje connection socket ja tapa lapsi.
  * Nyt siis voi olla useampia clienttejä kiinni yhtä aikaa. 
    Clientiksi kelpaa vaikka ”echo”-ohjelma tai voit kirjoittaa 
    yksinkertaisen clientin.
 */

/* Compiling and Execution
  $ gcc -o exec_c1 ravintola_sockets_client.c  -Wall
  $ ./exec_c1 localhost 5000  

  There can be several clients from different termials. 
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

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;  // old the way.     
    char buffer[256];
    char buffer_sent[256];
    int clientId, len; 
    char str_id[15];
    struct timeval tv;
    struct tm* ptm;
    char time_string[35];

    n = 1;  // in order to go to the 1st while loop
    
    if (argc < 3) {
	fprintf(stderr,"usage %s hostname port\n", argv[0]);
	exit(0);
    }
	
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

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
    memcpy((char *)&serv_addr.sin_addr.s_addr,(char *)server->h_addr, 
	   server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,
		sizeof(serv_addr)) < 0) {
	error("ERROR connecting");
    }

    srand(time(0));     
    clientId = rand() % 100;  // clientID less than 100
    sprintf(str_id, "client%d:", clientId); 

    while (n > 0) {
	printf("\nPlease enter the order from %s ", str_id);
	memset(buffer_sent,0,256);
	/* Obtain the time of day, and convert it to a tm struct. */
	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);
	strftime (time_string, sizeof (time_string), 
		    "%Y-%m-%d %H:%M:%S", ptm);
	/* Add client ID and time stamp */
	strcpy(buffer_sent, str_id); 
	strcat(buffer_sent, time_string);
	strcat(buffer_sent, " ");
	/* get order */
	len = 256-strlen(buffer_sent);
	memset(buffer,0,len);
	fgets(buffer,len,stdin); 
	strcat(buffer_sent, buffer);
	n = write(sockfd,buffer_sent,strlen(buffer_sent));
	if (n < 0) {
	    error("ERROR writing to socket");
	}  
    }
    return 1; 
}
