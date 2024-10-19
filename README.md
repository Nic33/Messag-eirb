# Messag-eirb

Pour ce projet, nous avons implémenté une messagerie client/serveur. Ce README vient compléter le rapport PDF que nous avons placé à la racine de ce projet.
Dans ce fichier, nous allons exclusivement parler des lignes de commande à exécuter pour utiliser le produit.
Nous aborderons également brièvement les résultats que nous obtenons et comment le code fonctionne pour parvenir à ces résultats. Nous rentrons plus dans les détails dans le rapport.

La branche main correspond à la dernière version de notre projet.

De plus, nous avons commenté le code. Pour l’ouvrir, il faut aller dans le dossier doc, puis ouvrir le fichier index.html dans un navigateur.

## Sommaire

- Les executables
- La documentation
- La phase de connexion et déconnexion
- Les informations utilisateur et des salons
- Discussion entre utilisateurs
- Discussion dans un salon
- L'envoie de fichier

## Executables

Nous avons créé un fichier Makefile pour :

- Générer la documentation du projet,
- Créer un exécutable client1,
- Créer un dossier client2 avec son exécutable, qui nous sera utile pour démontrer que l'envoi de fichier fonctionne correctement.

Les commandes sont les suivantes :

make  

Pour nettoyer le répertoire :  

make clean

## La documentation

La documentation est disponible dans le dossier doc. Il suffit de drag and drop le fichier index.html dans un navigateur.

Nous avons séparé les fonctions en 3 parties. Nous avons le fichier client.c, le fichier server.c avec ses propres fonctions, et les deux fichiers aux.h et aux.c qui possèdent les fonctions communes au client et au serveur.

## La phase de connexion et de déconnexion

Dans un premier temps, exécutez ./server, puis dans un autre terminal, exécutez ./client1.
Pour des raisons de sécurité, nous avons mis en place un système d'identification. Vous devez donc commencer par créer un compte : tapez 2, puis renseignez vos informations.
Ces informations seront enregistrées dans un fichier user.txt qui servira de base de données. Ensuite, tapez 1 pour entrer vos informations et vous connecter.

Vous avez la possibilité que quitter le menu d'identification en tapant exit.

Pour des raisons de sécurité, il est impossible de se connecter si l'un des trois champs (prénom, nom, mot de passe) ne correspond pas à ceux de la base de données. Par exemple, si vous avez le compte suivant :

Prénom : Joachim   
Nom : Bruneau   
Mot de passe : password  

Cela ne fonctionnera pas si vous essayez de vous connecter avec un champ différent de ceux enregistrés dans la base de données, par exemple : 

Prénom : oachim   
Nom : Bruneau   
Mot de passe : password  

De plus, si vous êtes déjà connecté, il est impossible qu'un autre utilisateur se connecte avec votre compte.

Vous êtes désormais connecté sur le menu utilisateur !

## Les informations utilisateur et des salons

Vous pouvez voir les informations utilisateur en tapant info. Pour voir les informations des salons tapez info salon.

## Discussion entre utilisateurs

Pour discuter avec un autre utilisateur, vous devez taper 1, puis renseigner le prénom de l'utilisateur avec lequel vous voulez discuter. L'autre utilisateur doit faire la même chose. Le prénom nous sert d'identifiant (ID).

Pour quitter une conversation, vous pouvez taper menu pour revenir au menu utilisateur.

## L'envoie de fichier

Pour envoyer un fichier, déplacez-vous dans le répertoire client2, exécutez ./client2, puis identifiez-vous. L'objectif est de vérifier que le fichier sera bien créé dans ce répertoire.

Pour envoyer un fichier, tapez 2, puis renseignez le prénom de l'autre utilisateur comme pour une discussion. Faites de même avec le client1.
Vous pouvez ensuite envoyer le fichier test.txt déjà présent à la racine du projet.

## Discussion dans un salon

Pour discuter dans un salon, vous pouvez connecter d'autres utilisateurs.
Pour créer un salon, il faut taper 3 dans le menu et donner un nom au salon.
Une fois le salon créé, vous pouvez y connecter tous les utilisateurs en tapant 4 et en renseignant le nom du salon.

Vous pouvez discuter, et pour revenir au menu, tapez menu.