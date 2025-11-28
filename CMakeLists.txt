cmake_minimum_required(VERSION 3.10)
project(FogPASS VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories
add_subdirectory(fogpass/Backend/Power)
add_subdirectory(fogpass/Backend/fogpassd)

