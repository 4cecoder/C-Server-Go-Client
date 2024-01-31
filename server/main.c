#include "appointments_handlers.h"
#include "database.h"
#include "doctors_handlers.h"
#include "medical_records_handlers.h"
#include "patient_handlers.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ulfius.h>

#define PORT 8080
#define BASE_URL "/api"

int request_handler(void *cls, struct MHD_Connection *connection,
                    const char *url, const char *method,
                    const char *version, const char *upload_data,
                    size_t *upload_data_size, void **con_cls) {

    // Global CORS headers for all responses
    struct MHD_Response *response = MHD_create_response_from_buffer(0, "", MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type, Authorization");

    // Handle preflight (OPTIONS) requests
    if (strcmp(method, "OPTIONS") == 0) {
      const int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    }

    // Check if the request is for an endpoint that is already handled by a specific callback.
    // If so, no action is needed here as the callback will take care of it.
    if (strncmp(url, BASE_URL, strlen(BASE_URL)) == 0) {
        MHD_destroy_response(response); // Destroy the response as it's not needed for callback-handled endpoints
        return MHD_YES;
    }

    // Handling of other endpoints that do not have specific callbacks
    // (example for illustrative purposes)
    if (strcmp(url, "/other-endpoint") == 0) {
        const char *response_page = "<html><body>Response for other endpoint</body></html>";
        MHD_add_response_header(response, "Content-Type", "text/html");
        MHD_destroy_response(response); // Destroy the initial empty response
        response = MHD_create_response_from_buffer(strlen(response_page), (void *)response_page, MHD_RESPMEM_PERSISTENT); // Create a new response with content
        const int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    }

    // Default response for unhandled endpoints
    const char *response_page = "<html><body>404 Not Found</body></html>";
    MHD_add_response_header(response, "Content-Type", "text/html");
    MHD_destroy_response(response); // Destroy the initial empty response
    response = MHD_create_response_from_buffer(strlen(response_page), (void *)response_page, MHD_RESPMEM_PERSISTENT); // Create a new response with content
    const int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}




int callback_api_home(const struct _u_request *request, struct _u_response *response, void *user_data) {
    const char *response_page =
    "<html>"
    "<head>"
    "<style>"
    "  body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f4; color: #333; }"
    "  h1, h2 { color: #5a5a5a; }"
    "  ul { list-style-type: none; padding: 0; }"
    "  li { margin: 10px 0; }"
    "  .container { max-width: 700px; margin: auto; padding: 20px; background-color: #fff; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }"
    "</style>"
    "</head>"
    "<body>"
    "<div class='container'>"
    "<h1>Healthcare System API Documentation</h1>"
    "<h2>Available Endpoints:</h2>"
    "<ul>"
    "<li>GET /api/patients - Retrieves all patients</li>"
    "<li>GET /api/patients/(patientID) - Retrieves a specific patient</li>"
    "<li>POST /api/patients - Creates a new patient</li>"
    "<li>PUT /api/patients/(patientID) - Updates a specific patient</li>"
    "<li>DELETE /api/patients/(patientID) - Deletes a specific patient</li>"
    "<li>GET /api/doctors - Retrieves all doctors</li>"
    "<li>GET /api/doctors/(doctorID) - Retrieves a specific doctor</li>"
    "<li>POST /api/doctors - Creates a new doctor</li>"
    "<li>PUT /api/doctors/(doctorID) - Updates a specific doctor</li>"
    "<li>DELETE /api/doctors/(doctorID) - Deletes a specific doctor</li>"
    "<li>GET /api/appointments - Retrieves all appointments</li>"
    "<li>GET /api/appointments/(appointmentID) - Retrieves a specific appointment</li>"
    "<li>POST /api/appointments - Creates a new appointment</li>"
    "<li>PUT /api/appointments/(appointmentID) - Updates a specific appointment</li>"
    "<li>DELETE /api/appointments/(appointmentID) - Deletes a specific appointment</li>"
    "<li>GET /api/medicalrecords - Retrieves all medical records</li>"
    "<li>GET /api/medicalrecords/(medicalRecordID) - Retrieves a specific medical record</li>"
    "<li>POST /api/medicalrecords - Creates a new medical record</li>"
    "<li>PUT /api/medicalrecords/(medicalRecordID) - Updates a specific medical record</li>"
    "<li>DELETE /api/medicalrecords/(medicalRecordID) - Deletes a specific medical record</li>"
    "</ul>"
    "</div>"
    "</body>"
    "</html>";

    ulfius_set_string_body_response(response, 200, response_page);
    return U_CALLBACK_CONTINUE;
}






int main() {
  // Initialize Yder-ULFIUS logs at DEBUG level
  y_init_logs("Ulfius", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting Ulfius Framework");

  if (init_db() != 0) {
    fprintf(stderr, "Database initialization failed\n");
    return 1;
  }

  struct _u_instance instance;

  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error initializing Ulfius instance\n");
    close_db();
    return 1;
  }

  // Add API home/documentation endpoint
  ulfius_add_endpoint_by_val(&instance, "GET", BASE_URL, NULL, 0, &callback_api_home, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", BASE_URL "/", NULL, 0, &callback_api_home, NULL);


  // Patients endpoints
  // Endpoint for fetching all patients
  ulfius_add_endpoint_by_val(&instance, "GET", BASE_URL "/patients", NULL, 0, &callback_patients_get_all, NULL);

  // Endpoint for fetching a single patient by ID
  ulfius_add_endpoint_by_val(&instance, "GET", BASE_URL "/patients/:id", NULL, 0, &callback_patients_get, NULL);

  ulfius_add_endpoint_by_val(&instance, "POST", BASE_URL "/patients", NULL, 0, &callback_patients_post, NULL);
  ulfius_add_endpoint_by_val(&instance, "PUT", BASE_URL "/patients", NULL, 0, &callback_patients_put, NULL);
  ulfius_add_endpoint_by_val(&instance, "DELETE", BASE_URL "/patients", NULL, 0, &callback_patients_delete, NULL);

  // Doctors endpoints
  ulfius_add_endpoint_by_val(&instance, "GET", BASE_URL "/doctors", NULL, 0, &callback_doctors_get, NULL);
  ulfius_add_endpoint_by_val(&instance, "POST", BASE_URL "/doctors", NULL, 0, &callback_doctors_post, NULL);
  ulfius_add_endpoint_by_val(&instance, "PUT", BASE_URL "/doctors", NULL, 0, &callback_doctors_put, NULL);
  ulfius_add_endpoint_by_val(&instance, "DELETE", BASE_URL "/doctors", NULL, 0, &callback_doctors_delete, NULL);

  // Appointments endpoints
  ulfius_add_endpoint_by_val(&instance, "GET", BASE_URL "/appointments", NULL, 0, &callback_appointments_get, NULL);
  ulfius_add_endpoint_by_val(&instance, "POST", BASE_URL "/appointments", NULL, 0, &callback_appointments_post, NULL);
  ulfius_add_endpoint_by_val(&instance, "PUT", BASE_URL "/appointments", NULL, 0, &callback_appointments_put, NULL);
  ulfius_add_endpoint_by_val(&instance, "DELETE", BASE_URL "/appointments", NULL, 0, &callback_appointments_delete, NULL);

  // Medical Records endpoints
  ulfius_add_endpoint_by_val(&instance, "GET", BASE_URL "/medicalrecords", NULL, 0, &callback_medical_records_get, NULL);
  ulfius_add_endpoint_by_val(&instance, "POST", BASE_URL "/medicalrecords", NULL, 0, &callback_medical_records_post, NULL);
  ulfius_add_endpoint_by_val(&instance, "PUT", BASE_URL "/medicalrecords", NULL, 0, &callback_medical_records_put, NULL);
  ulfius_add_endpoint_by_val(&instance, "DELETE", BASE_URL "/medicalrecords", NULL, 0, &callback_medical_records_delete, NULL);

  if (ulfius_start_framework(&instance) == U_OK) {
    printf("Server running on port %d\n", PORT);
    while (1) {
      sleep(1);  // Keep the program running
    }
  } else {
    fprintf(stderr, "Error starting Ulfius framework\n");
  }

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);
  close_db();
  return 0;
}

