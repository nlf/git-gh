OBJ = build/request.o build/config.o build/repo.o build/git-list.o
BIN = bin/git-list

build/request.o:
	gcc -c lib/request.c -o build/request.o

build/config.o:
	gcc -c lib/config.c -o build/config.o

build/repo.o:
	gcc -c lib/repo.c -o build/repo.o

build/git-list.o:
	gcc -c git-list.c -o build/git-list.o

all: $(BIN)

bin/git-list: $(OBJ)
	gcc build/git-list.o build/request.o build/config.o build/repo.o -o bin/git-list -l json -l curl -l iniparser

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)

install: $(BIN)
	install $(BIN) /usr/local/bin/
