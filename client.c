#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024 // Taille maximale du buffer pour les messages envoyés/ reçus

// Déclarations des fonctions
void print_error(int fd, char *s);
void write_on_socket(int fd, char *s);
void write_int_on_socket(int fd, int val);
void read_from_socket(int fd);

// Fonction d'affichage et gestion des erreurs
// Elle vérifie si une fonction a renvoyé un code d'erreur (fd < 0)
// Si c'est le cas, elle affiche le message d'erreur et arrête le programme
void print_error(int fd, char *s) {
    if (fd < 0) {
        perror(s);   // Affiche le message d'erreur associé
        exit(EXIT_FAILURE);  // Arrête le programme
    }
}

// Fonction d'écriture d'un message sur la socket
// Elle envoie la chaîne de caractères `s` sur la socket identifiée par `fd`
void write_on_socket(int fd, char *s) {
    int size = strlen(s);  // Calcule la taille du message à envoyer

    int nb_written = 0;    // Nombre d'octets déjà envoyés
    // Boucle pour s'assurer que tout le message est envoyé
    while (nb_written < size) {
        // Écriture sur la socket, en commençant à l'endroit où l'écriture s'est arrêtée
        int ret = write(fd, s + nb_written, size - nb_written);
        print_error(ret, "write");  // Vérifie s'il y a une erreur pendant l'écriture
        nb_written += ret;  // Augmente le nombre d'octets envoyés
    }
}

// Fonction d'écriture d'un entier sur la socket
// Envoie l'entier `val` sur la socket identifiée par `fd`
void write_int_on_socket(int fd, int val) {
    int nb_written = 0;  // Nombre d'octets déjà envoyés
    // Boucle pour s'assurer que tout l'entier est envoyé (car un entier peut être envoyé en plusieurs parties)
    while (nb_written < sizeof(int)) {
        // Écriture de l'entier sur la socket
        int ret = write(fd, (char *)&val + nb_written, sizeof(int) - nb_written);
        print_error(ret, "write val on socket");  // Vérifie s'il y a une erreur pendant l'écriture
        nb_written += ret;  // Augmente le nombre d'octets envoyés
    }
}

void read_from_socket(int fd) {
    char buffer[BUFFER_SIZE];
    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';  // Ajouter un caractère nul à la fin du message reçu

        // Extraire le prénom et le message (séparé par '\x02')
        char *separator = strchr(buffer, '\x02');
        if (separator != NULL) {
            *separator = '\0';  // Remplacer le séparateur par '\0' pour couper la chaîne
            char *prenom = buffer;  // Le prénom est avant le séparateur
            char *message = separator + 1;  // Le message est après le séparateur
            
            // Afficher le prénom et le message reçus
            printf("\nMessage from %s: %s\n", prenom, message);
        }
        else {
            printf("\nMessage :%s", buffer);
        }
    } else if (bytes_read == 0) {   
        // Si le serveur ferme la connexion
        printf("Server closed the connection\n");
        exit(EXIT_SUCCESS);
    } else {
        perror("read");  // Affiche une erreur en cas d'échec de la lecture
    }
}

int main(int argc, char const *argv[]) {
    // Création de la socket (domaine AF_INET, type SOCK_STREAM, protocole 0)
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    print_error(fd, "socket");  // Vérifie si la socket a été créée avec succès

    // Définition de l'adresse du serveur (côté client)
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;          // Protocole IPv4
    server_addr.sin_port = htons(8080);        // Numéro de port (converti en format réseau)
    inet_aton("127.0.0.1", &server_addr.sin_addr);  // Adresse IP du serveur (localhost)

    // Connexion au serveur
    int ret = connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    print_error(ret, "connect");  // Vérifie si la connexion a été établie avec succès

    char prenom[BUFFER_SIZE];
    // Lecture du prénom de l'utilisateur
    printf("Enter your name: ");
    fgets(prenom, BUFFER_SIZE, stdin);  // Lit le prénom de l'utilisateur

    // Suppression du caractère de nouvelle ligne '\n' ajouté par fgets()
    size_t prenom_len = strlen(prenom);
    if (prenom[prenom_len - 1] == '\n') {
        prenom[prenom_len - 1] = '\0';  // Remplace le '\n' par '\0' (fin de chaîne)
        prenom_len--;  // Ajuste la longueur du prénom
    }

    // Configuration de poll pour surveiller à la fois stdin et la socket
    struct pollfd fds[2];
    fds[0].fd = 0;  // 0 est stdin (entrée utilisateur)
    fds[0].events = POLLIN;
    fds[1].fd = fd;  // La socket du serveur
    fds[1].events = POLLIN;

    while (1) {
        int poll_count = poll(fds, 2, -1);
        if (poll_count > 0) {
            // Si des données sont disponibles sur stdin
            if (fds[0].revents & POLLIN) {
                // Buffer pour stocker le message à envoyer
                char buffer[BUFFER_SIZE];
                char message[BUFFER_SIZE + BUFFER_SIZE];  // Pour stocker [prenom][séparateur][message]

                // Lire le message depuis le terminal
                printf("\nEnter the message to send (type 'exit' to quit): ");
                fgets(buffer, BUFFER_SIZE, stdin);  // Lit un message de l'utilisateur

                // Suppression du caractère de nouvelle ligne '\n' ajouté par fgets()
                size_t message_len = strlen(buffer);
                if (buffer[message_len - 1] == '\n') {
                    buffer[message_len - 1] = '\0';  // Remplace le \n par \0 (fin de chaîne)
                    message_len--;  // Ajuste la longueur du message
                }
                // Vérifie si l'utilisateur veut quitter le programme
                if (strcmp(buffer, "exit") == 0) {
                    printf("Exiting client...\n");
                    break;  // Sort de la boucle et termine le programme
                }
                
                // Formatage du message à envoyer : [prenom][séparateur][message]
                snprintf(message, sizeof(message), "%s\x02%s", prenom, buffer);
                write_on_socket(fd, message);
            }

            // Si des données sont disponibles sur la socket
            if (fds[1].revents & POLLIN) {
                read_from_socket(fd);  // Appel de la fonction pour lire les données
            }
        }
    }

    close(fd);  // Fermer la socket lorsque le programme se termine
    return 0;
}
