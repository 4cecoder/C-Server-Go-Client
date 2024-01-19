cmake_minimum_required(VERSION 3.27)
project(server C)

set(CMAKE_C_STANDARD 23)

# Find the packages for sqlite3
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
pkg_check_modules(MHD REQUIRED libmicrohttpd)

if(WIN32)
    # Specify paths for Windows SQLite3, if not in standard locations
    include_directories(${PROJECT_SOURCE_DIR}/libs/windows/sqlite3)
    link_directories(${PROJECT_SOURCE_DIR}/libs/windows/sqlite3)
elseif(APPLE)
    # Specify the paths for macOS Homebrew installation of Ulfius and its dependencies
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif()

add_executable(server main.c)

# Link the libraries to your executable
target_include_directories(server PRIVATE ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(server ${SQLITE3_LIBRARIES})
target_include_directories(server PRIVATE ${MHD_INCLUDE_DIRS})
target_link_libraries(server ${MHD_LIBRARIES})

# Link Ulfius and its dependencies
if(APPLE)
    target_link_libraries(server ulfius gnutls jansson)
endif()

# For Windows, if additional libraries are needed, specify them here
if(WIN32)
    # Link additional Windows-specific libraries if necessary
endif()
