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
