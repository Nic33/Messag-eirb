#include <aux.h>
#include <ctype.h>  
#include <sys/poll.h>
#include <stdbool.h>

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
void associer_utilisateur_a_socket(Utilisateur* users, int socket_fd, char* prenom,  char* nom) {
  for (int i = 0; i < MAX_FDS; i++) {
    if (users[i].socket_fd == -1) {
      users[i].socket_fd = socket_fd;
      users[i].prenom = strdup(prenom);
      users[i].nom = strdup(nom);

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
      if (users[i].prenom != NULL) {
        free(users[i].prenom);  
        users[i].prenom = NULL;
      }
      users[i].socket_fd = -1;
      break;
    }
  }
}


// Envoie un message à l'utilisateur ayant le prénom spécifié

////// FONCTION A MODIFIER POUR LES SALONS + CREER LES SOCKETS DES SALONS 
void envoyer_message_a_utilisateur(Utilisateur* users, Utilisateur* user) {
  int found = 0;
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "\nMessage reçu de %s: %s", user->prenom, user->message);

  for (int i = 0; i < MAX_FDS; i++) {
    if (users[i].socket_fd != -1 && strcmp(users[i].prenom, user->dest) == 0) {
      printf("Socket trouvée pour l'utilisateur %s, socket_fd: %d\n", users[i].prenom, users[i].socket_fd);
      int ret = write(users[i].socket_fd, buffer, strlen(buffer));

      print_error(ret, "write");
      if (ret > 0) {
        printf("Envoi réussi du message \"%s\" à %s sur la socket %d\n", user->message, user->prenom, users[i].socket_fd);
      } else {
        printf("Échec de l'envoi du message à %s sur la socket %d\n", user->prenom, users[i].socket_fd);
      }

      found = 1;
      break;
    }
  }
  if (!found) {
    printf("Utilisateur %s non trouvé ou non connecté.\n", user->dest);
  }
}

// Envoie un message à tous les utilisateurs connectés sauf le nouvel utilisateur
void envoyer_message_a_tous_les_utilisateurs(Utilisateur* users, int except_socket_fd, const char *message) {
  for (int i = 0; i < MAX_FDS; i++) {
    if (users[i].socket_fd != -1 && users[i].socket_fd != except_socket_fd) {
      printf("Envoi du message à l'utilisateur %s sur la socket %d\n", users[i].prenom, users[i].socket_fd);
      int ret = write(users[i].socket_fd, message, strlen(message) - 1);
      if (ret < 0) {
        perror("Erreur lors de l'envoi du message");
      }
    }
  }
}

void afficher_utilisateurs_connectes(Utilisateur* users, int socket_fd, char *prenom) {
  printf("\n--- Utilisateurs connectés ---\n");
  
  char notification[1024];  
  memset(notification, 0, sizeof(notification));  // Initialiser à 0
  int count = 0;

  snprintf(notification, sizeof(notification), "\n*************\n");

  for (int i = 0; i < MAX_FDS; i++) {

    printf("users[i].prenom = %s\n", users[i].prenom);
    printf("user.prenom = %s\n", prenom);

    if (users[i].socket_fd != -1 && strcmp(users[i].prenom, prenom) != 0){

      if (strcmp(users[i].prenom, prenom) != 0) {
        strcat(notification, "Utilisateur : ");
        strcat(notification, users[i].prenom);
        strcat(notification, " est connecté\n");
        count++;
      }
    }
  }
  if (count == 0) {
    strcat(notification, "Vous êtes le seul utilisateur connecté\n");
  }
  if (count < 0) {
    strcat(notification, "Une erreur est survenue\n");
  }
  strcat(notification, "*************\n");
  int ret = write(socket_fd, notification, strlen(notification));
  if (ret < 0) {
    perror("Erreur lors de l'envoi du message");
  }
  printf("%s", notification);  
  printf("-----------------------------\n");
}

void envoyer_fichier(Utilisateur* users, Utilisateur* user, const char* filepath, long filesize) {
  int found = 0;

  for (int i = 0; i < MAX_FDS; i++) {
    if (users[i].socket_fd != -1 && strcmp(users[i].prenom, user->dest) == 0) {
      found = 1;
      // Envoi des métadonnées
      write(users[i].socket_fd, "file", 5);
      write(users[i].socket_fd, filepath, strlen(filepath));
      write(users[i].socket_fd, &filesize, sizeof(long));

      // Envoi du fichier en blocs
      FILE *file = fopen(filepath, "rb");
      char buffer[1024];
      size_t bytes_read;
      while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(users[i].socket_fd, buffer, bytes_read);
      }
      fclose(file);
      printf("Fichier relayé à %s\n", user->dest);
      break;
    }
  }

  if (!found) {
      printf("Utilisateur destinataire non trouvé\n");
  }
}

