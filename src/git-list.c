#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <json/json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

void print_header() {
    fprintf(stdout, "\x1b[1m%-5s %-100.100s %-15.15s %-15.15s %-5s\x1b[0m\n", "ID", "Title", "Assignee", "Milestone", "Comments");
}

void print_issue(json_object* issue, bool bold) {
    char *lineformat;

    char* format = "%-5d %-100.100s %-15.15s %-15.15s %-5d\n";
    char* bold_format = "\x1b[1m%-5d %-100.100s %-15.15s %-15.15s %-5d\x1b[0m\n";
    char* pr_format = "%-5d [pr] %-95.95s %-15.15s %-15.15s %-5d\n";
    char* pr_bold_format = "\x1b[1m%-5d [pr] %-95.95s %-15.15s %-15.15s %-5d\x1b[0m\n";
    bool is_pr = jsonh_get_string(issue, "pull_request.html_url") != NULL;
    char* title = jsonh_get_string(issue, "title");
    int number = jsonh_get_int(issue, "number");
    char* milestone = jsonh_get_string(issue, "milestone.title");
    char* assignee = jsonh_get_string(issue, "assignee.login");
    int comments = jsonh_get_int(issue, "comments");

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

    char *user = config_get_user();
    char *token = config_get_token();
    char *repo = repo_get_repo();
    bool bold = false;
    int len = 0;
    int i = 0;
    char *filter = "all";
    char *milestone = NULL;

    if (user == NULL || token == NULL || repo == NULL)
        return 1;

    if (argc > 1) {
        filter = argv[1];
        if (strcmp(filter, "milestone") == 0) {
            if (argc == 3) {
                milestone = argv[2];
            } else {
                fprintf(stderr, "Expected a milestone title, i.e. git list milestone \"my milestone\"\n");
                return 1;
            }
        }
    }

    issues = github_get_issues(repo, filter, milestone, token, user);
    if (!issues)
        return 1;

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
