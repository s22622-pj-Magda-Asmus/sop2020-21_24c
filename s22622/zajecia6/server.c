#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#define ip_protocol 0
#define port 6666
#define buffer_size 32
#define send_recv_flag 0
#define NOFILE "File is not Found"

volatile sig_atomic_t flag = 0;

void gentleman_exit(int sig){
    flag = 1; 
}

int send_file(FILE *fp, char *msg_buf, int s) {
    int i, len;
    if (fp == NULL) {
        strcpy(msg_buf, NOFILE);
        len = strlen(NOFILE);
        msg_buf[len] = EOF;
        return 1;
    }
    char ch1;
    for (i = 0; i < s; i++) {
        ch1 = fgetc(fp);
        msg_buf[i] = ch1;
        if (ch1 == EOF)
            return 1;
    }
    return 0;
}

void init_signal_handler(void)
{
    struct sigaction oldsigaction;
    assert( !sigaction( SIGINT, 0, &oldsigaction) );
    oldsigaction.sa_handler = gentleman_exit;
    assert( !sigaction( SIGINT, &oldsigaction, 0) );
}


int main() {
    int sock_fd, nBytes;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(port);
    addr_con.sin_addr.s_addr = INADDR_ANY;
    char net_buf1[buffer_size], net_buf2[buffer_size];
    char *net_buf_da;
    FILE *fp1;

    /* signal handler */
    init_signal_handler();

    /* socket() */
    sock_fd = socket(AF_INET, SOCK_DGRAM, ip_protocol);
    if (sock_fd < 0)
        printf("\nfile descriptor is not received!!\n");
    else
        printf("\nfile descriptor %d is received\n", sock_fd);

    /* bind() */
    if (bind(sock_fd, (struct sockaddr *) &addr_con,
             sizeof(addr_con)) == 0)
        printf("\nSuccessfully is binded!\n");
    else
        printf("\nBinding is Failed!\n");

    while (1) {
        printf("\nWaiting for name of file...\n");

        

        memset(net_buf2, '\0', sizeof(char) * buffer_size);
        nBytes = recvfrom(sock_fd, net_buf2,
                          buffer_size, send_recv_flag,
                          (struct sockaddr *) &addr_con,
                          &addrlen);
        if (flag == 1) {
            close(sock_fd);
            printf("Executed handler");
            return 0;
        }
        int lbajt;
        sscanf(net_buf2, "%d", &lbajt);

        net_buf_da = malloc(lbajt*sizeof(char));
        memset(net_buf_da, '\0', lbajt);
        nBytes = recvfrom(sock_fd, net_buf_da,
                          lbajt, send_recv_flag,
                          (struct sockaddr *) &addr_con,
                          &addrlen);

        memset(net_buf1, '\0', sizeof(char) * buffer_size);

        printf("===%s\n",net_buf_da);
        fp1 = fopen(net_buf_da, "r");
        free(net_buf_da);



        printf("\nFile Name is Received: %s\n", net_buf_da);
        if (fp1 == NULL) {
            printf("\nFile open is failed!\n");
            memset(net_buf1, '\0', sizeof(char) * buffer_size);
            snprintf (net_buf1, buffer_size, "%d", -1); 
            sendto(sock_fd, net_buf1, buffer_size,
                   send_recv_flag,
                   (struct sockaddr *) &addr_con,
                   addrlen);
            send_file(fp1, net_buf1, buffer_size);
            sendto(sock_fd, net_buf1, buffer_size,
                   send_recv_flag,
                   (struct sockaddr *) &addr_con,
                   addrlen);

            continue;
        }
        else
            printf("\nFile Successfully is opened!\n");
        while (1) {
            
            snprintf (net_buf1, buffer_size, "%d", buffer_size); 
            sendto(sock_fd, net_buf1, buffer_size,
                   send_recv_flag,
                   (struct sockaddr *) &addr_con,
                   addrlen);
            if (send_file(fp1, net_buf1, buffer_size)) {
                sendto(sock_fd, net_buf1, buffer_size,
                       send_recv_flag,
                       (struct sockaddr *) &addr_con,
                       addrlen);
                break;
            }
            
            snprintf (net_buf2, buffer_size, "%d", buffer_size);
            sendto(sock_fd, net_buf2, buffer_size,
                   send_recv_flag,
                   (struct sockaddr *) &addr_con,
                   addrlen);
            sendto(sock_fd, net_buf1, buffer_size,
                   send_recv_flag,
                   (struct sockaddr *) &addr_con, addrlen);
            memset(net_buf2, '\0', sizeof(char) * buffer_size);
            memset(net_buf1, '\0', sizeof(char) * buffer_size);
        }
        if (fp1 != NULL)
            fclose(fp1);
    }
    return 0;
}