void envoyer_fichier_salon(Utilisateur* users, Utilisateur* user, const char* filepath, long filesize, char **salons_name, char **salons_users){
int salon_trouve = -1;

  // Rechercher le salon correspondant
  for (int i = 0; i < NB_SERVERS; i++) {
    if (salons_name[i] != NULL && strcmp(salons_name[i], user->dest) == 0) {
      salon_trouve = i;
      break;
    }
  }

  if (salon_trouve != -1) {
    // Copie de la chaîne des utilisateurs du salon
    char *users_copy = strdup(salons_users[salon_trouve]);
    if (users_copy == NULL) {
      perror("Erreur lors de la copie des utilisateurs");
      return;
    }

    // Diviser la chaîne pour obtenir chaque utilisateur du salon
    char *utilisateur = strtok(users_copy, ",");
    while (utilisateur != NULL) {
      // Trouver l'utilisateur dans la liste et envoyer le fichier
      for (int j = 0; j < MAX_FDS; j++) {
        if (users[j].socket_fd != -1 && strcmp(users[j].prenom, utilisateur) == 0 && strcmp(user->prenom, users[j].prenom) != 0) {
          // Envoi des métadonnées du fichier
          write(users[j].socket_fd, "file", 5);
          write(users[j].socket_fd, filepath, strlen(filepath));
          write(users[j].socket_fd, &filesize, sizeof(long));

          // Ouverture et envoi du fichier en blocs
          FILE *file = fopen(filepath, "rb");
          if (file == NULL) {
            perror("Erreur lors de l'ouverture du fichier");
            free(users_copy);
            return;
          }

          char buffer[1024];
          size_t bytes_read;
          while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            write(users[j].socket_fd, buffer, bytes_read);
          }

          fclose(file);
          printf("Fichier envoyé à %s dans le salon %s\n", utilisateur, user->dest);
          break;
        }
      }

      // Passer à l'utilisateur suivant dans le salon
      utilisateur = strtok(NULL, ",");
    }

    // Libérer la mémoire allouée pour la copie
    free(users_copy);
  } else {
    printf("Le salon %s n'existe pas.\n", user->dest);
  }
}


void afficher_salons_connectes(Utilisateur* users, char **salons_name, char **salons_users, int socket_fd) {
  printf("\n--- Salons connectés ---\n");

  char notification[1024];

  memset(notification, 0, sizeof(notification));
  int salons_count = 0;

  snprintf(notification, sizeof(notification), "\n*************\n");

  for (int i = 0; i < NB_SERVERS; i++) {
    if (salons_name[i] != NULL && salons_users[i] != NULL) {
      salons_count++;
      strcat(notification, "Salon : ");
      strcat(notification, salons_name[i]);
      strcat(notification, "\nUtilisateurs : ");
      strcat(notification, salons_users[i]);
      strcat(notification, "\n");
    }
  }

  if (salons_count == 0) {
    strcat(notification, "Aucun salon\n");
  }

  printf("-------------------\n");

  strcat(notification, "*************\n");
  int ret = write(socket_fd, notification, strlen(notification));
  if (ret < 0) {
    perror("Erreur lors de l'envoi des salons connectés");
  }

  printf("%s", notification);
}

int find_client_by_name(Utilisateur* users,const char* dest) {
  for (int i = 0; i < MAX_FDS; i++) {
    if (strcmp(users[i].prenom, dest) == 0) {
      return users[i].socket_fd;
    }
  }
  return -1;
}

