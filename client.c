/**
 * @file client.c
 * @author Florian GUILLEMEAU & Sylvain LANGENBRONN
 *
 * ./pg_name IP PORT COMMANDE HASH [IP]
 */

#include "communication.h"


/**
 * @brief Réalise les actions associées à la commdande
 * @param cmd commande à réalisé
 * @param ipServeur ip du serveur à contacter
 * @param port port du serveur à contacter
 * @param hash hash à transmettre ou demander
 * @param ipAssocie ip associé au hash
 */
void interpretationCmd(
    type_t cmd, 
    struct in6_addr ipServeur,
    int port,
    char *hash,
    struct in6_addr *ipAssocie){

    char *msg, *msgFormate;
    char buf[2048];
    int i;
    switch(cmd){
        case PUT:
            // Faire format msg
            // Si ipAssocié est null
            if (ipAssocie != NULL){
                printf("ipAssocie non null\n");
                msg = creationMsg(hash, ipAssocie, 1);
            }
            // Si ipAssocié n'est pas null
            else{
                //msg = hash;
                printf("ipAssocie null\n");
                msg = creationMsg(hash, NULL, 0);
            }
            // Encapsuler message
            msgFormate = creationFormat(cmd, msg);

            // Envoyer msg
            envoieMsg(ipServeur, port, msgFormate);
            printf("fin put\n");
            break;
        case GET:
            // Faire msg
            msg = creationMsg(hash, NULL, 0);

            //Encapsuler msg
            msgFormate = creationFormat(cmd, msg);
            
            // Initialise Socket
            int socket = initSocketSansPort(in6addr_any);

            // Envoyer msg
            //envoieMsg(ipServeur, port, msgFormate);
            printf("envoie msg\n");
            envoie(socket, ipServeur, port, msgFormate);

            // Réception reponse
            printf("Attend msg\n");
            recevoir(socket, buf);

            printf("\ntraite msg\n");
            // Afficher reponse
            info_message infMessage = decryptageMsg(msg);

            printf("hash: %s\n", infMessage.hash);
            for (i = 0; i < infMessage.taille; ++i){
				char ipstr[INET6_ADDRSTRLEN];
				ipToString(infMessage.ips[i], ipstr);
                printf("\tip%d %s\n", i, ipstr);
            }
            close(socket);
            break;
        default:
            fprintf(stderr, "Type inconnue\n");
            exit(TYPE);
    }
}

int main(int argc, char **argv){

    int port_nb;    
    struct in6_addr ipServeur, ipTmp, *ipAssocie = NULL;
    type_t cmd;

    // check the number of args on command line
    if(argc < 5 ||   argc > 6){
        printf("USAGE: %s IP PORT COMMANDE HASH [IP]\n", argv[0]);
        exit(C_NB_ARGS);
    }

    // get addr from command line and convert it
    ipServeur = recuperer_adresse(argv[1]);
    //Si == 0 alors le port n'est pas un nombre, sinon s'en est un
    if(verification_port(argv[2]) == 0){
        fprintf(stderr, "Le numéro de port \'%s\' n'est pas \
        un nombre\n", argv[2]);
        exit(C_NUM_PORT);
    }
    else{
        port_nb = atoi(argv[2]);
    }

    // Vérification de la commande
    cmd = getTypeFromString(argv[3]);

    // Vérification du hash
    if (verificationHash(argv[4]) == 0){
        fprintf(stderr, "hash incorrecte\n");
        exit(C_HASH);
    }
    // Vérification de l'ip en option
    if (argc >= 6){
        ipTmp = recuperer_adresse(argv[5]);
        ipAssocie = &ipTmp;
    }
    
    interpretationCmd(cmd, ipServeur, port_nb, argv[4], ipAssocie);
    
    return 0;
}
