#include <stdio.h>
#include <string.h>
#include <json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

int main(int argc, char *argv[]) {
    char* issueid;
    struct json_object* response;

    char* token = config_get_token();
    char* repo = repo_get_repo();
    struct json_object* edited = json_object_new_object();
    struct json_object* state = json_object_new_string("closed");

    if (token == NULL || repo == NULL)
        return 1;

    if (argc <= 1) {
        fprintf(stderr, "Usage: git close <issue_id>\n");
        return 1;
    }

    issueid = argv[1];
    json_object_object_add(edited, "state", state);

    response = github_edit_issue(repo, issueid, edited, token);
    if (response) {
        printf("Successfully closed issue #%s\n", issueid);
        return 0;
    }

    return 1;
}
