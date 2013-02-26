#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <json/json.h>
#include "lib/request.h"
#include "lib/config.h"
#include "lib/repo.h"
#include "lib/jsonhelpers.h"

int maxLine = 0;

void printIssueHeader() {
    char* format = "\x1b[1m%-5s %-100.100s %-15.15s %-15.15s %-5s\x1b[0m\n";
    fprintf(stdout, format, "ID", "Title", "Assignee", "Milestone", "Comments");
}

void printIssue(json_object* issue, bool bold, bool is_pr) {
    const char* title = getString(issue, "title");
    int number = getInt(issue, "number");
    const char* milestone = getString(issue, "milestone.title");
    const char* assignee = getString(issue, "assignee.login");
    int comments = getInt(issue, "comments");
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

    fprintf(stdout, format, number, title, assignee, milestone, comments);
    free(format);
}

int findMilestone(char *search, char *repo, const char *token) {
    struct json_object* response;
    struct json_object* milestone;
    const char *title;
    int number = 0;
    char *path;
    int len;
    int i;
    int pathlen = strlen(repo) + 19;
    path = (char *)calloc(sizeof(char), pathlen);
    sprintf(path, "/repos/%s/milestones", repo);

    response = makeRequest(path, token);
    len = json_object_array_length(response);
    if (len > 0) {
        for (i = 0; i < len; i++) {
            milestone = json_object_array_get_idx(response, i);
            title = getString(milestone, "title");
            if (strcasecmp(title, search) == 0) {
                number = getInt(milestone, "number");
                break;
            }
        }
    }
    free(response);
    return number;
}

int main(int argc, char *argv[]) {
    struct json_object* config;
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
    user = getString(config, "user");
    token = getString(config, "token");

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
            bool is_pull_request = getString(issue, "pull_request.html_url") != NULL;
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
