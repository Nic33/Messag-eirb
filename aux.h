#ifndef AUX_H
#define AUX_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define MAX_FDS 4
#define PORT 8081
#define ADDRESS "127.0.0.1"
#define USER_FILE "user.txt"
#define BUFFER_SIZE 1024

typedef struct {
    char* prenom;
    char* nom;
    char* mdp;
    int socket_fd;
    char* type;
    char* message;
    char* dest;
} Utilisateur;

void print_error(int result, char* s);

int read_message_from_socket(int fd, char* buffer, int size);
int read_int_from_socket(int fd);

void write_on_socket(int fd, const char* buffer, int size);
void write_int_as_message(int fd, int val);

void deserialize_user(Utilisateur* user, const char* buffer);

#endif