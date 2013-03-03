CC = gcc
PREFIX = /usr/local/bin
LFLAGS = -lcurl
CFLAGS = -Wall -O2 -I3rdparty/iniparser/src -I3rdparty/json-c

BIN = bin/git-accept bin/git-assign bin/git-claim bin/git-close \
	  bin/git-comment bin/git-detail bin/git-ghsetup bin/git-issue \
	  bin/git-list bin/git-pr bin/git-review
OBJ = lib/jsonhelpers.o lib/config.o lib/repo.o lib/github.o
DEPS = 3rdparty/iniparser/libiniparser.a 3rdparty/json-c/.libs/libjson.a

all: $(BIN)

bin/%: src/%.o $(OBJ) $(DEPS)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

lib/%.o: lib/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

3rdparty/iniparser/libiniparser.a:
	cd 3rdparty/iniparser; make libiniparser.a

3rdparty/json-c/.libs/libjson.a:
	cd 3rdparty/json-c; ./configure
	cd 3rdparty/json-c; make

install:
	install -m 0755 $(BIN) $(PREFIX)

clean:
	find . -name '*.o' -exec rm {} \;
	cd 3rdparty/iniparser; make clean
	cd 3rdparty/json-c; make clean
	rm -f $(BIN)

.SECONDARY: $(wildcard src/*.o) $(wildcard lib/*.o)
.PHONY: clean all
