#
# Fonctions et variables auxiliaires utilisées pour les différents
# tests.
#

TEST=$(basename $0 .sh)

TMP=/tmp/$TEST-$$
LOG=$TEST.log
V=${VALGRIND}		# appeler avec la var. VALGRIND à "" ou "valgrind -q"

DEBUG_MUSEE=${DEBUG_MUSEE:-1} ; export DEBUG_MUSEE

# permet de rediriger stderr vers le log pour voir les résultats des tests
exec 2> $LOG
set -u			# erreur si utilisation d'une variable non définie
set -x			# mode trace

###############################################################################
# Une fonction qu'il vaudrait mieux ne pas avoir à appeler...

fail ()
{
    echo "==> Échec du test '$TEST' sur '$1'."
    echo "==> Log : '$LOG'."
    echo "==> Exit"
    exit 1
}

###############################################################################
# Teste la présence du traditionnel message : "usage: prog arg..." dans $TMP
# Renvoie vrai si trouvé, faux si pas trouvé

tu ()
{
    # rappel: "! cmd" => inverse le code de retour de cmd
    ! grep -q "usage: " $TMP
}

###############################################################################
# Renvoie vrai si le processus $1 existe

ps_existe ()
{
    kill -0 $1 2> /dev/null
}

###############################################################################
# Chronomètre à utiliser de la façon suivante :
#	c=$(chrono_start)
#	...
#	duree=$(chrono_end $c)

chrono_start ()
{
    date +%s
}

chrono_end ()
{
    local start=$1 end
    end=$(date +%s)
    # renvoyer la durée
    echo $((end-start))
}

