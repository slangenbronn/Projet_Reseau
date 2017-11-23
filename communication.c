#include "communication.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

/** Fonction de reception */
/**
 * Ouvre un socket pour la reception
 * @return id du socket
 */
int initReception(){
    int sockfd;

    // socket factory
    if((sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/**
 * Reçois un message
 * @param sockfd id du socket utilisé
 * @param port port d'écoute
 * @param buf buffeur pour récupérer le message
 * @return les informations de l'expéditeur
 */
struct sockaddr_in6 recevoir(int sockfd, int port, char* buf){
    //char buf[1024];
    socklen_t addrlen;

    struct sockaddr_in6 my_addr;
    struct sockaddr_in6 client;

    // init local addr structure and other params
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port   = port;
    my_addr.sin6_addr   = in6addr_any; // Ici pour changer addr serveur.
    addrlen             = sizeof(struct sockaddr_in6);
    memset(buf,'\0',1024);

    // bind addr structure with socket
    if(bind(sockfd,(struct sockaddr *)&my_addr,addrlen) == -1)
    {
        perror("bind"); 
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // reception de la chaine de caracteres
    if(recvfrom(sockfd,buf,1024,0,(struct sockaddr *)&client,&addrlen) == -1)
    {
        perror("recvfrom");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return client;
}

/**
 * Ferme le socket ouvert
 * @param sockfd socket à fermer
 */
void closeReception(int sockfd){
    close(sockfd);
}

/**
 * Ouvre un socket pour recevoir un message puis ferme le socket
 * @param port port de reception
 */
void recevoirMsg(int port){
    char buf[1024];
    int sockfd = initReception();
    
    struct sockaddr_in6 client = recevoir(sockfd, port, buf);

    printf("Message recu: %s\n",buf);
    printf("Longueur du message: %li\n",strlen(buf));

    char adr_ip[INET_ADDRSTRLEN];
    if(inet_ntop(AF_INET6,&client.sin6_addr,adr_ip,INET_ADDRSTRLEN)==NULL){
        perror("inet_ntop\n");
        exit(EXIT_FAILURE);
    }
    printf("Ip source: %s\n",adr_ip);
    printf("Numero de port de l'expediteur: %d\n",client.sin6_port);

    closeReception(sockfd);
}

/** Fonction d'envoie */
/**
 * Envoie un message
 * @param ip ip du destinataire
 * @param port port du destinataire
 * @param msg message à envoyer
 */
void envoieMsg(struct in6_addr ip, int port, char* msg){
    int sockfd;
    socklen_t addrlen;
    struct sockaddr_in6 dest;
  
    // socket factory
    if((sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
    {
        perror("socket\n");
        exit(EXIT_FAILURE);
    }

    // init remote addr structure and other params
    dest.sin6_family = AF_INET6;
    dest.sin6_port   = port;
    dest.sin6_addr   = ip;
    addrlen          = sizeof(struct sockaddr_in6);
    
    // send string
    if(sendto(sockfd,msg,strlen(msg),0,(struct sockaddr *)&dest,addrlen) == -1)
    {
        perror("sendto\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // close the socket
    close(sockfd);
}


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