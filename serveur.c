/**
 * @file serveur.c
 * @author Florian GUILLEMEAU & Sylvain LANGENBRONN
 *
 * ./pg_name IPv4_addr port_number
 */

#include "communication.h"
#include <pthread.h>
#include <signal.h>

#define MAX_CONNEXION 10

//Structure contenant un ip, et l'ip suivant, appartenant au même hash
typedef struct table_ip table_ip;
struct table_ip{
	struct in6_addr ip;
	time_t t_inser;
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

// Structure de carnet d'adresse
typedef struct adresse{
	struct in6_addr ip;
	int port;
} adresse;

// Données à envoyer à la fonction keep alive
typedef struct keepAlive{
	adresse *adr;
	struct in6_addr ip;
	int socket;
} varKeepAlive;

adresse *carnetAdrServeurGlobale;

/**
 * @brief Initialise une table contenant les hash les ip associés 
 * à ces derniers
 *
 * @return Un pointeur initialisé qui permettra de connaitre le premier hash
 * enregsitré dans la table
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
			if(strcmp(temp->hash, hash)==0){
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
 * @brief Vérifie l'existence ou non du couple hash-ip passé en paramètre
 *
 * @param t Liste contenant les hash et les ip
 * @param hash Valeur du hash que l'on veut trouver
 * @param ip Adresse ip dont nous voulons savoir si elle est associé au hash
 *
 * @return 1 si le couple existe, 0 sinon
 */
int existence_couple(table* t, char* hash, struct in6_addr ip){

	int trouve = 0;
	table_hash* temp_hash = existence_hash(t, hash);
	//Pour enregister l'ip en chaine
	char ipstr[INET6_ADDRSTRLEN];
	ipToString(ip, ipstr);
	//Pour enregistrer les ip de la table en chaine
	char tempstr[INET6_ADDRSTRLEN];

	//Si le hash à été trouvé dans la table
	if(temp_hash != NULL){
		//On cherche l'ip
		table_ip* temp_ip = temp_hash->t_ip;
		//On continue de chercher tant que l'on a pas atteint la fin 
		//de la list des ip de ce hash ou que l'ip voulu n'a pas été trouvé
		while(temp_ip != NULL && trouve == 0){
			ipToString(temp_ip->ip, tempstr);
			//Comparaison de l'ip donnée en paramètre de la fonction
			//Et les ip trouvés dans la table
			if(strcmp(tempstr, ipstr)==0){ 
				trouve = 1;
			}
			else{
				temp_ip = temp_ip->ip_suivant;
			}
		}
	}

	return trouve;
}

/**
 * @brief Compte le nombre d'adresse IP pour un hash donnée
 * 
 * @param t Table dans laquelle nous voulons travailler
 * @param hash Valeur du hash dont nous voulons connaitre 
 * le nombre d'ip associé
 *
 * @return Renvoie le nombre d'ip associé au hash donné
 */
int nombre_ip(table* t, char* hash){

	int nb_ip = 0;
	table_hash* temp_h = existence_hash(t, hash);
	if(temp_h != NULL){
		table_ip* temp_ip = temp_h->t_ip;
		//Parcours de la liste des ip
		while(temp_ip != NULL){
			nb_ip++;
			temp_ip = temp_ip->ip_suivant;
		}
	}

	return nb_ip;
}


/** 
 * @brief Remet le compteur de temps de l'ip associé au hash à 0
 *
 * @param t Liste contenant les ip et les hash
 * @param hash Valeur du hash dans laquelle est enregsitré l'ip
 * @param ip IP que dont l'on veut remettre le timer à 0
 */
void reset_timer(table* t, char* hash, struct in6_addr ip){
	
	table_hash* temp_h = existence_hash(t, hash);
	//Pour enregistrer l'ip en chaine
	char ipstr[INET6_ADDRSTRLEN];
	ipToString(ip, ipstr);
	//Pour enregistrer les ip trouvé dans la liste en chaine
	char tempstr[INET6_ADDRSTRLEN];

	if(temp_h != NULL){
		table_ip* temp_ip = temp_h->t_ip;
		while(temp_ip != NULL){
			ipToString(ip, tempstr);
			//Comparaison de l'ip passé en paramètre de la fonction
			//et des ip trouvés dans la liste
			if(strcmp(ipstr, tempstr) == 0){
				time(&temp_ip->t_inser);
				temp_ip = NULL;
			}
			else{
				temp_ip = temp_ip->ip_suivant;
			}
		}
	}
}


/**
 * @brief Insert dans la liste des hash et des ip associés un nouvel ip
 *
 * @param t Liste contenant tous les hash et les ip enregistrés
 * @param ip Adresse ip à ajouter
 * @param hash Valeur du hash a associer à l'ip
 */
void insertion_DHT(table *t, struct in6_addr ip, char* hash){

	//On créé le nouvel ip à inserer dans la liste
	table_ip *nouveau_ip = malloc(sizeof(*nouveau_ip));
	memcpy(nouveau_ip->ip.s6_addr, ip.s6_addr, sizeof(struct in6_addr));
	time(&nouveau_ip->t_inser);
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
		//Si l'ip est déjà enregistré dans la table de hash
		if(existence_couple(t, hash, ip) == 1){
			//On remet son conteur de temps à 0
			reset_timer(t, hash, ip);
		}
		else{
			//On fait pointer la valeur suivant sur l'ancien premier ip
			nouveau_ip->ip_suivant = temp->t_ip;
			//On ajoute le nouvel ip en début de liste
			temp->t_ip = nouveau_ip;
		}
	}
}

