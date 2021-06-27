#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

#define ip_protocol 0
#define ip_address "127.0.0.1"  
#define port 6666
#define buffer_size 32
#define send_recv_flag 0

int recv_file(char* msg_buf, int s1)
{
    int i;
    char ch;
    for (i = 0; i < s1; i++) {
        ch = msg_buf[i];
        if (ch == EOF)
            return 1;
        else
            printf("%c", ch);
    }
    return 0;
}

int main(){
    int sock_fd, nBytes;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(port);
    addr_con.sin_addr.s_addr = inet_addr(ip_address);
    char msg_buf1[buffer_size], msg_buf2[buffer_size];
    char *msg_buf_da;
    FILE* fp1;
    
    sock_fd = socket(AF_INET, SOCK_DGRAM,
                     ip_protocol);
    if (sock_fd < 0)
        printf("\nFile descriptor is not received!!\n");
    else
        printf("\nfile descriptor %d is received\n", sock_fd);
    while (1) {
        memset(msg_buf1, '\0', sizeof(char) * buffer_size);
        memset(msg_buf2, '\0', sizeof(char) * buffer_size);
        printf("\nPlease enter the name of file to receive:\n");
        scanf("%s", msg_buf1);

        snprintf (msg_buf2, buffer_size, "%d", strlen(msg_buf1)); 
        sendto(sock_fd, msg_buf2, buffer_size,
               send_recv_flag, (struct sockaddr*)&addr_con,
               addrlen);
        sendto(sock_fd, msg_buf1, buffer_size,
               send_recv_flag, (struct sockaddr*)&addr_con,
               addrlen);

        printf("\n---------Data is Received---------\n");
        while (1) {
            
            memset(msg_buf1, '\0', sizeof(char) * buffer_size);
            nBytes = recvfrom(sock_fd, msg_buf1, buffer_size,
                              send_recv_flag, (struct
                            sockaddr*)&addr_con,
                              &addrlen);
            int lbajt;
            sscanf(msg_buf1, "%d", &lbajt);

            memset(msg_buf1, '\0', sizeof(char) * buffer_size);
            nBytes = recvfrom(sock_fd, msg_buf1, buffer_size,
                              send_recv_flag, (struct
                            sockaddr*)&addr_con,
                              &addrlen);
            if(lbajt == -1) {
                printf("There is not such File\n");
                break;
            }


           
            if (recv_file(msg_buf1, buffer_size)) {
                break;
            }
        }
        printf("\n-------------------------------\n");
    }
    return 0;
}
