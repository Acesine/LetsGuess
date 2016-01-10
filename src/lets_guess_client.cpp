#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>

#include "utils.h"

void printHelp() {
        printf("[HostName][PortNumber]\n");
}

char * getCommand() {
        char *command = new char[10];
        scanf("%s", command);
        return command;
}

char * postRequest(char *msg, const char *host, const char *port) {
        struct addrinfo serverInfo;
        struct addrinfo * serverInfoList;
        memset(&serverInfo, 0, sizeof(serverInfo));

        serverInfo.ai_family = AF_INET;
        serverInfo.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(host, port, &serverInfo, &serverInfoList) != 0) {
                printf("Failed to get server info\n");
                return NULL;
        }

        int sockfd = socket(serverInfoList->ai_family, serverInfoList->ai_socktype, serverInfoList->ai_protocol);
        if (sockfd < 0) {
                printf("Cannot open socket\n");
                return NULL;
        }

        int t = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));

        if (connect(sockfd, serverInfoList->ai_addr, serverInfoList->ai_addrlen) == -1) {
                printf("Failed to connect to server\n");
        }
        printf("Sending request: %s\n", msg);
        if (send(sockfd, msg, strlen(msg)*sizeof(char), 0) < 0) {
                printf("Failed to request\n");
        }
        printf("Waiting for result...\n");
        char *result_buffer = new char[32];
        int bytes_recieved = recv(sockfd, result_buffer, 32, 0);
        if (bytes_recieved <= 0) printf("Failed to receive\n");
        close(sockfd);
        return result_buffer;
}

int main(int argc, char * argv[]) {
        if (argc < 3) {
                printHelp();
                exit(1);
        }

        printf("Client is running...\n");
        char help_cmd[] = "help";
        char req_cmd[] = "req";
        while (true) {
                printf("Command: ");
                char * command = getCommand();
                if (strcmp(command, help_cmd)==0) {
                        printf("Possible commands:\n");
                        printf("- help : help\n");
                        printf("- req : post a request.\n");
                        printf("-- 1. [query]\n");
                        printf("-- 2. [answer|...]\n");
                } else if (strcmp(command, req_cmd)==0) {
                        char msg[128];
                        printf("Request: ");
                        scanf("%s", msg);
                        char * result = postRequest(msg, argv[1], argv[2]);
                        printf("[Server] : %s\n", result);
                        delete result;
                } else {
                        printf("Unknown command\n");
                }
                delete command;
        }

        return 0;
}
