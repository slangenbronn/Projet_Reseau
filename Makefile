COV = -coverage

CFLAGS = -g -Wall -Wextra -Werror $(COVERAGE)

PROGS = client

all: $(PROGS) serveur

$(PROGS): communication.o

serveur: serveur.c
	cc -g -Wall -Wextra -Werror $(COVERAGE) serveur.c communication.o -o serveur -l pthread

communication.o: communication.h

test: test-sans-valgrind

test-sans-valgrind: all
	@for i in test-*.sh ; do echo $$i ; sh $$i || exit 1 ; done

clean:
	rm -f *.o $(PROGS)
	rm -f *.gc*
	rm -f *.log
	rm -f tags core
