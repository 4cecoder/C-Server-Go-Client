// appointments_handlers.h

#ifndef APPOINTMENTS_HANDLERS_H
#define APPOINTMENTS_HANDLERS_H

#include <ulfius.h>

// Handles GET requests for appointments
int callback_appointments_get(const struct _u_request *request, struct _u_response *response, void *user_data);

// Handles POST requests for appointments
int callback_appointments_post(const struct _u_request *request, struct _u_response *response, void *user_data);

// Handles PUT requests for appointments
int callback_appointments_put(const struct _u_request *request, struct _u_response *response, void *user_data);

// Handles DELETE requests for appointments
int callback_appointments_delete(const struct _u_request *request, struct _u_response *response, void *user_data);

#endif // APPOINTMENTS_HANDLERS_H
