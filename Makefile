#CC = go
#CMD = build
#PURGE = clean
#INST = install
#REL_FLAGS = -ldflags="-s -w"
#
#all: clean debug
#
#package: release install
#
#debug:
#	$(CC) $(CMD)
#
#release:
#	$(CC) $(CMD) $(REL_FLAGS)
#
#install:
#	$(CC) $(INST)
#
#clean:
#	$(CC) $(PURGE)

CC = gcc
BIN = todo
SRC = *.c
SRCDIR = src
D_FLAGS = -g -Wall -Wfatal-errors -Wextra 
R_FLAGS = -Wall -Wfatal-errors -Wextra -O3
PKGDIR = pkg

all: clean debug

debug:
	$(CC) -o $(BIN) $(SRCDIR)/$(SRC) $(D_FLAGS)

release: | $(PKGDIR)
	$(CC) -o $(PKGDIR)/$(BIN) $(SRCDIR)/$(SRC) $(R_FLAGS)
	strip -s $(PKGDIR)/$(BIN)

$(PKGDIR):
	mkdir $@

install:
	install $(PKGDIR)/$(BIN) /usr/local/bin

clean:
	rm -f $(BIN)
	rm -rf $(PKGDIR)
