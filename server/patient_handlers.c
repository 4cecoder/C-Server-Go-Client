#include "patient_handlers.h"

#include "database.h"
#include "cors.h"
#include "json_response.h"
#include <string.h>


// Patients CRUD Callback Functions
int callback_patients_get_all(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_patients_get_all: Starting to fetch all patients\n");
  const char *sql = "SELECT id, name FROM Patients";
  sqlite3_stmt *stmt;
  const int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    return U_CALLBACK_ERROR;
  }

  json_t *json_response = json_array();
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const int id = sqlite3_column_int(stmt, 0);
    const char *name = (const char *)sqlite3_column_text(stmt, 1);
    printf("Found patient - ID: %d, Name: %s\n", id, name);

    // Manually creating a JSON object and setting its key-value pairs
    json_t *json_patient = json_object();
    if (json_patient) {
      json_object_set_new(json_patient, "id", json_integer(id));
      json_object_set_new(json_patient, "name", json_string(name));
      json_array_append_new(json_response, json_patient);
    } else {
      printf("Failed to create JSON object for patient - ID: %d, Name: %s\n", id, name);
    }
  }
  sqlite3_finalize(stmt);

  printf("Finished fetching patients. Total found: %zu\n", json_array_size(json_response));

  ulfius_set_json_body_response(response, 200, json_response);
  json_decref(json_response);
  set_cors_headers(response);

  return U_CALLBACK_CONTINUE;
}


int callback_patients_get(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_patients_get: Function called\n");
  const char *id_str = u_map_get(request->map_url, "id");
  printf("callback_patients_get: Requested ID: %s\n", id_str ? id_str : "null");
  const int id = id_str ? atoi(id_str) : -1;
  Patient patient;
  const int result = read_patient(id, &patient);

  json_t *json_response;
  if (result == 0) {
    printf("callback_patients_get: Patient found with ID: %d\n", id);
    json_response = json_pack("{sisi}", "id", patient.id, "name", patient.name);
  } else {
    printf("callback_patients_get: Patient not found\n");
    json_response = json_object();
  }

  ulfius_set_json_body_response(response, 200, json_response);
  json_decref(json_response);
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}

int callback_patients_post(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("Patients POST called - Starting\n");

  // Attempt to parse the JSON body of the request
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  if (!json_request) {
    printf("Failed to parse JSON request body\n");
    set_json_error_response(response, 400, "Bad Request: Unable to parse JSON body");
    return U_CALLBACK_CONTINUE; // Early return if JSON parsing fails
  }

  // Attempt to retrieve the 'name' field from the JSON request
  const char *name = json_string_value(json_object_get(json_request, "name"));
  if (name) {
    printf("Received patient name: %s\n", name);

    // Prepare new patient struct and attempt to create the patient in the database
    Patient new_patient = { .id = 0, .name = "" };
    strncpy(new_patient.name, name, sizeof(new_patient.name) - 1);
    new_patient.name[sizeof(new_patient.name) - 1] = '\0'; // Ensure null termination

    printf("Attempting to create patient in database: %s\n", new_patient.name);
    const int result = create_patient(&new_patient);

    if (result == 0) {
      printf("Patient created successfully in database: %s\n", new_patient.name);
      json_t *success_json = json_pack("{ss}", "message", "Patient created successfully");
      ulfius_set_json_body_response(response, 201, success_json);
      json_decref(success_json);
    } else {
      printf("Database operation failed: Error creating patient: %s\n", new_patient.name);
      set_json_error_response(response, 500, "Internal Server Error: Failed to create patient");
    }
  } else {
    printf("Invalid or missing 'name' field in patient data\n");
    set_json_error_response(response, 400, "Invalid Data: Missing 'name' field");
  }

  // Cleanup
  if (json_request) {
    json_decref(json_request);
  }
  set_cors_headers(response);

  printf("Patients POST called - Ending\n");
  return U_CALLBACK_CONTINUE;
}



int callback_patients_put(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_patients_put: Function called\n");
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  const int id = json_integer_value(json_object_get(json_request, "id"));
  const char *name = json_string_value(json_object_get(json_request, "name"));
  printf("callback_patients_put: Patient ID: %d, Name: %s\n", id, name);

  if (id > 0 && name) {
    Patient patient = { .id = id, .name = "" };
    strncpy(patient.name, name, sizeof(patient.name) - 1);
    const int result = update_patient(&patient);

    if (result == 0) {
      printf("callback_patients_put: Patient updated successfully\n");
      ulfius_set_string_body_response(response, 200, "Patient updated");
    } else {
      printf("callback_patients_put: Error updating patient\n");
      ulfius_set_string_body_response(response, 500, "Error updating patient");
    }
  } else {
    printf("callback_patients_put: Invalid data\n");
    ulfius_set_string_body_response(response, 400, "Invalid data");
  }

  if (json_request) {
    json_decref(json_request);
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}

int callback_patients_delete(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_patients_delete: Function called\n");
  const char *id_str = u_map_get(request->map_url, "id");
  const int id = id_str ? atoi(id_str) : -1;
  printf("callback_patients_delete: Patient ID: %d\n", id);

  if (id > 0) {
    const int result = delete_patient(id);
    if (result == 0) {
      printf("callback_patients_delete: Patient deleted successfully\n");
      ulfius_set_string_body_response(response, 200, "Patient deleted");
    } else {
      printf("callback_patients_delete: Error deleting patient\n");
      ulfius_set_string_body_response(response, 500, "Error deleting patient");
    }
  } else {
    printf("callback_patients_delete: Invalid ID\n");
    ulfius_set_string_body_response(response, 400, "Invalid ID");
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}
