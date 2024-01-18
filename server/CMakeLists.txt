cmake_minimum_required(VERSION 3.27)
project(medi_example C)

set(CMAKE_C_STANDARD 23)

# Find the packages for microhttpd and sqlite3
find_package(PkgConfig REQUIRED)
pkg_check_modules(MICROHTTPD REQUIRED libmicrohttpd)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Add the path to the libraries
link_directories(/opt/homebrew/Cellar/libmicrohttpd/0.9.77/lib)

add_executable(medi_example main.c)

# Link the libraries to your executable
target_include_directories(medi_example PRIVATE ${MICROHTTPD_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(medi_example ${MICROHTTPD_LIBRARIES} ${SQLITE3_LIBRARIES})
