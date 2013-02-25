OBJ = build/request.o build/config.o build/repo.o build/git-list.o build/git-detail.o
BIN = bin/git-list bin/git-detail
CC = gcc
CFLAGS = -ljson -lcurl -liniparser

all: $(BIN)

build/request.o: lib/request.c lib/request.h
	$(CC) -c lib/request.c -o build/request.o

build/config.o: lib/config.c lib/config.h
	$(CC) -c lib/config.c -o build/config.o

build/repo.o: lib/repo.c lib/repo.h
	$(CC) -c lib/repo.c -o build/repo.o

build/git-list.o: git-list.c
	$(CC) -c git-list.c -o build/git-list.o

build/git-detail.o: git-detail.c
	$(CC) -c git-detail.c -o build/git-detail.o

bin/git-list: $(OBJ)
	$(CC) build/git-list.o build/request.o build/config.o build/repo.o -o bin/git-list $(CFLAGS)

bin/git-detail: $(OBJ)
	$(CC) build/git-detail.o build/request.o build/config.o build/repo.o -o bin/git-detail $(CFLAGS)

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)

install: $(BIN)
	install $(BIN) /usr/local/bin/
