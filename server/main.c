#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ulfius.h>

#define PORT 8080
#define BASE_URL "/api"


typedef struct {
  int id;
  char name[100];
} Patient;

typedef struct {
  int id;
  char name[100];
  char specialty[100];
} Doctor;

typedef struct {
  int id;
  int patient_id;
  int doctor_id;
  char date[100];
} Appointment;

typedef struct {
  int id;
  int patient_id;
  char details[255];
} MedicalRecord;

sqlite3 *db;

int init_db() {
  const char* db_path = "health.db";
  int rc = sqlite3_open(db_path, &db);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1; // Non-zero return value indicates failure
  }

  // Example SQL to create tables (if they don't exist)
  const char *sql =
      "CREATE TABLE IF NOT EXISTS Patients ("
      "  id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "  name TEXT NOT NULL"
      "); "
      "CREATE TABLE IF NOT EXISTS Doctors ("
      "  id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "  name TEXT NOT NULL, "
      "  specialty TEXT NOT NULL"
      "); "
      "CREATE TABLE IF NOT EXISTS Appointments ("
      "  id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "  patient_id INTEGER NOT NULL, "
      "  doctor_id INTEGER NOT NULL, "
      "  date TEXT NOT NULL, "
      "  FOREIGN KEY(patient_id) REFERENCES Patients(id), "
      "  FOREIGN KEY(doctor_id) REFERENCES Doctors(id)"
      "); "
      "CREATE TABLE IF NOT EXISTS MedicalRecords ("
      "  id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "  patient_id INTEGER NOT NULL, "
      "  details TEXT NOT NULL, "
      "  FOREIGN KEY(patient_id) REFERENCES Patients(id)"
      ");";


  char *err_msg = NULL;
  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return 1; // Failure
  }

  return 0; // Success
}


// Create a new patient
int create_patient(Patient *patient) {
  char *sql = "INSERT INTO Patients (name) VALUES (?)";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, patient->name, -1, SQLITE_STATIC);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE ? 0 : 1;
}

// Read a patient's details by ID
int read_patient(int id, Patient *patient) {
  char *sql = "SELECT id, name FROM Patients WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    patient->id = sqlite3_column_int(stmt, 0);
    strcpy(patient->name, (char *)sqlite3_column_text(stmt, 1));
  }
  sqlite3_finalize(stmt);
  return rc == SQLITE_ROW ? 0 : 1;
}

// Update a patient's details
int update_patient(const Patient *patient) {
  char *sql = "UPDATE Patients SET name = ? WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, patient->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, patient->id);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE ? 0 : 1;
}

// Delete a patient by ID
int delete_patient(int id) {
  char *sql = "DELETE FROM Patients WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE ? 0 : 1;
}



// Doctor CRUD operations
int create_doctor(Doctor *doctor) {
  char *sql = "INSERT INTO Doctors (id, name, specialty) VALUES (?, ?, ?)";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, doctor->id);
  sqlite3_bind_text(stmt, 2, doctor->name, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, doctor->specialty, -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int read_doctor(int id, Doctor *doctor) {
  char *sql = "SELECT id, name, specialty FROM Doctors WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    doctor->id = sqlite3_column_int(stmt, 0);
    strcpy(doctor->name, (char *)sqlite3_column_text(stmt, 1));
    strcpy(doctor->specialty, (char *)sqlite3_column_text(stmt, 2));
  }
  sqlite3_finalize(stmt);
  return 0;
}

