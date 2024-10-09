#include <aux.h>
#include <ctype.h>  
#include <sys/poll.h>

// Ajoute un utilisateur au fichier
void add_user_to_file(char *prenom, char *nom, char *password) {

  printf("Write in file : %s;%s;%s\n", prenom, nom, password);

  FILE *file = fopen(USER_FILE, "a"); 
  if (file == NULL) {
      perror("Erreur lors de l'ouverture du fichier");
      return;
  }

  fprintf(file, "%s;%s;%s\n", prenom, nom, password);
  fclose(file);
}

// Vérifie si l'utilisateur est présent dans le fichier
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

// Associe un utilisateur à un socket donné
void associer_utilisateur_a_socket(Utilisateur* users, int socket_fd, char* prenom) {
  for (int i = 0; i < MAX_FDS; i++) {
    if (users[i].socket_fd == -1) {
      users[i].socket_fd = socket_fd;
      users[i].prenom = prenom;
      printf("Utilisateur %s est maintenant connecté sur la socket %d\n", prenom, socket_fd);
      break;
    }
  }
}

// Libère la socket lorsqu'un utilisateur se déconnecte
void liberer_socket(Utilisateur* users, int socket_fd) {
  for (int i = 0; i < MAX_FDS; i++) {
    if (users[i].socket_fd == socket_fd) {
      printf("Utilisateur %s déconnecté de la socket %d\n", users[i].prenom, socket_fd);
      free(users[i].prenom);
      users[i].socket_fd = -1;
      break;
    }
  }
}

// Envoie un message à l'utilisateur ayant le prénom spécifié
void envoyer_message_a_utilisateur(Utilisateur* users, const char *prenom, const char *message) {
  int found = 0; 
  for (int i = 0; i < MAX_FDS; i++) {
    if (users[i].socket_fd != -1 && strcmp(users[i].prenom, prenom) == 0) {
      printf("Socket trouvée pour l'utilisateur %s, socket_fd: %d\n", prenom, users[i].socket_fd);
      int ret = write(users[i].socket_fd, message, strlen(message) + 1);
      print_error(ret, "write");
      if (ret > 0) {
        printf("Envoi réussi du message \"%s\" à %s sur la socket %d\n", message, prenom, users[i].socket_fd);
      } else {
        printf("Échec de l'envoi du message à %s sur la socket %d\n", prenom, users[i].socket_fd);
      }
      found = 1;
      break;
    }
  }
  if (!found) {
    printf("Utilisateur %s non trouvé ou non connecté.\n", prenom);
  }
}


int main(int argc, char const *argv[]) {
  int welcome_fd = socket(AF_INET, SOCK_STREAM, 0);
  print_error(welcome_fd, "socket");

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_aton(ADDRESS, &server_addr.sin_addr);

  int ret = bind(welcome_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  print_error(ret, "bind");

  ret = listen(welcome_fd, 2);
  print_error(ret, "listen");

  struct pollfd fds[MAX_FDS];
  fds[0].fd = welcome_fd;
  fds[0].events = POLLIN;

  for (int i = 1; i < MAX_FDS; i++) {
    fds[i].fd = -1;
    fds[i].events = 0;
  }

  Utilisateur users[MAX_FDS];
  for (int i = 0; i < MAX_FDS; i++) {
    users[i].socket_fd = -1;  // Initialiser toutes les sockets à -1
    users[i].prenom = NULL;
    users[i].nom = NULL;
    users[i].mdp = NULL;
    users[i].type = NULL;
    users[i].message = NULL;
    users[i].dest = NULL;
  }

  while (1) {
    int activity = poll(fds, MAX_FDS, -1);
    print_error(activity, "poll");

    for (int i = 0; i < MAX_FDS; i++) {
      // Cas 1: Nouveau client sur le socket d'écoute
      if (fds[i].fd == welcome_fd && (fds[i].revents & POLLIN) == POLLIN) {

        printf("\ncas 1\n");

        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(welcome_fd, (struct sockaddr *)&client_addr, &len);
        print_error(client_fd, "accept");
        printf("New socket created on fd %d\n", client_fd);

        int added = 0;
        for (int j = 1; j < MAX_FDS; j++) {
          if (fds[j].fd == -1) { 
            fds[j].fd = client_fd;
            fds[j].events = POLLIN;
            added = 1;
            break;
          }
        }

        if (!added) {
          printf("Trop de clients, fermeture de la connexion...\n");
          close(client_fd);
        }
      }
      // Cas 2 : Si un client envoie des données
      else if (fds[i].fd != -1 && (fds[i].revents & POLLIN) == POLLIN) {

        printf("\ncas 2\n");

        int client_fd = fds[i].fd;
        Utilisateur user;

        size_t lengths[6];
        ssize_t bytes_received = read(client_fd, lengths, sizeof(lengths));            
        if (bytes_received <= 0) {
          printf("Invalid message size\n");
          close(client_fd);
          fds[i].fd = -1; 
          continue; 
        }

        // Allocation de mémoire pour chaque chaîne
        user.prenom = malloc(lengths[0] * sizeof(char));
        user.nom = malloc(lengths[1] * sizeof(char));
        user.mdp = malloc(lengths[2] * sizeof(char));
        user.type = malloc(lengths[3] * sizeof(char));
        user.message = malloc(lengths[4] * sizeof(char));
        user.dest = malloc(lengths[5] * sizeof(char));

        read(client_fd, user.prenom, lengths[0]);
        read(client_fd, user.nom, lengths[1]);
        read(client_fd, user.mdp, lengths[2]);
        read(client_fd, user.type, lengths[3]);
        read(client_fd, user.message, lengths[4]);
        read(client_fd, user.dest, lengths[5]);

        printf("Utilisateur reçu:\n");
        printf("Prénom: %s\n", user.prenom);
        printf("Nom: %s\n", user.nom);
        printf("Mot de passe: %s\n", user.mdp);
        printf("Type: %s\n", user.type);
        printf("Message: %s\n", user.message);
        printf("Destinataire: %s\n", user.dest);

        if (strcmp(user.type, "new") == 0) {

          add_user_to_file(user.prenom, user.nom, user.mdp);
          associer_utilisateur_a_socket(users, client_fd, user.prenom);
          printf("Nouvel utilisateur ajouté : %s\n", user.prenom);

        }
        else if (strcmp(user.type, "connect") == 0) {
          if (verify_user_in_file(user.prenom, user.nom, user.mdp)) {
            associer_utilisateur_a_socket(users, client_fd, user.prenom);
            printf("Utilisateur %s connecté\n", user.prenom);
            write(client_fd, "Connexion réussie.", 18);
          } else {
            write(client_fd, "Connexion échouée.", 18);
          }
        }
        else if (strcmp(user.type, "message") == 0) {
          envoyer_message_a_utilisateur(users, user.dest, user.message);
        }

        free(user.prenom);
        free(user.nom);
        free(user.mdp);
        free(user.type);
        free(user.message);
        free(user.dest);
        
      }
    
      else if (fds[i].fd != -1 && ((fds[i].revents & POLLHUP) == POLLHUP)) {
        printf("Client disconnected on fd %d\n", fds[i].fd);
        liberer_socket(users, fds[i].fd);  // Libérer la socket de l'utilisateur
        close(fds[i].fd);
        fds[i].fd = -1;
      }
    }
  }

  close(welcome_fd);
  return 0;
}
