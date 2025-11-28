cmake_minimum_required(VERSION 3.10)
project(FogPASS_Power VERSION 1.0.0 LANGUAGES CXX)

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
    battery.cpp
)

# Create executable
add_executable(fogpass-power ${SOURCES})

# Link libraries
target_link_libraries(fogpass-power
    ${DBUS_LIBRARIES}
)

# Set compile flags
target_compile_options(fogpass-power PRIVATE ${DBUS_CFLAGS_OTHER})

# Install target
install(TARGETS fogpass-power
    RUNTIME DESTINATION bin
)

