/* Ravintola step 1
  A server to receive order from different clients
 
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

/* Compiling and execution: 
 $ gcc -o exec_s1 ravintola_sockets_server.c  -Wall
 $ ./exec_s1 5000
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h> 

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
			   (socklen_t *)&clilen);
	if (newsockfd < 0) {
	    error("ERROR on accept");
	}
	
	child = fork();
	if (child == 0) {
	    pid = getpid();
	    ppid = getppid();
	    printf("New client pid as %d; ppid as %d\n", pid, ppid);
	    while (n > 0 ) {
		memset(buffer,0,256);
		n = read(newsockfd,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		if ( n == 0 ) close(newsockfd);
		if ( strcmp(buffer, "client") > 0 ) {
		    // print order to stdio
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
