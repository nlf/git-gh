#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include <json.h>
#include <json_object_private.h>
#include "config.h"
#include "jsonhelpers.h"

extern struct json_object* config_get_config() {
    int file_length;
    char* config;
    struct json_object* json;
    FILE* fp;
    wordexp_t config_path;

    json = calloc(1, sizeof(struct json_object));
    wordexp("~/.gitgh", &config_path, 0);
    fp = fopen(config_path.we_wordv[0], "r");
    wordfree(&config_path);
    if (fp == NULL) {
        printf("Invalid configuration found\nPlease run git ghsetup to create one\n");
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    file_length = ftell(fp);
    rewind(fp);

    config = calloc(file_length + 1, sizeof(char));
    if (config == NULL) {
        fprintf(stderr, "insufficient memory\n");
        return NULL;
    }

    fread(config, file_length, 1, fp);
    fclose(fp);
    json = json_tokener_parse(config);
    free(config);
    return json;
}

extern char* config_get_token() {
    struct json_object* config = config_get_config();
    char* token = jsonh_get_string(config, "token");

    free(config);
    return token;
}

extern char* config_get_user() {
    struct json_object* config = config_get_config();
    char* user = jsonh_get_string(config, "user");

    free(config);
    return user;
}

extern int config_write_config(struct json_object* config) {
    wordexp_t config_path;
    FILE* fp;

    wordexp("~/.gitgh", &config_path, 0);
    fp = fopen(config_path.we_wordv[0], "w");
    wordfree(&config_path);
    if (fp == NULL)
        return 0;
    fputs(json_object_get_string(config), fp);
    fclose(fp);
    return 1;
}
