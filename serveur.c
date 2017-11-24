/**
 * @file serveur.c
 * @author Florian GUILLEMEAU & Sylvain LANGENBRONN
 *
 * ./pg_name IPv4_addr port_number
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include "communication.h"
#include <netinet/in.h>

//Structure contenant un ip, et l'ip suivant, appartenant au même hash
typedef struct table_ip table_ip;
struct table_ip{
	char* ip;
	table_ip *ip_suivant;
};

//Structure contenant un hash, ses ip associés, et le hash suivant
typedef struct table_hash table_hash;
struct table_hash{
	table_ip *t_ip;
	char* hash;
	table_hash *hash_suivant;
};

//Pointeur sur le premier element de la table de hash
typedef struct table table;
struct table{
	table_hash *premier;
};

/**
 * @brief Initialise une table contenant les hash les ip associés à ces derniers
 */
table *init_DHT(){

	table *t = malloc(sizeof(*t));

	if(t == NULL){
		fprintf(stderr, "Erreur initialisation table hash");
		exit(1);
	}

	t->premier = NULL;

	return t;
}


/**
 * @brief Verifie l'existance du hash dans la liste
 *
 * @param t Liste contenant tous les hash et les ip
 * @param hash Valeur du hash recherchée
 *
 * @return la struct hash si on a match la valeur du hash, NULL sinon
 */
table_hash *existence_hash(table *t, char* hash){

    int i = 0;
    table_hash *temp = t->premier;

    //Si la liste est non vide
    if(t->premier != NULL){
        //On parcours toute la liste
        while(i == 0 && temp != NULL){
            //Jusqu'a trouver le hash voulu
            if(temp->hash == hash){
                i = 1;
            }
            else{
                //Ou qu'on atteigne la fin de la liste
                temp = temp->hash_suivant;
            }
        }
    }

    return temp;
}


/**
 * @brief Insert dans la liste des hash et des ip associés un nouvel ip
 *
 * @param t Liste contenant tous les hash et les ip enregistrés
 * @param ip Adresse ip à ajouter
 * @param hash Valeur du hash a associer à l'ip
 */
void insertion_DHT(table *t, char* ip, char* hash){

	//On créé le nouvel ip à inserer dans la liste
	table_ip *nouveau_ip = malloc(sizeof(*nouveau_ip));
	nouveau_ip->ip = ip;

	//On vérifie l'existence du hash demandé
	table_hash *temp = existence_hash(t, hash);

	//Si le hash demandé n'est pas trouvé on en créé un nouveau
	if(temp == NULL){
		//On créé un nouveau hash contenant la valeur du hash
		table_hash *nouveau_hash = malloc(sizeof(*nouveau_hash));
		nouveau_hash->hash_suivant = t->premier;
		nouveau_hash->hash = hash;
		//Et on la rattache au début de la liste
		t->premier = nouveau_hash;

		nouveau_ip->ip_suivant = NULL;
		//On rattache la struct contenant l'ip à la struct contenant le hash
		nouveau_hash->t_ip = nouveau_ip;
	}
	//Si la valeur du hash est trouvé dans la liste on y ajoute juste l'ip
	else{
		//On fait pointer la valeur suivant sur l'ancien premier ip
		nouveau_ip->ip_suivant = temp->t_ip;
		//On ajoute le nouvel ip en début de liste
		temp->t_ip = nouveau_ip;
	}
}

int main(int argc, char* argv[]){

	if(argc != 3){
		printf("usage: %s <adresse> <port>\n", argv[0]);
		exit(1);
	}

	struct in6_addr ip;//IP de l'hote donnée en paramètre
	int port;
	int nbMessage = 3;
	int socket;
	int i;
	struct sockaddr_in6 client;
	char buf[1024];

	//Récupèration de l'adresse donnée en paramètre si elle existe
	ip = recuperer_adresse(argv[1]);

	//Si == 0 alors le port n'est pas un nombre, sinon s'en est un
	if(verification_port(argv[2]) == 0){
		fprintf(stderr, "Le numéro de port \'%s\' n'est pas \
		un nombre\n", argv[2]);
		exit(1);
	}
	else{
		port = atoi(argv[2]);
	}

	/** Initialisation */
	socket = initSocket();
	initReception(socket, port, ip);
	
	for (i = 0; i < nbMessage; ++i){
		printf("Attente message %d\n", i);

		/** Reception Message */
		client = recevoir(socket, buf);
		
		/** Affichage */
		printf("Message recu: %s\n",buf);
	    printf("Longueur du message: %li\n",strlen(buf));

	    char adr_ip[INET_ADDRSTRLEN];
	    if(inet_ntop(AF_INET6,&client.sin6_addr,adr_ip,INET_ADDRSTRLEN)==NULL){
	        perror("inet_ntop\n");
	        exit(EXIT_FAILURE);
	    }
	    printf("Ip source: %s\n",adr_ip);
	    printf("Numero de port de l'expediteur: %d\n",client.sin6_port);
	}
	
	/** Fermeture */
	closeSocket(port);
	return 0;
}
