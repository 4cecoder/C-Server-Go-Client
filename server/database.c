// database.c
#include "database.h"

#include "json_response.h"

#include <stdio.h>
#include <string.h>



// Here you will define all your functions
// For example:
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

void close_db() {
  sqlite3_close(db);
}

// Create a new patient
int create_patient(const Patient *patient) {
  const char *sql = "INSERT INTO Patients (name) VALUES (?)";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, patient->name, -1, SQLITE_STATIC);
  const int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE ? 0 : 1;
}

// Function to read all patients
int read_all_patients() {
  const char *sql = "SELECT id, name FROM Patients";
  sqlite3_stmt *stmt;
  const int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  printf("All Patients:\n");
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const int id = sqlite3_column_int(stmt, 0);
    // Explicitly cast the result of sqlite3_column_text to const char*
    const char *name = (const char *)sqlite3_column_text(stmt, 1);
    // Use %s to correctly format the string in json_pack
    json_array_append_new(json_response, json_pack("{sisi}", "id", id, "name", name));
  }


  sqlite3_finalize(stmt);
  return 0;
}


// Read a patient's details by ID
int read_patient(const int id, Patient *patient) {
  // Initialize the patient struct
  memset(patient, 0, sizeof(Patient));

  const char *sql = "SELECT id, name FROM Patients WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    patient->id = sqlite3_column_int(stmt, 0);
    const char *retrievedName = (const char *)sqlite3_column_text(stmt, 1);

    // Debug print statement
    printf("Retrieved name from DB: '%s'\n", retrievedName);

    if (retrievedName) {
      strncpy(patient->name, retrievedName, sizeof(patient->name) - 1);
      patient->name[sizeof(patient->name) - 1] = '\0'; // Ensure null-termination
    } else {
      // If name is NULL, handle it appropriately
      strcpy(patient->name, "Unknown");
    }
  }
  sqlite3_finalize(stmt);

  return rc == SQLITE_ROW ? 0 : 1;
}



// Update a patient's details
int update_patient(const Patient *patient) {
  const char *sql = "UPDATE Patients SET name = ? WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, patient->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, patient->id);
  const int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE ? 0 : 1;
}

// Delete a patient by ID
int delete_patient(const int id) {
  const char *sql = "DELETE FROM Patients WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  const int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE ? 0 : 1;
}


// Doctor CRUD operations
int create_doctor(const Doctor *doctor) {
  const char *sql = "INSERT INTO Doctors (id, name, specialty) VALUES (?, ?, ?)";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, doctor->id);
  sqlite3_bind_text(stmt, 2, doctor->name, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, doctor->specialty, -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int read_doctor(const int id, Doctor *doctor) {
  const char *sql = "SELECT id, name, specialty FROM Doctors WHERE id = ?";
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
  const char *sql = "UPDATE Doctors SET name = ?, specialty = ? WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, doctor->name, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, doctor->specialty, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, doctor->id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int delete_doctor(const int id) {
  const char *sql = "DELETE FROM Doctors WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

// Appointment CRUD operations
int create_appointment(const Appointment *appointment) {
  const char *sql = "INSERT INTO Appointments (id, patient_id, doctor_id, date) VALUES (?, ?, ?, ?)";
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

int read_appointment(const int id, Appointment *appointment) {
  const char *sql = "SELECT id, patient_id, doctor_id, date FROM Appointments WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);

  const int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    appointment->id = sqlite3_column_int(stmt, 0);
    appointment->patient_id = sqlite3_column_int(stmt, 1);
    appointment->doctor_id = sqlite3_column_int(stmt, 2);

    const char *date = (const char *)sqlite3_column_text(stmt, 3);
    if (date) {
      strncpy(appointment->date, date, sizeof(appointment->date) - 1);
      appointment->date[sizeof(appointment->date) - 1] = '\0'; // Ensure null-termination
    }
  }
  sqlite3_finalize(stmt);
  return rc == SQLITE_ROW ? 0 : 1;
}


int update_appointment(const Appointment *appointment) {
  const char *sql = "UPDATE Appointments SET patient_id = ?, doctor_id = ?, date = ? WHERE id = ?";
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
  const char *sql = "DELETE FROM Appointments WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

// MedicalRecord CRUD operations
int create_medical_record(const MedicalRecord *medical_record) {
  const char *sql = "INSERT INTO MedicalRecords (id, patient_id, details) VALUES (?, ?, ?)";
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
  const char *sql = "SELECT id, patient_id, details FROM MedicalRecords WHERE id = ?";
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
  const char *sql = "UPDATE MedicalRecords SET patient_id = ?, details = ? WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, medical_record->patient_id);
  sqlite3_bind_text(stmt, 2, medical_record->details, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, medical_record->id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

int delete_medical_record(const int id) {
  const char *sql = "DELETE FROM MedicalRecords WHERE id = ?";
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}