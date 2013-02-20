all:
	gcc git-list.c lib/request.c lib/repo.c lib/config.c -o bin/git-list -l json -l curl -l iniparser
