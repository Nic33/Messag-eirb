#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


void print_error(int fd, char *s){
    if (fd < 0){
        perror(s);
        exit(EXIT_FAILURE);
    }
}

void write_on_socket(int fd, char *s){
    //write data on socket 
    char buffer[1024];
    strcpy(buffer, s);
    int size = strlen(s) + 1; // 11 octets
    
    int nb_written = 0; 
    while (nb_written != size){
        int ret = write(fd, buffer + nb_written, size - nb_written); //write() appel systeme envoie des données sur la carte réseau
        print_error(ret, "write");
        nb_written += ret;
    }
    return;
}

void write_int_on_socket(int fd, int val){
    
    int nb_written = 0; 
    while (nb_written != sizeof(int)){
        int ret = write(fd, (char *)&val + nb_written, sizeof(int) - nb_written); //write() appel systeme envoie des données sur la carte réseau
        print_error(ret, "write val on socket");
        nb_written += ret;
    }
}

int main(int argc, char const *argv[]){

    //socket()

    int fd = socket(AF_INET,SOCK_STREAM,0);
    print_error(fd, "socket");


    //DEFINE client address
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_aton("127.0.0.1", &server_addr.sin_addr);

    //connect()
    int ret = connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    print_error(ret, "connect");

    //write date on socket
    write_on_socket(fd, "hello world");
    write_int_on_socket(fd, strlen("Hello world")+1);

    return 0;
}