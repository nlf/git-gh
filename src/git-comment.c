#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <json-c/json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

int main(int argc, char *argv[]) {
    char* issueid;
    char* body;
    struct json_object* response;

    char* token = config_get_token();
    char* repo = repo_get_repo();
    struct json_object* comment = json_object_new_object();

    if (token == NULL || repo == NULL)
        return 1;

    if (argc <= 2 || strcasecmp(argv[2], "-m") != 0) {
        fprintf(stderr, "Usage: git comment <issue_id> -m \"comment body\"\n");
        return 1;
    }
    issueid = argv[1];
    body = argv[3];

    json_object_object_add(comment, "body", json_object_new_string(body));
    
    response = github_create_comment(repo, issueid, comment, token);
    if (response) {
        printf("Added comment to issue #%s\n", issueid);
        return 0;
    }

    return 1;
}
