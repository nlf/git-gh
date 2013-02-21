#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <json/json.h>
#include "lib/request.h"
#include "lib/config.h"
#include "lib/repo.h"

int maxLine = 0;

void printIssueHeader() {
    char* format = "\x1b[1m%-5s %-100.100s %-15.15s %-15.15s %-5s\x1b[0m\n";
    fprintf(stdout, format, "ID", "Title", "Assignee", "Milestone", "Comments");
}

void printIssue(json_object* issue, bool bold) {
    struct json_object *title = NULL;
    struct json_object *number = NULL;
    struct json_object *milestone = NULL;
    struct json_object *milestone_title = NULL;
    struct json_object *assignee = NULL;
    struct json_object *assignee_login = NULL;
    struct json_object *comments = NULL;

    char* format = "%-5d %*.*s %-15.15s %-15.15s %-5d\n";
    if (bold)
        format = "\x1b[1m%-5d %*.*s %-15.15s %-15.15s %-5d\x1b[0m\n";

    title = json_object_object_get(issue, "title");
    number = json_object_object_get(issue, "number");
    comments = json_object_object_get(issue, "comments");
    assignee = json_object_object_get(issue, "assignee");
    if (assignee)
        assignee_login = json_object_object_get(assignee, "login");
    milestone = json_object_object_get(issue, "milestone");
    if (milestone)
        milestone_title = json_object_object_get(milestone, "title");

    fprintf(stdout, format, json_object_get_int(number), -100, 100, json_object_get_string(title), json_object_get_string(assignee_login), json_object_get_string(milestone_title), json_object_get_int(comments));
}

int main() {
    struct json_object* config;
    struct json_object* userobj;
    struct json_object* tokenobj;
    struct json_object* response;
    struct json_object* issue;
    const char *user;
    const char *token;
    char *repo;
    bool bold = false;
    int len = 0;
    int i = 0;

    repo = getRepo();
    if (repo == NULL)
        return 1;
    int pathlen = strlen(repo) + 15;
    char path[pathlen];
    strncpy(path, "/repos/", pathlen);
    strncat(path, repo, pathlen);
    strncat(path, "/issues\0", pathlen);

    config = readConfig();
    if (config == NULL) {
        fprintf(stderr, "You don't appear to have a ~/.gitgh config file\n");
        return 1;
    }
    userobj = json_object_object_get(config, "user");
    user = json_object_get_string(userobj);
    tokenobj = json_object_object_get(config, "token");
    token = json_object_get_string(tokenobj);

    response = makeRequest(path, token);
    len = json_object_array_length(response);
    if (len > 0) {
        printIssueHeader();
        for (i = 0; i < len; i++) {
            issue = json_object_array_get_idx(response, i);
            printIssue(issue, bold);
            bold = !bold;
        }
    } else {
        printf("No open issues found\n");
    }
    return 0;
}
