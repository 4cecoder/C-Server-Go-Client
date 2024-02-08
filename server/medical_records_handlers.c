// medical_records_handlers.c


#include "medical_records_handlers.h"
#include "database.h"
#include "cors.h"
#include "json_response.h"
#include <ulfius.h>
#include <string.h>


// POST: Create a Medical Record
int callback_medical_records_post(const struct _u_request *request, struct _u_response *response, void *user_data) {
    printf("MedicalRecords POST called\n");
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    if (json_request == NULL) {
        set_json_error_response(response, 400, "Invalid JSON");
        return U_CALLBACK_COMPLETE;
    }

    const int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
    const char *details = json_string_value(json_object_get(json_request, "details"));

    if (patient_id > 0 && details && strlen(details) < sizeof(((MedicalRecord*)0)->details)) {
        MedicalRecord new_record;
        memset(&new_record, 0, sizeof(MedicalRecord)); // Initialize the structure
        new_record.patient_id = patient_id;
        strncpy(new_record.details, details, sizeof(new_record.details) - 1);

        if (create_medical_record(&new_record) == 0) {
            ulfius_set_string_body_response(response, 201, "Medical record created successfully");
        } else {
            set_json_error_response(response, 500, "Failed to create medical record");
        }
    } else {
        set_json_error_response(response, 400, "Invalid data provided");
    }

    json_decref(json_request);
    set_cors_headers(response);
    return U_CALLBACK_CONTINUE;
}

// PUT: Update a Medical Record
int callback_medical_records_put(const struct _u_request *request, struct _u_response *response, void *user_data) {
    printf("MedicalRecords PUT called\n");
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    if (json_request == NULL) {
        set_json_error_response(response, 400, "Invalid JSON");
        return U_CALLBACK_COMPLETE;
    }

    const int id = json_integer_value(json_object_get(json_request, "id"));
    const int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
    const char *details = json_string_value(json_object_get(json_request, "details"));

    if (id > 0 && patient_id > 0 && details && strlen(details) < sizeof(((MedicalRecord*)0)->details)) {
        MedicalRecord record;
        memset(&record, 0, sizeof(MedicalRecord)); // Initialize the structure
        record.id = id;
        record.patient_id = patient_id;
        strncpy(record.details, details, sizeof(record.details) - 1);

        if (update_medical_record(&record) == 0) {
            ulfius_set_string_body_response(response, 200, "Medical record updated successfully");
        } else {
            set_json_error_response(response, 500, "Failed to update medical record");
        }
    } else {
        set_json_error_response(response, 400, "Invalid data provided");
    }

    json_decref(json_request);
    set_cors_headers(response);
    return U_CALLBACK_CONTINUE;
}

// DELETE: Delete a Medical Record
int callback_medical_records_delete(const struct _u_request *request, struct _u_response *response, void *user_data) {
    printf("MedicalRecords DELETE called\n");
    const char *id_str = u_map_get(request->map_url, "id");
    if (id_str == NULL) {
        set_json_error_response(response, 400, "No ID provided");
        return U_CALLBACK_COMPLETE;
    }

    int id = atoi(id_str);
    if (id > 0) {
        if (delete_medical_record(id) == 0) {
            ulfius_set_string_body_response(response, 200, "Medical record deleted successfully");
        } else {
            set_json_error_response(response, 500, "Failed to delete medical record");
        }
    } else {
        set_json_error_response(response, 400, "Invalid ID provided");
    }

    set_cors_headers(response);
    return U_CALLBACK_CONTINUE;
}
