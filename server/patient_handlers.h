#ifndef PATIENT_HANDLERS_H
#define PATIENT_HANDLERS_H


#include <ulfius.h>

// Declaration of the function to handle GET requests for patientsA
// Retrieves all patients
int callback_patients_get_all(const struct _u_request *request, struct _u_response *response, void *user_data);


// Declaration of the function to handle GET requests for patients
// Retrieves all patients or a specific patient based on the request parameters
int callback_patients_get(const struct _u_request *request, struct _u_response *response, void *user_data);

// Declaration of the function to handle POST requests for patients
// Creates a new patient based on the request body
int callback_patients_post(const struct _u_request *request, struct _u_response *response, void *user_data);

// Declaration of the function to handle PUT requests for patients
// Updates an existing patient based on the request body
int callback_patients_put(const struct _u_request *request, struct _u_response *response, void *user_data);

// Declaration of the function to handle DELETE requests for patients
// Deletes an existing patient based on the request parameters
int callback_patients_delete(const struct _u_request *request, struct _u_response *response, void *user_data);

#endif // PATIENT_HANDLERS_H