/** 
 * @brief Supprimer le hash ainsi que ces ip associés
 * 
 * @param t Table contenant les hash et les ip
 * @param hash Valeur hash à supprimer
 */
void supprimer_hash(table* t, char* hash){

	table_hash* temp_h = t->premier;
	
	//Si le hash est le premier de la liste
	if(strcmp(temp_h->hash, hash)==0){ 
		t->premier = temp_h->hash_suivant;
		//On libère la dernière ip associè à ce hash
		free(temp_h->t_ip);
		//on libère le hash
		free(temp_h);
	}
	else{

		int trouve = 0;
		table_hash* temp_pre_h = t->premier;
		temp_h = t->premier->hash_suivant;
		//On cherche dans la liste le hash et son précédeceur
		while(temp_h->hash_suivant != NULL && trouve == 0){
			if(strcmp(temp_h->hash, hash)==0){
				trouve = 1;
			}
			else{
				temp_pre_h = temp_h;
				temp_h = temp_h->hash_suivant;
			}
		}
		//Enlève le hash voulu de la liste
		temp_pre_h->hash_suivant = temp_h->hash_suivant;
	
		//libère le hash et sa dernière ip
		free(temp_h->t_ip);
		free(temp_h);
	}
}

/**
 * @brief Supprime un ip de la table associé à un hash
 *
 * @param t Liste contenant les hash et les ip
 * @param hash Valeur du hash que l'on veut trouver
 * @param ip Adresse ip que nous voulons supprimer
 */
