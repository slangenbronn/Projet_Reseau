#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/** Fonction de reception */
int initReception();
struct sockaddr_in6 recevoir(int sockfd, int port, char* buf);
void closeReception(int sockfd);
void recevoirMsg(int port);

/** Fonction d'envoie */
void envoieMsg(struct in6_addr ip, int port, char* msg);


struct in6_addr recuperer_adresse(char* adresse);
int verification_port(char* port);

//void insertion_hash(char* hash, char* ip6, char** DHT, int taille);
