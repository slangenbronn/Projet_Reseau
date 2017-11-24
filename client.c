/**
 * @file client.c
 * @author Florian GUILLEMEAU & Sylvain LANGENBRONN
 *
 * ./pg_name IPv4_addr port_number string
 */

#include "communication.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	int port_nb;	
    struct in6_addr ip;

    // check the number of args on command line
    if(argc != 4){
        printf("USAGE: %s @dest port_num string\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // get addr from command line and convert it
    ip = recuperer_adresse(argv[1]);
    //Si == 0 alors le port n'est pas un nombre, sinon s'en est un
    if(verification_port(argv[2]) == 0){
        fprintf(stderr, "Le numéro de port \'%s\' n'est pas \
        un nombre\n", argv[2]);
        exit(1);
    }
    else{
        port_nb = atoi(argv[2]);
    }
	
    envoieMsg(ip, port_nb, argv[3]);
    
    return 0;
}
