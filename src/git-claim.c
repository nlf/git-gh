#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

int main(int argc, char *argv[]) {
    char* issueid;
    struct json_object* assignee;
    struct json_object* response;

    char* user = config_get_user();
    char* token = config_get_token();
    char* repo = repo_get_repo();
    struct json_object* edited = json_object_new_object();

    if (user == NULL || token == NULL || repo == NULL)
        return 1;

    if (argc <= 1) {
        fprintf(stderr, "Usage: git claim <issue_id>\n");
        return 1;
    }

    issueid = argv[1];
    assignee = json_object_new_string(user);
    json_object_object_add(edited, "assignee", assignee);

    response = github_edit_issue(repo, issueid, edited, token);
    if (response) {
        printf("Successfully claimed issue #%s\n", issueid);
        return 0;
    }

    return 1;
}
