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

int main(int argc, char* argv[]){

	if(argc != 3){
		printf("usage: %s <adresse> <port>\n", argv[0]);
		exit(1);
	}

	struct in6_addr ip;//IP de l'hote donnée en paramètre
	int port;
	//char* DHT[2];

	//Récupèration de l'adresse donnée en paramètre si elle existe
	ip = recuperer_adresse(argv[1]);
	//printf("%s", ip.s6_addr);

	//Si == 0 alors le port n'est pas un nombre, sinon s'en est un
	if(verification_port(argv[2]) == 0){
		fprintf(stderr, "Le numéro de port \'%s\' n'est pas \
		un nombre\n", argv[2]);
		exit(1);
	}
	else{
		port = atoi(argv[2]);
	}
	
	recevoirMsg(port);
	
	//Les hashs transmis de serveur à serveur seront toujours bon (>65 octest)	
	//Les hashs seront vérifier lors de l'ajout dans une table de hashage
	//lorsqu'un client fait la demande d'en ajouter un
	return 0;
}
