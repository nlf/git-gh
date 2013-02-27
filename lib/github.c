#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <json/json.h>
#include "github.h"
#include "jsonhelpers.h"

#define BUF_LEN 1024*1024

long bytes_written = 0;
char* base_url = "https://api.github.com";

static int curl_write(void* buf, size_t len, size_t size, void *userdata) {
    size_t requested_len = len * size;
    if (requested_len) {
        if (bytes_written + requested_len >= BUF_LEN)
            return 0;
        memcpy(&((char*)userdata)[bytes_written], buf, requested_len);
        bytes_written += requested_len;
    }
    return requested_len;
}

struct json_object* curl_request(char* path, char* auth_type, char* auth, char* method, const char* body) {
    bytes_written = 0;
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        char *buffer;
        buffer = (char *)calloc(sizeof(buffer), BUF_LEN);

        int full_url_len = strlen(base_url) + strlen(path) + 1;
        char full_url[full_url_len];
        snprintf(full_url, full_url_len, "%s%s", base_url, path);

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (auth_type == "basic") {
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curl, CURLOPT_USERPWD, auth);
        } else if (auth_type == "token") {
            int auth_header_len = strlen(auth) + 22;
            char auth_header[auth_header_len];
            snprintf(auth_header, auth_header_len, "Authorization: token %s", auth);
            headers = curl_slist_append(headers, auth_header);
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, full_url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
        if (method == "POST" || method == "PUT" || method == "PATCH") {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(body));
        }
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        if (res != CURLE_OK) {
            free(buffer);
            return NULL;
        }
        //printf("raw response: %s\n", buffer);
        struct json_object* response = json_tokener_parse(buffer);
        free(buffer);
        char* message = jsonh_get_string(response, "message");
        if (message) {
            fprintf(stderr, "Error: %s\n", message);
            return NULL;
        }
        return response;
    } else {
        curl_global_cleanup();
        return NULL;
    }
}

extern char* github_find_milestone(char* repo, char* milestone, char* token) {
    int query_len = strlen(repo) + 19;
    char* query = (char*)calloc(sizeof(char), query_len);
    snprintf(query, query_len, "/repos/%s/milestones", repo);

    struct json_object* response = curl_request(query, "token", token, "GET", NULL);
    struct json_object* milestone_obj;
    char* milestone_title;
    int array_len = json_object_array_length(response);
    int i;
    char* number = NULL;
    for (i = 0; i < array_len; i++) {
        milestone_obj = json_object_array_get_idx(response, i);
        milestone_title = jsonh_get_string(milestone_obj, "title");
        if (strcasecmp(milestone_title, milestone) == 0) {
            number = jsonh_get_string(milestone_obj, "number");
            break;
        }
    }
    return number;
}

extern struct json_object* github_get_issues(char* repo, char* filter, char* milestone, char* token, char* user) {
    char* query;
    int query_len = strlen(repo) + 15;
    if (strcasecmp(filter, "mine") == 0) {
        query_len += (strlen(user) + 10);
        query = (char*)calloc(sizeof(char), query_len);
        snprintf(query, query_len, "/repos/%s/issues?assignee=%s", repo, user);
    } else if (strcasecmp(filter, "unassigned") == 0) {
        query_len += 20;
        query = (char*)calloc(sizeof(char), query_len);
        snprintf(query, query_len, "/repos/%s/issues?assignee=none", repo);
    } else if (strcasecmp(filter, "milestone") == 0) {
        char* milestone_id = github_find_milestone(repo, milestone, token);
        if (!milestone) {
            fprintf(stderr, "Unable to find milestone %s\n", milestone);
            return NULL;
        }
        query_len += (strlen(milestone_id) + 18);
        query = (char*)calloc(sizeof(char), query_len);
        snprintf(query, query_len, "/repos/%s/issues?milestone=%s", repo, milestone_id);
    } else {
        query = (char*)calloc(sizeof(char), query_len);
        snprintf(query, query_len, "/repos/%s/issues", repo);
    }

    struct json_object* response = curl_request(query, "token", token, "GET", NULL);
    if (strcasecmp(filter, "prs") == 0 || strcasecmp(filter, "issues") == 0) {
        struct json_object* new_array = json_object_new_array();
        struct json_object* array_item;
        bool is_pr;
        int array_len = json_object_array_length(response);
        int i;
        for (i = 0; i < array_len; i++) {
            array_item = json_object_array_get_idx(response, i);
            is_pr = jsonh_get_string(array_item, "pull_request.html_url") != NULL;
            if (is_pr && (strcasecmp(filter, "prs") == 0)) {
                json_object_array_add(new_array, array_item);
            } else if (!is_pr && (strcasecmp(filter, "issues") == 0)) {
                json_object_array_add(new_array, array_item);
            }
        }
        return new_array;
    }
    return response;
}

