#include <aux.h>

int main(int argc, char const *argv[]) {

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    print_error(client_fd, "socket"); 

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(PORT);     
    inet_aton(ADDRESS, &server_addr.sin_addr);

    int ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    print_error(ret, "connect");  

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
    user.message = malloc(BUFFER_SIZE * sizeof(char));
    user.dest = malloc(50 * sizeof(char));

    // Variables interaction terminal
    char input[BUFFER_SIZE];
    int identification = 0;

    bool file_data = false;
    FILE *file = NULL;

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

                strcpy(user.message, "empty");
                strcpy(user.dest, "empty");

                // Octets envoyé au server
                write_int_as_message(client_fd, strlen(user.prenom));
                write_on_socket(client_fd, user.prenom);

                write_int_as_message(client_fd, strlen(user.nom));
                write_on_socket(client_fd, user.nom);

                write_int_as_message(client_fd, strlen(user.mdp));
                write_on_socket(client_fd, user.mdp);

                write_int_as_message(client_fd, strlen(user.type));
                write_on_socket(client_fd, user.type);

                write_int_as_message(client_fd, strlen(user.message));
                write_on_socket(client_fd, user.message);

                write_int_as_message(client_fd, strlen(user.dest));
                write_on_socket(client_fd, user.dest);

                // Octets reçu du server
                int message_size = read_int_from_socket(client_fd);
                

                if (message_size <= 0 || message_size >= BUFFER_SIZE) {
                    printf("Invalid message size: %d\n", message_size);
                    break;
                }

                char* buffer = (char*)malloc(message_size + 1);
                int read_status = read_message_from_socket(client_fd, buffer, message_size);
                if (read_status <= 0) {
                    printf("Server disconnected or error occurred.\n");
                    free(buffer);
                    break;
                }
                buffer[message_size] = '\0'; 

                if (strcmp(buffer, "Connexion réussie") == 0) {
                    printf("Bienvenue %s !\n", user.prenom);
                    identification = 1;

                } else if (strcmp(buffer, "Connexion échouée") == 0) {
                    printf("Connexion échouée : Identifiants incorrects.\n");
                } else {
                    printf("Réponse inattendue du serveur : %s\n", buffer);
                }

                free(buffer);
            
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

                strcpy(user.message, "empty");
                strcpy(user.dest, "empty");

                // Octets envoyé au server
                write_int_as_message(client_fd, strlen(user.prenom));
                write_on_socket(client_fd, user.prenom);

                write_int_as_message(client_fd, strlen(user.nom));
                write_on_socket(client_fd, user.nom);

                write_int_as_message(client_fd, strlen(user.mdp));
                write_on_socket(client_fd, user.mdp);

                write_int_as_message(client_fd, strlen(user.type));
                write_on_socket(client_fd, user.type);

                write_int_as_message(client_fd, strlen(user.message));
                write_on_socket(client_fd, user.message);

                write_int_as_message(client_fd, strlen(user.dest));
                write_on_socket(client_fd, user.dest);

                // Octets reçu du server
                int message_size = read_int_from_socket(client_fd);
                

                if (message_size <= 0 || message_size >= BUFFER_SIZE) {
                    printf("Invalid message size: %d\n", message_size);
                    break;
                }

                char* buffer = (char*)malloc(message_size + 1);
                int read_status = read_message_from_socket(client_fd, buffer, message_size);
                if (read_status <= 0) {
                    printf("Server disconnected or error occurred.\n");
                    free(buffer);
                    break;
                }
                buffer[message_size] = '\0'; 
                printf("%s\n", buffer);

                free(buffer);
            
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

                write_int_as_message(client_fd, strlen(user.prenom));
                write_on_socket(client_fd, user.prenom);
                write_int_as_message(client_fd, strlen(user.nom));
                write_on_socket(client_fd, user.nom);
                write_int_as_message(client_fd, strlen(user.mdp));
                write_on_socket(client_fd, user.mdp);
                write_int_as_message(client_fd, strlen(user.type));
                write_on_socket(client_fd, user.type);
                write_int_as_message(client_fd, strlen(user.message));
                write_on_socket(client_fd, user.message);
                write_int_as_message(client_fd, strlen(user.dest));
                write_on_socket(client_fd, user.dest);
            
                free(user.prenom);
                free(user.nom);
                free(user.mdp);
                free(user.type);
                free(user.message);
                free(user.dest);

                break;  

            }else if (strcmp(input, "info") == 0) {

                strcpy(user.type, "info");

                write_int_as_message(client_fd, strlen(user.prenom));
                write_on_socket(client_fd, user.prenom);
                write_int_as_message(client_fd, strlen(user.nom));
                write_on_socket(client_fd, user.nom);
                write_int_as_message(client_fd, strlen(user.mdp));
                write_on_socket(client_fd, user.mdp);
                write_int_as_message(client_fd, strlen(user.type));
                write_on_socket(client_fd, user.type);
                write_int_as_message(client_fd, strlen(user.message));
                write_on_socket(client_fd, user.message);
                write_int_as_message(client_fd, strlen(user.dest));
                write_on_socket(client_fd, user.dest);

                // Octets reçu du server
                int message_size = read_int_from_socket(client_fd);
                if (message_size <= 0 || message_size >= BUFFER_SIZE) {
                    printf("Invalid message size: %d\n", message_size);
                    break;
                }

                char* buffer = (char*)malloc(message_size + 1);
                int read_status = read_message_from_socket(client_fd, buffer, message_size);
                if (read_status <= 0) {
                    printf("Server disconnected or error occurred.\n");
                    free(buffer);
                    break;
                }
                buffer[message_size] = '\0'; 
                printf("\n%s\n", buffer);

                free(buffer);

            }else if (strcmp(input, "info salon") == 0) {

                strcpy(user.type, "info salon");

                write_int_as_message(client_fd, strlen(user.prenom));
                write_on_socket(client_fd, user.prenom);
                write_int_as_message(client_fd, strlen(user.nom));
                write_on_socket(client_fd, user.nom);
                write_int_as_message(client_fd, strlen(user.mdp));
                write_on_socket(client_fd, user.mdp);
                write_int_as_message(client_fd, strlen(user.type));
                write_on_socket(client_fd, user.type);
                write_int_as_message(client_fd, strlen(user.message));
                write_on_socket(client_fd, user.message);
                write_int_as_message(client_fd, strlen(user.dest));
                write_on_socket(client_fd, user.dest);

                int message_size = read_int_from_socket(client_fd);
                

                if (message_size <= 0 || message_size >= BUFFER_SIZE) {
                    printf("Invalid message size: %d\n", message_size);
                    break;
                }

                char* buffer = (char*)malloc(message_size + 1);
                int read_status = read_message_from_socket(client_fd, buffer, message_size);
                if (read_status <= 0) {
                    printf("Server disconnected or error occurred.\n");
                    free(buffer);
                    break;
                }
                buffer[message_size] = '\0'; 
                printf("\n%s\n", buffer);

                free(buffer);

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
                strcpy(user.type, "new salon");

                strcpy(user.message, "empty");
                strcpy(user.dest, input);

                write_int_as_message(client_fd, strlen(user.prenom));
                write_on_socket(client_fd, user.prenom);
                write_int_as_message(client_fd, strlen(user.nom));
                write_on_socket(client_fd, user.nom);
                write_int_as_message(client_fd, strlen(user.mdp));
                write_on_socket(client_fd, user.mdp);
                write_int_as_message(client_fd, strlen(user.type));
                write_on_socket(client_fd, user.type);
                write_int_as_message(client_fd, strlen(user.message));
                write_on_socket(client_fd, user.message);
                write_int_as_message(client_fd, strlen(user.dest));
                write_on_socket(client_fd, user.dest);
                
            }else if (strcmp(input, "4") == 0) {

                printf("Enter le nom du salon:\n");

                memset(input, 0, sizeof(input));
                fgets(input, sizeof(input), stdin);
                size_t len = strlen(input);
                if (len > 0 && input[len - 1] == '\n') {
                    input[len - 1] = '\0';
                }
                strcpy(user.type, "join salon"); 

                strcpy(user.message, "empty");
                strcpy(user.dest, input);

                write_int_as_message(client_fd, strlen(user.prenom));
                write_on_socket(client_fd, user.prenom);
                write_int_as_message(client_fd, strlen(user.nom));
                write_on_socket(client_fd, user.nom);
                write_int_as_message(client_fd, strlen(user.mdp));
                write_on_socket(client_fd, user.mdp);
                write_int_as_message(client_fd, strlen(user.type));
                write_on_socket(client_fd, user.type);
                write_int_as_message(client_fd, strlen(user.message));
                write_on_socket(client_fd, user.message);
                write_int_as_message(client_fd, strlen(user.dest));
                write_on_socket(client_fd, user.dest);

                // Octets reçu du server
                int message_size = read_int_from_socket(client_fd);
                if (message_size <= 0 || message_size >= BUFFER_SIZE) {
                    printf("Invalid message size: %d\n", message_size);
                    break;
                }

                char* buffer = (char*)malloc(message_size + 1);
                int read_status = read_message_from_socket(client_fd, buffer, message_size);
                if (read_status <= 0) {
                    printf("Server disconnected or error occurred.\n");
                    free(buffer);
                    break;
                }
                buffer[message_size] = '\0'; 
                printf("\n%s\n", buffer);

                free(buffer);

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
                        write_int_as_message(client_fd, strlen(user.prenom));
                        write_on_socket(client_fd, user.prenom);
                        write_int_as_message(client_fd, strlen(user.nom));
                        write_on_socket(client_fd, user.nom);
                        write_int_as_message(client_fd, strlen(user.mdp));
                        write_on_socket(client_fd, user.mdp);
                        write_int_as_message(client_fd, strlen(user.type));
                        write_on_socket(client_fd, user.type);
                        write_int_as_message(client_fd, strlen(user.message));
                        write_on_socket(client_fd, user.message);
                        write_int_as_message(client_fd, strlen(user.dest));
                        write_on_socket(client_fd, user.dest);
                    }
                }
                
                else if (fds[1].revents & POLLIN) {

                    int message_size = read_int_from_socket(client_fd);
                    if (message_size <= 0 || message_size >= BUFFER_SIZE) {
                        printf("Invalid message size: %d\n", message_size);
                        break;
                    }

                    char* buffer = (char*)malloc(message_size + 1);
                    int read_status = read_message_from_socket(client_fd, buffer, message_size);
                    if (read_status <= 0) {
                        printf("Server disconnected or error occurred.\n");
                        free(buffer);
                        break;
                    }
                    buffer[message_size] = '\0'; 
                    printf("%s\n", buffer);

                    free(buffer);

                }
            }
        }else if(identification == 3){

            int poll_count = poll(fds, 2, -1);
            if (poll_count > 0) {

                if (fds[0].revents & POLLIN) {
                    strcpy(user.type, "message salon");

                    memset(input, 0, sizeof(input));
                    fgets(input, sizeof(input), stdin);
                    size_t len = strlen(input);
                    if (len > 0 && input[len - 1] == '\n') {
                        input[len - 1] = '\0';
                    }
                    strcpy(user.message, input);

                    if (strcmp(input, "menu") == 0) {

                        strcpy(user.type, "dec salon");
                        identification = 1;

                    }

                    write_int_as_message(client_fd, strlen(user.prenom));
                    write_on_socket(client_fd, user.prenom);
                    write_int_as_message(client_fd, strlen(user.nom));
                    write_on_socket(client_fd, user.nom);
                    write_int_as_message(client_fd, strlen(user.mdp));
                    write_on_socket(client_fd, user.mdp);
                    write_int_as_message(client_fd, strlen(user.type));
                    write_on_socket(client_fd, user.type);
                    write_int_as_message(client_fd, strlen(user.message));
                    write_on_socket(client_fd, user.message);
                    write_int_as_message(client_fd, strlen(user.dest));
                    write_on_socket(client_fd, user.dest);
                }
                
                else if (fds[1].revents & POLLIN) {

                    int message_size = read_int_from_socket(client_fd);
                    if (message_size <= 0 || message_size >= BUFFER_SIZE) {
                        printf("Invalid message size: %d\n", message_size);
                        break;
                    }

                    char* buffer = (char*)malloc(message_size + 1);
                    int read_status = read_message_from_socket(client_fd, buffer, message_size);
                    if (read_status <= 0) {
                        printf("Server disconnected or error occurred.\n");
                        free(buffer);
                        break;
                    }
                    buffer[message_size] = '\0'; 
                    printf("%s\n", buffer);

                    free(buffer);

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

                        file = fopen(filepath, "rb");
                        if (file == NULL) {
                            perror("Erreur lors de l'ouverture du fichier");
                            continue; 
                        }

                        fseek(file, 0L, SEEK_END);
                        long taille = ftell(file);
                        rewind(file);

                        write_int_as_message(client_fd, strlen(user.prenom));
                        write_on_socket(client_fd, user.prenom);
                        write_int_as_message(client_fd, strlen(user.nom));
                        write_on_socket(client_fd, user.nom);
                        write_int_as_message(client_fd, strlen(user.mdp));
                        write_on_socket(client_fd, user.mdp);
                        write_int_as_message(client_fd, strlen(user.type));
                        write_on_socket(client_fd, user.type);
                        write_int_as_message(client_fd, strlen(filepath));
                        write_on_socket(client_fd, filepath);
                        write_int_as_message(client_fd, strlen(user.dest));
                        write_on_socket(client_fd, user.dest);

                        char buffer[taille];
                        int message_size;

                        while ((message_size = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {

                            write_int_as_message(client_fd, strlen(user.prenom));
                            write_on_socket(client_fd, user.prenom);
                            write_int_as_message(client_fd, strlen(user.nom));
                            write_on_socket(client_fd, user.nom);
                            write_int_as_message(client_fd, strlen(user.mdp));
                            write_on_socket(client_fd, user.mdp);
                            write_int_as_message(client_fd, strlen(user.type));
                            write_on_socket(client_fd, user.type);
                            write_int_as_message(client_fd, strlen(buffer));
                            write_on_socket(client_fd, buffer);
                            write_int_as_message(client_fd, strlen(user.dest));
                            write_on_socket(client_fd, user.dest);

                        }

                        identification = 1;

                        fclose(file);
                        printf("Fichier envoyé avec succès.\n");
                    }
                }else if (fds[1].revents & POLLIN) {
                    
                    if (file_data == false){ // Création du fichier avec le bon nom

                        int message_size = read_int_from_socket(fds[1].fd);
                        if (message_size <= 0 || message_size >= BUFFER_SIZE) {
                            printf("Invalid message size: %d\n", message_size);
                            break;
                        }

                        char* buffer = (char*)malloc(message_size + 1);
                        int read_status = read_message_from_socket(fds[1].fd, buffer, message_size);
                        if (read_status <= 0) {
                            printf("Server disconnected or error occurred.\n");
                            free(buffer);
                            break;
                        }
                        buffer[message_size] = '\0'; 

                        file = fopen(buffer, "wb");
                        if (file == NULL) {
                            perror("Erreur lors de la création du fichier");
                        }

                        printf("Création du fichier : %s\n", buffer);

                        file_data = true;

                        free(buffer);

                    } else {

                        int message_size;

                        while ((message_size = read_int_from_socket(fds[1].fd)) > 0) {

                            char* buffer = (char*)malloc(message_size + 1);
                            int read_status = read_message_from_socket(fds[1].fd, buffer, message_size);
                            if (read_status <= 0) {
                                printf("Server disconnected or error occurred.\n");
                                free(buffer);
                                break;
                            }
                            buffer[message_size] = '\0'; 

                            printf("Octets lus: %d\n", message_size);

                            size_t bytesWritten = fwrite(buffer, sizeof(char), message_size, file);
                            if (bytesWritten != message_size) {
                                perror("Erreur lors de l'écriture dans le fichier");
                            } else {
                                printf("Écriture réussie de %zu octets\n", bytesWritten);
                                break;
                            }

                            free(buffer);
                        }

                        fclose(file);

                        identification = 1;
                        file_data = false;
                    }
                }
            }
        }
    }
    close(client_fd);  
    return 0;
}