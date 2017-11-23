#include <sys/socket.h>
#include <netinet/in.h>

/** --Ouverture socket-- */
int initSocket();
void closeSocket(int sockfd);

/** --Fonction de reception-- */
struct sockaddr_in6 recevoir(int sockfd, int port, char* buf);
void recevoirMsg(int port);

/** --Fonction d'envoie-- */
void envoieMsg(struct in6_addr ip, int port, char* msg);


void recuperer_adresse(char* adresse, char *ip6);
int verification_port(char* port);