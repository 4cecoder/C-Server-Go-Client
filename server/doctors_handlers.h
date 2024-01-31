// doctors_handlers.h

#ifndef DOCTORS_HANDLERS_H
#define DOCTORS_HANDLERS_H

#include <ulfius.h>

// Handles GET requests for doctors
int callback_doctors_get(const struct _u_request *request, struct _u_response *response, void *user_data);

// Handles POST requests for doctors
int callback_doctors_post(const struct _u_request *request, struct _u_response *response, void *user_data);

// Handles PUT requests for doctors
int callback_doctors_put(const struct _u_request *request, struct _u_response *response, void *user_data);

// Handles DELETE requests for doctors
int callback_doctors_delete(const struct _u_request *request, struct _u_response *response, void *user_data);

#endif // DOCTORS_HANDLERS_H
