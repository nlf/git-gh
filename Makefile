LIB = lib/request.o lib/config.o lib/repo.o lib/jsonhelpers.o
BIN = git-list git-detail
CC = gcc
LINKLIBS = -ljson -lcurl -liniparser

all: $(BIN)

git-list: $(LIB) git-list.o
	$(CC) git-list.o lib/request.o lib/config.o lib/repo.o lib/jsonhelpers.o -o git-list $(LINKLIBS)

git-detail: $(LIB) git-detail.o
	$(CC) git-detail.o lib/request.o lib/config.o lib/repo.o lib/jsonhelpers.o -o git-detail $(LINKLIBS)

clean:
	rm -rf *.o
	rm -f $(BIN)

install: $(BIN)
	install $(BIN) /usr/local/bin/
