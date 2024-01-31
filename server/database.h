#ifndef DATABASE_H
#define DATABASE_H
#include <sqlite3.h>

sqlite3 *db;



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

int init_db();
void close_db();



int create_patient(const Patient *patient);
int read_patient(const int id, Patient *patient);
int update_patient(const Patient *patient);
int delete_patient(const int id);
int create_doctor(const Doctor *doctor);
int read_doctor(const int id, Doctor *doctor);
int update_doctor(const Doctor *doctor);
int delete_doctor(const int id);
int create_appointment(const Appointment *appointment);
int read_appointment(const int id, Appointment *appointment);
int update_appointment(const Appointment *appointment);
int delete_appointment(int id);
int create_medical_record(const MedicalRecord *medical_record);
int read_medical_record(int id, MedicalRecord *medical_record);
int update_medical_record(const MedicalRecord *medical_record);
int delete_medical_record(const int id);

#endif // DATABASE_H