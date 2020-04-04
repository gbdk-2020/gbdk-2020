# Makefile for maccer
CC	= gcc
LDADD	= -lm
CFLAGS	= -Wall -O

VER 	= 0.25
CFLAGS 	+= -DVERSION=$(VER) -DVERSION_STRING=\"$(VER)\"

BIN	= maccer
SRC	= maccer.c test-1.ms Makefile 
DOC	= README ChangeLog
OBJS	= maccer.o
PREFIX	?= /usr/local/bin

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LDADD)

clean:
	rm -f $(BIN) $(BIN).exe $(OBJS) *~
	rm -rf html

install: $(BIN)
	install -D --strip $(BIN) $(PREFIX)/$(BIN)

dist-src: clean
	rm -f ../$(BIN)-$(VER).tar.gz
	cd ..; tar czf $(BIN)-$(VER).tar.gz maccer

dist-linux: all
	rm -f ../$(BIN)-$(VER)_linux_glibc2.tar.gz
	rm -f $(OBJS)
	cd ..; tar czf $(BIN)-$(VER)_linux_glibc2.tar.gz maccer

dist-dos: all
	rm -f ../$(BIN)-$(VER)_dos.zip
	rm -f $(OBJS)
	zip ../$(BIN)-$(VER)_dos.zip $(BIN).exe $(DOC) $(SRC)
