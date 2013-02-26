#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json/json.h>
#include <json/json_object_private.h>
#include "request.h"

#define BUF_LEN 1024*1024

char *baseUrl = "https://api.github.com";
long bytesWritten = 0;
long bytesRead = 0;
long bytesTotal = 0;

static int writeFn(void *buf, size_t len, size_t size, void *userdata) {
    size_t sLen = len * size;
    if (sLen > 0) {
        if (bytesWritten + sLen >= BUF_LEN) {
            fprintf(stderr, "buffer size exceeded\n");
            return 0;
        }
        memcpy(&((char*)userdata)[bytesWritten], buf, sLen);
        bytesWritten += sLen;
    }
    return sLen;
}

extern struct json_object *makePostBasic(char *path, char *user, char *pass, char* body) {
    CURL *curl;
    CURLcode res;
    struct json_object *response = NULL;
    char* data;
    data = (char *)calloc(sizeof(char), BUF_LEN);

    int bodylen = strlen(body);
    char* postdata;
    postdata = (char *)calloc(sizeof(char), bodylen);
    memcpy(postdata, body, bodylen);
    bytesTotal = bodylen;
    bytesRead = 0;

    struct curl_slist *headers = NULL;
    int authlen = strlen(user) + strlen(pass) + 2;
    char auth[authlen];
    snprintf(auth, authlen, "%s:%s", user, pass);
    
    int urllen = strlen(baseUrl) + strlen(path) + 2;
    char fullUrl[urllen];
    snprintf(fullUrl, urllen, "%s%s", baseUrl, path);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl);
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, bytesTotal);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFn);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return NULL;
        }
        response = json_tokener_parse(data);
        free(data);
        return response;
    } else {
        curl_global_cleanup();
        return NULL;
    }
}

extern struct json_object *makePost(char *path, const char *token, char* body) {
    CURL *curl;
    CURLcode res;
    struct json_object *response = NULL;
    char* data;
    data = (char *)calloc(sizeof(char), BUF_LEN);

    int bodylen = strlen(body);
    char* postdata;
    postdata = (char *)calloc(sizeof(char), bodylen);
    memcpy(postdata, body, bodylen);
    bytesTotal = bodylen;
    bytesRead = 0;

    struct curl_slist *headers = NULL;
    char *tokenHeader = "Authorization: token ";
    int headerlen = strlen(tokenHeader) + strlen(token) + 2;
    char authHeader[headerlen];
    snprintf(authHeader, headerlen, "%s%s", tokenHeader, token);
    
    int urllen = strlen(baseUrl) + strlen(path) + 2;
    char fullUrl[urllen];
    snprintf(fullUrl, urllen, "%s%s", baseUrl, path);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, authHeader);
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, bytesTotal);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFn);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return NULL;
        }
        response = json_tokener_parse(data);
        free(data);
        return response;
    } else {
        curl_global_cleanup();
        return NULL;
    }
}

extern struct json_object *makeRequest(char *path, const char *token) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    struct json_object *response = NULL;
    response = (struct json_object*)calloc(sizeof(struct json_object), 1);
    char *data;
    char *tokenHeader = "Authorization: token ";
    int headerlen = strlen(tokenHeader) + strlen(token) + 2;
    char authHeader[headerlen];
    snprintf(authHeader, headerlen, "%s%s", tokenHeader, token);

    data = (char *)calloc(sizeof(char), BUF_LEN);

    int urllen = strlen(baseUrl) + strlen(path) + 2;
    char fullUrl[urllen];
    snprintf(fullUrl, urllen, "%s%s", baseUrl, path);

    bytesWritten = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, authHeader);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFn);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return NULL;
        }
        response = json_tokener_parse(data);
        free(data);
        return response;
    } else {
        curl_global_cleanup();
        return NULL;
    }
}
