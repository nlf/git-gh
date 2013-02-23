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

void printIssue(json_object* issue, bool bold, bool is_pr) {
    struct json_object *title = NULL;
    struct json_object *number = NULL;
    struct json_object *milestone = NULL;
    struct json_object *milestone_title = NULL;
    struct json_object *assignee = NULL;
    struct json_object *assignee_login = NULL;
    struct json_object *comments = NULL;
    char *boldstart = "\x1b[1m";
    char *boldend = "\x1b[0m";
    int formatlen;
    char *lineformat;
    char *format;

    if (is_pr) {
        lineformat = "%-5d [pr] %-95.95s %-15.15s %-15.15s %-5d";
    } else {
        lineformat = "%-5d %-100.100s %-15.15s %-15.15s %-5d";
    }
    if (bold) {
        formatlen = strlen(lineformat) + strlen(boldstart) + strlen(boldend) + 1;
        format = (char *)calloc(sizeof(char), formatlen);
        sprintf(format, "\x1b[1m%s\x1b[0m\n", lineformat);
    } else {
        formatlen = strlen(lineformat) + 1;
        format = (char *)calloc(sizeof(char), formatlen);
        sprintf(format, "%s\n", lineformat);
    }

    title = json_object_object_get(issue, "title");
    number = json_object_object_get(issue, "number");
    comments = json_object_object_get(issue, "comments");
    assignee = json_object_object_get(issue, "assignee");
    if (assignee)
        assignee_login = json_object_object_get(assignee, "login");
    milestone = json_object_object_get(issue, "milestone");
    if (milestone)
        milestone_title = json_object_object_get(milestone, "title");

    fprintf(stdout, format, json_object_get_int(number), json_object_get_string(title), json_object_get_string(assignee_login), json_object_get_string(milestone_title), json_object_get_int(comments));
    free(format);
}

int findMilestone(char *search, char *repo, const char *token) {
    struct json_object* response;
    struct json_object* milestone;
    struct json_object* titleobj;
    struct json_object* numberobj;
    const char *title;
    int number = 0;
    char *path;
    int len;
    int i;
    int pathlen = strlen(repo) + 19;
    path = (char *)calloc(sizeof(char), pathlen);
    strncpy(path, "/repos/", pathlen);
    strncat(path, repo, pathlen);
    strncat(path, "/milestones\0", pathlen);

    response = makeRequest(path, token);
    len = json_object_array_length(response);
    if (len > 0) {
        for (i = 0; i < len; i++) {
            milestone = json_object_array_get_idx(response, i);
            titleobj = json_object_object_get(milestone, "title");
            title = json_object_get_string(titleobj);
            if (strcasecmp(title, search) == 0) {
                numberobj = json_object_object_get(milestone, "number");
                number = json_object_get_int(numberobj);
                break;
            }
        }
    }
    free(response);
    return number;
}

int main(int argc, char *argv[]) {
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
    char *filter = "all";
    char *milestone = NULL;

    if (argc > 1) {
        filter = argv[1];
        if (strcmp(filter, "milestone") == 0 && argc == 3) {
            milestone = argv[2];
        }
    }

    config = readConfig();
    if (config == NULL) {
        fprintf(stderr, "You don't appear to have a ~/.gitgh config file\n");
        return 1;
    }
    userobj = json_object_object_get(config, "user");
    user = json_object_get_string(userobj);
    tokenobj = json_object_object_get(config, "token");
    token = json_object_get_string(tokenobj);

    repo = getRepo();
    if (repo == NULL)
        return 1;
    int querylen = 8;
    char *query;
    if (strcmp(filter, "mine") == 0) {
        querylen += (10 + strlen(user));
        query = (char *)calloc(sizeof(char), querylen);
        sprintf(query, "/issues?assignee=%s", user);
    } else if (strcmp(filter, "unassigned") == 0) {
        querylen += 14;
        query = (char *)calloc(sizeof(char), querylen);
        sprintf(query, "/issues?assignee=none");
    } else if (strcmp(filter, "milestone") == 0) {
        int milestonenum = findMilestone(milestone, repo, token);
        if (milestonenum) {
            querylen += (18 + strlen(milestone));
            query = (char *)calloc(sizeof(char), querylen);
            sprintf(query, "/issues?milestone=%d", milestonenum);
        } else {
            fprintf(stderr, "Unable to find milestone \"%s\"\n", milestone);
            return 1;
        }
    } else {
        query = (char *)calloc(sizeof(char), querylen);
        sprintf(query, "/issues");
    }

    int pathlen = strlen(repo) + 7 + strlen(query);
    char path[pathlen];
    sprintf(path, "/repos/%s%s", repo, query);
    free(query);

    response = makeRequest(path, token);
    len = json_object_array_length(response);
    if (len > 0) {
        printIssueHeader();
        for (i = 0; i < len; i++) {
            issue = json_object_array_get_idx(response, i);
            bool is_pull_request = json_object_get_string(json_object_object_get(json_object_object_get(issue, "pull_request"), "html_url")) != NULL;
            bool valid = true;
            if (strcmp(filter, "prs") == 0 && !is_pull_request)
                valid = false;
            if (strcmp(filter, "issues") == 0 && is_pull_request)
                valid = false;
            if (valid) {
                printIssue(issue, bold, is_pull_request);
                bold = !bold;
            }
        }
    } else {
        printf("No open issues found\n");
    }
    free(response);
    return 0;
}
