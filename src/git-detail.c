#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <json/json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

char* bold(char* str) {
    int str_len;
    char* newstr;

    char* boldstart = "\x1b[1m";
    char* boldend = "\x1b[0m";

    str_len = strlen(boldstart) + strlen(boldend) + strlen(str) + 1;
    newstr = calloc(str_len, sizeof(char));
    snprintf(newstr, str_len, "%s%s%s", boldstart, str, boldend);
    return newstr;
}

void print_issue(struct json_object* issue, char* token, char* repo) {
    struct json_object* comments;
    struct json_object* comment;
    char* comment_user;
    char* comment_body;
    int len;
    int i;

    char* title = jsonh_get_string(issue, "title");
    char* body = jsonh_get_string(issue, "body");
    char* user = jsonh_get_string(issue, "user.login");
    char* assignee = jsonh_get_string(issue, "assignee.login");
    char* milestone = jsonh_get_string(issue, "milestone.title");
    char* url = jsonh_get_string(issue, "pull_request.html_url");
    char* status = jsonh_get_string(issue, "state");
    char* type = url == NULL ? "Issue" : "Pull Request";
    char* issue_id = jsonh_get_string(issue, "number");
    int comment_count = jsonh_get_int(issue, "comments");

    printf("%s: %s\t\t%s: %s\t\t%s: %s\t\t%s: %s\n", bold("Title"), title, bold("Opened by"), user, bold("Assigned to"), assignee, bold("Milestone"), milestone);
    printf("%s: %s\t\t%s: %s\t\t%s: %s\n", bold("URL"), url, bold("Type"), type, bold("Status"), status);
    printf("%s: %s\n\n", bold("Body"), body);

    if (comment_count > 0) {
        comments = github_get_comments(repo, issue_id, token);
        len = json_object_array_length(comments);
        for (i = 0; i < len; i++) {
            comment = json_object_array_get_idx(comments, i);
            comment_user = jsonh_get_string(comment, "user.login");
            comment_body = jsonh_get_string(comment, "body");
            printf("%s:\t%s\n\n", bold(comment_user), comment_body);
        }
    }
}

int main(int argc, char *argv[]) {
    char* issueid;
    struct json_object* issue;

    char* token = config_get_token();
    char* repo = repo_get_repo();

    if (token == NULL || repo == NULL)
        return 1;

    if (argc <= 1) {
        fprintf(stderr, "Usage: git detail <issue_id>\n");
        return 1;
    }

    issueid = argv[1];

    issue = github_get_issue(repo, issueid, token);
    if (issue) {
        print_issue(issue, token, repo);
        return 0;
    }

    return 1;
}
