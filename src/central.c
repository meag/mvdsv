
// central.c - communication with central server

#include "qwsvdef.h"
#include <curl/curl.h>

#define GENERATE_CHALLENGE_PATH     "/Authentication/GenerateChallenge"
#define VERIFY_RESPONSE_PATH        "/Authentication/VerifyResponse"
#define SUBMIT_GAME_PATH            "/Game/Submit"

#ifdef _WIN32
#pragma comment(lib, "libcurld.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#endif

static cvar_t central_server_address = { "cs_address", "" };
static cvar_t central_server_authkey = { "cs_authkey", "" };

static CURLM* curl_handle = NULL;

#define MAX_RESPONSE_LENGTH    4096

typedef void(*web_response_func_t)(struct web_request_data_s* req, qbool valid);

typedef struct web_request_data_s {
	CURL*               handle;
	double              time_sent;

	// response from server
	char                response[MAX_RESPONSE_LENGTH];
	size_t              response_length;

	// form data
	struct curl_httppost *first_form_ptr;
	struct curl_httppost *last_form_ptr;

	// called when response complete
	web_response_func_t onCompleteCallback;
	void*               internal_data;
	char*               request_id;                 // if set, content will be passed to game-mod. will be Q_free()'d

	struct web_request_data_s* next;
} web_request_data_t;

static web_request_data_t* web_requests;

size_t Web_StandardTokenWrite(void* buffer, size_t size, size_t nmemb, void* userp)
{
	web_request_data_t* data = (web_request_data_t*)userp;
	size_t available = sizeof(data->response) - data->response_length;
	if (size * nmemb > available) {
		return 0;
	}
	else if (size * nmemb > 0) {
		memcpy(data->response + data->response_length, buffer, size * nmemb);
		data->response_length += size * nmemb;
	}
	return size * nmemb;
}

typedef struct response_field_s {
	const char* name;
	const char** value;
} response_field_t;

static int ProcessWebResponse(web_request_data_t* req, response_field_t* fields, int field_count)
{
	char* colon, *newline;
	char* start = req->response;
	int i;
	int total_fields = 0;

	// Response should be multiple lines, <Key>:<Value>
	req->response[req->response_length] = '\0';
	while ((colon = strchr(start, ':'))) {
		newline = strchr(colon, '\n');
		if (newline) {
			*newline = '\0';
		}

		*colon = '\0';
		for (i = 0; i < field_count; ++i) {
			if (!strcmp(start, fields[i].name)) {
				*fields[i].value = colon + 1;
			}
		}

		if (newline == NULL) {
			break;
		}

		start = newline + 1;
		while (*start && (*start == 10 || *start == 13)) {
			++start;
		}
		++total_fields;
	}

	return total_fields;
}

void Auth_GenerateChallengeResponse(web_request_data_t* req, qbool valid)
{
	client_t* client = (client_t*) req->internal_data;
	const char* response = NULL;
	const char* challenge = NULL;
	const char* message = NULL;
	response_field_t fields[] = {
		{ "Result", &response },
		{ "Challenge", &challenge },
		{ "Message", &message }
	};

	if (client->login_request_time != req->time_sent) {
		// Ignore result, subsequent request sent
		return;
	}

	req->internal_data = NULL;
	ProcessWebResponse(req, fields, sizeof(fields) / sizeof(fields[0]));

	if (response && !strcmp(response, "Success") && challenge) {
		char buffer[128];
		strlcpy(buffer, "//challenge ", sizeof(buffer));
		strlcat(buffer, challenge, sizeof(buffer));
		strlcat(buffer, "\n", sizeof(buffer));

		strlcpy(client->challenge, challenge, sizeof(client->challenge));
		SV_ClientPrintf(client, PRINT_HIGH, "Challenge stored...\n", message);

		ClientReliableWrite_Begin (client, svc_stufftext, 2+strlen(buffer));
		ClientReliableWrite_String (client, buffer);
	}
	else if (message) {
		SV_ClientPrintf(client, PRINT_HIGH, "Error: %s\n", message);
	}
	else {
		// Maybe add CURLOPT_ERRORBUFFER?
		SV_ClientPrintf(client, PRINT_HIGH, "Error: unknown error\n");
	}
}