void supprimer_ip(table* t, char* hash, struct in6_addr ip){

	int trouve = 0;
	table_hash* temp_hash = existence_hash(t, hash);

	//On enregistre l'ip en chaine
	char ipstr[INET6_ADDRSTRLEN];
	ipToString(ip, ipstr);
	//Pour enregistrer les ip trouvés dans la liste en chaine
	char tempstr[INET6_ADDRSTRLEN];

	//Si le hash à été trouvé dans la table
	if(temp_hash != NULL){
		//On cherche l'ip
		table_ip* temp_ip = temp_hash->t_ip;
		table_ip* temp_pre_ip = NULL;
		//On continue de chercher tant que l'on a pas atteint la fin
		//de la list des ip de ce hash ou que l'ip voulu n'a pas été trouvé
		while(temp_ip != NULL && trouve == 0){
			ipToString(temp_ip->ip, tempstr);
			//Comparaison de l'ip passé en paramètre
			//et les ip de la liste
			if(strcmp(ipstr, tempstr) == 0){
				trouve = 1;
			}
			else{
				if(temp_ip->ip_suivant != NULL){
					temp_pre_ip = temp_ip;
					temp_ip = temp_ip->ip_suivant;
				}
			}
		}

 		//Si l'adresse IP à été trouvé pour ce hash
		if(trouve == 1){
			//Si l'ip précédent de l'ip actuel dans le tableau n'est pas null
			//Ce n'est donc pas le premier objet de la liste
			if(temp_pre_ip != NULL){
				temp_pre_ip->ip_suivant = temp_ip->ip_suivant;
				free(temp_ip);
			}
			//Si l'ip précédent est NULL, alors l'ip cherché est le premier de
			//la liste
 			else{
 				//Si l'ip est le seul de la liste on supprime le hash
 				if(temp_ip->ip_suivant == NULL){
					supprimer_hash(t, hash);
  				}
  				//Si l'ip n'est pas le seul de la liste
  				else{
 					temp_hash->t_ip = temp_ip->ip_suivant;
 					free(temp_ip);
 				}
 			}
 		}
 	}
 }

/**
 * @brief Vérifie qu'une ip est encore valide ou non
 *
 * @param ip Un champs ip
 *
 * @return 1 si le timer n'est pas dépassé, 0 sinon
 */
int ip_valide(table_ip* ip){

	time_t end;
	//On initialise un timer avec l'heure actuelle
	time(&end);
	//On la compare avec l'heure de l'insertion de l'ip
	//Si la différence entre les deux est de plus de 30s
	if(difftime(end, ip->t_inser) >= 30){
		return 0;
	}
	else{
		return 1;
	}
}

/**
 * @brief Vérifie si les ip sont toujours valide ou non
 *
 * @param t Table contenant les hash et les ip associés
 */
void nettoyage_ip(table* t, char* hash){

	table_hash* temp_h = existence_hash(t, hash);
	table_ip* temp_ip = temp_h->t_ip;

	if(temp_h != NULL){
		while(temp_ip != NULL){
			//On vérifie que les ip ne sont pas obsolètes
			if(ip_valide(temp_ip) == 0){
				//On les supprime si elle le sont
				supprimer_ip(t, temp_h->hash, temp_ip->ip);
			}
			temp_ip = temp_ip->ip_suivant;
		}
	}
}


/**
 * @brief Renvoie un tableau d'ip assicié à un hash
 *
 * @param t Table dans laquelle sont stocké les ip et les hash
 * @param hash Hash dont nous voulons les ip
 *
 */
struct in6_addr* get_ip(table* t, char* hash){

	table_hash* temp_h = existence_hash(t, hash);
	struct in6_addr* table_ip6;
	if(temp_h != NULL){
		//On supprime les ip du hash qui sont obsolètes
		nettoyage_ip(t, hash);
		//On calcul le nombre de d'ip pour ce hash
		int taille = nombre_ip(t, hash);
		int i;
		table_ip6 = malloc(sizeof(struct in6_addr)*taille);
		//Puis on enregistre toutes les ip associès à ce hash dans une table
		table_ip* temp_ip = temp_h->t_ip;
		for(i = 0; i < taille; i++){
			table_ip6[i] = temp_ip->ip;
			temp_ip = temp_ip->ip_suivant;
		}
	}

	return table_ip6;
	
}


/**
 * @brief Affiche tous les IPs et les hashs enregistrés dans la liste
 */
