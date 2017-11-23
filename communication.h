#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/** --Ouverture socket-- */
int initSocket();
void closeSocket(int sockfd);

/** --Fonction de reception-- */
void initReception(int sockfd, int port);
struct sockaddr_in6 recevoir(int sockfd, char* buf);
void recevoirMsg(int port);

/** --Fonction d'envoie-- */
void envoieMsg(struct in6_addr ip, int port, char* msg);


struct in6_addr recuperer_adresse(char* adresse);
int verification_port(char* port);

//void insertion_hash(char* hash, char* ip6, char** DHT, int taille);
