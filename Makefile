all: git-list

libs:
	mkdir -p build
	gcc -c lib/request.c -o build/request.o
	gcc -c lib/config.c -o build/config.o
	gcc -c lib/repo.c -o build/repo.o

git-list: libs
	gcc -c git-list.c -o build/git-list.o
	gcc build/git-list.o build/request.o build/config.o build/repo.o -o bin/git-list -l json -l curl -l iniparser

clean:
	rm -rf build
	rm -rf bin/*

install: git-list
	install bin/* /usr/local/bin/
