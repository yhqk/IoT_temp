/* http://www.cs.odu.edu/~cs476/SocketProgramming/c/UDPServer1.c */

/*
NAME: 		UDPServer1
SYNOPSIS:   	UDPServer1 [port]
DESCRIPTION:    The program creates a datagram socket in the inet 
                domain then receive messages from clients process 
                and echo it back to it. 
                If [port] is not specified, the program uses any available port.
*/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>

#define MAXHOSTNAME 80

void reusePort(int sock);

int main(int argc, char *argv[])
   {
   int   sd;
   struct   sockaddr_in server;
   struct  hostent *hp, *gethostbyname();
   struct  servent *sp;
   struct sockaddr_in from;
   int fromlen;
   int length;
   char buf[512];
   int rc;
   char ThisHost[80];

   sp = getservbyname("echo", "udp");

/** get Host information, NAME and INET ADDRESS */

   gethostname(ThisHost, MAXHOSTNAME);
   /* OR strcpy(ThisHost,"localhost"); */

   printf("----UDPServer1 running at host NAME: %s\n", ThisHost);
   if  ( (hp = gethostbyname(ThisHost)) == NULL ) {
      fprintf(stderr, "Can't find host name\n");
      exit(-1);
   }
   bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
   printf("    (UDPServer1 INET ADDRESS is: %s )\n", 
                     inet_ntoa(server.sin_addr));

 /** Construct name of socket to send to. */
   server.sin_family = AF_INET; 
   /* OR server.sin_family = hp->h_addrtype; */

   server.sin_addr.s_addr = htonl(INADDR_ANY);
   if (argc == 1)
        server.sin_port = htons(0);  
   else 
        server.sin_port = htons(atoi(argv[1])); 
   /*OR    server.sin_port = sp->s_port; */
    
/** Create socket on which to send  and receive */

   sd = socket (AF_INET,SOCK_DGRAM,0); 
   /* OR sd = socket (hp->h_addrtype,SOCK_DGRAM,0);

   if (sd<0) {
      perror("opening datagram socket");
      exit(-1);
   }

/** to allow another process to use the same port 
      howver, only ONE gets the message */

   reusePort(sd);

   if ( bind( sd, (SA *) &server, sizeof(server) ) < 0 ) {
      close(sd);
      perror("binding name to datagram socket");
      exit(-1);
   }

/** get port information and  prints it out */
   length = sizeof(server);
   if ( getsockname (sd, (SA *)&server,&length) ) {
      perror("getting socket name");
      exit(0);
   }
   printf("Server Port is: %d\n", ntohs(server.sin_port));

/**  get data from  clients and send it back */
   for(;;){
      fromlen = sizeof(from);
      printf("\n...server is waiting...\n");
      if ((rc=recvfrom(sd, buf, sizeof(buf), 0, (SA *) &from, &fromlen)) < 0)
         perror("receiving datagram  message");
printf("Recieved %d characters\n", rc);

      if (rc > 0){
         buf[rc]=NULL;
         printf("Received: %s\n", buf);
         printf("From %s:%d\n", inet_ntoa(from.sin_addr),
             ntohs(from.sin_port));
         if ((hp = gethostbyaddr( (char *)&from.sin_addr.s_addr,
             sizeof(from.sin_addr.s_addr),AF_INET)) == NULL)
            fprintf(stderr, "Can't find host %s\n", inet_ntoa(from.sin_addr));
         else
            printf("(Name is : %s)\n", hp->h_name);
         if (sendto(sd, buf, rc, 0, (SA *) &from, sizeof(from)) <0 )
            perror("sending datagram message");
      }
   }
   return 0; 	
}
void reusePort(int s)
{
  int one=1;
  
  if ( setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *) &one,sizeof(one)) == -1 )
  {
    printf("error in setsockopt,SO_REUSEPORT \n");
    exit(-1);
  }
}

