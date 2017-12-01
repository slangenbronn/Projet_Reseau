#!/bin/sh

#
# Tests basiques de vérification de syntaxe
#

. ./ftest.sh

###############################################################################
# Tests d'arguments invalides
# (on attend un message d'erreur du type "usage: ..." pour être sûr
# que le problème de syntaxe est bien détecté)

# Test de serveur.c
./serveur						2> $TMP >&2 || tu && fail "serveur: pas d'arg"
./serveur xxx					2> $TMP >&2 || tu && fail "serveur: arg invalide 1"
./serveur xxx xxx xxx			2> $TMP >&2 || tu && fail "serveur: nombre d'arg invalide 3"
./serveur xxx xxx xxx xxx xxx	2> $TMP >&2 || tu && fail "serveur: nombre d'arg invalide 5"
./serveur xxxx 3000				2> $TMP >&2 || tu && fail "serveur: adresse ip incorrecte"
./serveur ::1 -3				2> $TMP >&2 || tu && fail "serveur: port incorrecte"
./serveur xxxx 3000				2> $TMP >&2 || tu && fail "serveur: adresse ip incorrecte"
./serveur ::1 3000 xxxx 3001	2> $TMP >&2 || tu && fail "serveur: adresse ipServeur incorrecte"
./serveur ::1 3000 ::1 -3		2> $TMP >&2 || tu && fail "serveur: adresse ipServeur incorrecte"
./serveur ip6-localhost -1 		2> $TMP >&2 || tu && fail "serveur: adresse ip reconnue mais port incorrecte"


#usage: %s <adresse> <port> [adresse port]
# Test de client.h