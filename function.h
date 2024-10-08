#ifndef FUNCTION_H
#define FUNCTION_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define MAX_CLIENT 10
#define PORT 8081
#define ADDRESS "127.0.0.1"
#define USER_FILE "user.txt"

void print_error(int result, char* s);
int read_message_from_socket(int fd, char* buffer, int size);
void write_on_socket(int fd, char* s);
int read_int_from_socket(int fd);
void write_int_as_message(int fd, int val);

#endif
