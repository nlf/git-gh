#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include <json/json.h>
#include <json/json_object_private.h>
#include "config.h"

extern struct json_object* config_get_config() {
    int fileLength;
    char *config;
    struct json_object* json;
    json = (struct json_object*)calloc(sizeof(struct json_object), 1);
    wordexp_t configPath;
    wordexp("~/.gitgh", &configPath, 0);
    FILE *fp = fopen(configPath.we_wordv[0], "r");
    wordfree(&configPath);
    if (fp == NULL) {
        printf("Invalid configuration found\nPlease run git ghsetup to create one\n");
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    fileLength = ftell(fp);
    rewind(fp);

    config = calloc(fileLength + 1, sizeof(char));
    if (config == NULL) {
        fprintf(stderr, "insufficient memory\n");
        return NULL;
    }

    fread(config, fileLength, 1, fp);
    fclose(fp);
    json = json_tokener_parse(config);
    free(config);
    return json;
}

extern char* config_get_token() {
    json_object* config = config_get_config();
    json_object* token;
    if (json_object_object_get_ex(config, "token", &token))
        return (char *)json_object_get_string(token);
    printf("Invalid configuration found\nPlease run git ghsetup to create one\n");
    return NULL;
}

extern char* config_get_user() {
    json_object* config = config_get_config();
    json_object* user;
    if (json_object_object_get_ex(config, "user", &user))
        return (char *)json_object_get_string(user);
    printf("Invalid configuration found\nPlease run git ghsetup to create one\n");
    return NULL;
}

extern int config_write_config(struct json_object* config) {
    wordexp_t configPath;
    wordexp("~/.gitgh", &configPath, 0);
    FILE *fp = fopen(configPath.we_wordv[0], "w");
    wordfree(&configPath);
    if (fp == NULL)
        return 0;
    fputs(json_object_get_string(config), fp);
    fclose(fp);
    return 1;
}