void affiche(table* t){
	
	table_hash* temp_h = t->premier;
	table_ip* temp_ip = temp_h->t_ip;
	char ipstr[INET6_ADDRSTRLEN];

	while(temp_h != NULL){
		//Quand on rencontre un nouveau hash on l'affiche
		printf("--- Hash : %s ---\n", temp_h->hash);
		//Et on affiche toutes ces ip
		while(temp_ip != NULL){
			ipToString(temp_ip->ip, ipstr);
			printf("ip : %s\n", ipstr);
			temp_ip = temp_ip->ip_suivant;
		}
		temp_h = temp_h->hash_suivant;
		//Si nous n'avons pas atteint la fin de la liste des hash
		//On passe à la table d'ip du hash suivant
		if(temp_h != NULL){
			temp_ip = temp_h->t_ip;
		}
		printf("\n");
	}
	
}


void test(table *t){
	
	insertion_DHT(t, recuperer_adresse("::1"), "456");
	insertion_DHT(t, recuperer_adresse("::1"), "21");
	insertion_DHT(t, recuperer_adresse("google.fr"), "456");
	insertion_DHT(t, recuperer_adresse("::1"), "456");
	affiche(t);
}


/**
 * @brief Envoie de message keep alive aux serveurs du carnet d'adresse
 * @param socket socket d'envoie
 * @param adrServeur carnet d'adresse de serveur
 */
 adresse* envoieKeepAlive(int socket, adresse *adrServeur){
	char* msgFormat;
	char buf[1024];
	char adrString1[INET6_ADDRSTRLEN], 
		adrString2[INET6_ADDRSTRLEN];
	type_t type;
	struct sockaddr_in6 envoyeur;
	fd_set rfds;
    struct timeval tv;
    int retval;

    if (adrServeur != NULL){
    	// Préparation du message
		msgFormat = creationFormat(ARE_YOU_ALIVE, NULL);

		// Envoie du message
		envoie(socket, adrServeur->ip, adrServeur->port, msgFormat);

		//Initialisation select
	    FD_ZERO(&rfds);
	    FD_SET(socket, &rfds);

	    /* Pendant 5 secondes maxi */
	    tv.tv_sec = TIME_OUT;
	    tv.tv_usec = 0;

	    retval = select(socket+1, &rfds, NULL, NULL, &tv);
	    // Attend la réponse du serveur
	    if (retval == -1){
	        perror("connexionServeur: select()");
	    }
	    else if (retval){
			envoyeur = recevoir(socket, buf);
			// Si le message que l'on reçois vient du serveur
			if (strcmp(ipToString(adrServeur->ip, adrString1), 
				ipToString(envoyeur.sin6_addr, adrString2)) ==0 
				&& envoyeur.sin6_port == adrServeur->port){
				
				type = getTypeFromFormat(buf);
				//Si il répond qu'il n'est pas vivant
				if (type != IM_ALIVE){ //----
					free(adrServeur);
					adrServeur = NULL;
					printf("\tpas vivant\n");
				}
				else{
					printf("\tLe serveur est vivant\n");
				}
			}
	    }
	    else{
	        printf("Aucune données durant les %d secondes\n", TIME_OUT);
	        free(carnetAdrServeurGlobale);
			carnetAdrServeurGlobale = NULL;
	    }
    }
    
	return adrServeur;
}

/**
 * @brief Envoie toute sa table de hash 
 * lors d'une premiere connexion avec un autre serveur
 * @param socket socket d'envoie
 * @param adresse du serveur
 * @param t table de hash
 */
void envoieTableHash(int socket, table *t){
	int tailleTabIp=0;
	struct in6_addr* tabIp;
	table_hash *temp;
	char *msg, *msgFormat;

	for (temp = t->premier; temp != NULL; temp = temp->hash_suivant){
		tailleTabIp = nombre_ip(t, temp->hash);
		tabIp = get_ip(t, temp->hash);

		// On initialise le message
		msg = creationMsg(temp->hash, tabIp, tailleTabIp);
		msgFormat = creationFormat(PUT, msg);

		// Envoie du msg
		envoie(socket, carnetAdrServeurGlobale->ip, carnetAdrServeurGlobale->port, msgFormat);
		free(msgFormat);
	}

	// Envoie de message fin de transmission table
	printf("Fin transmission table\n");
	msgFormat = creationFormat(FIN_TRANSMISSION_TABLE, NULL);
	// Envoie du msg
	envoie(socket, carnetAdrServeurGlobale->ip, carnetAdrServeurGlobale->port, msgFormat);
}

