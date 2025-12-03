cmake_minimum_required(VERSION 3.20)
project(fogpassd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(fogpassd_gnss
    src/gnss_reader.cpp
)

target_include_directories(fogpassd_gnss
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(fogpassd
    src/main.cpp
    src/locator.cpp
    src/landmark_engine.cpp
    src/alert_manager.cpp
    src/ipc_handler.cpp
)

target_link_libraries(fogpassd
    PRIVATE
        fogpassd_gnss
)

target_include_directories(fogpassd
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(TARGETS fogpassd RUNTIME DESTINATION bin)
