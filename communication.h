/**
 * @file communication.h
 * @author Florian GUILLEMEAU & Sylvain LANGENBRONN
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/** --Type pour le format de données-- */
#define TAILLE_MSG_MAX 1024
enum type {GET=1, PUT};
typedef char type_t;
// +------+--------+---------+
// | TYPE | TAILLE | MESSAGE |
// +------+--------+---------+
//   1o       2o      0-1ko

/** --Constante pour le Hash-- */
#define TAILLE_HASH_MAX 128
#define TAILLE_HASH_MIN 3

/** --Séparateur-- */
#define SEPARATEUR_HASH_IP '\t'
#define SEPARATEUR_IPS '\n'
// Format du message
//HASH + SEPARATEUR_HASH_IP + IP + SEPARATEUR_IPS + IP + ...
typedef struct infoMessage{
	char* hash;
	struct in6_addr* ips;
	int taille;
} info_message;


/** --Fonction de vérification du hash-- */
int verificationHash(char* hash);

/** --Ouverture socket-- */
int initSocketPort(int port, struct in6_addr ip);
int initSocketSansPort(struct in6_addr ip);

/** --Fonction de reception-- */
struct sockaddr_in6 recevoir(int sockfd, char* buf);
void recevoirMsg(int port);

/** --Fonction d'envoie-- */
void envoieMsg(struct in6_addr ip, int port, char* msg);
void envoie(int sockfd, struct in6_addr ip, int port, char* msg);

/** --Format de données-- */
type_t getTypeFromString(char* string);
char* creationFormat(type_t type, char* message);
type_t getTypeFromFormat(char* format);
short getTailleFromFormat(char* format);
char* getMsgFromFormat(short taille, char* format);

/** -- Création du message-- */
char* creationMsg(char* hash, struct in6_addr* ips, int taille);
info_message decryptageMsg(char* msg);

/** --Fonction Utile-- */
void ipToString(struct in6_addr ip, char* res);
struct in6_addr recuperer_adresse(char* adresse);
int verification_port(char* port);

//void insertion_hash(char* hash, char* ip6, char** DHT, int taille);