/**
 * @brief établie une connexion avec le serveur
 * @param socket socket de communication
 * @param adr adresse du serveur à contacter
 * @return 1 si connexion étable sinon 0
 */
int connexionServeur(int socket, adresse *adr){
	char buf[2048];
	struct sockaddr_in6 serveur;
	type_t type;
	char *msgFormate;
	char ipString1[INET6_ADDRSTRLEN], ipString2[INET6_ADDRSTRLEN];
	fd_set rfds;
    struct timeval tv;
    int retval;

	msgFormate = creationFormat(CONNECT, NULL);

	// Envoie le message de connexion
	envoie(socket, adr->ip, adr->port, msgFormate);
	free(msgFormate);

	// Attend la réponse
	//Initialisation select
    FD_ZERO(&rfds);
    FD_SET(socket, &rfds);

    /* Pendant 5 secondes maxi */
    tv.tv_sec = TIME_OUT;
    tv.tv_usec = 0;

    retval = select(socket+1, &rfds, NULL, NULL, &tv);
    /* Considérer tv comme indéfini maintenant ! */

    if (retval == -1){
        perror("connexionServeur: select()");
    }
    else if (retval){
    	serveur = recevoir(socket, buf);

		// Vérifie si l'identité du répondeur correspond au serveur
		if (strcmp(ipToString(adr->ip, ipString1), 
				ipToString(serveur.sin6_addr, ipString2)) !=0){
			fprintf(stderr, "C'est pas la bonne personne\n");
			exit(1);
		}

		// Regarde si la connexion est accepté
		type = getTypeFromFormat(buf);
		if (type != ACCEPTE_CONNECT){
        	printf("Echecs de connexion: connexion refusé\n");
		}
    }
    else{
        printf("Echecs de connexion: Aucune données durant les %d secondes\n", 
        	TIME_OUT);
    }

	return type == ACCEPTE_CONNECT;
}

/**
 * @brief déconnexion d'un serveur
 * @param socket d'envoie
 * @param adrServeur adresseur du serveur connecté
 */
void deconnexionServeur(int socket){
	char* msgFormate;
	if(carnetAdrServeurGlobale != NULL){
		msgFormate = creationFormat(DISCONNECT, NULL);

		// Envoie le message de connexion
		envoie(socket, carnetAdrServeurGlobale->ip, carnetAdrServeurGlobale->port, msgFormate);
		free(msgFormate);
	}
}

/**
 * @brief Envoie des nouvelles données reçus aux autres serveurs
 * @param socket socket d'envoie
 * @param envoyeur personne ayant envoyé le put
 * @param adrServeur carnet d'adresse de serveur
 * @param msg message reçus
 */
void envoiePutServeur(int socket, struct sockaddr_in6 envoyeur, char* msg){
	char adrString1[INET6_ADDRSTRLEN], 
		adrString2[INET6_ADDRSTRLEN];
	char* msgFormat;

	if (carnetAdrServeurGlobale != NULL){
		// Vérifie si l'envoyeur n'est pas le serveur auquel on est connecté
		if (strcmp(ipToString(carnetAdrServeurGlobale->ip, adrString1), 
				ipToString(envoyeur.sin6_addr, adrString2)) !=0 
				|| envoyeur.sin6_port != carnetAdrServeurGlobale->port){
			msgFormat = creationFormat(PUT, msg);
			envoie(socket, carnetAdrServeurGlobale->ip, carnetAdrServeurGlobale->port, msgFormat);
			free(msgFormat);
		}
	}
}

/**
 * @brief fonction pour le thread qui enverra des keep alive
 * @param arg
 */
