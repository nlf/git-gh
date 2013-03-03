#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <json/json.h>
#include "../lib/config.h"
#include "../lib/repo.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

int main(int argc, char *argv[]) {
    int c;
    extern char* optarg;
    char* title;
    char* body;
    struct json_object* response;

    char* token = config_get_token();
    char* repo = repo_get_repo();
    struct json_object* issue = json_object_new_object();

    if (token == NULL || repo == NULL)
        return 1;

    while ((c = getopt(argc, argv, "t:m:")) != EOF) {
        switch (c) {
            case 't':
                title = optarg;
                break;
            case 'm':
                body = optarg;
                break;
        }
    }
    if (title == NULL || body == NULL) {
        fprintf(stderr, "Usage: git issue -m \"issue body\" -t \"issue title\"\n");
        return 1;
    }

    json_object_object_add(issue, "title", json_object_new_string(title));
    json_object_object_add(issue, "body", json_object_new_string(body));
    
    response = github_create_issue(repo, issue, token);
    if (response) {
        printf("Created issue #%s\n", jsonh_get_string(response, "number"));
        return 0;
    }

    return 1;
}
