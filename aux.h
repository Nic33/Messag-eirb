#ifndef AUX_H
#define AUX_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>  
#include <ctype.h>  
#include <sys/poll.h>
#include <stdbool.h>

/// Nombre maximum de descripteurs de fichiers surveillés
#define MAX_FDS 4
/// Port utilisé pour la connexion
#define PORT 8080
/// Adresse IP du serveur
#define ADDRESS "127.0.0.1"
/// Nom du fichier contenant les informations utilisateurs
#define USER_FILE "user.txt"
/// Taille du buffer utilisé pour les communications
#define BUFFER_SIZE 1024
/// Nombre maximum de serveurs
#define NB_SERVERS 10

/**
 * @brief Structure représentant un utilisateur connecté au système.
 */
typedef struct {
    char* prenom;      ///< Prénom de l'utilisateur.
    char* nom;         ///< Nom de l'utilisateur.
    char* mdp;         ///< Mot de passe de l'utilisateur.
    int socket_fd;     ///< Descripteur de socket de l'utilisateur.
    char* type;        ///< Type d'utilisateur (client, serveur, etc.).
    char* message;     ///< Dernier message envoyé par l'utilisateur.
    char* dest;        ///< Destination du message.
} Utilisateur;

/**
 * @brief Affiche un message d'erreur et arrête le programme en cas de problème.
 * 
 * @param result Code de retour de la fonction à vérifier.
 * @param s Message personnalisé à afficher en cas d'erreur.
 */
void print_error(int result, char* s);

/**
 * @brief Lit un message à partir d'un socket.
 * 
 * @param fd Descripteur du socket à lire.
 * @param buffer Buffer où le message sera stocké.
 * @param size Taille maximale du message à lire.
 * @return Retourne le nombre d'octets lus, ou -1 en cas d'erreur.
 */
int read_message_from_socket(int fd, char* buffer, int size);

/**
 * @brief Écrit un message sur un socket.
 * 
 * @param fd Descripteur du socket où écrire.
 * @param s Message à envoyer.
 */
void write_on_socket(int fd, char* s);

/**
 * @brief Lit un entier à partir d'un socket.
 * 
 * @param fd Descripteur du socket à lire.
 * @return Retourne l'entier lu ou -1 en cas d'erreur.
 */
int read_int_from_socket(int fd);

/**
 * @brief Écrit un entier sous forme de message sur un socket.
 * 
 * @param fd Descripteur du socket où écrire.
 * @param val Valeur entière à envoyer.
 */
void write_int_as_message(int fd, int val);

#endif
