#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "github.h"
#include "jsonhelpers.h"

#define BUF_LEN 1024*1024

long bytes_written = 0;

static size_t curl_write(void* buf, size_t len, size_t size, void *userdata) {
    size_t requested_len = len * size;
   
    if (requested_len) {
        if (bytes_written + requested_len >= BUF_LEN)
            return 0;
        memcpy(&((char*)userdata)[bytes_written], buf, requested_len);
        bytes_written += requested_len;
    }
    return requested_len;
}

extern char* curl_raw_request(char* url, char* auth_type, char* auth, char* method, const char* body, char* token) {
    CURL* curl;
    CURLcode res;
    char* buffer;
    char* auth_header;

    int auth_header_len = strlen(auth) + 22;
    struct curl_slist* headers = NULL;

    bytes_written = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        buffer = calloc(BUF_LEN, sizeof(char));

        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "User-Agent: git-gh");
        if (strcasecmp(auth_type, "basic") == 0) {
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curl, CURLOPT_USERPWD, auth);
            if (token) {
                int token_header_len = strlen(token) + 15;
                char* token_header = calloc(token_header_len, sizeof(char));
                snprintf(token_header, token_header_len, "X-GitHub-OTP: %s", token);
                headers = curl_slist_append(headers, token_header);
            }
        } else if (strcasecmp(auth_type, "token") == 0) {
            auth_header = calloc(auth_header_len, sizeof(char));
            snprintf(auth_header, auth_header_len, "Authorization: token %s", auth);
            headers = curl_slist_append(headers, auth_header);
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
        if (strcasecmp(method, "POST") == 0 || strcasecmp(method, "PUT") == 0 || strcasecmp(method, "PATCH") == 0) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(body));
        }
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        if (res != CURLE_OK) {
            fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
            return NULL;
        }
        return buffer;
    } else {
        curl_global_cleanup();
        return NULL;
    }
}

struct json_object* curl_request(char* path, char* auth_type, char* auth, char* method, const char* body, char* token) {
    struct json_object* response;
    char* message;
    char* merged;
    char* buffer;
    char* full_url;

    char* base_url = "https://api.github.com";
    int full_url_len = strlen(base_url) + strlen(path) + 1;

    full_url = calloc(full_url_len, sizeof(char));
    snprintf(full_url, full_url_len, "%s%s", base_url, path);

    buffer = curl_raw_request(full_url, auth_type, auth, method, body, token);
    response = json_tokener_parse(buffer);
    free(buffer);
    message = jsonh_get_string(response, "message");
    merged = jsonh_get_string(response, "merged");
    if (message && (!merged || strcasecmp(merged, "true") != 0)) {
        fprintf(stderr, "Error: %s\n", message);
        return NULL;
    }
    return response;
}

