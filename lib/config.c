#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include <json/json.h>
#include <json/json_object_private.h>
#include "config.h"

extern struct json_object* readConfig() {
    int fileLength;
    char *config;
    struct json_object* json;
    json = (struct json_object*)calloc(sizeof(struct json_object), 1);
    wordexp_t configPath;
    wordexp("~/.gitgh", &configPath, 0);
    FILE *fp = fopen(configPath.we_wordv[0], "r");
    wordfree(&configPath);
    if (fp == NULL)
        return NULL;

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

extern int writeConfig(struct json_object* config) {
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
