cmake_minimum_required(VERSION 3.10)
project(server)

# Source files
set(SOURCE_FILES main.c
        database.c
        patient_handlers.h
        patient_handlers.c
        cors.c
        cors.h
        json_response.h
        json_response.c
        doctors_handlers.h
        doctors_handlers.c
        appointments_handlers.h
        appointments_handlers.c
        medical_records_handlers.h
        medical_records_handlers.c)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Executable
add_executable(server ${SOURCE_FILES})

# System-specific configurations
if(APPLE)
    # macOS-specific settings
    include_directories(
            /opt/homebrew/Cellar/ulfius/2.7.15/include
            /opt/homebrew/Cellar/gnutls/3.8.3/include
            /opt/homebrew/Cellar/jansson/2.14/include
            /opt/homebrew/Cellar/libmicrohttpd/0.9.77/include
            /opt/homebrew/Cellar/orcania/2.3.3/include
            /opt/homebrew/Cellar/yder/1.4.20/include
    )

    # Libraries
    set(ULFIUS_LIB /opt/homebrew/Cellar/ulfius/2.7.15/lib/libulfius.dylib)
    set(GNUTLS_LIB /opt/homebrew/Cellar/gnutls/3.8.3/lib/libgnutls.dylib)
    set(JANSSON_LIB /opt/homebrew/Cellar/jansson/2.14/lib/libjansson.dylib)
    set(MICROHTTPD_LIB /opt/homebrew/Cellar/libmicrohttpd/0.9.77/lib/libmicrohttpd.dylib)
    set(ORCANIA_LIB /opt/homebrew/Cellar/orcania/2.3.3/lib/liborcania.dylib)
    set(YDER_LIB /opt/homebrew/Cellar/yder/1.4.20/lib/libyder.dylib)

elseif(WIN32)
    # Windows-specific settings
elseif(UNIX AND NOT APPLE) # Linux
    # Linux-specific settings
endif()

# Link libraries
target_link_libraries(server
        ${ULFIUS_LIB}
        ${MICROHTTPD_LIB}
        ${JANSSON_LIB}
        ${GNUTLS_LIB}
        ${ORCANIA_LIB}
        ${YDER_LIB}
        sqlite3
        curl
)