void *fctKeepAlive(void *arg){
	varKeepAlive *var = (varKeepAlive *)arg;
	int socket = initSocketSansPort(var->ip);

	while(1){
		sleep(TEMPS_KEEP_ALIVE);
		var->adr = envoieKeepAlive(socket, var->adr);
	}

	pthread_exit(0);
}

/**
 * @brief Interprete la commande
 * @param cmd commande à interpréter
 * @param envoyeur info de l'envoyeur
 * @param msg message reçus
 * @param t table de hash 
 */
adresse* interpretationCmd(
	int socket,
	type_t cmd, 
	struct sockaddr_in6 envoyeur,
	char* msg, 
	table *t){

	int i, tailleTabIp;
	info_message infMessage;
	struct in6_addr* tabIp;
	char* msgFormat;
	char adrString1[INET6_ADDRSTRLEN], adrString2[INET6_ADDRSTRLEN];

	switch(cmd){
		case PUT:
			printf("PUT\n");
			// Envoie de l'info aux autres serveurs
			envoiePutServeur(socket, envoyeur, msg);


			infMessage = decryptageMsg(msg);

			if (infMessage.taille != 0){
				for (i = 0; i < infMessage.taille; ++i){
					insertion_DHT(t, infMessage.ips[i], infMessage.hash);
				}
			}
			// Si il n'y a pas d'ip on ajoute l'ip  à envoyer
			else{
				insertion_DHT(t, envoyeur.sin6_addr, infMessage.hash);
			}
			affiche(t);
			break;
		case GET:
			printf("GET\n");
			infMessage = decryptageMsg(msg);
			free(msg);
			// On récupère le tableau d'ip associé
			tailleTabIp = nombre_ip(t, infMessage.hash);
			tabIp = get_ip(t, infMessage.hash);

			// On initialise le message
			msg = creationMsg(infMessage.hash, tabIp, tailleTabIp);
			msgFormat = creationFormat(PUT, msg);

			// Envoie du msg
			envoie(socket, envoyeur.sin6_addr, envoyeur.sin6_port, msgFormat);
			break;
		case CONNECT:
			printf("CONNECT\n");

			// On peut accepter un serveur
			// Si on a pas de serveur 
			if (carnetAdrServeurGlobale == NULL){
				printf("ACCEPTE_CONNECT\n");
				msgFormat = creationFormat(ACCEPTE_CONNECT, NULL);
				carnetAdrServeurGlobale = malloc(sizeof(adresse));
				carnetAdrServeurGlobale->ip = envoyeur.sin6_addr;
				carnetAdrServeurGlobale->port = envoyeur.sin6_port;

				// Envoie du msg
				envoie(socket, envoyeur.sin6_addr, envoyeur.sin6_port, msgFormat);

				// On envoie notre table
				envoieTableHash(socket, t);
			}
			// On ne peut pas accepter le serveur
			else{
				// Si la demande vient d'un serveur que l'on a déjà 
				// dans le carnet d'adresse
				if (strcmp(ipToString(carnetAdrServeurGlobale->ip, adrString1), 
						ipToString(envoyeur.sin6_addr, adrString2)) ==0 
					&& envoyeur.sin6_port == carnetAdrServeurGlobale->port){
					printf("Meme serveur\n");
					// On envoie qu'on accepte la connexion
					msgFormat = creationFormat(ACCEPTE_CONNECT, NULL);

					// Envoie du msg
					envoie(socket, envoyeur.sin6_addr, envoyeur.sin6_port, msgFormat);

					// On envoie notre table
					envoieTableHash(socket, t);
				}
				else{
					printf("DENIED_CONNECT\n");
					msgFormat = creationFormat(DENIED_CONNECT, NULL);

					// Envoie du msg
					envoie(socket, envoyeur.sin6_addr, envoyeur.sin6_port, msgFormat);
				}
			}			
			break;
		case DISCONNECT:
			printf("DISCONNECT\n");
			if (strcmp(ipToString(carnetAdrServeurGlobale->ip, adrString1), 
					ipToString(envoyeur.sin6_addr, adrString2)) ==0 
				&& envoyeur.sin6_port == carnetAdrServeurGlobale->port){
				// On envoie qu'on accepte la connexion
				free(carnetAdrServeurGlobale);
				carnetAdrServeurGlobale = NULL;
			}
			break;
		case ACCEPTE_CONNECT:
			/* Ignorer */
			break;
		case FIN_TRANSMISSION_TABLE:
			/* rien à faire */
			break;
		case ARE_YOU_ALIVE:
			printf("ARE_YOU_ALIVE\n");

			msgFormat = creationFormat(IM_ALIVE, NULL);
			// Envoie du message
			envoie(socket, envoyeur.sin6_addr, envoyeur.sin6_port, msgFormat);
			free(msgFormat);
			break;
		case IM_ALIVE:
			/* Rien à faire */
			break;
		default:
			fprintf(stderr, "commande inconnue\n");
			char ipstr[INET6_ADDRSTRLEN];
			ipToString(envoyeur.sin6_addr, ipstr);
			printf("%s\n", ipstr);
			exit(1);
	}
	return carnetAdrServeurGlobale;	
}

