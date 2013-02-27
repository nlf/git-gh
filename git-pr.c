#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include "lib/config.h"
#include "lib/repo.h"
#include "lib/jsonhelpers.h"
#include "lib/github.h"

int main(int argc, char *argv[]) {
    char* token = config_get_token();
    char* repo = repo_get_repo();
    if (token == NULL || repo == NULL)
        return 1;

    int c;
    extern int optind;
    extern char* optarg;
    char* title;
    char* body;
    char* base = "master";
    char* head = repo_get_branch();
    while ((c = getopt(argc, argv, "t:m:b:h:")) != EOF) {
        switch (c) {
            case 't':
                title = optarg;
                break;
            case 'm':
                body = optarg;
                break;
        }
    }
    if (title == NULL || body == NULL) {
        fprintf(stderr, "Usage: git pr -m \"pr body\" -t \"pr title\" [-b base] [-h head]\n");
        return 1;
    }

    struct json_object* pr = json_object_new_object();
    json_object_object_add(pr, "title", json_object_new_string(title));
    json_object_object_add(pr, "body", json_object_new_string(body));
    json_object_object_add(pr, "base", json_object_new_string(base));
    json_object_object_add(pr, "head", json_object_new_string(head));
    
    struct json_object* response = github_create_pr(repo, pr, token);
    if (response)
        printf("Created pr #%s\n", jsonh_get_string(response, "number"));

    return 0;
}
