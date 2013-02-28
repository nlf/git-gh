#include <stdio.h>
#include <ctype.h>
#include <iniparser.h>
#include "repo.h"

void trim(char *str) {
    int i;

    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace(str[begin]))
        begin++;
    while (isspace(str[end]) && (end >= begin))
        end--;
    for (i = begin; i <= end; i++)
        str[i - begin] = str[i];
    str[i - begin] = '\0';
}

extern char* repo_get_owner() {
    char* repo = repo_get_repo();

    return strtok(repo, "/");
}

extern char* repo_get_name() {
    char* repo = repo_get_repo();
    char* name = strchr(repo, '/');

    name++;
    return name;
}

extern char* repo_get_repo() {
    dictionary *ini;
    char line[130];
    char *url;
    int repo_len;
    int start;
    char* repo;

    FILE *fp = popen("git rev-parse --show-toplevel", "r");

    if (fgets(line, sizeof line, fp) != NULL) {
        trim(line);
        strlcat(line, "/.git/config", sizeof line);
        ini = iniparser_load(line);
        if (ini) {
            url = iniparser_getstring(ini, "remote \"origin\":url", NULL);
            if (url) {
                if (strncmp(url, "git@github.com:", 15) == 0) {
                    repo_len = strlen(url) - 19;
                    start = 15;
                } else if (strncmp(url, "https://github.com/", 19) == 0) {
                    repo_len = strlen(url) - 23;
                    start = 19;
                }
                repo = (char*)calloc(repo_len, sizeof(char));
                strncpy(repo, &url[start], repo_len);
                return repo;
            }
        }
    }
    fprintf(stderr, "Unable to find a valid github origin\n");
    return NULL;
}

extern char* repo_get_branch() {
    char* branch;
    char line[130];
    int branch_len;

    char* user = repo_get_owner();
    FILE *fp = popen("git rev-parse --abbrev-ref HEAD", "r");

    if (fgets(line, sizeof line, fp) != NULL) {
        trim(line);
        branch_len = strlen(user) + strlen(line) + 2;
        branch = (char*)calloc(sizeof(char), branch_len);
        snprintf(branch, branch_len, "%s:%s", user, line);
    }
    return branch;
}
