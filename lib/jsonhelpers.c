#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include "jsonhelpers.h"

struct json_object* findObject(struct json_object* obj, char* path) {
    struct json_object* this_object;
    struct json_object* last_object;
    char* nextTok;
    int pathlen = strlen(path);
    char new_path[pathlen];
    sprintf(new_path, "%s", path);
    nextTok = strtok(new_path, ".");
    this_object = obj;
    do {
        this_object = json_object_object_get(this_object, nextTok);
    } while (nextTok = strtok(NULL, "."));
    return this_object;
}

extern int getInt(struct json_object* obj, char* path) {
    struct json_object* object = findObject(obj, path);
    int i = json_object_get_int(object);
    free(object);
    return i;
}

extern const char* getString(struct json_object* obj, char* path) {
    struct json_object* object = findObject(obj, path);
    const char* str = json_object_get_string(object);
    free(object);
    return str;
}
