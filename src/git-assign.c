#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

int main(int argc, char *argv[]) {
    char* issueid;
    char* assignee;
    struct json_object* assignee_obj;
    struct json_object* response;

    char* token = config_get_token();
    char* repo = repo_get_repo();
    struct json_object* edited = json_object_new_object();

    if (token == NULL || repo == NULL)
        return 1;

    if (argc <= 2) {
        fprintf(stderr, "Usage: git assign <issue_id> <assignee>\n");
        return 1;
    }

    issueid = argv[1];
    assignee = argv[2];
    assignee_obj = json_object_new_string(assignee);
    json_object_object_add(edited, "assignee", assignee_obj);

    response = github_edit_issue(repo, issueid, edited, token);
    if (response) {
        printf("Successfully assigned issue #%s to %s\n", issueid, assignee);
        return 0;
    }

    return 1;
}
