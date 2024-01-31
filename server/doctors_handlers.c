// doctors_handlers.c

#include "doctors_handlers.h"

#include "cors.h"
#include "database.h" // Include your database operations header file here
#include "json_response.h"

#include <string.h>
// #include "utils.h" // Include if you have utility functions (like error handling, CORS setting, etc.)

// Implementation of GET request handler for doctors
// Doctors CRUD Callback Functions

int callback_doctors_get(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_doctors_get: Function called\n");
  const char *id_str = u_map_get(request->map_url, "id");
  printf("callback_doctors_get: Requested ID: %s\n", id_str ? id_str : "null");
  int id = id_str ? atoi(id_str) : -1;
  Doctor doctor;
  int result = read_doctor(id, &doctor);

  json_t *json_response;
  if (result == 0) {
    printf("callback_doctors_get: Doctor found with ID: %d\n", id);
    json_response = json_pack("{sisi}", "id", doctor.id, "name", doctor.name, "specialty", doctor.specialty);
  } else {
    printf("callback_doctors_get: Doctor not found\n");
    json_response = json_object();
  }

  ulfius_set_json_body_response(response, 200, json_response);
  json_decref(json_response);
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}

// Doctors POST Callback Function
int callback_doctors_post(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("Doctors POST called\n");
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  const char *name = json_string_value(json_object_get(json_request, "name"));
  const char *specialty = json_string_value(json_object_get(json_request, "specialty"));

  if (name && specialty) {
    printf("Creating doctor: %s, Specialty: %s\n", name, specialty);
    Doctor new_doctor = { .id = 0, .name = "", .specialty = "" };
    strncpy(new_doctor.name, name, sizeof(new_doctor.name) - 1);
    strncpy(new_doctor.specialty, specialty, sizeof(new_doctor.specialty) - 1);
    const int result = create_doctor(&new_doctor);

    if (result == 0) {
      printf("Doctor created successfully\n");
      ulfius_set_string_body_response(response, 201, "Doctor created");
    } else {
      printf("Error creating doctor\n");
      set_json_error_response(response, 500, "Error creating doctor");
    }
  } else {
    printf("Invalid doctor data\n");
    set_json_error_response(response, 400, "Invalid data");
  }

  if (json_request) {
    json_decref(json_request);
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}

int callback_doctors_put(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_doctors_put: Function called\n");
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  int id = json_integer_value(json_object_get(json_request, "id"));
  const char *name = json_string_value(json_object_get(json_request, "name"));
  const char *specialty = json_string_value(json_object_get(json_request, "specialty"));
  printf("callback_doctors_put: Doctor ID: %d, Name: %s, Specialty: %s\n", id, name, specialty);

  if (id > 0 && name && specialty) {
    Doctor doctor = { .id = id, .name = "", .specialty = "" };
    strncpy(doctor.name, name, sizeof(doctor.name) - 1);
    strncpy(doctor.specialty, specialty, sizeof(doctor.specialty) - 1);
    const int result = update_doctor(&doctor);

    if (result == 0) {
      printf("callback_doctors_put: Doctor updated successfully\n");
      ulfius_set_string_body_response(response, 200, "Doctor updated");
    } else {
      printf("callback_doctors_put: Error updating doctor\n");
      ulfius_set_string_body_response(response, 500, "Error updating doctor");
    }
  } else {
    printf("callback_doctors_put: Invalid data\n");
    ulfius_set_string_body_response(response, 400, "Invalid data");
  }

  if (json_request) {
    json_decref(json_request);
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}

int callback_doctors_delete(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_doctors_delete: Function called\n");
  const char *id_str = u_map_get(request->map_url, "id");
  printf("callback_doctors_delete: Doctor ID: %s\n", id_str ? id_str : "null");
  int id = id_str ? atoi(id_str) : -1;

  if (id > 0) {
    int result = delete_doctor(id);
    if (result == 0) {
      printf("callback_doctors_delete: Doctor deleted successfully\n");
      ulfius_set_string_body_response(response, 200, "Doctor deleted");
    } else {
      printf("callback_doctors_delete: Error deleting doctor\n");
      ulfius_set_string_body_response(response, 500, "Error deleting doctor");
    }
  } else {
    printf("callback_doctors_delete: Invalid ID\n");
    ulfius_set_string_body_response(response, 400, "Invalid ID");
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}
