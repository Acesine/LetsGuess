#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <algorithm>
#include <time.h>

#include "utils.h"

using namespace std;

vector<string> words;

void setup() {
        FILE * fp;
        fp = fopen("words.txt", "r");
        rewind(fp);
        char word[30];
        while(fscanf(fp, "%s", word) > 0) {
                string w = convertToString(word);
                words.push_back(w);
                printf("Read word: %s\n", word);
        }
        fclose(fp);

        srand (time(NULL));
        int r = rand() % words.size();
        string currentWord = words[r];
        string currentShuffle = currentWord;
        random_shuffle(currentShuffle.begin(), currentShuffle.end());

        FILE * shuffle = fopen("tmp", "w+");
        fprintf(shuffle, "%s\n", currentWord.c_str());
        fprintf(shuffle, "%s\n", currentShuffle.c_str());
        fclose(shuffle);
}

void printHelp() {
        printf("Please provide a port number!\n");
}

void parseCommand(char *buffer, char *command, char *data) {
        while(buffer != NULL && *buffer != '\0') {
                if (*buffer == '|') {
                        buffer ++;
                        break;
                }
                *command = *buffer;
                command ++;
                buffer ++;
        }
        *command = '\0';
        while(buffer != NULL && *buffer != '\0') {
                *data = *buffer;
                data ++;
                buffer ++;
        }
        *data = '\0';
}

void handleRequest(char *buffer, char *result) {
        char command[24];
        char data[30];
        parseCommand(buffer, command, data);
        printf("Received command: %s, data: %s\n", command, data);
        char query_cmd[] = "query";
        char answer_cmd[] = "answer";
        if (strcmp(command, query_cmd)==0) {
                // read file
                FILE * shuffle = fopen("tmp", "r");
                char tmpWord[30];
                fscanf(shuffle, "%s", tmpWord);
                fscanf(shuffle, "%s", tmpWord);
                fclose(shuffle);
                printf("%s\n", tmpWord);
                strcpy(result, tmpWord);
        } else if (strcmp(command, answer_cmd)==0) {
                // read file
                FILE * shuffle = fopen("tmp", "r");
                char tmpWord[30];
                fscanf(shuffle, "%s", tmpWord);
                fclose(shuffle);
                if (strcmp(data, tmpWord) == 0) {
                        srand (time(NULL));
                        int r = rand() % words.size();
                        printf("Choosing word %d\n", r);
                        string currentWord = words[r];
                        string currentShuffle = currentWord;
                        random_shuffle(currentShuffle.begin(), currentShuffle.end());
                        // putting to FILE
                        FILE * shuffle = fopen("tmp", "w");
                        fprintf(shuffle, "%s\n", currentWord.c_str());
                        fprintf(shuffle, "%s\n", currentShuffle.c_str());
                        fclose(shuffle);

                        strcpy(result, "Congratulations!");
                } else {
                        strcpy(result, "Wrong answer!");
                }
        } else {
                strcpy(result, "Unknown command");
        }
}

bool handleClient(int sock) {
   char buffer[256];
   memset(buffer, 0, 256);

   int received = 0;
   while (received < 256) {
           int r = recv(sock, buffer + received, 256 - received, 0);
           if (r <= 0) break;
           received += r;
           printf("Received %d chars\n", received);
           if (buffer[received] == '\0') break;
   }

   printf("Message from client: %s\n", buffer);

   char *msg = new char[30];
   handleRequest(buffer, msg);
   if (send(sock, msg, strlen(msg)*sizeof(char), 0) < 0) {
           printf("Failed to send to socket\n");
           return false;
   } else {
           printf("Sent message: %s\n", msg);
   }

   delete msg;
   return true;
}

int main(int argc, char * argv[]) {
        if (argc < 2) {
                printHelp();
                exit(1);
        }

        int port = atoi(argv[1]);
        if (port < 0) {
                printf("%s is not a valid port number!\n", argv[1]);
                exit(1);
        }
        printf("Starting server at port %d...\n", port);

        struct addrinfo serverInfo;
        struct addrinfo * serverInfoList;
        memset(&serverInfo, 0, sizeof(serverInfo));

        serverInfo.ai_family = AF_INET;
        serverInfo.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(NULL, argv[1], &serverInfo, &serverInfoList) != 0) {
                printf("Failed to get server info\n");
                exit(1);
        }

        int sockfd = socket(serverInfoList->ai_family, serverInfoList->ai_socktype, serverInfoList->ai_protocol);
        if (sockfd < 0) {
                printf("Cannot open server socket\n");
                exit(1);
        }

        printf("Binding\n");
        if (bind(sockfd, serverInfoList->ai_addr, serverInfoList->ai_addrlen) == -1) {
              printf("Failed to bind socket\n");
              exit(1);
        }

        printf("Listening at port %d\n", port);
        if (listen(sockfd, 2) == -1) {
                printf("Failed to listen\n");
                exit(1);
        }

        setup();

        printf("Server is running...\n");
        while (true) {
                int clientfd;
                struct sockaddr_storage client_addr;
                socklen_t addr_size = sizeof(client_addr);
                clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
                if (clientfd == -1) {
                        printf("Failed to accept\n");
                } else {
                        printf("Connection accepted\n");
                }

                int pid = fork();
                if (pid < 0) {
                        printf("Failed to create child process\n");
                        continue;
                }
                if (pid == 0) {
                        close(sockfd);
                        handleClient(clientfd);
                        close(clientfd);
                        exit(0);
                }
                else {
                        close(clientfd);
                }
        }

        return 0;
}
