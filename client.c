#include <aux.h>
#include <ctype.h>  
#include <sys/poll.h>
#include <termios.h> 

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
    memset(input, 0, sizeof(input));

    while (1) {

        if(identification == 0){
            printf("\nQuitter tapez 'exit'\n");
            printf("Identification tapez '1'\n");
            printf("Nouvel utilisateur tapez '2'\n");

            memset(input, 0, sizeof(input));
            fgets(input, sizeof(input), stdin);
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0';
                len--;
            }
            if (strcmp(input, "exit") == 0) {
                printf("Exiting...\n");
                break;

            }else if (strcmp(input, "1") == 0) {

                strcpy(user.type, "connect");
                
                memset(input, 0, sizeof(input));
                printf("Prénom : \n");
                fgets(input, sizeof(input), stdin);
                size_t len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.prenom, input); 

                memset(input, 0, sizeof(input));
                printf("Nom : \n");
                fgets(input, sizeof(input), stdin);
                len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.nom, input); 

                memset(input, 0, sizeof(input));
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
              
                char buffer[1024];
                ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer));
                if (bytes_received < 0) {
                    perror("Erreur lors de la lecture de la réponse");
                    close(client_fd);
                    exit(EXIT_FAILURE);
                }

                buffer[bytes_received] = '\0';

                if (strcmp(buffer, "Connexion réussie") == 0) {
                    printf("Bienvenue %s !\n", user.prenom);

                    identification = 1;

                } else if (strcmp(buffer, "Connexion échouée") == 0) {
                    printf("Connexion échouée : Identifiants incorrects.\n");
                } else {
                    printf("Réponse inattendue du serveur : %s\n", buffer);
                }
            
            }else if (strcmp(input, "2") == 0) {

                strcpy(user.type, "new");

                memset(input, 0, sizeof(input));
                printf("Prénom : \n");
                fgets(input, sizeof(input), stdin);
                size_t len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.prenom, input); 

                memset(input, 0, sizeof(input));
                printf("Nom : \n");
                fgets(input, sizeof(input), stdin);
                len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.nom, input); 

                memset(input, 0, sizeof(input));
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
                printf("Valeur non reconnue\n");
            }
        }
        else if (identification == 1){
            
            printf("\nQuitter tapez 'exit'\n");
            printf("Info utilisateurs tapez 'info'\n");
            printf("Info salons tapez 'info salon'\n\n");
            printf("Discuter avec un destinataire tapez '1'\n");
            printf("Envoyer un fichier à un destinataire tapez '2'\n");
            printf("Créer un salon tapez '3'\n");
            printf("Rejoindre un salon tapez '4'\n");

            memset(input, 0, sizeof(input));
            fgets(input, sizeof(input), stdin);
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0';
            }

            if (strcmp(input, "exit") == 0) {
                printf("Exiting client...\n");

                strcpy(user.type, "deconnexion");

                size_t lengths[] = {strlen(user.prenom) + 1, strlen(user.nom) + 1, strlen(user.mdp) + 1, 
                    strlen(user.type) + 1, strlen(user.message) + 1, strlen(user.dest) + 1};

                write(client_fd, lengths, sizeof(lengths));
                write(client_fd, user.prenom, lengths[0]);
                write(client_fd, user.nom, lengths[1]);
                write(client_fd, user.mdp, lengths[2]);
                write(client_fd, user.type, lengths[3]);
                write(client_fd, user.message, lengths[4]);
                write(client_fd, user.dest, lengths[5]);
            
                free(user.prenom);
                free(user.nom);
                free(user.mdp);
                free(user.type);
                free(user.message);
                free(user.dest);

                break;  

            }if (strcmp(input, "info") == 0) {

                strcpy(user.type, "info");

                size_t lengths[] = {strlen(user.prenom) + 1, strlen(user.nom) + 1, strlen(user.mdp) + 1, 
                    strlen(user.type) + 1, strlen(user.message) + 1, strlen(user.dest) + 1};

                write(client_fd, lengths, sizeof(lengths));
                write(client_fd, user.prenom, lengths[0]);
                write(client_fd, user.nom, lengths[1]);
                write(client_fd, user.mdp, lengths[2]);
                write(client_fd, user.type, lengths[3]);
                write(client_fd, user.message, lengths[4]);
                write(client_fd, user.dest, lengths[5]);

                char buffer[1024];
                size_t bytes_received = read(client_fd, buffer, sizeof(buffer));
                if (bytes_received < 0) {
                    perror("Erreur lors de la lecture de la réponse");
                    close(client_fd);
                    exit(EXIT_FAILURE);
                }
                buffer[bytes_received] = '\0';
                printf("\n%s\n", buffer);

            }else if (strcmp(input, "info salon") == 0) {

                strcpy(user.type, "info_salon");

                size_t lengths[] = {strlen(user.prenom) + 1, strlen(user.nom) + 1, strlen(user.mdp) + 1, 
                    strlen(user.type) + 1, strlen(user.message) + 1, strlen(user.dest) + 1};

                write(client_fd, lengths, sizeof(lengths));
                write(client_fd, user.prenom, lengths[0]);
                write(client_fd, user.nom, lengths[1]);
                write(client_fd, user.mdp, lengths[2]);
                write(client_fd, user.type, lengths[3]);
                write(client_fd, user.message, lengths[4]);
                write(client_fd, user.dest, lengths[5]);

                char buffer[1024];
                size_t bytes_received = read(client_fd, buffer, sizeof(buffer));
                if (bytes_received < 0) {
                    perror("Erreur lors de la lecture de la réponse");
                    close(client_fd);
                    exit(EXIT_FAILURE);
                }
                buffer[bytes_received] = '\0';
                printf("\n%s\n", buffer);

            }else if (strcmp(input, "1") == 0) {

                printf("Enter le prénom:\n");

                memset(input, 0, sizeof(input));
                fgets(input, sizeof(input), stdin);
                size_t len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.dest, input); 

                identification = 2;
                printf("Tapez un message:\n");

            }else if (strcmp(input, "2") == 0) {

                printf("Enter le prénom:\n");

                memset(input, 0, sizeof(input));
                fgets(input, sizeof(input), stdin);
                size_t len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.dest, input); 

                identification = 4;
                printf("Entrez '<chemin_du_fichier>' pour envoyer un fichier:\n");

            }else if (strcmp(input, "3") == 0) {

                printf("Enter le nom de votre nouveau salon:\n");

                memset(input, 0, sizeof(input));
                fgets(input, sizeof(input), stdin);
                size_t len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.type, "new_salon");

                strcpy(user.message, "");
                strcpy(user.dest, input);

                size_t lengths[] = {strlen(user.prenom) + 1, strlen(user.nom) + 1, strlen(user.mdp) + 1, 
                    strlen(user.type) + 1, strlen(user.message) + 1, strlen(user.dest) + 1};

                write(client_fd, lengths, sizeof(lengths));

                write(client_fd, user.prenom, lengths[0]);
                write(client_fd, user.nom, lengths[1]);
                write(client_fd, user.mdp, lengths[2]);
                write(client_fd, user.type, lengths[3]);
                write(client_fd, user.message, lengths[4]);
                write(client_fd, user.dest, lengths[5]);
                
            }else if (strcmp(input, "4") == 0) {

                printf("Enter le nom du salon:\n");

                memset(input, 0, sizeof(input));
                fgets(input, sizeof(input), stdin);
                size_t len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.type, "join_salon"); 

                strcpy(user.message, "");
                strcpy(user.dest, input);

                size_t lengths[] = {strlen(user.prenom) + 1, strlen(user.nom) + 1, strlen(user.mdp) + 1, 
                    strlen(user.type) + 1, strlen(user.message) + 1, strlen(user.dest) + 1};

                write(client_fd, lengths, sizeof(lengths));

                write(client_fd, user.prenom, lengths[0]);
                write(client_fd, user.nom, lengths[1]);
                write(client_fd, user.mdp, lengths[2]);
                write(client_fd, user.type, lengths[3]);
                write(client_fd, user.message, lengths[4]);
                write(client_fd, user.dest, lengths[5]);

                char buffer[1024];
                ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer));
                if (bytes_received < 0) {
                    perror("Erreur lors de la lecture de la réponse");
                    close(client_fd);
                    exit(EXIT_FAILURE);
                }

                buffer[bytes_received] = '\0';
                printf("%s\n", buffer);

                if (strcmp(buffer, "Pas de salon") == 0) {
                    printf("\nCe salon n'existe pas\n");                
                }else{
                    printf("\nBienvenu dans le canal %s\n", user.dest);
                    printf("Vous povez envoyer des messages !'\n");
                
                    identification = 3;
                }

            }else{
                printf("Valeur non reconnue\n");
            }
        }else if(identification == 2){

            int poll_count = poll(fds, 2, -1);
            if (poll_count > 0) {

                if (fds[0].revents & POLLIN) {
                    strcpy(user.type, "message");

                    memset(input, 0, sizeof(input));
                    fgets(input, sizeof(input), stdin);
                    size_t len = strlen(input);
                    if (len > 0 && input[len - 1] == '\n') {
                        input[len - 1] = '\0';
                    }
                    strcpy(user.message, input);

                    if (strcmp(input, "menu") == 0) {
                        identification = 1;

                    }
                    else {

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
                
                else if (fds[1].revents & POLLIN) {
                    char message[BUFFER_SIZE];

                    ssize_t message_bytes = read(fds[1].fd, message, sizeof(message));
                    if (message_bytes <= 0) {
                        printf("La personne s'est déconnectée\n");
                        identification = 1;
                    }

                    message[message_bytes] = '\0';  

                    printf("%s\n", message);

                }
            }
        }else if(identification == 3){

            int poll_count = poll(fds, 2, -1);
            if (poll_count > 0) {

                if (fds[0].revents & POLLIN) {
                    strcpy(user.type, "message_salon");

                    memset(input, 0, sizeof(input));
                    fgets(input, sizeof(input), stdin);
                    size_t len = strlen(input);
                    if (len > 0 && input[len - 1] == '\n') {
                        input[len - 1] = '\0';
                    }
                    strcpy(user.message, input);

                    if (strcmp(input, "menu") == 0) {

                        strcpy(user.type, "dec_salon");

                        size_t lengths[] = {strlen(user.prenom) + 1, strlen(user.nom) + 1, strlen(user.mdp) + 1, 
                            strlen(user.type) + 1, strlen(user.message) + 1, strlen(user.dest) + 1};

                        write(client_fd, lengths, sizeof(lengths));
                        write(client_fd, user.prenom, lengths[0]);
                        write(client_fd, user.nom, lengths[1]);
                        write(client_fd, user.mdp, lengths[2]);
                        write(client_fd, user.type, lengths[3]);
                        write(client_fd, user.message, lengths[4]);
                        write(client_fd, user.dest, lengths[5]);
                    
                        identification = 1;

                    }else {
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
                
                else if (fds[1].revents & POLLIN) {
                    char message[256];

                    ssize_t message_bytes = read(fds[1].fd, message, sizeof(message));
                    if (message_bytes <= 0) {
                        printf("La personne s'est déconnecté\n");
                        identification = 1;
                    }
                    message[message_bytes] = '\0';  
                    printf("%s\n", message);
                }
            }
        }else if(identification == 4){ // fichier

            int poll_count = poll(fds, 2, -1);
            if (poll_count > 0) {

                if (fds[0].revents & POLLIN) {
                    strcpy(user.type, "file");

                    memset(input, 0, sizeof(input));
                    fgets(input, sizeof(input), stdin);
                    size_t len = strlen(input);
                    if (len > 0 && input[len - 1] == '\n') {
                        input[len - 1] = '\0';
                    }
                    strcpy(user.message, input);

                    if (strcmp(input, "menu") == 0) {
                        identification = 1;

                    }else {
                        
                        char *filepath = input;

                        FILE *file = fopen(filepath, "rb");
                        if (file == NULL) {
                            perror("Erreur lors de l'ouverture du fichier");
                            continue; 
                        }

                        fseek(file, 0L, SEEK_END);
                        rewind(file);

                        size_t lengths[] = {
                            strlen(user.prenom) + 1,
                            strlen(user.nom) + 1,
                            strlen(user.mdp) + 1,
                            strlen(user.type) + 1,
                            strlen(filepath) + 1, 
                            strlen(user.dest) + 1
                        };

                        write(client_fd, lengths, sizeof(lengths));

                        write(client_fd, user.prenom, lengths[0]);
                        write(client_fd, user.nom, lengths[1]);
                        write(client_fd, user.mdp, lengths[2]);
                        write(client_fd, user.type, lengths[3]);
                        write(client_fd, filepath, lengths[4]);
                        write(client_fd, user.dest, lengths[5]);

                        char buffer[1];
                        size_t bytes_read;
                        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                            write(client_fd, buffer, bytes_read);
                        }

                        fclose(file);
                        printf("Fichier envoyé avec succès.\n");

                        identification = 1;
                    }
                }else if (fds[1].revents & POLLIN) {
                    
                    char message[8];

                    ssize_t message_bytes = read(fds[1].fd, message, sizeof(message));
                    if (message_bytes <= 0) {
                        printf("La personne s'est déconnectée\n");
                        identification = 1;
                    }

                    message[message_bytes] = '\0';  
                        
                    char *filename = message; 
                    filename = strtok(filename, "\n");

                    FILE *file = fopen(filename, "wb");
                    if (file == NULL) {
                        perror("Erreur lors de la création du fichier");
                    }

                    printf("Création du fichier test : %s\n", filename);

                    // Lire la taille du fichier
                    long filesize = 11;

                    printf("\nTaille du fichier à recevoir: %ld octets\n", filesize);

                    char file_buffer[1];
                    ssize_t bytes_read;
                    long total_bytes_read = 0;
                    while (total_bytes_read < filesize && (bytes_read = read(fds[1].fd, file_buffer, sizeof(file_buffer))) > 0) {
                        printf("Octets lus: %zd\n", bytes_read);
                        fwrite(file_buffer, 1, bytes_read, file);
                        total_bytes_read += bytes_read;
                        printf("Total des octets reçus: %ld/%ld\n", total_bytes_read, filesize);
                    }

                    if (total_bytes_read == filesize) {
                        printf("Fichier %s reçu avec succès.\n", filename);
                    } else {
                        printf("Erreur : fichier incomplet reçu. Seulement %ld/%ld octets reçus.\n", total_bytes_read, filesize);
                    }

                    fclose(file);

                    identification = 1;

                }
            }
        }
    }

    close(client_fd);  
    return 0;
}