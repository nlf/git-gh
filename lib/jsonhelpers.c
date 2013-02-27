#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include "jsonhelpers.h"

extern struct json_object* jsonh_find_object(struct json_object* obj, char* path) {
    struct json_object* this_object;
    struct json_object* last_object;
    char* nextTok;
    int pathlen = strlen(path) + 1;
    char new_path[pathlen];
    snprintf(new_path, pathlen, "%s", path);
    nextTok = strtok(new_path, ".");
    this_object = obj;
    do {
        this_object = json_object_object_get(this_object, nextTok);
    } while (nextTok = strtok(NULL, "."));
    return this_object;
}

extern int jsonh_get_int(struct json_object* obj, char* path) {
    struct json_object* object = jsonh_find_object(obj, path);
    int i = json_object_get_int(object);
    return i;
}

extern char* jsonh_get_string(struct json_object* obj, char* path) {
    struct json_object* object = jsonh_find_object(obj, path);
    const char* str = json_object_get_string(object);
    return (char*)str;
}
