/**
 * @file communication.c
 * @author Florian GUILLEMEAU & Sylvain LANGENBRONN
 */

#include "communication.h"

/** --Fonction de vérification du hash-- */
/**
 * @brief Vérification de la validité du hash
 * @param hash hash à vérifier
 * @return 1 si correct sinon 0
 */
int verificationHash(char* hash){
    int taille = strlen(hash);
    return taille >= TAILLE_HASH_MIN && taille < TAILLE_HASH_MAX;
}

/** --Ouverture socket-- */
/**
 * @brief initialise le socket avec un port spécifique
 * @param port port attribué au socket
 * @param ip ip du socket
 * @return socket binder
 */
int initSocketPort(int port, struct in6_addr ip){
    int sockfd;

    // socket factory
    if((sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    socklen_t addrlen;

    struct sockaddr_in6 my_addr;

    // init local addr structure and other params
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port   = port;
    my_addr.sin6_addr   = ip; // Ici pour changer addr serveur.
    addrlen             = sizeof(struct sockaddr_in6);

    // bind addr structure with socket
    if(bind(sockfd,(struct sockaddr *)&my_addr,addrlen) == -1)
    {
        perror("bind"); 
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/**
 * @brief initialise le socket sans port spécifique
 * @param ip associé au socket
 * @return socket binder
 */
int initSocketSansPort(struct in6_addr ip){
    int sockfd;

    // socket factory
    if((sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    socklen_t addrlen;

    struct sockaddr_in6 my_addr;

    // init local addr structure and other params
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_addr   = ip; // Ici pour changer addr serveur.
    addrlen             = sizeof(struct sockaddr_in6);

    // bind addr structure with socket
    if(bind(sockfd,(struct sockaddr *)&my_addr,addrlen) == -1)
    {
        perror("bind"); 
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/** --Fonction de reception-- */
/**
 * @brief Bind le socket pour la reception
 * @param sockfd socket à binder
 * @param port pour du socket
 */
void initReception(int sockfd, int port, struct in6_addr ip){
    socklen_t addrlen;

    struct sockaddr_in6 my_addr;

    // init local addr structure and other params
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port   = port;
    my_addr.sin6_addr   = ip; // Ici pour changer addr serveur.
    addrlen             = sizeof(struct sockaddr_in6);

    // bind addr structure with socket
    if(bind(sockfd,(struct sockaddr *)&my_addr,addrlen) == -1)
    {
        perror("bind"); 
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Reçois un message
 * @param sockfd id du socket utilisé
 * @param port port d'écoute
 * @param buf buffeur pour récupérer le message
 * @return les informations de l'expéditeur
 */
struct sockaddr_in6 recevoir(int sockfd, char* buf){
    socklen_t addrlen;

    //struct sockaddr_in6 my_addr;
    struct sockaddr_in6 client;

    //
    addrlen             = sizeof(struct sockaddr_in6);
    memset(buf,'\0',1024);

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
 * @brief Ouvre un socket pour recevoir un message puis ferme le socket
 * @param port port de reception
 */
void recevoirMsg(int port){
    char buf[1024];
    int sockfd = initSocketPort(port, in6addr_any);

    struct sockaddr_in6 client = recevoir(sockfd, buf);

    printf("Message recu: %s\n",buf);
    printf("Longueur du message: %li\n",strlen(buf));

    char adr_ip[INET6_ADDRSTRLEN];
    if(inet_ntop(AF_INET6,&client.sin6_addr,adr_ip,INET6_ADDRSTRLEN)==NULL){
        perror("inet_ntop\n");
        exit(EXIT_FAILURE);
    }
    printf("Ip source: %s\n",adr_ip);
    printf("Numero de port de l'expediteur: %d\n",client.sin6_port);

    close(sockfd);
}

/** --Fonction d'envoie-- */
/**
 * @brief Envoie un message
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

/**
 * @brief envoie un message
 * @param sockfd socket d'envoie
 * @param ip ip du destinataire
 * @param port port du destinataire
 * @param msg message à envoyer
 */
void envoie(int sockfd, struct in6_addr ip, int port, char* msg){
    socklen_t addrlen;
    struct sockaddr_in6 dest;

    // init remote addr structure and other params
    dest.sin6_family = AF_INET6;
    dest.sin6_port   = port;
    dest.sin6_addr   = ip;
    addrlen          = sizeof(struct sockaddr_in6);
    
    // send string
    if(sendto(sockfd,msg,strlen(msg),0,(struct sockaddr *)&dest,addrlen) == -1){
        perror("sendto\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // close the socket
    //close(sockfd);
}

/** --Format de données-- */
/**
 * @brief Renvoie le type associé à la string
 * @param string chaine du type
 * @return type associé
 */
type_t getTypeFromString(char* string){
    if (strcmp(string, "put") == 0){
        return PUT;
    }
    else if (strcmp(string, "get") == 0){
        return GET;
    }
    else{
        fprintf(stderr, "type inconnue: %s\n", string);
        exit(1);
    }
}

/**
 * @brief Crée une chaine dans le format de données
 * @param type type de message
 * @param message message à envoyer
 * @return message encapsulé dans le format
 */
char* creationFormat(type_t type, char* message){
    char* buf;
    short tailleMsg = 0;
    char ty[1];
    char tai[2];
    short taille; 
    int index = 0;

    if (message != NULL){
        tailleMsg = strlen(message);
    }
    
    if (tailleMsg > TAILLE_MSG_MAX){
        fprintf(stderr, "taille du message trop grand\n");
        exit(1);
    }

    taille = sizeof(type_t) + sizeof(short) + tailleMsg;
    buf = malloc(taille);

    ty[0] = type;
    tai[0] = ~taille;
    tai[1] = (~taille)>>8;
    memset(buf, '\0', taille);

    // Insertion du type
    strncpy(buf + index, ty, 1);
    index += 1;

    // Insertion de la taille
    buf[index] = tai[0];
    buf[index+1] = tai[1];
    index += sizeof(short);

    // Insertion du message
    strncat(buf+index, message, tailleMsg);

    return buf;
}

/**
 * @brief Retourne le type contenue dans le format
 * @param format format à decrypter
 * @return type dans le format
 */
type_t getTypeFromFormat(char* format){
    return (type_t)format[0];
}

/**
 * Renvoie la taille contenue dans le format
 * @param format format à décrypter
 * @return taille
 */
short getTailleFromFormat(char* format){
    short taille;
    
    taille = ((~format[2])<<8) + ~format[1];
    
    return taille;
}

/**
 * Renvoie le message contenue dans le format
 * @param format format à décrypter
 * @return message
 */
char* getMsgFromFormat(short taille, char* format){
    char* message = malloc(sizeof(char)*taille);

    strcpy(message, format+sizeof(type_t)+sizeof(short));

    return message;
}

/** -- Création du message-- */
/**
 * @brief Crée le message à envoyer.
 * @param hash hash à envoyer
 * @param ips tableau d'ips à envoyer
 * @param taille taille du tableau d'ips
 * @return concaténation des différents éléments séparer par un séparateur
 */
char* creationMsg(char* hash, struct in6_addr* ips, int taille){
    char* msg;
    char tSep[1];
    char ipstr[INET6_ADDRSTRLEN];
    int tailleMsg, tailleHash;
    int i;

    // Calcule de la taille du message
    tailleHash = strlen(hash);
    tailleMsg = tailleHash + INET6_ADDRSTRLEN*taille + taille;
    msg = malloc(tailleMsg);

    // Copies le hash
    strncpy(msg, hash, tailleHash);

    if (taille > 0)
    {
        tSep[0] = SEPARATEUR_HASH_IP;
        strncat(msg, tSep, 1);
        tSep[0] = SEPARATEUR_IPS;

        //Copies les ips
        for (i = 0; i < taille; i++){
            // Conversion de l'adresse IP en une chaîne de caractères
            inet_ntop(AF_INET6, (void*)&(ips[i]), ipstr, sizeof(ipstr));
            
            // Concatene la chaine
            strncat(msg, ipstr, sizeof(ipstr));
           
            if (i < taille-1){
                strncat(msg, tSep, 1);
            }
        }
    }
    
    return msg;
}

/**
 * Décrypte le message
 * @param msg message à décrypter
 * @return les info contenus dans le message
 */
info_message decryptageMsg(char* msg){
    char *tmpHash, *tmpIp;
    char delim[1];;
    char tabIp[100][INET6_ADDRSTRLEN];
    int i;
    info_message infMsg;

    // Récupération du hash
    delim[0] = SEPARATEUR_HASH_IP;
    tmpHash = strtok(msg, delim);
    infMsg.hash = malloc(strlen(tmpHash));
    strncpy(infMsg.hash, tmpHash, strlen(tmpHash));

    
    // Prépare le reste de la chaine pour le traitement
    tmpHash =  strtok(NULL, delim);
    i = 0;
    if (tmpHash != NULL){
        delim[0] = SEPARATEUR_IPS;
        tmpIp =  strtok(tmpHash, delim);
        
        // Récupération des ips
        if (tmpIp != NULL){
            strncpy(tabIp[i], tmpIp, INET6_ADDRSTRLEN);
            i++;
        }
        while((tmpIp = strtok(NULL, delim)) != NULL){
            strncpy(tabIp[i], tmpIp, INET6_ADDRSTRLEN);
            i++;
        }
    }
    
    infMsg.taille = i;

    // Convertie en type ip
    int j;
    infMsg.ips = malloc(sizeof(struct in6_addr)*infMsg.taille);
    for (j = 0; j < infMsg.taille; ++j){
        infMsg.ips[j] = recuperer_adresse(tabIp[j]);   
    }

    return infMsg;
}

/** --Fonction Utile-- */
/**
 * @brief Transforme une adresse ip en string
 * @param ip ip à transformer
 * @return ip en string
 */
char* ipToString(struct in6_addr ip, char* res){
    char ipstr[INET6_ADDRSTRLEN];
	void* addr = &ip;
    inet_ntop(AF_INET6, addr, ipstr, sizeof(ipstr));
	memcpy(res, ipstr, sizeof(ipstr));

    return res;
}

/**
 * @brief Donne l'adresse correspondante au host donnée en paramètre
 *
 * @param adresse Host d'on nous voulons l'adresse IPV6
 * @param ip6 L'adresse est enregistré dans cette variable
 */
struct in6_addr recuperer_adresse(char* adresse){

    struct addrinfo hints, *res;

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

    // Libération de la mémoire occupée par les enregistrements
    freeaddrinfo(res);

	return ipv6->sin6_addr;
}

/**
 * @brief Vérifie si le numéro de port donné en argument est bien 
 * un nombre ou non
 *
 * @param port Chaine contenant le numéro de port à vérifier
 */
int verification_port(char* port){

    int taille = strlen(port);
    int i;
    int is_number = 1;
    for(i = 0; i < taille; i++){
	//Vérifie tous les caractère de la chaine pour voir si ce sont tous
	//des chiffres
		if(port[i] > '9' || port[i] < '0'){
            is_number = 0;
        }
    }
    return is_number;
}
