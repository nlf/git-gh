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

    char* issueid;
    char* assignee;

    if (argc > 2) {
        issueid = argv[1];
        assignee = argv[2];
    } else {
        fprintf(stderr, "Usage: git assign <issue_id> <assignee>\n");
        return 1;
    }

    struct json_object* edited = json_object_new_object();
    struct json_object* assignee_obj = json_object_new_string(assignee);
    json_object_object_add(edited, "assignee", assignee_obj);

    struct json_object* response = github_edit_issue(repo, issueid, edited, token);
    if (response)
        printf("Successfully assigned issue #%s to %s\n", issueid, assignee);
    return 0;
}
