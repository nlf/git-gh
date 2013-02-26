#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include "lib/config.h"
#include "lib/repo.h"
#include "lib/jsonhelpers.h"
#include "lib/request.h"

int main(int argc, char *argv[]) {
    struct json_object* config = readConfig();
    const char* user = getString(config, "user");
    const char* token = getString(config, "token");
    char* repo = getRepo();
    char* issueid;
    char* assignee;

    if (argc > 2) {
        issueid = argv[1];
        assignee = argv[2];
    } else {
        fprintf(stderr, "Usage: git assign <issue_id> <assignee>\n");
        return 1;
    }

    int querylen = strlen(repo) + strlen(issueid) + 16;
    char query[querylen];
    snprintf(query, querylen, "/repos/%s/issues/%s", repo, issueid);

    struct json_object* response = makeRequest(query, token);
    const char* message = getString(response, "message");
    free(response);
    if (message != NULL) {
        fprintf(stderr, "Failed to get issue, message: %s\n", message);
        return 1;
    }

    struct json_object* edited = json_object_new_object();
    struct json_object* assignee_obj = json_object_new_string(assignee);
    json_object_object_add(edited, "assignee", assignee_obj);
    const char* post_object = json_object_get_string(edited);
    int postlen = strlen(post_object) + 1;
    char post[postlen];
    snprintf(post, postlen, "%s", post_object);
    response = makePatch(query, token, post);
    const char* edit_message = getString(response, "message");
    if (edit_message != NULL) {
        fprintf(stderr, "Failed to assign issue to %s: %s\n", assignee, message);
        return 1;
    }
    printf("Successfully assigned issue #%s to %s\n", issueid, assignee);
    return 0;
}
