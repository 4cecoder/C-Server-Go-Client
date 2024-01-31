// medical_records_handlers.h

#ifndef MEDICAL_RECORDS_HANDLERS_H
#define MEDICAL_RECORDS_HANDLERS_H

#include <ulfius.h>

// Handles GET requests for medical records
int callback_medical_records_get(const struct _u_request *request, struct _u_response *response, void *user_data);

// Handles POST requests for medical records
int callback_medical_records_post(const struct _u_request *request, struct _u_response *response, void *user_data);

// Handles PUT requests for medical records
int callback_medical_records_put(const struct _u_request *request, struct _u_response *response, void *user_data);

// Handles DELETE requests for medical records
int callback_medical_records_delete(const struct _u_request *request, struct _u_response *response, void *user_data);

#endif // MEDICAL_RECORDS_HANDLERS_H
