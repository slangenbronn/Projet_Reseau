/**
 * @file sender-udp.c
 * @author Julien Montavont
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Simple program that creates an IPv4 UDP socket and sends a string
 * to a remote host. The string, IPv4 addr and port number of the
 * remote host are passed as command line parameters as follow:
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
    if(argc != 4)
    {
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
