/* http://stackoverflow.com/questions/18559182/creating-a-file-transfer-application-using-udp-in-c */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

#define BUF_SIZE  256

int main() {
    char content[BUF_SIZE];
    char buff[20];
    int sockUDPfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(struct sockaddr_in);

    sockUDPfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockUDPfd == -1) {
        puts("socket not created in server");
        return 1;
    } else {
        puts("UDP socket created in server");
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(7802);

    if (bind(sockUDPfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        puts("Not binded");
        return 1;
    } else {
        puts("Binded");
    }

    recvfrom(sockUDPfd, buff, 1024, 0, (struct sockaddr *)&cliaddr, &len);

    printf("%s\n", buff);

    FILE *fp = fopen("RandomNumbers_256B", "r");

/*    FILE *fp = fopen(buff, "r"); */
    if (fp == NULL) {
        printf("File does not exist \n");
        return 1;
    }

	/* bytes_read = read(fp, content, sizeof(content)); */
    while ( fgets (content, BUF_SIZE, fp)!=NULL ) {
       	sendto(sockUDPfd, content, sizeof(content), 0, (struct sockaddr*)&cliaddr, len);
      	/* writing content to stdout */
 /*     	puts(content); */
	bzero(content, BUF_SIZE);
      	}

    strcpy(content, "end");
    puts(content); 
    sendto(sockUDPfd, content, sizeof(content), 0, (struct sockaddr*)&cliaddr, len);

    return 0;
}
