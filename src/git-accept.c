#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

int main(int argc, char *argv[]) {
    char* issueid;
    struct json_object* response;

    char* token = config_get_token();
    char* repo = repo_get_repo();

    if (token == NULL || repo == NULL)
        return 1;

    if (argc <= 1) {
        fprintf(stderr, "Usage: git accept <issue_id>\n");
        return 1;
    }

    issueid = argv[1];

    response = github_merge_pr(repo, issueid, token);
    if (response) {
        printf("Successfully merged pr #%s\n", issueid);
        return 0;
    }

    return 1;
}
