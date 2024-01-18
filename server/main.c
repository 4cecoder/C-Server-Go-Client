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

static sqlite3 *db;

void init_db() {
    char *err_msg = 0;

    int rc = sqlite3_open("healthcare.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    // Create patients table if not exists
    const char *create_table_query =
        "CREATE TABLE IF NOT EXISTS patients (id INTEGER PRIMARY KEY, name TEXT);";

    rc = sqlite3_exec(db, create_table_query, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

static int get_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, BASE_URL "/patients") == 0) {
        // Retrieve a list of all patients
        sqlite3_stmt *stmt;
        const char *query = "SELECT * FROM patients;";
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

        if (rc == SQLITE_OK) {
            printf("Patients:\n");
            printf("ID\tName\n");

            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                printf("%d\t%s\n", sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1));
            }

            sqlite3_finalize(stmt);
        } else {
            fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
        }
    } else if (strstr(url, BASE_URL "/patients/") != NULL) {
        // Retrieve details of a specific patient
        // Extract patient ID from URL
        char patient_id_str[10];
        sscanf(url, BASE_URL "/patients/%s", patient_id_str);
        int patient_id = atoi(patient_id_str);

        sqlite3_stmt *stmt;
        const char *query = "SELECT * FROM patients WHERE id = ?;";
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

        if (rc == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, patient_id);

            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                printf("Patient Details:\n");
                printf("ID\tName\n");
                printf("%d\t%s\n", sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1));
            }

            sqlite3_finalize(stmt);
        } else {
            fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
        }
    }

    // Add similar checks for other endpoints (doctors, appointments, medicalrecords)

    const char *page = "<html><body>Hello, this is a simple REST API server.</body></html>";
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static int post_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
    if (strcmp(url, BASE_URL "/patients") == 0) {
        // Create a new patient record
        Patient new_patient;

        if (*upload_data_size > 0) {
            // Assuming the request body is in JSON format with "name" field
            sscanf(upload_data, "name=%s", new_patient.name);
        } else {
            return MHD_NO; // No data received
        }

        sqlite3_stmt *stmt;
        const char *query = "INSERT INTO patients (name) VALUES (?);";
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

        if (rc == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, new_patient.name, -1, SQLITE_STATIC);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
            }

            sqlite3_finalize(stmt);
        } else {
            fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
        }
    }

    // Add similar checks for other endpoints (doctors, appointments, medicalrecords)

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
        &get_handler,
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
