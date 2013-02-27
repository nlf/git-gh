LIB = lib/config.o lib/repo.o lib/jsonhelpers.o lib/github.o
BIN = git-list git-detail git-ghsetup git-claim git-assign git-close git-issue git-pr
CC = gcc
LINKLIBS = -ljson -lcurl -liniparser

all: $(BIN)

git-list: $(LIB) git-list.o
	$(CC) git-list.o $(LIB) -o git-list $(LINKLIBS)

git-detail: $(LIB) git-detail.o
	$(CC) git-detail.o $(LIB) -o git-detail $(LINKLIBS)

git-ghsetup: $(LIB) git-ghsetup.o
	$(CC) git-ghsetup.o $(LIB) -o git-ghsetup $(LINKLIBS)

git-claim: $(LIB) git-claim.o
	$(CC) git-claim.o $(LIB) -o git-claim $(LINKLIBS)

git-assign: $(LIB) git-assign.o
	$(CC) git-assign.o $(LIB) -o git-assign $(LINKLIBS)

git-close: $(LIB) git-close.o
	$(CC) git-close.o $(LIB) -o git-close $(LINKLIBS)

git-issue: $(LIB) git-issue.o
	$(CC) git-issue.o $(LIB) -o git-issue $(LINKLIBS)

git-pr: $(LIB) git-pr.o
	$(CC) git-pr.o $(LIB) -o git-pr $(LINKLIBS)

clean:
	rm -rf *.o
	rm -f $(BIN)

install: $(BIN)
	install $(BIN) /usr/local/bin/
