#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <microhttpd.h>
#include <sqlite3.h>

#define BASE_URL "/api"

// Define Patient structure
typedef struct {
    int id;
    char name[100];
} Patient;

// Define Doctor structure
typedef struct {
    int id;
    char name[100];
    char specialty[100];
} Doctor;

// Define Appointment structure
typedef struct {
    int id;
    int patient_id;
    int doctor_id;
    char date[100];
} Appointment;

// Define MedicalRecord structure
typedef struct {
    int id;
    int patient_id;
    char details[255];
} MedicalRecord;

static sqlite3 *db;

void init_db() {
    char *err_msg = 0;

    int rc = sqlite3_open("healthcare.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    // Create tables if not exists
    const char *create_table_query =
        "CREATE TABLE IF NOT EXISTS patients (id INTEGER PRIMARY KEY, name TEXT);"
        "CREATE TABLE IF NOT EXISTS doctors (id INTEGER PRIMARY KEY, name TEXT, specialty TEXT);"
        "CREATE TABLE IF NOT EXISTS appointments (id INTEGER PRIMARY KEY, patient_id INTEGER, doctor_id INTEGER, date TEXT);"
        "CREATE TABLE IF NOT EXISTS medicalrecords (id INTEGER PRIMARY KEY, patient_id INTEGER, details TEXT);";

    rc = sqlite3_exec(db, create_table_query, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

// Function to set ID field for different entity types
void set_id_field(void *entity, int id) {
    if (entity != NULL) {
        *((int *)entity) = id;
    }
}

// Generic function to handle GET requests for all entities
static int get_entities_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
    char entity_type[50];
    sscanf(url, BASE_URL "/%s", entity_type);

    // Retrieve a list of all entities
    sqlite3_stmt *stmt;
    const char *query = NULL;
    int rc = 0;

    if (strcmp(entity_type, "patients") == 0) {
        query = "SELECT * FROM patients;";
    } else if (strcmp(entity_type, "doctors") == 0) {
        query = "SELECT * FROM doctors;";
    } else if (strcmp(entity_type, "appointments") == 0) {
        query = "SELECT * FROM appointments;";
    } else if (strcmp(entity_type, "medicalrecords") == 0) {
        query = "SELECT * FROM medicalrecords;";
    }

    if (query != NULL) {
        rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

        if (rc == SQLITE_OK) {
            printf("%s:\n", entity_type);
            printf("ID\tName\n");

            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                const char *name = sqlite3_column_text(stmt, 1);

                printf("%d\t%s\n", id, name);
            }

            sqlite3_finalize(stmt);
        } else {
            fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
        }
    }

    const char *page = "<html><body>Hello, this is a simple REST API server.</body></html>";
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

// Generic function to handle GET requests for a specific entity by ID
static int get_entity_by_id_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
    char entity_type[50];
    int entity_id;
    sscanf(url, BASE_URL "/%s/%d", entity_type, &entity_id);

    // Retrieve details of a specific entity
    sqlite3_stmt *stmt;
    const char *query = NULL;
    int rc = 0;

    if (strcmp(entity_type, "patients") == 0) {
        query = "SELECT * FROM patients WHERE id = ?;";
    } else if (strcmp(entity_type, "doctors") == 0) {
        query = "SELECT * FROM doctors WHERE id = ?;";
    } else if (strcmp(entity_type, "appointments") == 0) {
        query = "SELECT * FROM appointments WHERE id = ?;";
    } else if (strcmp(entity_type, "medicalrecords") == 0) {
        query = "SELECT * FROM medicalrecords WHERE id = ?;";
    }

    if (query != NULL) {
        rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

        if (rc == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, entity_id);

            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                printf("%s Details:\n", entity_type);
                printf("ID\tName\n");
                printf("%d\t%s\n", sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1));
            }

            sqlite3_finalize(stmt);
        } else {
            fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
        }
    }

    const char *page = "<html><body>Hello, this is a simple REST API server.</body></html>";
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

// Generic function to handle POST requests for creating a new entity
static int create_entity_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
    char entity_type[50];
    sscanf(url, BASE_URL "/%s", entity_type);

    // Create a new entity
    sqlite3_stmt *stmt;
    const char *query = NULL;
    int rc = 0;

    if (strcmp(entity_type, "patients") == 0) {
        query = "INSERT INTO patients (name) VALUES (?);";
    } else if (strcmp(entity_type, "doctors") == 0) {
        query = "INSERT INTO doctors (name, specialty) VALUES (?, ?);";
    } else if (strcmp(entity_type, "appointments") == 0) {
        query = "INSERT INTO appointments (patient_id, doctor_id, date) VALUES (?, ?, ?);";
    } else if (strcmp(entity_type, "medicalrecords") == 0) {
        query = "INSERT INTO medicalrecords (patient_id, details) VALUES (?, ?);";
    }

    if (query != NULL) {
        rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

        if (rc == SQLITE_OK) {
            if (*upload_data_size > 0) {
                // Assuming the request body is in JSON format with necessary fields
                const char *json_data = upload_data;
                sqlite3_bind_text(stmt, 1, json_data, -1, SQLITE_STATIC);

                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE) {
                    fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
                }
            }

            sqlite3_finalize(stmt);
        } else {
            fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
        }
    }

    const char *page = "<html><body>Hello, this is a simple REST API server.</body></html>";
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

int main() {
    init_db();

    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(
        MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
        8080,
        NULL,
        NULL,
        &get_entities_handler,
        NULL,
        MHD_OPTION_END
    );

    if (NULL == daemon) {
        fprintf(stderr, "Failed to start the REST API server\n");
        return 1;
    }

    printf("REST API server is running on http://localhost:8080%s\n", BASE_URL);
    getchar(); // Press Enter to stop the server

    MHD_stop_daemon(daemon);

    sqlite3_close(db); // Close the database connection

    return 0;
}
