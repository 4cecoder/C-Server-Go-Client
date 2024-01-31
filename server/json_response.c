#include "json_response.h"
#include <jansson.h>

void set_json_error_response(struct _u_response *response, int status, const char *error_message) {
  json_t *error_json = json_pack("{ss}", "error", error_message);
  ulfius_set_json_body_response(response, status, error_json);
  json_decref(error_json);
}
void set_json_success_response(struct _u_response *response, int status, const char *success_message) {
  json_t *success_json = json_pack("{ss}", "message", success_message);
  ulfius_set_json_body_response(response, status, success_json);
  json_decref(success_json);
}


void json_response(struct _u_response *response, int status, const char *message) {
  if (status >= 400) {
    set_json_error_response(response, status, message);
  } else {
    json_t *success_json = json_pack("{ss}", "message", message);
    ulfius_set_json_body_response(response, status, success_json);
    json_decref(success_json);
  }
}