void Auth_ProcessLoginAttempt(web_request_data_t* req, qbool valid)
{
	client_t* client = (client_t*) req->internal_data;
	const char* response = NULL;
	const char* login = NULL;
	const char* preferred_alias = NULL;
	const char* message = NULL;
	response_field_t fields[] = {
		{ "Result", &response },
		{ "Alias", &preferred_alias },
		{ "Login", &login },
		{ "Message", &message }
	};

	req->internal_data = NULL;
	if (client->login_request_time != req->time_sent) {
		// Ignore result, subsequent request sent
		return;
	}

	ProcessWebResponse(req, fields, sizeof(fields) / sizeof(fields[0]));

	if (response && !strcmp(response, "Success")) {
		if (login) {
			char oldval[MAX_EXT_INFO_STRING];

			strlcpy(oldval, Info_Get(&client->_userinfo_ctx_, "*auth"), sizeof(oldval));
			strlcpy(client->login, login, sizeof(client->login));

			Info_SetStar(&client->_userinfo_ctx_, "*auth", login);
			ProcessUserInfoChange(client, "*auth", oldval);
		}

		if (!preferred_alias) {
			preferred_alias = login;
		}

		if (preferred_alias) {
			char oldval[MAX_EXT_INFO_STRING];
			strlcpy (oldval, client->name, MAX_EXT_INFO_STRING);

			// Change nickname
			SV_BroadcastPrintf(PRINT_HIGH, "%s logged in as %s\n", client->name, preferred_alias);
			Info_Set(&client->_userinfo_ctx_, "name", preferred_alias);

			ProcessUserInfoChange(client, "name", oldval);

			if (strcmp(preferred_alias, oldval)) {
				// Change name cvar in client
				MSG_WriteByte (&client->netchan.message, svc_stufftext);
				MSG_WriteString (&client->netchan.message, va("name %s\n", preferred_alias));
			}
		}
		client->logged = true;
	}
	else if (message) {
		SV_ClientPrintf(client, PRINT_HIGH, "Error: %s\n", message);
	}
	else {
		// Maybe add CURLOPT_ERRORBUFFER?
		SV_ClientPrintf(client, PRINT_HIGH, "Error: unknown error\n");
	}
}

void Web_PostResponse(web_request_data_t* req, qbool valid)
{
	if (valid) {
		const char* broadcast = NULL;
		const char* upload = NULL;
		response_field_t fields[] = {
			{ "Broadcast", &broadcast },
			{ "Upload", &upload }
		};

		req->response[req->response_length] = '\0';
		ProcessWebResponse(req, fields, sizeof(fields) / sizeof(fields[0]));

		if (broadcast) {
			// Server is making announcement
			SV_BroadcastPrintfEx(PRINT_HIGH, 0, "%s\n", broadcast);
		}
		if (upload) {
			// TODO: Server has requested file upload, do so if it exists...
		}

		//Con_Printf(req->response);
	}
	else {
		Con_Printf("Failure contacting central server.\n");
	}

	if (req->internal_data) {
		Q_free(req->internal_data);
	}
}

void Web_SubmitRequest(const char* url, const char* postData, web_response_func_t callback, void* internal_data)
{
	CURL* req = curl_easy_init();

	web_request_data_t* data = Q_malloc(sizeof(web_request_data_t));
	data->onCompleteCallback = callback;
	data->time_sent = sv.time;
	data->internal_data = internal_data;
	data->handle = req;

	curl_easy_setopt(req, CURLOPT_URL, url);
	curl_easy_setopt(req, CURLOPT_WRITEDATA, data);
	curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, Web_StandardTokenWrite); 
	if (postData != 0 && postData[0]) {
		curl_easy_setopt(req, CURLOPT_POST, 1);
		curl_easy_setopt(req, CURLOPT_COPYPOSTFIELDS, postData);
	}

	curl_multi_add_handle(curl_handle, req);
	data->next = web_requests;
	web_requests = data;
}

