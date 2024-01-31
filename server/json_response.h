#ifndef JSON_RESPONSE_H
#define JSON_RESPONSE_H

#include <ulfius.h>

void set_json_error_response(struct _u_response *response, int status, const char *error_message);

void set_json_success_response(struct _u_response *response, int status, const char *success_message);
void json_response(struct _u_response *response, int status, const char *message);


#endif // JSON_RESPONSE_H