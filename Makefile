COV = -coverage

CFLAGS = -g -Wall -Wextra -Werror $(COVERAGE)

PROGS = serveur client

all: $(PROGS)

$(PROGS): communication.o

communication.o: communication.h

clean:
	rm -f *.o $(PROGS)
	rm -f *.gc*
	rm -f *.log
	rm -f tags core
