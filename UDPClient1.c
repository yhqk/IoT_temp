/* http://www.cs.odu.edu/~cs476/SocketProgramming/c/UDPClient1.c */

/*
NAME: 	 UDPClient1 
SYNOPSIS:    UDPClient <hostid> <portnumber> 
DESCRIPTION:    The program creates a datagram socket in the inet 
                domain, get messages  typed by a user
                and send them to UDPServer1 running on <hostid>
                and waiting at <portnumber>. 
                <hostid> can be either name or ip address.
                The program waits for a reply from  the UDPServer1
                and show it back to the user, with a message
                indicating if there is an error during the round trip. 
*/

#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<string.h>
#include <netdb.h>

#define MAXHOSTNAME 80
#define BUFSIZE 1024
char buf[BUFSIZE];
char rbuf[BUFSIZE];

main( argc, argv )
int argc;
char *argv[];
{
   int   sd;
   struct   sockaddr_in server;
   struct  hostent *hp, *gethostbyname();
   struct  servent *sp;
   struct sockaddr_in from;
   struct sockaddr_in addr;
   int fromlen;
   int length;
   int rc, cc;
   char ThisHost[80];

   sp = getservbyname("echo", "udp");
/** get UDPClient1 Host information, NAME and INET ADDRESS */

   gethostname(ThisHost, MAXHOSTNAME);
   /* OR strcpy(ThisHost,"localhost"); */

   printf("----UDPClient1 running at host NAME: %s\n", ThisHost);
   if  ( (hp = gethostbyname(ThisHost)) == NULL ) {
      fprintf(stderr, "Can't find host %s\n", argv[1]);
      exit(-1);
   }
   bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
   printf("(UDPClient1 INET ADDRESS is: %s )\n", inet_ntoa(server.sin_addr));

/* get UDPServer1 Host information, NAME and INET ADDRESS */
   if  ( (hp = gethostbyname(argv[1])) == NULL ) {
      addr.sin_addr.s_addr = inet_addr(argv[1]);
      if ((hp = gethostbyaddr((char *)&addr.sin_addr.s_addr,
          sizeof(addr.sin_addr.s_addr),AF_INET)) == NULL) {
         fprintf(stderr, "Can't find host %s\n", argv[1]);
         exit(-1);
      }
   }
   printf("----UDPServer1 running at host NAME: %s\n", hp->h_name);
   bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
   printf("(UDPServer1 INET ADDRESS is: %s )\n", inet_ntoa(server.sin_addr));

/** Construct name of socket to send to. */
   server.sin_family = AF_INET; 
   /* OR server.sin_family = hp->h_addrtype; */
 
   server.sin_port = htons(atoi(argv[2]));
   /*OR server.sin_port = sp->s_port; */

/** Create socket on which to send  and receive */

   sd = socket (hp->h_addrtype,SOCK_DGRAM,0);
   /*OR  sd = socket (AF_INET,SOCK_DGRAM,0); */

   if (sd<0) {
      perror("opening datagram socket");
      exit(-1);
   }

/** get data from USER, send it SERVER,
      receive it from SERVER, display it back to USER  */
   for(;;) {
      printf("\nType anything followed by RETURN, or type CTRL-D to exit\n");
      cleanup(buf);
      cleanup(rbuf);
      rc=read(0,buf, sizeof(buf));
      if (rc == 0) break;

      if (sendto(sd, buf, rc, 0, (SA *)&server, sizeof(server)) <0 )
         perror("sending datagram message");
      fromlen= sizeof(from);
      if ((cc=recvfrom(sd, rbuf, sizeof(rbuf), 0, (SA *) &from, &fromlen)) < 0)
         perror("receiving echo");
           rbuf[cc]=NULL;
           printf("  Received: %s", rbuf);
            printf("  from %s:%d\n", inet_ntoa(from.sin_addr),
                ntohs(from.sin_port));
            if ((hp = gethostbyaddr((char *)&from.sin_addr.s_addr,
                sizeof(from.sin_addr.s_addr),AF_INET)) == NULL)
               fprintf(stderr, "Can't find host %s\n",
                   inet_ntoa(from.sin_addr));
            else
               printf("  (Name is : %s)\n", hp->h_name);

      if (cc == rc)
         if (strcmp(buf, rbuf) == 0){
            printf("Echo is good\n");
         }
         else
            printf("Echo bad -- strings unequal: %s\n", rbuf);
      else
         printf("Echo bad -- lengths unequal: %s\n", rbuf);
   }
   printf ("EOF... exit\n");
   close(sd);
   exit (0);
}
cleanup(buf)
char *buf;
{
   int i;
   for(i=0; i<BUFSIZE; i++) buf[i]=NULL;
}
