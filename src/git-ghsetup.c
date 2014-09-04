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
    char* token;
    char* prompt;
    int userlen;
    int tokenlen;
    int promptlen;

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

    prompt = calloc(10, sizeof(char));
    printf("Use two-factor authentication? (y/n): ");
    if (fgets(prompt, 10, stdin)) {
        promptlen = strlen(prompt) - 1;
        if (prompt[promptlen] == '\n') prompt[promptlen] = '\0';
    }

    if (strcasecmp(prompt, "y") == 0 || strcasecmp(prompt, "yes") == 0) {
        token = calloc(10, sizeof(char));
        printf("Enter your two-factor token: ");
        if (fgets(token, 10, stdin)) {
            tokenlen = strlen(token) - 1;
            if (token[tokenlen] == '\n') token[tokenlen] = '\0';
        }
        response = github_create_token_2fa(user, pass, token);
    } else {
        response = github_create_token(user, pass);
    }

    if (!response) {
        return 1;
    }

    config_write_config(response);
    printf("Saved new configuration\n");
    return 0;
}