int main(int argc, char* argv[]){
	if(argc != 3 && argc != 5){
		printf("usage: %s <adresse> <port> [adresse port [...]]\n", argv[0]);
		exit(1);
	}

	struct in6_addr ip;
	int port, socket;
	int nbMessage = 20;
	int i;
	struct sockaddr_in6 client;
	char buf[1024];
	char *msg;
	table *t;
	//adresse *carnetAdrServeur = NULL;
	pthread_t tid;
	varKeepAlive vKeepAlive;

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

	// Si il y a des serveurs en option
	if (argc > 3){
		carnetAdrServeurGlobale = malloc(sizeof(adresse));

		carnetAdrServeurGlobale->ip = recuperer_adresse(argv[3]);

		if(verification_port(argv[4]) == 0){
			fprintf(stderr, "Le numéro de port \'%s\' n'est pas \
			un nombre\n", argv[4]);
			exit(1);
		}
		else{
			carnetAdrServeurGlobale->port = atoi(argv[4]);
		}
	}

	t = init_DHT();

	/** Initialisation */
	socket = initSocketPort(port, ip);

	/** Contacte les serveurs des options */
	if (carnetAdrServeurGlobale != NULL){
		if (!connexionServeur(socket, carnetAdrServeurGlobale)){
			free(carnetAdrServeurGlobale);
			carnetAdrServeurGlobale = NULL;
		}
		else{
			printf("connexion etablie\n");
		}
	}
	//Creation pthread
	vKeepAlive.adr = carnetAdrServeurGlobale;
	vKeepAlive.socket = socket;
	vKeepAlive.ip = ip;

	if (( pthread_create (&tid , NULL, fctKeepAlive, &vKeepAlive)) != 0){	
		perror("pthread_create");
		exit(1);
	}

    // Ecoute
	for (i = 0; i < nbMessage; ++i){
	   
		printf("\nAttente message %d\n", i);	
	    /** Reception Message */
		client = recevoir(socket, buf);
		printf("Message reçus\n");

		msg = getMsgFromFormat(getTailleFromFormat(buf),buf);
		carnetAdrServeurGlobale = interpretationCmd( socket,
			getTypeFromFormat(buf), client, msg, t/*, carnetAdrServeurGlobale*/);
	}

	// Prévient les autres serveurs qu'il s'arrete.	
	deconnexionServeur(socket);

	/** Fermeture */
	close(port);
	free(carnetAdrServeurGlobale);

	// Destruction thead
	if (pthread_kill(tid, SIGINT) != 0){
		fprintf(stderr, "pthread_kill \n");
		exit(1);
	}
	return 0;
}