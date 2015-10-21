/* http://stackoverflow.com/questions/18559182/creating-a-file-transfer-application-using-udp-in-c */


#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netinet/in.h>
#include <sys/types.h>

/* ??*/
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE  256

int main() {
    int count = 0, n=1; 
    char buff[20], output[20];
    char file_buffer[BUF_SIZE];
    int sockUDPfd;
    struct sockaddr_in servaddr;

    socklen_t len = sizeof(struct sockaddr_in);

    sockUDPfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockUDPfd == -1) {
        puts("socket not created in client");
        return 1;
    } else {
        puts("socket created in  client");
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; // ANY address or use specific address
    servaddr.sin_port = htons(7802);  // Port address
/*
    puts("Type your UDP client message");
    scanf("%s", buff);
*/ 
    puts("enter the name of new file to be saved");
    scanf("%s", output);

    // send msg to server
/*
    sendto(sockUDPfd, buff, strlen(buff) + 1, 0,
           (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
    count++;
    printf("%d\n", count);
*/

    FILE *fp = fopen(output, "w");
    if (fp == NULL) {
        puts("error in file handling");
        return 1;
    }
/*
    recvfrom(sockUDPfd, file_buffer, BUF_SIZE, 0, (struct sockaddr *)&servaddr, &len);
*/
    while (n > 0 ) {
        n = recvfrom(sockUDPfd, file_buffer, sizeof(file_buffer), 0,(struct sockaddr *)&servaddr, &len);
        if (strcmp(file_buffer, "end") == 0)
            break;
        printf("%s", file_buffer); 
	fwrite(file_buffer,1,BUF_SIZE,fp); 
        bzero(file_buffer, BUF_SIZE);
    }
    fclose(fp);

    puts("completed");
    return 0;
}
