#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <json-c/json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

int main(int argc, char *argv[]) {
    int c;
    extern char* optarg;
    struct json_object* response;

    char* title = NULL;
    char* body = NULL;
    char* token = config_get_token();
    char* repo = repo_get_repo();
    char* base = "master";
    char* head = repo_get_branch();
    struct json_object* pr = json_object_new_object();

    if (token == NULL || repo == NULL)
        return 1;

    while ((c = getopt(argc, argv, "t:m:b:h:")) != EOF) {
        switch (c) {
            case 't':
                title = optarg;
                break;
            case 'm':
                body = optarg;
                break;
            case 'b':
                base = optarg;
                break;
            case 'h':
                head = optarg;
                break;
        }
    }
    if (title == NULL || body == NULL) {
        fprintf(stderr, "Usage: git pr -m \"pr body\" -t \"pr title\" [-b base] [-h head]\n");
        return 1;
    }

    json_object_object_add(pr, "title", json_object_new_string(title));
    json_object_object_add(pr, "body", json_object_new_string(body));
    json_object_object_add(pr, "base", json_object_new_string(base));
    json_object_object_add(pr, "head", json_object_new_string(head));
    
    response = github_create_pr(repo, pr, token);
    if (response) {
        printf("Created pr #%s\n", jsonh_get_string(response, "number"));
        return 0;
    }

    return 1;
}
