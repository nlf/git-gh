extern char* curl_raw_request(char*, char*, char*, char*, const char*);

extern char* github_find_milestone(char*, char*, char*);

extern struct json_object* github_get_issues(char*, char*, char*, char*, char*);
extern struct json_object* github_get_issue(char*, char*, char*);
extern struct json_object* github_edit_issue(char*, char*, struct json_object*, char*);
extern struct json_object* github_create_issue(char*, struct json_object*, char*);

extern struct json_object* github_create_pr(char*, struct json_object*, char*);
extern struct json_object* github_merge_pr(char*, char*, char*);

extern struct json_object* github_get_comments(char*, char*, char*);
extern struct json_object* github_get_comment(char*, char*, char*);
extern struct json_object* gitub_edit_comment(char*, char*, struct json_object*, char*);
extern struct json_object* github_del_comment(char*, char*, char*);
extern struct json_object* github_create_comment(char*, char*, struct json_object*, char*);

extern struct json_object* github_create_token(char*, char*);
