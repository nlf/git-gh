OBJ = build/request.o build/config.o build/repo.o build/git-list.o
BIN = bin/git-list

all: $(BIN)

build/request.o: lib/request.c lib/request.h
	gcc -c lib/request.c -o build/request.o

build/config.o: lib/config.c lib/config.h
	gcc -c lib/config.c -o build/config.o

build/repo.o: lib/repo.c lib/repo.h
	gcc -c lib/repo.c -o build/repo.o

build/git-list.o: git-list.c
	gcc -c git-list.c -o build/git-list.o

bin/git-list: $(OBJ)
	gcc build/git-list.o build/request.o build/config.o build/repo.o -o bin/git-list -l json -l curl -l iniparser

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)

install: $(BIN)
	install $(BIN) /usr/local/bin/
