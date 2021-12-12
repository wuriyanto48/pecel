cmake_minimum_required(VERSION 3.1...3.15)

project(pecel VERSION 1.0 LANGUAGES C)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

MESSAGE( [Main] " ----- DEBUGGING ----- ")
MESSAGE( [Main] " PROJECT_NAME = ${PROJECT_NAME}")
MESSAGE( [Main] " CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
MESSAGE( [Main] " CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
MESSAGE( [Main] " PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
MESSAGE( [Main] " CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
MESSAGE( [Main] " ----- DEBUGGING ----- ")

set(HEADER_FILES
    src/server.h
    src/client.h
    src/type.h
    src/config.h
    src/database.h
    src/pecel_utils.h
    src/cmd.h
)

set(SOURCE_FILES 
    src/server.c
    src/client.c
    src/config.c
    src/database.c
    src/pecel_utils.c
    src/cmd.c
)

# Add definition for library
add_library(pecelcore STATIC ${SOURCE_FILES} ${HEADER_FILES})

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME} pecelcore  -lpthread)