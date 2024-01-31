// appointments_handlers.c

#include "appointments_handlers.h"
#include "database.h" // Include your database operations header file here
#include "json_response.h"
#include "cors.h"

#include <string.h>

// Appointments CRUD Callback Functions

// GET: Retrieve an Appointment
int callback_appointments_get(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_appointments_get: Function called\n");
  const char *id_str = u_map_get(request->map_url, "id");
  printf("callback_appointments_get: Requested ID: %s\n", id_str ? id_str : "null");
  const int id = id_str ? atoi(id_str) : -1;
  Appointment appointment;
  const int result = read_appointment(id, &appointment);

  json_t *json_response;
  if (result == 0) {
    printf("callback_appointments_get: Appointment found for ID: %d\n", id);
    json_response = json_pack("{sisisisi}", "id", appointment.id, "patient_id", appointment.patient_id, "doctor_id", appointment.doctor_id, "date", appointment.date);
  } else {
    printf("callback_appointments_get: Appointment not found\n");
    json_response = json_object();
  }

  ulfius_set_json_body_response(response, 200, json_response);
  json_decref(json_response);
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}

// POST: Create an Appointment
// Appointments POST Callback Function
int callback_appointments_post(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("Appointments POST called\n");
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  const int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
  const int doctor_id = json_integer_value(json_object_get(json_request, "doctor_id"));
  const char *date = json_string_value(json_object_get(json_request, "date"));

  if (patient_id > 0 && doctor_id > 0 && date) {
    printf("Creating appointment: Patient ID %d, Doctor ID %d, Date %s\n", patient_id, doctor_id, date);
    Appointment new_appointment = { .id = 0, .patient_id = patient_id, .doctor_id = doctor_id, .date = "" };
    strncpy(new_appointment.date, date, sizeof(new_appointment.date) - 1);
    const int result = create_appointment(&new_appointment);

    if (result == 0) {
      printf("Appointment created successfully\n");
      ulfius_set_string_body_response(response, 201, "Appointment created");
    } else {
      printf("Error creating appointment\n");
      set_json_error_response(response, 500, "Error creating appointment");
    }
  } else {
    printf("Invalid appointment data\n");
    set_json_error_response(response, 400, "Invalid data");
  }

  if (json_request) {
    json_decref(json_request);
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}


// PUT: Update an Appointment
int callback_appointments_put(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_appointments_put: Function called\n");
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  int id = json_integer_value(json_object_get(json_request, "id"));
  int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
  int doctor_id = json_integer_value(json_object_get(json_request, "doctor_id"));
  const char *date = json_string_value(json_object_get(json_request, "date"));
  printf("callback_appointments_put: ID: %d, Patient ID: %d, Doctor ID: %d, Date: %s\n", id, patient_id, doctor_id, date);

  if (id > 0 && patient_id > 0 && doctor_id > 0 && date) {
    Appointment appointment = { .id = id, .patient_id = patient_id, .doctor_id = doctor_id, .date = "" };
    strncpy(appointment.date, date, sizeof(appointment.date) - 1);
    const int result = update_appointment(&appointment);

    if (result == 0) {
      printf("callback_appointments_put: Appointment updated successfully\n");
      ulfius_set_string_body_response(response, 200, "Appointment updated");
    } else {
      printf("callback_appointments_put: Error updating appointment\n");
      ulfius_set_string_body_response(response, 500, "Error updating appointment");
    }
  } else {
    printf("callback_appointments_put: Invalid data\n");
    ulfius_set_string_body_response(response, 400, "Invalid data");
  }

  if (json_request) {
    json_decref(json_request);
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}

// DELETE: Delete an Appointment
int callback_appointments_delete(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_appointments_delete: Function called\n");
  const char *id_str = u_map_get(request->map_url, "id");
  printf("callback_appointments_delete: Requested ID: %s\n", id_str ? id_str : "null");
  int id = id_str ? atoi(id_str) : -1;

  if (id > 0) {
    const int result = delete_appointment(id);
    if (result == 0) {
      printf("callback_appointments_delete: Appointment deleted successfully\n");
      ulfius_set_string_body_response(response, 200, "Appointment deleted");
    } else {
      printf("callback_appointments_delete: Error deleting appointment\n");
      ulfius_set_string_body_response(response, 500, "Error deleting appointment");
    }
  } else {
    printf("callback_appointments_delete: Invalid ID\n");
    ulfius_set_string_body_response(response, 400, "Invalid ID");
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}



// MedicalRecords CRUD Callback Functions

// GET: Retrieve a Medical Record
int callback_medical_records_get(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_medical_records_get: Function called\n");
  const char *id_str = u_map_get(request->map_url, "id");
  printf("callback_medical_records_get: Requested ID: %s\n", id_str ? id_str : "null");
  const int id = id_str ? atoi(id_str) : -1;
  MedicalRecord record;
  const int result = read_medical_record(id, &record);

  json_t *json_response;
  if (result == 0) { // Record found
    printf("callback_medical_records_get: Medical record found for ID: %d\n", id);
    json_response = json_pack("{sisis}", "id", record.id, "patient_id", record.patient_id, "details", record.details);
  } else { // Record not found or error
    printf("callback_medical_records_get: Medical record not found or error\n");
    json_response = json_object(); // Returns an empty JSON object
  }

  ulfius_set_json_body_response(response, 200, json_response);
  json_decref(json_response);
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}

