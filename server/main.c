#include <microhttpd.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    // Database initialization code...
    return 0; // Return 0 if successful
}

// Patient CRUD operations
int create_patient(Patient *patient) {
  char *sql = "INSERT INTO Patients (id, name) VALUES (?, ?)";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, patient->id);
  sqlite3_bind_text(stmt, 2, patient->name, -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int read_patient(int id, Patient *patient) {
  char *sql = "SELECT id, name FROM Patients WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    patient->id = sqlite3_column_int(stmt, 0);
    strcpy(patient->name, (char *)sqlite3_column_text(stmt, 1));
  }
  sqlite3_finalize(stmt);
  return 0;
}

int update_patient(const Patient *patient) {
  char *sql = "UPDATE Patients SET name = ? WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, patient->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, patient->id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int delete_patient(int id) {
  char *sql = "DELETE FROM Patients WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
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

    response = MHD_create_response_from_buffer(strlen(response_page), (void *)response_page, MHD_RESPMEM_PERSISTENT);
    MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return MHD_YES;
  }

int main() {
   if (init_db() != 0) {
     return 1;
   }

   struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL, &request_handler, NULL, MHD_OPTION_END);
   if (daemon == NULL) {
     sqlite3_close(db);
     return 1;
   }

   printf("Server running on port %d\n", PORT);
   getchar();

   MHD_stop_daemon(daemon);
   sqlite3_close(db);
   return 0;
 }
