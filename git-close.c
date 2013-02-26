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

    if (argc > 1) {
        issueid = argv[1];
    } else {
        fprintf(stderr, "Usage: git close <issue_id>\n");
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
    struct json_object* state = json_object_new_string("closed");
    json_object_object_add(edited, "state", state);
    const char* post_object = json_object_get_string(edited);
    int postlen = strlen(post_object) + 1;
    char post[postlen];
    snprintf(post, postlen, "%s", post_object);
    response = makePatch(query, token, post);
    const char* edit_message = getString(response, "message");
    if (edit_message != NULL) {
        fprintf(stderr, "Failed to close issue #%s\n", issueid);
        return 1;
    }
    printf("Successfully closed issue #%s\n", issueid);
    return 0;
}
