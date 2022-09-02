CC = gcc
SRC = *.c
SRCDIR = src
FLAGS = -Wall -Wfatal-errors -Wextra -g
BIN = todo
PKGDIR = pkg
RFLAGS = -Wall -Wfatal-errors -Wextra -O3

all: clean debug

debug:
	$(CC) -o $(BIN) $(SRCDIR)/$(SRC) $(FLAGS)

release: | $(PKGDIR)
	$(CC) -o $(PKGDIR)/$(BIN) $(SRCDIR)/$(SRC) $(RFLAGS)
	strip -s $(PKGDIR)/$(BIN)

$(PKGDIR):
	mkdir $@

clean:
	rm -rf $(PKGDIR)
	rm -f $(BIN)
