#include <aux.h>
#include <ctype.h>  
#include <sys/poll.h>


int main(int argc, char const *argv[]) {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    print_error(client_fd, "socket"); 

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(PORT);     
    inet_aton(ADDRESS, &server_addr.sin_addr);

    int ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    print_error(ret, "connect");  

    int identification = 0;

    struct pollfd fds[2];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[1].fd = client_fd;
    fds[1].events = POLLIN;

    Utilisateur user;
    user.prenom = malloc(50 * sizeof(char));
    user.nom = malloc(50 * sizeof(char));
    user.mdp = malloc(50 * sizeof(char));
    user.type = malloc(20 * sizeof(char));
    user.message = malloc(256 * sizeof(char));
    user.dest = malloc(50 * sizeof(char));

    char input[256];

    while (1) {

        if(identification == 0){
            printf("Exit tape 'quit'\n");
            printf("Identification tape '1'\n");
            printf("New user tape '2'\n");

            fgets(input, sizeof(input), stdin);
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0';
                len--;
            }
            if (strcmp(input, "quit") == 0) {
                printf("Exiting...\n");
                break;

            }else if (strcmp(input, "1") == 0) {

                strcpy(user.type, "connect");

                printf("Prénom : \n");
                fgets(input, sizeof(input), stdin);
                size_t len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.prenom, input); 

                printf("Nom : \n");
                fgets(input, sizeof(input), stdin);
                len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.nom, input); 

                printf("Mot de passe : \n");
                fgets(input, sizeof(input), stdin);
                len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.mdp, input);

                strcpy(user.message, "");
                strcpy(user.dest, "");

                size_t lengths[] = {strlen(user.prenom) + 1, strlen(user.nom) + 1, strlen(user.mdp) + 1, 
                    strlen(user.type) + 1, strlen(user.message) + 1, strlen(user.dest) + 1};

                write(client_fd, lengths, sizeof(lengths));

                write(client_fd, user.prenom, lengths[0]);
                write(client_fd, user.nom, lengths[1]);
                write(client_fd, user.mdp, lengths[2]);
                write(client_fd, user.type, lengths[3]);
                write(client_fd, user.message, lengths[4]);
                write(client_fd, user.dest, lengths[5]);
              
                char buffer[256];
                ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
                if (bytes_received < 0) {
                    perror("Erreur lors de la lecture de la réponse");
                    close(client_fd);
                    exit(EXIT_FAILURE);
                }

                buffer[bytes_received] = '\0';
                printf("Réponse du serveur: %s\n", buffer);

                if (strcmp(buffer, "Connexion réussie") == 0) {
                    printf("Connexion réussie : Bienvenue %s !\n", user.prenom);
                    identification = 1;
                } else if (strcmp(buffer, "Connexion échouée") == 0) {
                    printf("Connexion échouée : Identifiants incorrects.\n");
                } else {
                    printf("Réponse inattendue du serveur : %s\n", buffer);
                }
            
            }else if (strcmp(input, "2") == 0) {

                strcpy(user.type, "new");

                printf("Prénom : \n");
                fgets(input, sizeof(input), stdin);
                size_t len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.prenom, input); 

                printf("Nom : \n");
                fgets(input, sizeof(input), stdin);
                len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.nom, input); 

                printf("Mot de passe : \n");
                fgets(input, sizeof(input), stdin);
                len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.mdp, input);

                strcpy(user.message, "");
                strcpy(user.dest, "");

                size_t lengths[] = {strlen(user.prenom) + 1, strlen(user.nom) + 1, strlen(user.mdp) + 1, 
                    strlen(user.type) + 1, strlen(user.message) + 1, strlen(user.dest) + 1};

                // Envoi des longueurs
                write(client_fd, lengths, sizeof(lengths));
                // Envoi des chaînes de caractères
                write(client_fd, user.prenom, lengths[0]);
                write(client_fd, user.nom, lengths[1]);
                write(client_fd, user.mdp, lengths[2]);
                write(client_fd, user.type, lengths[3]);
                write(client_fd, user.message, lengths[4]);
                write(client_fd, user.dest, lengths[5]);
                
            }else{
                printf("I don't understand\n");
            }
        }
        else if (identification == 1){
            
            printf("Exit tape 'exit'\n");
            printf("Enter a destinataire: ");

            fgets(input, sizeof(input), stdin);
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0';
            }
            strcpy(user.dest, input); 

            if (strcmp(input, "exit") == 0) {
                printf("Exiting client...\n");

                free(user.prenom);
                free(user.nom);
                free(user.mdp);
                free(user.type);
                free(user.message);
                free(user.dest);

                break;  
            }

            identification = 2;
        }
        else if(identification == 2){

            int poll_count = poll(fds, 2, -1);
            if (poll_count > 0) {

                if (fds[0].revents & POLLIN) {

                    strcpy(user.type, "message");

                    printf("Enter the message to send (type 'menu' to go back to menu):\n");

                    fgets(input, sizeof(input), stdin);
                    size_t len = strlen(input);
                    if (len > 0 && input[len - 1] == '\n') {
                        input[len - 1] = '\0';
                    }
                    strcpy(user.message, input);

                    if (strcmp(input, "menu") == 0) {
                        identification = 1;
                    } else {
                        size_t lengths[] = {strlen(user.prenom) + 1, strlen(user.nom) + 1, strlen(user.mdp) + 1, 
                            strlen(user.type) + 1, strlen(user.message) + 1, strlen(user.dest) + 1};

                        write(client_fd, lengths, sizeof(lengths));
                        write(client_fd, user.prenom, lengths[0]);
                        write(client_fd, user.nom, lengths[1]);
                        write(client_fd, user.mdp, lengths[2]);
                        write(client_fd, user.type, lengths[3]);
                        write(client_fd, user.message, lengths[4]);
                        write(client_fd, user.dest, lengths[5]);
                    }
                }
                
                if (fds[1].revents & POLLIN) {
                    char buffer[256];
                    ssize_t bytes_received = read(fds[1].fd, buffer, sizeof(buffer) - 1);

                    if (bytes_received < 0) {
                        perror("Erreur lors de la lecture du message reçu");
                        close(client_fd);
                        exit(EXIT_FAILURE);
                    } else if (bytes_received == 0) {
                        printf("Connexion au serveur perdue.\n");
                        break;
                    } else {
                        buffer[bytes_received] = '\0';
                        printf("Message reçu d'un autre client : %s\n", buffer);
                    }
                }
            }
        }
    }

    close(client_fd);  
    return 0;
}
