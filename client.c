#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int clnt_sock;

void handle_signal(int signum)
{
    if (signum == SIGINT)
    {
        printf("Received SIGINT. Closing socket and exiting.\n");

        // Close client socket if it's still open
        if (clnt_sock != -1)
            close(clnt_sock);

        exit(0);
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    char msg[50] = {0}; // Adjust the size based on your expected message size

    // Register the signal handler
    signal(SIGINT, handle_signal);

    if (argc != 3)
    {
        printf("Usage : %s <server_ip> <port>\n", argv[0]);
        exit(1);
    }

    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (clnt_sock == -1)
    {
        perror("socket() error");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    // Use inet_pton for IPv4 address conversion
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        perror("inet_pton() error");
        exit(1);
    }

    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(clnt_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("connect() error");
        exit(1);
    }

    while (1)
    {
        // Receive the message from the server
        ssize_t bytes_received = read(clnt_sock, msg, sizeof(msg));
        if (bytes_received <= 0)
        {
            // Handle the server termination or communication error
            printf("Server terminated or communication error. Exiting.\n");
            break;
        }

        // Check if the received message indicates server termination
        if (strcmp(msg, "police win") == 0)
        {
            printf("Police win! Game over.\n");
            break;
        }
    }

    // Close the client socket
    close(clnt_sock);

    return 0;
}
