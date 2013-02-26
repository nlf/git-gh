#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <json/json.h>
#include "lib/request.h"
#include "lib/config.h"
#include "lib/repo.h"
#include "lib/jsonhelpers.h"

char *boldstart = "\x1b[1m";
char *boldend = "\x1b[0m";

void printComment(struct json_object* comment) {
    const char* user = getString(comment, "user.login");
    const char* body = getString(comment, "body");
    printf("%s%s%s:\t%s\n\n", boldstart, user, boldend, body);
}

void printComments(char* repo, struct json_object* issue, const char* token) {
    struct json_object* response;
    const char* issue_id = getString(issue, "number");
    int querylen = strlen(repo) + strlen(issue_id) + 25;
    char query[querylen];
    snprintf(query, querylen, "/repos/%s/issues/%s/comments", repo, issue_id);

    response = makeRequest(query, token);
    int len = json_object_array_length(response);
    int i;
    struct json_object* comment;
    for (i = 0; i < len; i++) {
        comment = json_object_array_get_idx(response, i);
        printComment(comment);
        free(comment);
    }
}

void printDetail(struct json_object* issue, const char* token, char* repo) {
    const char* title = getString(issue, "title");
    const char* user = getString(issue, "user.login");
    const char* assignee = getString(issue, "assignee.login");
    const char* milestone = getString(issue, "milestone.title");
    const char* url = getString(issue, "pull_request.html_url");
    const char* status = getString(issue, "state");
    int comments = getInt(issue, "comments");
    char* type = url == NULL ? "Issue" : "Pull Request";
    printf("%sTitle:%s %s\t\t%sOpened by:%s %s\t\t%sAssigned to:%s %s\t\t%sMilestone:%s %s\n", boldstart, boldend, title, boldstart, boldend, user, boldstart, boldend, assignee, boldstart, boldend, milestone);
    printf("%sURL:%s %s\t\t%sType:%s %s\t\t%sStatus:%s %s\n", boldstart, boldend, url, boldstart, boldend, type, boldstart, boldend, status);
    printf("\n");
    if (comments > 0)
        printComments(repo, issue, token);
}

int main(int argc, char *argv[]) {
    struct json_object* config;
    struct json_object* response;
    const char* user;
    const char* token;
    const char* message;
    char* repo;
    char* issueid;

    if (argc > 1) {
        issueid = argv[1];
    } else {
        fprintf(stderr, "Usage: git detail <issue_id>\n");
        return 1;
    }

    config = readConfig();
    if (config == NULL) {
        fprintf(stderr, "You don't appear to have a ~/.gitgh config file\n");
        return 1;
    }
    user = getString(config, "user");
    token = getString(config, "token");

    repo = getRepo();
    if (repo == NULL)
        return 1;

    int querylen = 16 + strlen(issueid) + strlen(repo);
    char query[querylen];
    snprintf(query, querylen, "/repos/%s/issues/%s", repo, issueid);
    
    response = makeRequest(query, token);
    message = getString(response, "message");
    if (message != NULL) {
        fprintf(stderr, "Failed to get issue, message: %s\n", message);
        return 1;
    }

    printDetail(response, token, repo);
    return 0;
}
