cmake_minimum_required(VERSION 3.27)
project(server C)

set(CMAKE_C_STANDARD 23)

# Find the packages for microhttpd and sqlite3
find_package(PkgConfig REQUIRED)
pkg_check_modules(MICROHTTPD REQUIRED libmicrohttpd)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Set the library path based on the operating system
if(APPLE)
    link_directories(/opt/homebrew/Cellar/libmicrohttpd/0.9.77/lib)
elseif(WIN32)
    # Assuming the binaries are in a subdirectory relative to the project root
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64-bit specific path
        link_directories(${CMAKE_SOURCE_DIR}/libmicrohttpd-0.9.77-w32-bin/x86_64/lib)
    else()
        # 32-bit specific path
        link_directories(${CMAKE_SOURCE_DIR}/libmicrohttpd-0.9.77-w32-bin/x86/lib)
    endif()
elseif(UNIX AND NOT APPLE)
    # Linux specific settings...
endif()

add_executable(server main.c)

# Link the libraries to your executable
target_include_directories(server PRIVATE ${MICROHTTPD_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(server ${MICROHTTPD_LIBRARIES} ${SQLITE3_LIBRARIES})