void Web_SubmitRequestForm(const char* url, struct curl_httppost *first_form_ptr, struct curl_httppost *last_form_ptr, web_response_func_t callback, const char* requestId, void* internal_data)
{
	CURL* req = curl_easy_init();
	web_request_data_t* data = Q_malloc(sizeof(web_request_data_t));
	CURLFORMcode code = curl_formadd(&first_form_ptr, &last_form_ptr,
		CURLFORM_PTRNAME, "authKey",
		CURLFORM_COPYCONTENTS, central_server_authkey.string,
		CURLFORM_END
	);

	data->onCompleteCallback = callback;
	data->time_sent = sv.time;
	data->internal_data = internal_data;
	data->handle = req;
	data->request_id = requestId && requestId[0] ? strdup(requestId) : NULL;
	data->first_form_ptr = first_form_ptr;

	curl_easy_setopt(req, CURLOPT_URL, url);
	curl_easy_setopt(req, CURLOPT_WRITEDATA, data);
	curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, Web_StandardTokenWrite);
	if (first_form_ptr) {
		curl_easy_setopt(req, CURLOPT_POST, 1);
		curl_easy_setopt(req, CURLOPT_HTTPPOST, first_form_ptr);
	}

	curl_multi_add_handle(curl_handle, req);
	data->next = web_requests;
	web_requests = data;
}

void Central_VerifyChallengeResponse(client_t* client, const char* challenge, const char* response)
{
	char postData[1024];
	char url[512];
	char* encoded_authkey = curl_easy_escape(curl_handle, central_server_authkey.string, 0);
	char* encoded_challenge = curl_easy_escape(curl_handle, challenge, 0);
	char* encoded_response = curl_easy_escape(curl_handle, response, 0);

	if (encoded_authkey && encoded_challenge && encoded_response) {
		strlcpy(url, central_server_address.string, sizeof(url));
		strlcat(url, VERIFY_RESPONSE_PATH, sizeof(url));

		strlcpy(postData, "authKey=", sizeof(postData));
		strlcat(postData, encoded_authkey, sizeof(postData));
		strlcat(postData, "&challenge=", sizeof(postData));
		strlcat(postData, encoded_challenge, sizeof(postData));
		strlcat(postData, "&response=", sizeof(postData));
		strlcat(postData, encoded_response, sizeof(postData));

		client->login_request_time = sv.time;
		Web_SubmitRequest(url, postData, Auth_ProcessLoginAttempt, client);
	}

	curl_free(encoded_authkey);
	curl_free(encoded_challenge);
	curl_free(encoded_response);
}

void Central_GenerateChallenge(client_t* client, const char* username)
{
	char postData[1024];
	char url[512];

	char* encoded_authkey = curl_easy_escape(curl_handle, central_server_authkey.string, 0);
	char* encoded_username = curl_easy_escape(curl_handle, username, strlen(username));

	if (encoded_authkey && encoded_username) {
		strlcpy(url, central_server_address.string, sizeof(url));
		strlcat(url, GENERATE_CHALLENGE_PATH, sizeof(url));

		strlcpy(postData, "authKey=", sizeof(postData));
		strlcat(postData, encoded_authkey, sizeof(postData));
		strlcat(postData, "&userName=", sizeof(postData));
		strlcat(postData, encoded_username, sizeof(postData));

		client->login_request_time = sv.time;
		Web_SubmitRequest(url, postData, Auth_GenerateChallengeResponse, client);
	}

	curl_free(encoded_authkey);
	curl_free(encoded_username);
}

static void Web_SendRequest(qbool post)
{
	char postData[1024];
	char url[512];
	char* requestId = NULL;
	int i;
	char* paramString = (post ? postData : url);
	int max_length = (post ? sizeof(postData) : sizeof(url));
	char* encoded_authkey = curl_easy_escape(curl_handle, central_server_authkey.string, 0);

	if (Cmd_Argc() < 3) {
		Con_Printf("Usage: %s <url> <request-id> (<key> <value>)*\n", Cmd_Argv(0));
		return;
	}

	strlcpy(url, central_server_address.string, sizeof(url));
	strlcat(url, Cmd_Argv(1), sizeof(url));

	requestId = Cmd_Argv(2);
	if (requestId[0]) {
		requestId = Q_strdup(requestId);
	}
	else {
		requestId = NULL;
	}

	postData[0] = '\0';
	strlcpy(postData, "authKey=", sizeof(postData));
	strlcat(postData, encoded_authkey, sizeof(postData));
	curl_free(encoded_authkey);

	if (!post) {
		strlcat(url, "?", sizeof(url));
	}
	for (i = 3; i < Cmd_Argc() - 1; i += 2) {
		char* key = curl_easy_escape(curl_handle, Cmd_Argv(i), 0);
		char* value = curl_easy_escape(curl_handle, Cmd_Argv(i + 1), 0);

		if (!key || !value || strlen(paramString) >= max_length - strlen(key) - strlen(value) - 3) {
			curl_free(key);
			curl_free(value);
			Con_Printf("Request failed\n");
			return;
		}

		strlcat(postData, "&", sizeof(postData));
		strlcat(postData, key, sizeof(postData));
		strlcat(postData, "=", sizeof(postData));
		strlcat(postData, value, sizeof(postData));
		curl_free(key);
		curl_free(value);
	}

	Web_SubmitRequest(url, postData, Web_PostResponse, requestId);
}

