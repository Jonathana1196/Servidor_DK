//
// Created by kenneth on 12/06/19.
//

//
// Created by kenneth on 08/06/19.
//
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#define PORT 8080

int opt = (1==1);
int master_socket , addrlen , new_socket , client_socket[30] ,
        max_clients = 30 , activity, i , valread , sd;
int max_sd;
struct sockaddr_in address;

char buffer[1025];  //data buffer of 1K

//set of socket descriptors
fd_set readfds;

//a message
char *message = "Mensaje xd";

void * config() {
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    //create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
                   sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (1 == 1) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for (i = 0; i < max_clients; i++) {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            //highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n ", new_socket,
                   inet_ntoa(address.sin_addr), ntohs
                           (address.sin_port));

            //send new connection greeting message
            if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) {
                //if position is empty
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    sleep(0.2);
                    if (i == 0) {
                        send(new_socket, "main", strlen("main"), 0);
                    }
                    else {
                        send(client_socket[i], "oyente", strlen("oyente"), 0);
                        send(client_socket[0], "get", strlen("get"), 0);

                    }
                    break;
                }

            }
        }


        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    //Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr *) &address, \
                        (socklen_t *) &addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                }

                    //Echo back the message that came in
                else {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    buffer[valread] = '\0';
                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }
    }
}
int strcmp1(char *s, char *p){
    int indice = 0;
    while (1==1){
        if(p[indice]=='\0' && s[indice]=='\0' ){
            return 0;
        }
        if (p[indice]!= s[indice]){
            return 1;
        }
        if(p[indice]=='\0' ){
            return 1;
        }
        if(s[indice]=='\0' ){
            return 1;
        }
        indice++;
    }
}
void * open_console() {
    printf("Introduce el tipo de barril, N para el normal, C para caida libre y T para el que cae en cierto tiempo: \n");
    while (1 == 1) {
        char str[100];
        while (1 == 1) {
            gets(str);
            if (!strcmp1(str, "T") || !strcmp1(str, "N") || !strcmp1(str, "C")) {
                if (client_socket[0] != 0) {
                    send(client_socket[0], str, strlen(str), 0);
                }
            } else if (client_socket[0] == 0) {
                printf("Error, no hay clientes conectados");
            } else {
                printf("El comando esta mal, recuerda solo acpetamos T N y C \n");
            }
        }
    }
}
void * escuchar_Host(){
    while (1 == 1) {
        char str[1025];
        while (1 == 1) {
                if (client_socket[0] != 0) {
                    read(client_socket[0], str, 1025);
                    printf(str);
                    send_toAll(str);
                    memset(str,0,1024) ;
                }
             else if (client_socket[0] == 0) {
                //printf("Error, no hay clientes conectados");
            }
        }
    }
}
void send_toAll(char* caracteres){
    int indice = 1;
    while (indice<30){
        if(client_socket[indice]!=0){
            send(client_socket[indice], caracteres, strlen(caracteres), 0);
        }
        indice++;
    }
}



