CC = go
CMD = build
PURGE = clean
REL_FLAGS = -ldflags="-s -w"

all: clean debug

debug:
	$(CC) $(CMD)

release:
	$(CC) $(CMD) $(REL_FLAGS)

clean:
	$(CC) $(PURGE)
