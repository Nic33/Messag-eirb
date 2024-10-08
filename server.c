#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>

#define MAX_FDS 3
#define USER_FILE "user.txt"
//#define MAX_USERS 3

typedef struct {
    char prenom[20];
    int socket_fd;
} Utilisateur;

Utilisateur utilisateurs[MAX_FDS] = {
        {"mathieu", -1},
        {"nicolas", -1},
        {"evan", -1},
    };

void print_error(int fd, char *s){
    if (fd < 0){
        perror(s);
        exit(EXIT_FAILURE);
    }
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

void associer_utilisateur_a_socket(int socket_fd) {
    for (int i = 0; i < MAX_FDS; i++) {
        if (utilisateurs[i].socket_fd == -1) {
            utilisateurs[i].socket_fd = socket_fd;
            printf("Utilisateur %s est maintenant connecté sur la socket %d\n", utilisateurs[i].prenom, socket_fd);
            break;
        }
    }
}

void liberer_socket(int socket_fd) {
    for (int i = 0; i < MAX_FDS; i++) {
        if (utilisateurs[i].socket_fd == socket_fd) {
            printf("Utilisateur %s déconnecté de la socket %d\n", utilisateurs[i].prenom, socket_fd);
            utilisateurs[i].socket_fd = -1;
            break;
        }
    }
}

void envoyer_message_a_utilisateur(char *prenom, char *message) {
    for (int i = 0; i < MAX_FDS; i++) {
        if (strcmp(utilisateurs[i].prenom, prenom) == 0 && utilisateurs[i].socket_fd != -1) {
            int ret = write(utilisateurs[i].socket_fd, message, strlen(message));
            print_error(ret, "write");
            printf("Envoi du message à %s sur la socket %d\n", prenom, utilisateurs[i].socket_fd);
            break;
        }
    }
}
void write_int_as_message(int fd, int val) {
  int size = sizeof(val);
  int send = 0;
  
  while (send < size) {
    int temp_send = write(fd, ((char*)&val) + send, size - send);
    print_error(temp_send, "write_int");
    send += temp_send;
  }
}
void write_on_socket(int fd, char* s) {
  int size = strlen(s);
  int send = 0;

  while (send < size) {
    int temp_send = write(fd, s + send, size - send);
    print_error(temp_send, "write");
    send += temp_send;
  }
}

int main(int argc, char const *argv[]){
    //CREATE socket()
    int welcome_fd = socket(AF_INET,SOCK_STREAM,0);
    print_error(welcome_fd, "socket");


    //DEFINE Server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8081);
    inet_aton("127.0.0.1", &server_addr.sin_addr);

    //bind()
    int ret = bind(welcome_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    print_error(ret, "bind");

    //listen() -> demande de connexion / pas d'autre traffic
    ret = listen(welcome_fd, 2); // max deux connexions
    print_error(ret,"listen");

    ////////////////////////////////////////////////////////
    struct pollfd fds[MAX_FDS];
    fds[0].fd = welcome_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    
    for (int i = 1; i < MAX_FDS; i++){
        fds[i].fd = -1;
        fds[i].events = 0;
        fds[i].revents = 0;
    }

    //accept()
    while(1){
        poll(fds,MAX_FDS,-1);

        for(int i = 0; i<MAX_FDS; i++){
            
            if(fds[i].fd == welcome_fd && ((fds[i].revents & POLLIN) == POLLIN)){
                //accept()
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                printf("Waiting for client...\n");
                int client_fd = accept(welcome_fd, (struct sockaddr *)&client_addr, &len);
                print_error(ret, "accept");
                printf("New socket created on fd %d\n", client_fd);
                //Conserver la nouvelle socket -> passer a la fonction poll, mettre dans une case du tableau vide (-1)

                associer_utilisateur_a_socket(client_fd);  // Associer la nouvelle socket à un utilisateur
                

                // Ajouter la nouvelle connexion au tableau pollfd
                int added = 0;
                for (int j = 1; j < MAX_FDS; j++) {
                    if (fds[j].fd == -1) {  // Trouver un emplacement vide
                        fds[j].fd = client_fd;
                        fds[j].events = POLLIN;
                        added = 1;
                        break;
                    }
                }

                if (!added) {
                    printf("Too many clients, closing connection...\n");
                    close(client_fd);
                }
            }

            //Si un client à des données à lire
            if(fds[i].fd != -1 && fds[i].fd != welcome_fd && (fds[i].revents & POLLIN) == POLLIN){

                int client_fd = fds[i].fd;
                //printf("En attente de données");
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                int bytes_read = read(fds[i].fd, buffer, sizeof(buffer) - 1);
                
                if (bytes_read > 0) {
                    // Chercher le séparateur '\x02' dans le buffer
                    char *separator = strchr(buffer, '\x02');
                    if (separator != NULL) {
                        *separator = '\0';  // Remplace le séparateur par une fin de chaîne pour séparer
                        char *destinataire = buffer;  // Prénom est avant le séparateur
                        char *message = separator + 1;  // Message est après le séparateur

                        // Affichage du prénom et du message
                        printf("Received data from fd %d:\n", fds[i].fd);
                        printf("Destinataire: %s, Message: %s\n", destinataire, message);
                        
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
                                envoyer_message_a_utilisateur(destinataire, message);
                            }
                            else {
                                printf("Type de requête inconnu : %s\n", token);
                                envoyer_message_a_utilisateur(destinataire, "Requête inconnue.\n");
                            }
                        }

                        free(buffer);
                        // Ecrire le message sur la socket du client 
                    } 
                }
            }
            
            if (fds[i].fd != -1 && ((fds[i].revents & POLLHUP) == POLLHUP)) {
                printf("Client disconnected on fd %d\n", fds[i].fd);
                liberer_socket(fds[i].fd);  // Libérer la socket de l'utilisateur
                close(fds[i].fd);
                fds[i].fd = -1;
            }
        }
    }
    //Créer un tableau avec mathieu dedans et qui associe a mathieu le numéro de socket fd lorsque le client se connecte 
    //Lorsque je recois le message je lis le nom jusqu'à la variable de séparation et j'associe le nom a la socket pour écrire
    //les donnees sur la socket qui correspond au nom dans le tableau


    return 0;
}