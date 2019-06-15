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

char buffer[1025];

fd_set readfds;

char *message = "Mensaje xd";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Nombre: config
///Parametros de entrada:None
///Salida:None
///Fuente: https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
///Modificado: Kenneth
///Cambios realizados: Lo que se hizo fue cambiar ciertos parametros de aceptacion, que cuando se aceptara el primero este se definiera como esl host
/// al mismo tiempo definio ciertos sends para hacer un setup a los diferentes clienttes
///Funcion: lo que hace es basicamente crear el socket principal y aceptar los clientes que entren
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void * config() {
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
                   sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (1 == 1) {
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection , socket fd is %d , ip is : %s , port : %d \n ", new_socket,
                   inet_ntoa(address.sin_addr), ntohs
                           (address.sin_port));

            if (send("", message, strlen(""), 0) != strlen("")) {
                perror("send");
            }

            puts("Welcome message sent successfully");

            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
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


        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {

                if ((valread = read(sd, buffer, 1024)) == 0) {
                    getpeername(sd, (struct sockaddr *) &address, \
                        (socklen_t *) &addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_socket[i] = 0;
                }

                else {
                    buffer[valread] = '\0';
                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Nombre: strcmp1
///Parametros de entrada:dos strings a comparar
///Salida:int, 0 si son iguales y 1 si no lo son.
///Creador: Kenneth
///Funcion: lo que hace es basicamente comparar un par de strings, si son de diferente tamano o diferentes dan 0
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Nombre: open_console
///Parametros de entrada:NONE
///Salida:NONO
///Creador: Kenneth
///Funcion: escucha de forma continua a la consola y cuando recibe algo se envia este "algo" a todos los usuarios
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Nombre: escuchar_Host
///Parametros de entrada:NONE
///Salida:NONO
///Creador: Kenneth
///Funcion:escucha de forma continua al primer socket conectado
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Nombre: send_toAll
///Parametros de entrada:caracteres, es el mesnaje que se quiere enviar a todos los usuarios
///Salida:NONO
///Creador: Kenneth
///Funcion:escucha envia un string a todos los usuarios
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void send_toAll(char* caracteres){
    int indice = 1;
    while (indice<30){
        if(client_socket[indice]!=0){
            send(client_socket[indice], caracteres, strlen(caracteres), 0);
        }
        indice++;
    }
}



