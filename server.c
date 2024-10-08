#include "function.h"
#include <ctype.h>  
#include <sys/poll.h>

void add_user_to_file(const char *prenom, const char *nom, const char *password);
int verify_user_in_file(const char *prenom, const char *nom, const char *password);


int main(int argc, char** argv) {

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  print_error(listen_fd, "socket");

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_aton(ADDRESS, &server_addr.sin_addr);

  int ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  print_error(ret, "bind");

  ret = listen(listen_fd, MAX_CLIENT);
  print_error(ret, "listen");

  struct pollfd fds[MAX_CLIENT + 1];
  fds[0].fd = listen_fd;
  fds[0].events = POLLIN; 
  fds[0].revents = 0; 

  for (int i = 1; i < MAX_CLIENT; i++){
    fds[i].fd = -1;
    fds[i].events = 0;
    fds[i].revents = 0;
  }

  printf("Server listening on port %d\n", PORT);

  while (1) {

    int activity = poll(fds, MAX_CLIENT + 1, -1);
    print_error(activity, "poll");

    for (int i = 0; i < MAX_CLIENT + 1; i++) {

      // Cas 1: Nouveau client sur le socket d'écoute
      if (fds[i].fd == listen_fd && (fds[i].revents & POLLIN) == POLLIN) {
        struct sockaddr client_addr;
        socklen_t len = sizeof(client_addr);
        printf("Waiting for clients...\n");
        int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len);
        print_error(client_fd, "accept");
        printf("New socket created on fd %d\n", client_fd);

        char insert = 0;
        for (int j = 1; j <= MAX_CLIENT; j++) {  
          if (fds[j].fd == -1) {
            fds[j].fd = client_fd;
            fds[j].events = POLLIN;  // Qurveiller les événements de lecture
            insert = 1;
            break;
          }
        }
        if (!insert) {
          printf("Connection refuse, no socket available\n");
          close(client_fd);
        }
      }         
      // Cas 2: Un client existant a envoyé un message
      else if (fds[i].fd != listen_fd && (fds[i].revents & POLLIN) == POLLIN) {
        
        printf("cas 2\n");

        int client_fd = fds[i].fd;

        // Lire la taille du message envoyé par le client
        int message_size = read_int_from_socket(client_fd);
        if (message_size <= 0 || message_size >= 1024) {
          printf("Invalid message size: %d\n", message_size);
          close(client_fd);
          fds[i].fd = -1;  // Libérer l'entrée du tableau de pollfd pour ce client
          continue;  // Passer au prochain client
        }

        char* buffer = (char*)malloc(message_size + 1);
        if (buffer == NULL) {
          perror("Erreur allocation mémoire");
          continue;  
        }
            
        buffer[message_size] = '\0';  
        read_message_from_socket(client_fd, buffer, message_size);
        printf("Message reçu du client : %s\n", buffer);

        char *token = strtok(buffer, ":");
        if (token != NULL) {
          if (strcmp(token, "new") == 0) {
            token = strtok(NULL, ";");
            char *prenom = token;
            token = strtok(NULL, ";");
            char *nom = token;
            token = strtok(NULL, ";");
            char *password = token;

            if (prenom && nom && password) {
              add_user_to_file(prenom, nom, password);
              printf("Nouvel utilisateur ajouté : %s %s\n", prenom, nom);

              char message[] = "Utilisateur ajouté avec succès.\n";
              write_int_as_message(client_fd, strlen(message));
              write_on_socket(client_fd, message);
            }
          }
          else if (strcmp(token, "connect") == 0) {
            token = strtok(NULL, ";");
            char *prenom = token;
            token = strtok(NULL, ";");
            char *nom = token;
            token = strtok(NULL, ";");
            char *password = token;

            if (prenom && nom && password) {
              int user_found = verify_user_in_file(prenom, nom, password);
              if (user_found) {
                char message[] = "Connexion réussie.\n";
                write_int_as_message(client_fd, strlen(message));
                write_on_socket(client_fd, message);
                printf("Connexion réussie pour : %s %s\n", prenom, nom);
              } else {
                char message[] = "Connexion refusée.\n";
                write_int_as_message(client_fd, strlen(message));
                write_on_socket(client_fd, message);
                printf("Connexion refusée pour : %s %s\n", prenom, nom);
              }
            }
          }
          else if (strcmp(token, "message") == 0) {

            char new_message[1024];
            strcpy(new_message, &token[8]);
            printf("Message du client : %s\n", new_message);
          }
          else {
            printf("Type de requête inconnu : %s\n", token);
            write_on_socket(client_fd, "Requête inconnue.\n");
          }
        }

        free(buffer); 
      }

      // Cas 3: Détection de la fermeture de connexion d'un client
      else if (fds[i].fd != listen_fd && (fds[i].revents & POLLHUP) == POLLHUP) {
        printf("Client hung up on fd %d\n", fds[i].fd);
        close(fds[i].fd);
        fds[i].fd = -1;  
      }
    }
  }

  return 0;
}

void add_user_to_file(const char *prenom, const char *nom, const char *password) {
  FILE *file = fopen(USER_FILE, "a"); 
  if (file == NULL) {
      perror("Erreur lors de l'ouverture du fichier");
      return;
  }

  fprintf(file, "%s;%s;%s\n", prenom, nom, password);
  fclose(file);
}

int verify_user_in_file(const char *prenom, const char *nom, const char *password) {
  FILE *file = fopen(USER_FILE, "r"); 
  if (file == NULL) {
    perror("Erreur lors de l'ouverture du fichier");
    return 0;
  }

  char line[256];
  char user_entry[256] = "";  

  strcat(user_entry, prenom);
  strcat(user_entry, ";");
  strcat(user_entry, nom);
  strcat(user_entry, ";");
  strcat(user_entry, password);

  while (fgets(line, sizeof(line), file)) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }

    if (strcmp(line, user_entry) == 0) {
      fclose(file);
      return 1;  
    }
  }

  fclose(file);
  return 0; 
}