int update_doctor(const Doctor *doctor) {
  char *sql = "UPDATE Doctors SET name = ?, specialty = ? WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, doctor->name, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, doctor->specialty, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, doctor->id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int delete_doctor(int id) {
  char *sql = "DELETE FROM Doctors WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

// Appointment CRUD operations
int create_appointment(Appointment *appointment) {
  char *sql = "INSERT INTO Appointments (id, patient_id, doctor_id, date) VALUES (?, ?, ?, ?)";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, appointment->id);
  sqlite3_bind_int(stmt, 2, appointment->patient_id);
  sqlite3_bind_int(stmt, 3, appointment->doctor_id);
  sqlite3_bind_text(stmt, 4, appointment->date, -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int read_appointment(int id, Appointment *appointment) {
  char *sql = "SELECT id, patient_id, doctor_id, date FROM Appointments WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    appointment->id = sqlite3_column_int(stmt, 0);
    appointment->patient_id = sqlite3_column_int(stmt, 1);
    appointment->doctor_id = sqlite3_column_int(stmt, 2);
    strcpy(appointment->date, (char *)sqlite3_column_text(stmt, 3));
  }
  sqlite3_finalize(stmt);
  return 0;
}

int update_appointment(const Appointment *appointment) {
  char *sql = "UPDATE Appointments SET patient_id = ?, doctor_id = ?, date = ? WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, appointment->patient_id);
  sqlite3_bind_int(stmt, 2, appointment->doctor_id);
  sqlite3_bind_text(stmt, 3, appointment->date, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 4, appointment->id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int delete_appointment(int id) {
  char *sql = "DELETE FROM Appointments WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

// MedicalRecord CRUD operations
int create_medical_record(MedicalRecord *medical_record) {
  char *sql = "INSERT INTO MedicalRecords (id, patient_id, details) VALUES (?, ?, ?)";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, medical_record->id);
  sqlite3_bind_int(stmt, 2, medical_record->patient_id);
  sqlite3_bind_text(stmt, 3, medical_record->details, -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int read_medical_record(int id, MedicalRecord *medical_record) {
  char *sql = "SELECT id, patient_id, details FROM MedicalRecords WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    medical_record->id = sqlite3_column_int(stmt, 0);
    medical_record->patient_id = sqlite3_column_int(stmt, 1);
    strcpy(medical_record->details, (char *)sqlite3_column_text(stmt, 2));
  }
  sqlite3_finalize(stmt);
  return 0;
}

int update_medical_record(const MedicalRecord *medical_record) {
  char *sql = "UPDATE MedicalRecords SET patient_id = ?, details = ? WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, medical_record->patient_id);
  sqlite3_bind_text(stmt, 2, medical_record->details, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, medical_record->id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int delete_medical_record(int id) {
  char *sql = "DELETE FROM MedicalRecords WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int request_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls)
{
  const char *response_page;
  struct MHD_Response *response;
  int ret;

  if (strcmp(url, BASE_URL) == 0 || strcmp(url, BASE_URL "/") == 0) {
    // Handle GET /api
   response_page =
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

    response = MHD_create_response_from_buffer(strlen(response_page), (void *)response_page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
  }
     if (strncmp(url, BASE_URL "/patients", strlen(BASE_URL "/patients")) == 0) {
      const char *endpoint = url + strlen(BASE_URL "/patients");

      if (strcmp(method, "GET") == 0 && strlen(endpoint) == 0) {
        // Handle GET /patients
        response_page = "<html><body>GET /patients Request Processed</body></html>";
      } else if (strcmp(method, "GET") == 0) {
        // Handle GET /patients/(patientID)
        response_page = "<html><body>GET /patients/(patientID) Request Processed</body></html>";
      } else if (strcmp(method, "POST") == 0 && strlen(endpoint) == 0) {
        // Handle POST /patients
        response_page = "<html><body>POST /patients Request Processed</body></html>";
      } else if (strcmp(method, "PUT") == 0) {
        // Handle PUT /patients/(patientID)
        response_page = "<html><body>PUT /patients/(patientID) Request Processed</body></html>";
      } else if (strcmp(method, "DELETE") == 0) {
        // Handle DELETE /patients/(patientID)
        response_page = "<html><body>DELETE /patients/(patientID) Request Processed</body></html>";
      } else {
        response_page = "<html><body>Invalid Request</body></html>";
      }
    } else if (strncmp(url, BASE_URL "/doctors", strlen(BASE_URL "/doctors")) == 0) {
      const char *endpoint = url + strlen(BASE_URL "/doctors");

      if (strcmp(method, "GET") == 0 && strlen(endpoint) == 0) {
        // Handle GET /doctors
        response_page = "<html><body>GET /doctors Request Processed</body></html>";
      } else if (strcmp(method, "GET") == 0) {
        // Handle GET /doctors/(doctorID)
        response_page = "<html><body>GET /doctors/(doctorID) Request Processed</body></html>";
      } else if (strcmp(method, "POST") == 0 && strlen(endpoint) == 0) {
        // Handle POST /doctors
        response_page = "<html><body>POST /doctors Request Processed</body></html>";
      } else if (strcmp(method, "PUT") == 0) {
        // Handle PUT /doctors/(doctorID)
        response_page = "<html><body>PUT /doctors/(doctorID) Request Processed</body></html>";
      } else if (strcmp(method, "DELETE") == 0) {
        // Handle DELETE /doctors/(doctorID)
        response_page = "<html><body>DELETE /doctors/(doctorID) Request Processed</body></html>";
      } else {
        response_page = "<html><body>Invalid Request</body></html>";
      }
    } else if (strncmp(url, BASE_URL "/appointments", strlen(BASE_URL "/appointments")) == 0) {
      const char *endpoint = url + strlen(BASE_URL "/appointments");

      if (strcmp(method, "GET") == 0 && strlen(endpoint) == 0) {
        // Handle GET /appointments
        response_page = "<html><body>GET /appointments Request Processed</body></html>";
      } else if (strcmp(method, "GET") == 0) {
        // Handle GET /appointments/(appointmentID)
        response_page = "<html><body>GET /appointments/(appointmentID) Request Processed</body></html>";
      } else if (strcmp(method, "POST") == 0 && strlen(endpoint) == 0) {
        // Handle POST /appointments
        response_page = "<html><body>POST /appointments Request Processed</body></html>";
      } else if (strcmp(method, "PUT") == 0) {
        // Handle PUT /appointments/(appointmentID)
        response_page = "<html><body>PUT /appointments/(appointmentID) Request Processed</body></html>";
      } else if (strcmp(method, "DELETE") == 0) {
        // Handle DELETE /appointments/(appointmentID)
        response_page = "<html><body>DELETE /appointments/(appointmentID) Request Processed</body></html>";
      } else {
        response_page = "<html><body>Invalid Request</body></html>";
      }
    } else if (strncmp(url, BASE_URL "/medicalrecords", strlen(BASE_URL "/medicalrecords")) == 0) {
      const char *endpoint = url + strlen(BASE_URL "/medicalrecords");
      if (strcmp(method, "GET") == 0 && strlen(endpoint) == 0) {
        // Handle GET /medicalrecords
        response_page = "<html><body>GET /medicalrecords Request Processed</body></html>";
      } else if (strcmp(method, "GET") == 0) {
        // Handle GET /medicalrecords/(medicalRecordID)
        response_page = "<html><body>GET /medicalrecords/(medicalRecordID) Request Processed</body></html>";
      } else if (strcmp(method, "POST") == 0 && strlen(endpoint) == 0) {
        // Handle POST /medicalrecords
        response_page = "<html><body>POST /medicalrecords Request Processed</body></html>";
      } else if (strcmp(method, "PUT") == 0) {
        // Handle PUT /medicalrecords/(medicalRecordID)
        response_page = "<html><body>PUT /medicalrecords/(medicalRecordID) Request Processed</body></html>";
      } else if (strcmp(method, "DELETE") == 0) {
        // Handle DELETE /medicalrecords/(medicalRecordID)
        response_page = "<html><body>DELETE /medicalrecords/(medicalRecordID) Request Processed</body></html>";
      } else {
        response_page = "<html><body>Invalid Request</body></html>";
      }
    } else {
      // Handle non-API requests or send a 404 Not Found response
      response_page = "<html><body>404 Not Found</body></html>";
    }

    // Add CORS headers
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");

    response = MHD_create_response_from_buffer(strlen(response_page), (void *)response_page, MHD_RESPMEM_PERSISTENT);
    MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return MHD_YES;
  }

// Patients CRUD Callback Functions

int callback_patients_get(const struct _u_request *request, struct _u_response *response, void *user_data) {
  const char *id_str = u_map_get(request->map_url, "id");
  int id = id_str ? atoi(id_str) : -1;
  Patient patient;
  int result = read_patient(id, &patient);

  if (result == 0) { // Patient found
    json_t *json_response = json_pack("{sisi}", "id", patient.id, "name", patient.name);
    ulfius_set_json_body_response(response, 200, json_response);
    json_decref(json_response);
  } else {
    ulfius_set_string_body_response(response, 404, "Patient not found");
  }

  return U_CALLBACK_CONTINUE;
}


int callback_patients_post(const struct _u_request *request, struct _u_response *response, void *user_data) {
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  const char *name = json_string_value(json_object_get(json_request, "name"));
  if (name) {
    Patient new_patient = { .id = 0, .name = "" };
    strncpy(new_patient.name, name, sizeof(new_patient.name) - 1);
    int result = create_patient(&new_patient);

    if (result == 0) {
      ulfius_set_string_body_response(response, 201, "Patient created");
    } else {
      ulfius_set_string_body_response(response, 500, "Error creating patient");
    }
  } else {
    ulfius_set_string_body_response(response, 400, "Invalid data");
  }

  if (json_request) {
    json_decref(json_request);
  }

  return U_CALLBACK_CONTINUE;
}


int callback_patients_put(const struct _u_request *request, struct _u_response *response, void *user_data) {
  json_t *json_request = ulfius_get_json_body_request(request, NULL);
  int id = json_integer_value(json_object_get(json_request, "id"));
  const char *name = json_string_value(json_object_get(json_request, "name"));

  if (id > 0 && name) {
    Patient patient = { .id = id, .name = "" };
    strncpy(patient.name, name, sizeof(patient.name) - 1);
    int result = update_patient(&patient);

    if (result == 0) {
      ulfius_set_string_body_response(response, 200, "Patient updated");
    } else {
      ulfius_set_string_body_response(response, 500, "Error updating patient");
    }
  } else {
    ulfius_set_string_body_response(response, 400, "Invalid data");
  }

  if (json_request) {
    json_decref(json_request);
  }

  return U_CALLBACK_CONTINUE;
}


int callback_patients_delete(const struct _u_request *request, struct _u_response *response, void *user_data) {
  const char *id_str = u_map_get(request->map_url, "id");
  int id = id_str ? atoi(id_str) : -1;

  if (id > 0) {
    int result = delete_patient(id);
    if (result == 0) {
      ulfius_set_string_body_response(response, 200, "Patient deleted");
    } else {
      ulfius_set_string_body_response(response, 500, "Error deleting patient");
    }
  } else {
    ulfius_set_string_body_response(response, 400, "Invalid ID");
  }

  return U_CALLBACK_CONTINUE;
}


// Doctors CRUD Callback Functions

int callback_doctors_get(const struct _u_request *request, struct _u_response *response, void *user_data) {
  const char *id_str = u_map_get(request->map_url, "id");
  int id = id_str ? atoi(id_str) : -1;
    Doctor doctor;
    int result = read_doctor(id, &doctor);

    if (result == 0) { // Doctor found
        json_t *json_response = json_pack("{sisi}", "id", doctor.id, "name", doctor.name, "specialty", doctor.specialty);
        ulfius_set_json_body_response(response, 200, json_response);
        json_decref(json_response);
    } else {
        ulfius_set_string_body_response(response, 404, "Doctor not found");
    }

    return U_CALLBACK_CONTINUE;
}

int callback_doctors_post(const struct _u_request *request, struct _u_response *response, void *user_data) {
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    const char *name = json_string_value(json_object_get(json_request, "name"));
    const char *specialty = json_string_value(json_object_get(json_request, "specialty"));

    if (name && specialty) {
        Doctor new_doctor = { .id = 0, .name = "", .specialty = "" };
        strncpy(new_doctor.name, name, sizeof(new_doctor.name) - 1);
        strncpy(new_doctor.specialty, specialty, sizeof(new_doctor.specialty) - 1);
        int result = create_doctor(&new_doctor);

        if (result == 0) {
            ulfius_set_string_body_response(response, 201, "Doctor created");
        } else {
            ulfius_set_string_body_response(response, 500, "Error creating doctor");
        }
    } else {
        ulfius_set_string_body_response(response, 400, "Invalid data");
    }

    if (json_request) {
        json_decref(json_request);
    }

    return U_CALLBACK_CONTINUE;
}

int callback_doctors_put(const struct _u_request *request, struct _u_response *response, void *user_data) {
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    int id = json_integer_value(json_object_get(json_request, "id"));
    const char *name = json_string_value(json_object_get(json_request, "name"));
    const char *specialty = json_string_value(json_object_get(json_request, "specialty"));

    if (id > 0 && name && specialty) {
        Doctor doctor = { .id = id, .name = "", .specialty = "" };
        strncpy(doctor.name, name, sizeof(doctor.name) - 1);
        strncpy(doctor.specialty, specialty, sizeof(doctor.specialty) - 1);
        int result = update_doctor(&doctor);

        if (result == 0) {
            ulfius_set_string_body_response(response, 200, "Doctor updated");
        } else {
            ulfius_set_string_body_response(response, 500, "Error updating doctor");
        }
    } else {
        ulfius_set_string_body_response(response, 400, "Invalid data");
    }

    if (json_request) {
        json_decref(json_request);
    }

    return U_CALLBACK_CONTINUE;
}

int callback_doctors_delete(const struct _u_request *request, struct _u_response *response, void *user_data) {
  const char *id_str = u_map_get(request->map_url, "id");
  int id = id_str ? atoi(id_str) : -1;

    if (id > 0) {
        int result = delete_doctor(id);
        if (result == 0) {
            ulfius_set_string_body_response(response, 200, "Doctor deleted");
        } else {
            ulfius_set_string_body_response(response, 500, "Error deleting doctor");
        }
    } else {
        ulfius_set_string_body_response(response, 400, "Invalid ID");
    }

    return U_CALLBACK_CONTINUE;
}


// Appointments CRUD Callback Functions
// GET: Retrieve an Appointment
int callback_appointments_get(const struct _u_request *request, struct _u_response *response, void *user_data) {
  const char *id_str = u_map_get(request->map_url, "id");
  int id = id_str ? atoi(id_str) : -1;
    Appointment appointment;
    int result = read_appointment(id, &appointment);

    if (result == 0) { // Appointment found
        json_t *json_response = json_pack("{sisisisi}", "id", appointment.id, "patient_id", appointment.patient_id, "doctor_id", appointment.doctor_id, "date", appointment.date);
        ulfius_set_json_body_response(response, 200, json_response);
        json_decref(json_response);
    } else {
        ulfius_set_string_body_response(response, 404, "Appointment not found");
    }

    return U_CALLBACK_CONTINUE;
}

// POST: Create an Appointment
int callback_appointments_post(const struct _u_request *request, struct _u_response *response, void *user_data) {
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
    int doctor_id = json_integer_value(json_object_get(json_request, "doctor_id"));
    const char *date = json_string_value(json_object_get(json_request, "date"));

    if (patient_id > 0 && doctor_id > 0 && date) {
        Appointment new_appointment = { .id = 0, .patient_id = patient_id, .doctor_id = doctor_id, .date = "" };
        strncpy(new_appointment.date, date, sizeof(new_appointment.date) - 1);
        int result = create_appointment(&new_appointment);

        if (result == 0) {
            ulfius_set_string_body_response(response, 201, "Appointment created");
        } else {
            ulfius_set_string_body_response(response, 500, "Error creating appointment");
        }
    } else {
        ulfius_set_string_body_response(response, 400, "Invalid data");
    }

    if (json_request) {
        json_decref(json_request);
    }

    return U_CALLBACK_CONTINUE;
}

// PUT: Update an Appointment
int callback_appointments_put(const struct _u_request *request, struct _u_response *response, void *user_data) {
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    int id = json_integer_value(json_object_get(json_request, "id"));
    int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
    int doctor_id = json_integer_value(json_object_get(json_request, "doctor_id"));
    const char *date = json_string_value(json_object_get(json_request, "date"));

    if (id > 0 && patient_id > 0 && doctor_id > 0 && date) {
        Appointment appointment = { .id = id, .patient_id = patient_id, .doctor_id = doctor_id, .date = "" };
        strncpy(appointment.date, date, sizeof(appointment.date) - 1);
        int result = update_appointment(&appointment);

        if (result == 0) {
            ulfius_set_string_body_response(response, 200, "Appointment updated");
        } else {
            ulfius_set_string_body_response(response, 500, "Error updating appointment");
        }
    } else {
        ulfius_set_string_body_response(response, 400, "Invalid data");
    }

    if (json_request) {
        json_decref(json_request);
    }

    return U_CALLBACK_CONTINUE;
}

// DELETE: Delete an Appointment
int callback_appointments_delete(const struct _u_request *request, struct _u_response *response, void *user_data) {
  const char *id_str = u_map_get(request->map_url, "id");
  int id = id_str ? atoi(id_str) : -1;

    if (id > 0) {
        int result = delete_appointment(id);
        if (result == 0) {
            ulfius_set_string_body_response(response, 200, "Appointment deleted");
        } else {
            ulfius_set_string_body_response(response, 500, "Error deleting appointment");
        }
    } else {
        ulfius_set_string_body_response(response, 400, "Invalid ID");
    }

    return U_CALLBACK_CONTINUE;
}


// MedicalRecords CRUD Callback Functions
// GET: Retrieve a Medical Record
int callback_medical_records_get(const struct _u_request *request, struct _u_response *response, void *user_data) {
  const char *id_str = u_map_get(request->map_url, "id");
  int id = id_str ? atoi(id_str) : -1;
    MedicalRecord record;
    int result = read_medical_record(id, &record);

    if (result == 0) { // Record found
        json_t *json_response = json_pack("{sisis}", "id", record.id, "patient_id", record.patient_id, "details", record.details);
        ulfius_set_json_body_response(response, 200, json_response);
        json_decref(json_response);
    } else {
        ulfius_set_string_body_response(response, 404, "Medical record not found");
    }

    return U_CALLBACK_CONTINUE;
}

// POST: Create a Medical Record
int callback_medical_records_post(const struct _u_request *request, struct _u_response *response, void *user_data) {
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
    const char *details = json_string_value(json_object_get(json_request, "details"));

    if (patient_id > 0 && details) {
        MedicalRecord new_record = { .id = 0, .patient_id = patient_id, .details = "" };
        strncpy(new_record.details, details, sizeof(new_record.details) - 1);
        int result = create_medical_record(&new_record);

        if (result == 0) {
            ulfius_set_string_body_response(response, 201, "Medical record created");
        } else {
            ulfius_set_string_body_response(response, 500, "Error creating medical record");
        }
    } else {
        ulfius_set_string_body_response(response, 400, "Invalid data");
    }

    if (json_request) {
        json_decref(json_request);
    }

    return U_CALLBACK_CONTINUE;
}

// PUT: Update a Medical Record
int callback_medical_records_put(const struct _u_request *request, struct _u_response *response, void *user_data) {
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    int id = json_integer_value(json_object_get(json_request, "id"));
    int patient_id = json_integer_value(json_object_get(json_request, "patient_id"));
    const char *details = json_string_value(json_object_get(json_request, "details"));

    if (id > 0 && patient_id > 0 && details) {
        MedicalRecord record = { .id = id, .patient_id = patient_id, .details = "" };
        strncpy(record.details, details, sizeof(record.details) - 1);
        int result = update_medical_record(&record);

        if (result == 0) {
            ulfius_set_string_body_response(response, 200, "Medical record updated");
        } else {
            ulfius_set_string_body_response(response, 500, "Error updating medical record");
        }
    } else {
        ulfius_set_string_body_response(response, 400, "Invalid data");
    }

    if (json_request) {
        json_decref(json_request);
    }

    return U_CALLBACK_CONTINUE;
}

// DELETE: Delete a Medical Record
int callback_medical_records_delete(const struct _u_request *request, struct _u_response *response, void *user_data) {
  const char *id_str = u_map_get(request->map_url, "id");
  int id = id_str ? atoi(id_str) : -1;

    if (id > 0) {
        int result = delete_medical_record(id);
        if (result == 0) {
            ulfius_set_string_body_response(response, 200, "Medical record deleted");
        } else {
            ulfius_set_string_body_response(response, 500, "Error deleting medical record");
        }
    } else {
        ulfius_set_string_body_response(response, 400, "Invalid ID");
    }

    return U_CALLBACK_CONTINUE;
}



int main() {
  if (init_db() != 0) {
    return 1;
  }

  struct _u_instance instance;

  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    sqlite3_close(db);
    return 1;
  }

  // Patients endpoints
  ulfius_add_endpoint_by_val(&instance, "GET", BASE_URL "/patients", NULL, 0, &callback_patients_get, NULL);
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
    getchar(); // Wait for user input to stop the server
  } else {
    fprintf(stderr, "Error starting framework\n");
  }

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);
  sqlite3_close(db);
  return 0;
}
