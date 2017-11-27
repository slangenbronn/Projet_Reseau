/**
 * @file client.c
 * @author Florian GUILLEMEAU & Sylvain LANGENBRONN
 *
 * ./pg_name IP PORT COMMANDE HASH [IP]
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




/**
 * @brief Réalise les actions associées à la commdande
 * @param cmd commande à réalisé
 * @param ipServeur ip du serveur à contacter
 * @param port port du serveur à contacter
 * @param hash hash à transmettre ou demander
 * @param ipAssocie ip associé au hash
 */
/*void interpretationCmd(
    type_t cmd, 
    struct in6_addr ipServeur,
    int port,
    char* hash,
    struct in6_addr ipAssocie){
    char* msg, msgFormate;
    switch(cmd){
        case PUT:
            // Faire format msg
            // Si ipAssocié est null
            if (ipAssocie != NULL){
                
            }
            // Si ipAssocié n'est pas null
            else{
                msg = hash;
            }
            // Encapsuler message

            // Envoyer msg
            free(msg);
            break;
        case GET:
            // Faire msg
            msg = hash;

            //Encapsuler msg
            msgFormate = creationFormat(cmd, msg);
            
            // Envoyer msg

            // Afficher reponse

            break
        default:
            fprintf(stderr, "Type inconnue\n");
            exit(1);
    }
}*/

int main(int argc, char **argv)
{
	int port_nb;	
    struct in6_addr ipServeur, ipAssocie;
    type_t cmd;

    // check the number of args on command line
    if(argc < 5 ||   argc > 6){
        printf("USAGE: %s IP PORT COMMANDE HASH [IP]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // get addr from command line and convert it
    ipServeur = recuperer_adresse(argv[1]);
    //Si == 0 alors le port n'est pas un nombre, sinon s'en est un
    if(verification_port(argv[2]) == 0){
        fprintf(stderr, "Le numéro de port \'%s\' n'est pas \
        un nombre\n", argv[2]);
        exit(1);
    }
    else{
        port_nb = atoi(argv[2]);
    }

    // Vérification de la commande
    cmd = getTypeFromString(argv[3]);
    printf("%d\n", cmd);

    // Vérification du hash
    if (verificationHash(argv[4]) == 0){
        fprintf(stderr, "hassh incorrecte\n");
        exit(1);
    }
    // Vérification de l'ip en option
    if (argc >= 6){
        ipAssocie = recuperer_adresse(argv[5]);
        printf("ipAssocie %s\n", ipAssocie.s6_addr); //Pour eviter les erreurs de unused
    }
    
    char* msg = creationMsg(argv[4], &ipServeur, 1);
    //char* msg = creationMsg(argv[4], &ipServeur, 1);
    printf("msg: %s\n", msg);
    printf("port, %d\n", port_nb);
    free(msg);
    //envoieMsg(ipServeur, port_nb, argv[3]);
    
    return 0;
}
