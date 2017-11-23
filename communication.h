#include <sys/socket.h>
#include <netinet/in.h>

/** Fonction de reception */
int initReception();
struct sockaddr_in6 recevoir(int sockfd, int port, char* buf);
void closeReception(int sockfd);
void recevoirMsg(int port);

/** Fonction d'envoie */
void envoieMsg(struct in6_addr ip, int port, char* msg);


void recuperer_adresse(char* adresse, char *ip6);
int verification_port(char* port);