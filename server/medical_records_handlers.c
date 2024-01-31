// medical_records_handlers.c

#include "medical_records_handlers.h"

#include "cors.h"
#include "json_response.h"
#include <string.h>
#include <database.h>
#include <secure/_string.h>

// POST: Create a Medical Record
// MedicalRecords POST Callback Function
int callback_medical_records_post(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("MedicalRecords POST called\n");
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  const int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
  const char *details = json_string_value(json_object_get(json_request, "details"));

  if (patient_id > 0 && details) {
    printf("Creating medical record: Patient ID %d, Details %s\n", patient_id, details);
    MedicalRecord new_record = { .id = 0, .patient_id = patient_id, .details = "" };
    strncpy(new_record.details, details, sizeof(new_record.details) - 1);
    const int result = create_medical_record(&new_record);

    if (result == 0) {
      printf("Medical record created successfully\n");
      ulfius_set_string_body_response(response, 201, "Medical record created");
    } else {
      printf("Error creating medical record\n");
      set_json_error_response(response, 500, "Error creating medical record");
    }
  } else {
    printf("Invalid medical record data\n");
    set_json_error_response(response, 400, "Invalid data");
  }

  if (json_request) {
    json_decref(json_request);
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}


// PUT: Update a Medical Record
int callback_medical_records_put(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_medical_records_put: Function called\n");
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  const int id = json_integer_value(json_object_get(json_request, "id"));
  const int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
  const char *details = json_string_value(json_object_get(json_request, "details"));
  printf("callback_medical_records_put: ID: %d, Patient ID: %d, Details: %s\n", id, patient_id, details);

  if (id > 0 && patient_id > 0 && details) {
    MedicalRecord record = { .id = id, .patient_id = patient_id, .details = "" };
    strncpy(record.details, details, sizeof(record.details) - 1);
    const int result = update_medical_record(&record);

    if (result == 0) {
      printf("callback_medical_records_put: Medical record updated successfully\n");
      ulfius_set_string_body_response(response, 200, "Medical record updated");
    } else {
      printf("callback_medical_records_put: Error updating medical record\n");
      ulfius_set_string_body_response(response, 500, "Error updating medical record");
    }
  } else {
    printf("callback_medical_records_put: Invalid data\n");
    ulfius_set_string_body_response(response, 400, "Invalid data");
  }

  if (json_request) {
    json_decref(json_request);
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}

// DELETE: Delete a Medical Record
int callback_medical_records_delete(const struct _u_request *request, struct _u_response *response, void *user_data) {
  printf("callback_medical_records_delete: Function called\n");
  const char *id_str = u_map_get(request->map_url, "id");
  printf("callback_medical_records_delete: Medical Record ID: %s\n", id_str ? id_str : "null");
  const int id = id_str ? atoi(id_str) : -1;

  if (id > 0) {
    const int result = delete_medical_record(id);
    if (result == 0) {
      printf("callback_medical_records_delete: Medical record deleted successfully\n");
      ulfius_set_string_body_response(response, 200, "Medical record deleted");
    } else {
      printf("callback_medical_records_delete: Error deleting medical record\n");
      ulfius_set_string_body_response(response, 500, "Error deleting medical record");
    }
  } else {
    printf("callback_medical_records_delete: Invalid ID\n");
    ulfius_set_string_body_response(response, 400, "Invalid ID");
  }
  set_cors_headers(response);
  return U_CALLBACK_CONTINUE;
}
