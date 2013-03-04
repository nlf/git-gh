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
    int path_len;
    char *path;
    char *url;
    char* repo;

    int repo_len = 0;
    int start = 0;
    FILE *fp = popen("git rev-parse --show-toplevel", "r");

    if (fgets(line, sizeof line, fp) != NULL) {
        trim(line);
        path_len = strlen(line) + 13;
        path = calloc(path_len, sizeof(char));
        snprintf(path, path_len, "%s/.git/config", line);
        ini = iniparser_load(path);
        free(path);
        if (ini) {
            url = iniparser_getstring(ini, "remote \"origin\":url", NULL);
            if (url) {
                if (strncmp(url, "git@github.com:", 15) == 0) {
                    repo_len = strlen(url) - 18;
                    start = 15;
                } else if (strncmp(url, "git://github.com/", 17) == 0) {
                    repo_len = strlen(url) - 20;
                    start = 17;
                } else if (strncmp(url, "https://github.com/", 19) == 0) {
                    repo_len = strlen(url) - 22;
                    start = 19;
                }
                if (start) {
                    repo = calloc(repo_len, sizeof(char));
                    snprintf(repo, repo_len, "%s", &url[start]);
                    return repo;
                }
            }
        }
    }
    fprintf(stderr, "Unable to find a valid github origin\n");
    return NULL;
}

extern char* repo_get_branch() {
    char line[130];
    int branch_len;

    char* branch = NULL;
    char* user = repo_get_owner();
    FILE *fp = popen("git rev-parse --abbrev-ref HEAD", "r");

    if (fgets(line, sizeof line, fp) != NULL) {
        trim(line);
        branch_len = strlen(user) + strlen(line) + 2;
        branch = calloc(branch_len, sizeof(char));
        snprintf(branch, branch_len, "%s:%s", user, line);
    }
    return branch;
}
