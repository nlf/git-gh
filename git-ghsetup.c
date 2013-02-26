#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <json/json.h>
#include "lib/config.h"
#include "lib/jsonhelpers.h"
#include "lib/request.h"

int main() {
    struct json_object* response;
    char* user;
    char* pass;
    int userlen;
    printf("This utility will create a new ~/.gitgh configuration file for you\n");
    printf("NOTE: Your password will *not* be stored, it is only used to generate a token for authorization\n\n");

    user = (char *)calloc(sizeof(char), 128);
    printf("Enter your github username: ");
    fgets(user, 128, stdin);
    userlen = strlen(user) - 1;
    if (user[userlen] == '\n') user[userlen] = '\0';

    pass = (char *)calloc(sizeof(char), 128);
    pass = getpass("Enter your github password: ");

    char *payload = "{\"scopes\": [\"repo\"], \"note\": \"git-gh extensions\"}";
    response = makePostBasic("/authorizations", user, pass, payload);
    const char *message = getString(response, "message");
    if (message != NULL) {
        fprintf(stderr, "Failed to create token: %s\n", message);
        return 1;
    }

    struct json_object* config = json_object_new_object();
    struct json_object* token = json_object_object_get(response, "token");
    struct json_object* username = json_object_new_string(user);
    json_object_object_add(config, "token", token);
    json_object_object_add(config, "user", username);

    writeConfig(config);
}
