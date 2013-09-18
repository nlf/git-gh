#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <json-c/json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

int main(int argc, char *argv[]) {
    char* issueid;
    char* diff_url;
    char* diff;
    struct json_object* issue;

    char* token = config_get_token();
    char* repo = repo_get_repo();

    if (token == NULL || repo == NULL)
        return 1;

    if (argc <= 1) {
        fprintf(stderr, "Usage: git review <issue_id>\n");
        return 1;
    }

    issueid = argv[1];

    issue = github_get_issue(repo, issueid, token);
    if (issue) {
        diff_url = jsonh_get_string(issue, "pull_request.diff_url");
        if (diff_url) {
            diff = curl_raw_request(diff_url, "token", token, "GET", NULL);
            printf("%s\n", diff);
            return 0;
        }
        return 1;
    }

    return 1;
}
