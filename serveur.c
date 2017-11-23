#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

void recuperer_adresse(char* adresse, char *ip6){

	struct addrinfo hints, *res;
	void *addr;
	char ipstr[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET6; // IPv6
	hints.ai_socktype = SOCK_STREAM; // Une seule famille de socket

	//Récupération des données de l'adresse dans res
	if (getaddrinfo(adresse, NULL, &hints, &res) != 0) {
		fprintf(stderr, "Adresse ou nom de domaine non correct\n");
		exit(1);
	}

	//On récupère l'adresse qui est contenu dans la structure res
	struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
	addr = &(ipv6->sin6_addr);

    // Conversion de l'adresse IP en une chaîne de caractères
    inet_ntop(res->ai_family, addr, ipstr, sizeof ipstr);

	// Libération de la mémoire occupée par les enregistrements
	freeaddrinfo(res);

	strcpy(ip6, ipstr);
}

int verification_port(char* port){

	int taille = strlen(port);
	int i;
	int is_number = 1;
	for(i = 0; i < taille; i++){
		if(port[i] > '9' || port[i] < '0'){
			is_number = 0;
		}
	}
	return is_number;
}

int main(int argc, char* argv[]){

	if(argc != 3){
		printf("usage: %s <adresse> <port>\n", argv[0]);
		exit(1);
	}

	char* adresse = malloc(sizeof(INET6_ADDRSTRLEN));
	int port;
	char *DHT[];

	//Récupèration de l'adresse donnée en paramètre si elle existe
	recuperer_adresse(argv[1], adresse);
	printf("IPV6 : %s\n", adresse);

	//Si == 0 alors le port n'est pas un nombre, sinon s'en est un
	if(verification_port(argv[2]) == 0){
		fprintf(stderr, "Le numéro de port \'%s\' n'est pas un nombre\n", argv[2]);
		exit(1);
	}
	else{
		port = atoi(argv[2]);
	}
	

	
	return 0;
}
