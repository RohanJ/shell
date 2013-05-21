CC = gcc
OPTS = -g -Wall -W

all: shell testlog doc/html

doc/html:
	doxygen doc/Doxyfile

shell: shell.c log.o
	$(CC) -o $@ $^ $(OPTS)

testlog: testlog.c log.o
	$(CC) -o $@ $^ $(OPTS)

log.o: log.c log.h
	$(CC) -o $@ $< -c $(OPTS)

clean:
	rm -rf *.o shell testlog doc/html