extern char* github_find_milestone(char* repo, char* milestone, char* token) {
    char* query;
    struct json_object* response;
    struct json_object* milestone_obj;
    char* milestone_title;
    int array_len;
    int i;

    char* number = NULL;
    int query_len = strlen(repo) + 19;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/milestones", repo);

    response = curl_request(query, "token", token, "GET", NULL, NULL);
    array_len = json_object_array_length(response);
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
    char* milestone_id;
    struct json_object* response;
    struct json_object* new_array;
    struct json_object* array_item;
    bool is_pr;
    int array_len;
    int i;

    int query_len = strlen(repo) + 15;

    if (strcasecmp(filter, "mine") == 0) {
        query_len += (strlen(user) + 10);
        query = calloc(query_len, sizeof(char));
        snprintf(query, query_len, "/repos/%s/issues?assignee=%s", repo, user);
    } else if (strcasecmp(filter, "unassigned") == 0) {
        query_len += 20;
        query = calloc(query_len, sizeof(char));
        snprintf(query, query_len, "/repos/%s/issues?assignee=none", repo);
    } else if (strcasecmp(filter, "milestone") == 0) {
        milestone_id = github_find_milestone(repo, milestone, token);
        if (!milestone) {
            fprintf(stderr, "Unable to find milestone %s\n", milestone);
            return NULL;
        }
        query_len += (strlen(milestone_id) + 18);
        query = calloc(query_len, sizeof(char));
        snprintf(query, query_len, "/repos/%s/issues?milestone=%s", repo, milestone_id);
    } else {
        query = calloc(query_len, sizeof(char));
        snprintf(query, query_len, "/repos/%s/issues", repo);
    }

    response = curl_request(query, "token", token, "GET", NULL, NULL);
    free(query);
    if (strcasecmp(filter, "prs") == 0 || strcasecmp(filter, "issues") == 0) {
        new_array = json_object_new_array();
        array_len = json_object_array_length(response);
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
    char* query;
    struct json_object* response;

    int query_len = strlen(repo) + strlen(issue) + 16;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/issues/%s", repo, issue);

    response = curl_request(query, "token", token, "GET", NULL, NULL);
    free(query);
    return response;
}

extern struct json_object* github_edit_issue(char* repo, char* issue, struct json_object* edit, char* token) {
    char* query;
    struct json_object* response;

    const char* payload = json_object_get_string(edit);
    int query_len = strlen(repo) + strlen(issue) + 16;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/issues/%s", repo, issue);

    response = curl_request(query, "token", token, "PATCH", payload, NULL);
    free(query);
    return response;
}

extern struct json_object* github_create_issue(char* repo, struct json_object* issue, char* token) {
    char* query;
    struct json_object* response;

    const char* payload = json_object_get_string(issue);
    int query_len = strlen(repo) + 15;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/issues", repo);

    response = curl_request(query, "token", token, "POST", payload, NULL);
    free(query);
    return response;
}

extern struct json_object* github_create_pr(char* repo, struct json_object* pr, char* token) {
    char* query;
    struct json_object* response;

    const char* payload = json_object_get_string(pr);
    int query_len = strlen(repo) + 14;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/pulls", repo);

    response = curl_request(query, "token", token, "POST", payload, NULL);
    free(query);
    return response;
}

extern struct json_object* github_merge_pr(char* repo, char* issue, char* token) {
    char* query;
    struct json_object* response;

    int query_len = strlen(repo) + strlen(issue) + 22;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/pulls/%s/merge", repo, issue);

    response = curl_request(query, "token", token, "PUT", "{}", NULL);
    free(query);
    return response;
}

extern struct json_object* github_get_comments(char* repo, char* issue, char* token) {
    char* query;
    struct json_object* response;

    int query_len = strlen(repo) + strlen(issue) + 25;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/issues/%s/comments", repo, issue);

    response = curl_request(query, "token", token, "GET", NULL, NULL);
    free(query);
    return response;
}

extern struct json_object* github_get_comment(char* repo, char* comment, char* token) {
    char* query;
    struct json_object* response;

    int query_len = strlen(repo) + strlen(comment) + 25;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/issues/comments/%s", repo, comment);

    response = curl_request(query, "token", token, "GET", NULL, NULL);
    free(query);
    return response;
}

extern struct json_object* github_edit_comment(char* repo, char* comment, struct json_object* edit, char* token) {
    char* query;
    struct json_object* response;

    const char* payload = json_object_get_string(edit);
    int query_len = strlen(repo) + strlen(comment) + 25;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/issues/comments/%s", repo, comment);

    response = curl_request(query, "token", token, "PATCH", payload, NULL);
    free(query);
    return response;
}

extern struct json_object* github_del_comment(char* repo, char* comment, char* token) {
    char* query;
    struct json_object* response;

    int query_len = strlen(repo) + strlen(comment) + 25;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/issues/comments/%s", repo, comment);

    response = curl_request(query, "token", token, "DELETE", NULL, NULL);
    free(query);
    return response;
}

extern struct json_object* github_create_comment(char* repo, char* issue, struct json_object* comment, char* token) {
    char* query;
    struct json_object* response;

    const char* payload = json_object_get_string(comment);
    int query_len = strlen(repo) + strlen(issue) + 25;

    query = calloc(query_len, sizeof(char));
    snprintf(query, query_len, "/repos/%s/issues/%s/comments", repo, issue);

    response = curl_request(query, "token", token, "POST", payload, NULL);
    free(query);
    return response;
}

extern struct json_object* github_create_token(char* user, char* pass) {
    char* auth;
    struct json_object* response;

    const char* payload = "{\"scopes\": [\"repo\"], \"note\": \"git-gh extensions\"}";
    int auth_len = strlen(user) + strlen(pass) + 2;
    struct json_object* config = json_object_new_object();

    auth = calloc(auth_len, sizeof(char));
    snprintf(auth, auth_len, "%s:%s", user, pass);

    response = curl_request("/authorizations", "basic", auth, "POST", payload, NULL);
    free(auth);
    if (response) {
        json_object_object_add(config, "token", json_object_object_get(response, "token"));
        json_object_object_add(config, "user", json_object_new_string(user));
        return config;
    }
    return NULL;
}

extern struct json_object* github_create_token_2fa(char* user, char* pass, char* token) {
    char* auth;
    struct json_object* response;

    const char* payload = "{\"scopes\": [\"repo\"], \"note\": \"git-gh extensions\"}";
    int auth_len = strlen(user) + strlen(pass) + 2;
    struct json_object* config = json_object_new_object();

    auth = calloc(auth_len, sizeof(char));
    snprintf(auth, auth_len, "%s:%s", user, pass);

    response = curl_request("/authorizations", "basic", auth, "POST", payload, token);
    free(auth);
    if (response) {
        json_object_object_add(config, "token", json_object_object_get(response, "token"));
        json_object_object_add(config, "user", json_object_new_string(user));
        return config;
    }
    return NULL;
}