static void Web_GetRequest_f(void)
{
	Web_SendRequest(false);
}

static void Web_PostRequest_f(void)
{
	Web_SendRequest(true);
}

static void Web_PostFileRequest_f(void)
{
	struct curl_httppost *first_form_ptr = NULL;
	struct curl_httppost *last_form_ptr = NULL;
	char* requestId = NULL;
	char url[512];
	char path[MAX_OSPATH];
	CURLFORMcode code = 0;
	const char* specified = Cmd_Argv(3);

	if (specified[0] == '*' && specified[1] == '\0') {
		if (!sv.mvdrecording || !demo.dest) {
			Con_Printf("Not recording demo!\n");
			return;
		}

		// FIXME: dunno is this right, just using first dest, also may be we must use demo.dest->path instead of sv_demoDir
		snprintf(path, MAX_OSPATH, "%s/%s/%s", fs_gamedir, sv_demoDir.string, SV_MVDName2Txt(demo.dest->name));
	}
	else {
		snprintf(path, MAX_OSPATH, "%s/%s", fs_gamedir, Cmd_Argv(3));
	}

	if (Cmd_Argc() < 4) {
		Con_Printf("Usage: %s <url> <request-id> <file> (<key> <value>)*\n", Cmd_Argv(0));
		return;
	}

	if (FS_UnsafeFilename(path)) {
		Con_Printf("Filename invalid\n");
		return;
	}

	strlcpy(url, central_server_address.string, sizeof(url));
	strlcat(url, Cmd_Argv(1), sizeof(url));

	requestId = Cmd_Argv(2);
	if (requestId[0]) {
		requestId = Q_strdup(requestId);
	}
	else {
		requestId = NULL;
	}

	{
		FILE* f;
		if (!(f = fopen(path, "rb"))) {
			Con_Printf("Failed to open file\n");
			return;
		}
		fclose(f);
	}

	code = curl_formadd(&first_form_ptr, &last_form_ptr,
		CURLFORM_PTRNAME, "file",
		CURLFORM_FILE, path,
		CURLFORM_END
	);

	strlcpy(url, central_server_address.string, sizeof(url));
	strlcat(url, SUBMIT_GAME_PATH, sizeof(url));

	Web_SubmitRequestForm(url, first_form_ptr, last_form_ptr, Web_PostResponse, requestId, NULL);
}

void Central_ProcessResponses(void)
{
	CURLMsg* msg;
	int running_handles = 0;
	int messages_in_queue = 0;

	curl_multi_perform(curl_handle, &running_handles);

	while ((msg = curl_multi_info_read(curl_handle, &messages_in_queue))) {
		if (msg->msg == CURLMSG_DONE) {
			CURL* handle = msg->easy_handle;
			CURLcode result = msg->data.result;
			web_request_data_t** list_pointer = &web_requests;

			curl_multi_remove_handle(curl_handle, handle);

			while (*list_pointer) {
				web_request_data_t* this = *list_pointer;

				if (this->handle == handle) {
					long response_code = 0;
					curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response_code);
					if (this->onCompleteCallback) {
						this->onCompleteCallback(this, response_code);
					}
					curl_formfree(this->first_form_ptr);
					Q_free(this->request_id);

					*list_pointer = this->next;
					Q_free(this);
					break;
				}

				list_pointer = &this->next;
			}

			curl_easy_cleanup(handle);
		}
	}
}

void Central_Shutdown(void)
{
	if (curl_handle) {
		curl_multi_cleanup(curl_handle);
		curl_handle = NULL;
	}

	curl_global_cleanup();
}

void Central_Init(void)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);

	Cvar_Register(&central_server_address);
	Cvar_Register(&central_server_authkey);

	curl_handle = curl_multi_init();

	if (curl_handle) {
		Cmd_AddCommand("sv_web_get", Web_GetRequest_f);
		Cmd_AddCommand("sv_web_post", Web_PostRequest_f);
		Cmd_AddCommand("sv_web_postfile", Web_PostFileRequest_f);
	}
}
