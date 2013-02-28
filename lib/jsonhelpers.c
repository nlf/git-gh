#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include "jsonhelpers.h"

extern struct json_object* jsonh_find_object(struct json_object* obj, char* path) {
    struct json_object* this_object;
    char* token;
    int path_len;
    char* new_path;
    
    path_len = strlen(path) + 1;
    new_path = (char*)calloc(sizeof(char), path_len);
    snprintf(new_path, path_len, "%s", path);

    token = strtok(new_path, ".");
    this_object = obj;
    do {
        this_object = json_object_object_get(this_object, token);
    } while ((token = strtok(NULL, ".")));

    free(new_path);
    return this_object;
}

extern int jsonh_get_int(struct json_object* obj, char* path) {
    struct json_object* object;
    int i;

    object = jsonh_find_object(obj, path);
    i = json_object_get_int(object);
    return i;
}

extern char* jsonh_get_string(struct json_object* obj, char* path) {
    struct json_object* object;
    const char* str;

    object = jsonh_find_object(obj, path);
    str = json_object_get_string(object);
    return (char*)str;
}