extern struct json_object* github_get_issue(char* repo, char* issue, char* token) {
    int query_len = strlen(repo) + strlen(issue) + 16;
    char query[query_len];
    snprintf(query, query_len, "/repos/%s/issues/%s", repo, issue);

    struct json_object* response = curl_request(query, "token", token, "GET", NULL);
    return response;
}

extern struct json_object* github_edit_issue(char* repo, char* issue, struct json_object* edit, char* token) {
    const char* payload = json_object_get_string(edit);
    int query_len = strlen(repo) + strlen(issue) + 16;
    char query[query_len];
    snprintf(query, query_len, "/repos/%s/issues/%s", repo, issue);

    struct json_object* response = curl_request(query, "token", token, "PATCH", payload);
    return response;
}

extern struct json_object* github_create_issue(char* repo, struct json_object* issue, char* token) {
    const char* payload = json_object_get_string(issue);
    int query_len = strlen(repo) + 15;
    char query[query_len];
    snprintf(query, query_len, "/repos/%s/issues", repo);

    struct json_object* response = curl_request(query, "token", token, "POST", payload);
    return response;
}

extern struct json_object* github_create_pr(char* repo, struct json_object* pr, char* token) {
    const char* payload = json_object_get_string(pr);
    int query_len = strlen(repo) + 14;
    char query[query_len];
    snprintf(query, query_len, "/repos/%s/pulls", repo);

    struct json_object* response = curl_request(query, "token", token, "POST", payload);
    return response;
}

extern struct json_object* github_get_comments(char* repo, char* issue, char* token) {
    int query_len = strlen(repo) + strlen(issue) + 25;
    char query[query_len];
    snprintf(query, query_len, "/repos/%s/issues/%s/comments", repo, issue);

    struct json_object* response = curl_request(query, "token", token, "GET", NULL);
    return response;
}

extern struct json_object* github_get_comment(char* repo, char* comment, char* token) {
    int query_len = strlen(repo) + strlen(comment) + 25;
    char query[query_len];
    snprintf(query, query_len, "/repos/%s/issues/comments/%s", repo, comment);

    struct json_object* response = curl_request(query, "token", token, "GET", NULL);
    return response;
}

extern struct json_object* github_edit_comment(char* repo, char* comment, struct json_object* edit, char* token) {
    const char* payload = json_object_get_string(edit);
    int query_len = strlen(repo) + strlen(comment) + 25;
    char query[query_len];
    snprintf(query, query_len, "/repos/%s/issues/comments/%s", repo, comment);

    struct json_object* response = curl_request(query, "token", token, "PATCH", payload);
    return response;
}

extern struct json_object* github_del_comment(char* repo, char* comment, char* token) {
    int query_len = strlen(repo) + strlen(comment) + 25;
    char query[query_len];
    snprintf(query, query_len, "/repos/%s/issues/comments/%s", repo, comment);

    struct json_object* response = curl_request(query, "token", token, "DELETE", NULL);
    return response;
}

extern struct json_object* github_create_comment(char* repo, char* issue, struct json_object* comment, char* token) {
    const char* payload = json_object_get_string(comment);
    int query_len = strlen(repo) + strlen(issue) + 25;
    char query[query_len];
    snprintf(query, query_len, "/repos/%s/issues/%s/comments", repo, issue);

    struct json_object* response = curl_request(query, "token", token, "POST", payload);
    return response;
}

extern struct json_object* github_create_token(char* user, char* pass) {
    int auth_len = strlen(user) + strlen(pass) + 2;
    char auth[auth_len];
    snprintf(auth, auth_len, "%s:%s", user, pass);

    char *payload = "{\"scopes\": [\"repo\"], \"note\": \"git-gh extensions\"}";
    struct json_object* response = curl_request("/authorizations", "basic", auth, "POST", payload);
    if (response) {
        struct json_object* config = json_object_new_object();
        json_object_object_add(config, "token", json_object_object_get(response, "token"));
        json_object_object_add(config, "user", json_object_new_string(user));
        return config;
    }
    return NULL;
}
