#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <json/json.h>
#include "lib/config.h"
#include "lib/repo.h"
#include "lib/jsonhelpers.h"
#include "lib/github.h"

char* format = "%-5d %-100.100s %-15.15s %-15.15s %-5d\n";
char* bold_format = "\x1b[1m%-5d %-100.100s %-15.15s %-15.15s %-5d\x1b[0m\n";
char* pr_format = "%-5d [pr] %-95.95s %-15.15s %-15.15s %-5d\n";
char* pr_bold_format = "\x1b[1m%-5d [pr] %-95.95s %-15.15s %-15.15s %-5d\x1b[0m\n";

void print_header() {
    fprintf(stdout, "\x1b[1m%-5s %-100.100s %-15.15s %-15.15s %-5s\x1b[0m\n", "ID", "Title", "Assignee", "Milestone", "Comments");
}

void print_issue(json_object* issue, bool bold) {
    bool is_pr = jsonh_get_string(issue, "pull_request.html_url") != NULL;
    char* title = jsonh_get_string(issue, "title");
    int number = jsonh_get_int(issue, "number");
    char* milestone = jsonh_get_string(issue, "milestone.title");
    char* assignee = jsonh_get_string(issue, "assignee.login");
    int comments = jsonh_get_int(issue, "comments");
    char *lineformat;

    if (bold) {
        if (is_pr) {
            lineformat = pr_bold_format;
        } else {
            lineformat = bold_format;
        }
    } else {
        if (is_pr) {
            lineformat = pr_format;
        } else {
            lineformat = format;
        }
    }
    fprintf(stdout, lineformat, number, title, assignee, milestone, comments);
}

int main(int argc, char *argv[]) {
    struct json_object* issues;
    struct json_object* issue;
    char *user;
    char *token;
    char *repo;
    bool bold = false;
    int len = 0;
    int i = 0;
    char *filter = "all";
    char *milestone = NULL;

    if (argc > 1) {
        filter = argv[1];
        if (strcmp(filter, "milestone") == 0 && argc == 3) {
            milestone = argv[2];
        }
    }

    user = config_get_user();
    token = config_get_token();
    repo = repo_get_repo();
    if (user == NULL || token == NULL || repo == NULL)
        return 1;

    issues = github_get_issues(repo, filter, milestone, token, user);
    len = json_object_array_length(issues);
    if (len > 0) {
        print_header();
        for (i = 0; i < len; i++) {
            issue = json_object_array_get_idx(issues, i);
            print_issue(issue, bold);
            bold = !bold;
        }
    } else {
        printf("No open issues found\n");
    }
    return 0;
}
