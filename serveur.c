#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include "communication.h"

int main(int argc, char* argv[]){

	if(argc != 3){
		printf("usage: %s <adresse> <port>\n", argv[0]);
		exit(1);
	}

	char* adresse = malloc(sizeof(INET6_ADDRSTRLEN));
	int port;

	//Récupèration de l'adresse donnée en paramètre si elle existe
	recuperer_adresse(argv[1], adresse);
	printf("IPV6 : %s\n", adresse);

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
	
	return 0;
}
