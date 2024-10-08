#include "function.h"
#include <stdbool.h>

int main(int argc, char** argv) {

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  print_error(fd, "socket");

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_aton(ADDRESS, &server_addr.sin_addr);

  int ret = connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  print_error(ret, "Server connected");

  char message[256];

  bool identification = true;

  while (1) {

    if (identification){
      printf("What do you want ?\n\n");
      printf("Exit tape 'quit'\n");
      printf("Identification tape '1'\n");
      printf("New user tape '2'\n");

    }else{

      printf("You are identified\n\n");
      printf("Exit tape 'quit'\n");
      printf("Write a message : \n");
    }

    fgets(message, sizeof(message), stdin);
    size_t len = strlen(message);
    if (len > 0 && message[len - 1] == '\n') {
      message[len - 1] = '\0';
      len--;
    }

    if (identification){
      if (strcmp(message, "quit") == 0) {
        printf("Exiting...\n");
        break;

      }else if (strcmp(message, "1") == 0) {

        char user_info[256] = "connect:";   
        char input[256];

        printf("Prénom : \n");
        fgets(input, sizeof(input), stdin);

        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        strcat(user_info, input);
        strcat(user_info, ";");  

        printf("Nom : \n");
        fgets(input, sizeof(input), stdin);

        len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        strcat(user_info, input);
        strcat(user_info, ";");

        printf("Mot de passe : \n");
        fgets(input, sizeof(input), stdin);

        len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        strcat(user_info, input);
        len = strlen(user_info);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        printf("Value sent : %s\n", user_info);

        write_int_as_message(fd, len);
        write_on_socket(fd, user_info);
        printf("Message sent\n");
        
        int message_size = read_int_from_socket(fd);
        printf("Message size received: %d\n", message_size);

        if (message_size <= 0 || message_size >= 1024) {
          printf("Invalid message size: %d\n", message_size);
          break;
        }

        char* buffer = (char*)malloc(message_size + 1);
        int read_status = read_message_from_socket(fd, buffer, message_size);
        if (read_status <= 0) {
          printf("Server disconnected or error occurred.\n");
          free(buffer);
          break;
        }

        buffer[message_size - 1] = '\0'; 
        printf("Message received from server: %s\n", buffer);

        if (strcmp(buffer, "Connexion réussie.") == 0) {
          identification = false;
        }

        free(buffer);
  
      }else if (strcmp(message, "2") == 0) {

        char user_info[256] = "new:";   
        char input[256];

        printf("Prénom : \n");
        fgets(input, sizeof(input), stdin);

        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        strcat(user_info, input);
        strcat(user_info, ";");  

        printf("Nom : \n");
        fgets(input, sizeof(input), stdin);

        len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        strcat(user_info, input);
        strcat(user_info, ";");

        printf("Mot de passe : \n");
        fgets(input, sizeof(input), stdin);

        len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        strcat(user_info, input);
        len = strlen(user_info);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        printf("Value sent : %s\n", user_info);

        write_int_as_message(fd, len);
        write_on_socket(fd, user_info);
        printf("Message sent\n");
        
        int message_size = read_int_from_socket(fd);
        printf("Message size received: %d\n", message_size);

        if (message_size <= 0 || message_size >= 1024) {
          printf("Invalid message size: %d\n", message_size);
          break;
        }

        char* buffer = (char*)malloc(message_size + 1);
        int read_status = read_message_from_socket(fd, buffer, message_size);
        if (read_status <= 0) {
          printf("Server disconnected or error occurred.\n");
          free(buffer);
          break;
        }
        buffer[message_size] = '\0'; 
        printf("Message received from server: %s\n", buffer);

        free(buffer);
        
      }else{
      printf("I don't understand\n");
    }
    }else{
      if (strcmp(message, "quit") == 0) {
        printf("Exiting...\n");
        break;
      }else{
        char user_info[256] = "message:"; 
        char input[256];

        fgets(input, sizeof(input), stdin);

        strcat(user_info, input);
        len = strlen(user_info);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        printf("Value sent : %s\n", user_info);

        write_int_as_message(fd, len);
        write_on_socket(fd, user_info);
        printf("Message sent\n");
      
      }
    }
  }

  close(fd);

  return 0;
}
