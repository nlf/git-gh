#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <json-c/json.h>
#include "../lib/config.h"
#include "../lib/jsonhelpers.h"
#include "../lib/github.h"

int main() {
    struct json_object* response;
    char* user;
    char* pass;
    int userlen;

    printf("This utility will create a new ~/.gitgh configuration file for you\n");
    printf("NOTE: Your password will *not* be stored, it is only used to generate a token for authorization\n\n");

    user = calloc(128, sizeof(char));
    printf("Enter your github username: ");
    if (fgets(user, 128, stdin)) {
        userlen = strlen(user) - 1;
        if (user[userlen] == '\n') user[userlen] = '\0';
    }

    pass = calloc(128, sizeof(char));
    pass = getpass("Enter your github password: ");

    if (user == NULL || pass == NULL) {
        fprintf(stderr, "You must enter both username and password to continue\n");
        return 1;
    }

    response = github_create_token(user, pass);
    if (!response)
        return 1;

    config_write_config(response);
    printf("Saved new configuration\n");
    return 0;
}
