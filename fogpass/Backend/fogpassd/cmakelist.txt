cmake_minimum_required(VERSION 3.10)
project(FogPASS_Daemon VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)

# Include directories
include_directories(${DBUS_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/gnss_reader.cpp
)

# Create executable
add_executable(fogpassd ${SOURCES})

# Link libraries
target_link_libraries(fogpassd
    ${DBUS_LIBRARIES}
)

# Set compile flags
target_compile_options(fogpassd PRIVATE ${DBUS_CFLAGS_OTHER})

# Install target
install(TARGETS fogpassd
    RUNTIME DESTINATION bin
)

