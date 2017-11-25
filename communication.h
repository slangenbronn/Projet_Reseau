/**
 * @file communication.h
 * @author Florian GUILLEMEAU & Sylvain LANGENBRONN
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/** --Type pour le format de données-- */
#define TAILLE_MSG_MAX 1024
enum type {GET, PUT};
typedef char type_t;
// +------+--------+---------+
// | TYPE | TAILLE | MESSAGE |
// +------+--------+---------+
//   1o       2o      0-1ko

/** --Constante pour le Hash-- */
#define TAILLE_HASH_MAX 256
#define TAILLE_HASH_MIN 65

/** --Séparateur-- */
#define SEPARATEUR_HASH_IP '\t'
#define SEPARATEUR_IPS '\n'

/** --Fonction de vérification du hash-- */
int verificationHash(char* hash);

/** --Ouverture socket-- */
int initSocket();
void closeSocket(int sockfd);

/** --Fonction de reception-- */
void initReception(int sockfd, int port, struct in6_addr ip);
struct sockaddr_in6 recevoir(int sockfd, char* buf);
void recevoirMsg(int port);

/** --Fonction d'envoie-- */
void envoieMsg(struct in6_addr ip, int port, char* msg);

/** --Format de données-- */
type_t getTypeFromString(char* string);
char* creationFormat(type_t type, char* message);
type_t getTypeFromFormat(char* format);
short getTailleFromFormat(char* format);
char* getMsgFromFormat(short taille, char* format);

/** -- Création du message-- */
char* creationMsg(char* hash, struct in6_addr* ips, int taille);

struct in6_addr recuperer_adresse(char* adresse);
int verification_port(char* port);

//void insertion_hash(char* hash, char* ip6, char** DHT, int taille);