void handle_file_transfer(int client_fd, Utilisateur* user, Utilisateur* users) {
  long filesize;
  read(client_fd, &filesize, sizeof(long));

  char buffer[BUFFER_SIZE];
  ssize_t bytes_read;
  long total_bytes_read = 0;

  while (total_bytes_read < filesize && (bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
    int dest_fd = find_client_by_name(users, user->dest);
    if (dest_fd != -1) {
      write(dest_fd, buffer, bytes_read);
    }
    total_bytes_read += bytes_read;
  }

  printf("Fichier reçu et transféré à %s\n", user->dest);
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

  ///GESTION DES SALONS 
  char *salons_name[NB_SERVERS];
  char *salons_users[NB_SERVERS];
  //int salons_index = -1;
  for (int i = 0; i < NB_SERVERS; i++) {
    salons_name[i] = NULL;
    salons_users[i] = NULL;
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
          printf("Nouvel utilisateur ajouté : %s\n", user.prenom);

        }
        else if (strcmp(user.type, "connect") == 0) {

          bool user_exist = false;
          const char *message_connexion = "Connexion échouée";

          if (verify_user_in_file(user.prenom, user.nom, user.mdp)) {
            
            for (int i = 0; i < MAX_FDS; i++) {
              if (users[i].socket_fd != -1 && 
                strcmp(users[i].prenom, user.prenom) == 0 && 
                strcmp(users[i].nom, user.nom) == 0) {
                user_exist = true;
                break;
              }
            }

            if (user_exist == false){
              associer_utilisateur_a_socket(users, client_fd, user.prenom, user.nom);
              printf("Connexion réussie pour %s\n", user.prenom);

              message_connexion = "Connexion réussie";
            }
          } 
          write(client_fd, message_connexion, strlen(message_connexion));
        }

        else if (strcmp(user.type, "message") == 0) {
          envoyer_message_a_utilisateur(users, &user);
        }
        else if (strcmp(user.type, "file") == 0) {
          handle_file_transfer(client_fd, &user, users);
          
        }else if (strcmp(user.type, "file_salon") == 0) {

          char filepath[256];
          strcpy(filepath, user.message);

          long filesize;
          read(client_fd, &filesize, sizeof(long));

          FILE *file = fopen(filepath, "wb");
          if (file == NULL) {
            perror("Erreur lors de l'ouverture du fichier pour écriture");
            continue;
          }

          char buffer[1024];
          ssize_t bytes_read;
          long bytes_received = 0;

          while (bytes_received < filesize && (bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytes_read, file);
            bytes_received += bytes_read;
          }

          fclose(file);
          printf("Fichier reçu avec succès de %s\n", user.prenom);

          envoyer_fichier_salon(users, &user, filepath, filesize, salons_name, salons_users);
        }
        else if (strcmp(user.type, "deconnexion") == 0) {
          char notification[256];
          snprintf(notification, sizeof(notification), "\n*************\n%s s'est déconnecté !\n*************\n", user.prenom);
          envoyer_message_a_tous_les_utilisateurs(users, client_fd, notification);
        }

        //Création d'un nouveau salon, lis le user.dest = "nom_du_salon" et ajoute dans le tableau salons_name ou la casse suivante à "-1" le nom du salon
        //Ajouter au même index du tableau salons_users le prenom de la source sous la forme "prenom,"
        else if (strcmp(user.type, "new_salon") == 0) {
            // Chercher si le salon existe déjà
            int salon_existe = 0;
            for (int i = 0; i < NB_SERVERS; i++) {
                // Vérifier si le salon existe déjà dans salons_name
                if (salons_name[i] != NULL && strcmp(salons_name[i], user.dest) == 0) {
                    salon_existe = 1;
                    break;
                }
            }
            
            if (!salon_existe) {
                // Trouver un indice disponible dans salons_name
                for (int i = 0; i < NB_SERVERS; i++) {
                    if (salons_name[i] == NULL) {  // S'il n'y a pas encore de salon à cet indice
                        salons_name[i] = strdup(user.dest);  // Ajouter le nom du salon
                        salons_users[i] = malloc(256 * sizeof(char));  // Allouer un espace pour les utilisateurs
                        printf("Nouveau salon créé: %s\n", salons_name[i]);
                        break;
                    }
                }
            } else {
                printf("Le salon %s existe déjà.\n", user.dest);
            }
        }

        //Regarde le nom du salon dans user.dest, si le salon existe alors dans le tableau salons_name à l'indice "n" alors la variable "salons_index" prend la valeur de "n"
        //Le user est ajouter à l'emplacement salons_users[n] -> "prenom,"
        else if (strcmp(user.type, "join_salon") == 0){
          int salon_trouve = -1;
          for (int i = 0; i < NB_SERVERS; i++) {
            if (salons_name[i] != NULL && strcmp(salons_name[i], user.dest) == 0) {
              salon_trouve = i;  // Trouvé, stocker l'index du salon
              break;
            }
          }
          const char *message_connexion = "Salon trouvé"; 

          if (salon_trouve != -1) {
            strcat(salons_users[salon_trouve], user.prenom);  // Ajouter le prénom de l'utilisateur
            strcat(salons_users[salon_trouve], ",");  // Séparer les utilisateurs par une virgule
            printf("Utilisateur %s a rejoint le salon %s\n", user.prenom, salons_name[salon_trouve]);
            
          } else {
            message_connexion = "Pas de salon";          
            printf("Le salon %s n'existe pas.\n", user.dest); 
          }

          write(client_fd, message_connexion, strlen(message_connexion));

        }
        else if (strcmp(user.type, "message_salon") == 0) {
          int salon_trouve = -1;
          for (int i = 0; i < NB_SERVERS; i++) {
              if (salons_name[i] != NULL && strcmp(salons_name[i], user.dest) == 0) {
                  salon_trouve = i;
                  break;
              }
          }
          if (salon_trouve != -1) {
              // Faire une copie de la chaîne utilisateurs pour éviter de modifier l'original
              char *users_copy = strdup(salons_users[salon_trouve]);
              if (users_copy == NULL) {
                  perror("Erreur lors de la copie des utilisateurs");
              }

              // Diviser les utilisateurs dans le salon à partir de la copie
              char *utilisateur = strtok(users_copy, ",");
              while (utilisateur != NULL) {
                  printf("Utilisateur trouvé : %s\n", utilisateur);
                  // Trouver le socket de l'utilisateur et lui envoyer le message
                  for (int j = 0; j < MAX_FDS; j++) {
                      if (users[j].socket_fd != -1 && strcmp(users[j].prenom, utilisateur) == 0 && strcmp(user.prenom, users[j].prenom) != 0) {
                          char buffer[512];
                          snprintf(buffer, sizeof(buffer), "Message de %s : %s", user.prenom, user.message);
                          int ret = write(users[j].socket_fd, buffer, strlen(buffer));
                          if (ret < 0) {
                              perror("Erreur lors de l'envoi du message au salon");
                          }
                          break;
                      }
                  }
                  utilisateur = strtok(NULL, ",");
              }

              // Libérer la mémoire allouée pour la copie
              free(users_copy);
          } else {
              printf("Le salon %s n'existe pas.\n", user.dest);
          }
      }

        else if (strcmp(user.type, "dec_salon") == 0){
          int salon_trouve = -1;
          for (int i = 0; i < NB_SERVERS; i++) {
            if (salons_name[i] != NULL && strcmp(salons_name[i], user.dest) == 0) {
              salon_trouve = i;
              break;
            }
          }
          if (salon_trouve != -1) {
            // Remplacer le prénom de l'utilisateur par une chaîne vide
            char *new_list = malloc(256 * sizeof(char));
            strcpy(new_list, "");
            char *utilisateur = strtok(salons_users[salon_trouve], ",");
            while (utilisateur != NULL) {
                if (strcmp(utilisateur, user.prenom) != 0) {
                    strcat(new_list, utilisateur);
                    strcat(new_list, ",");
                }
                utilisateur = strtok(NULL, ",");
            }
            strcpy(salons_users[salon_trouve], new_list);  // Mettre à jour la liste des utilisateurs
            free(new_list);
            printf("Utilisateur %s s'est déconnecté du salon %s\n", user.prenom, user.dest);
            
            char buffer[512];
            snprintf(buffer, sizeof(buffer), "%s s'est déconnecté du salon\n", user.prenom);
            envoyer_message_a_tous_les_utilisateurs(users, user.socket_fd, buffer);
          } else {
            printf("Le salon %s n'existe pas.\n", user.dest);
          }

        }else if (strcmp(user.type, "info") == 0){
          for (int i = 0; i < MAX_FDS; i++) {
            if (users[i].socket_fd != -1 && strcmp(users[i].prenom, user.prenom) == 0) {
              afficher_utilisateurs_connectes(users, users[i].socket_fd, user.prenom);
            }
          }
        }else if (strcmp(user.type, "info_salon") == 0){
          for (int i = 0; i < MAX_FDS; i++) {
            if (users[i].socket_fd != -1 && strcmp(users[i].prenom, user.prenom) == 0) {
              afficher_salons_connectes(users, salons_name, salons_users, users[i].socket_fd);
            }
          }
        }else{
          printf("Error: commande non reconnue\n");
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