CC = gcc
PREFIX = /usr/local/bin
LFLAGS = -ljson -lcurl -liniparser
CFLAGS = -Wall -ansi -pedantic

BIN = bin/git-assign bin/git-claim bin/git-close bin/git-comment bin/git-detail bin/git-ghsetup bin/git-issue bin/git-list bin/git-pr
OBJ = lib/jsonhelpers.o lib/config.o lib/repo.o lib/github.o

all: $(BIN)

bin/%: src/%.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

lib/%.o: lib/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

install:
	install -m 0755 $(BIN) $(PREFIX)

clean:
	find . -name '*.o' -exec rm {} \;
	rm -f $(BIN)

.SECONDARY: $(wildcard src/*.o) $(wildcard lib/*.o)
.PHONY: clean all
