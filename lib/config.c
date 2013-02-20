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

    fseek(fp, 0L, SEEK_END);
    fileLength = ftell(fp);
    rewind(fp);

    config = calloc(fileLength + 1, sizeof(char));
    if (config == NULL) {
        printf("insufficient memory\n");
        return NULL;
    }

    fread(config, fileLength, 1, fp);
    json = json_tokener_parse(config);
    free(config);
    return json;
}
