#include <stdio.h>
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

extern char* getRepo() {
    dictionary *ini = NULL;
    FILE *fp;
    char line[130];
    char *section;
    char *url;
    int seccount;
    int i;

    fp = popen("git rev-parse --show-toplevel", "r");
    if (fgets(line, sizeof line, fp) != NULL) {
        trim(line);
        strlcat(line, "/.git/config", sizeof line);
        ini = iniparser_load(line);
        if (ini) {
            seccount = iniparser_getnsec(ini);
            for (i = 0; i < seccount; i++) {
                section = iniparser_getsecname(ini, i);
                if (strncmp(section, "remote \"origin\"", 15) == 0) {
                    url = iniparser_getstring(ini, "remote \"origin\":url", NULL);
                    int repolen;
                    int start;
                    if (strncmp(url, "git@github.com:", 15) == 0) {
                        repolen = strlen(url) - 19;
                        start = 15;
                    } else if (strncmp(url, "https://github.com/", 19) == 0) {
                        repolen = strlen(url) - 23;
                        start = 19;
                    }
                    char *repo = (char*)calloc(repolen, sizeof(char));;
                    strncpy(repo, &url[start], repolen);
                    return repo;
                }
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}
