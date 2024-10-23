cmake_minimum_required(VERSION 3.10)
include(CMakePrintHelpers)

# set the project name
project(Game VERSION 0.1)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# add the executable
add_executable(${PROJECT_NAME} main.c)

# add the libs directories
add_subdirectory(external/raylib)
add_subdirectory(external/raylib-nuklear)

# add base directory for includes
target_include_directories(${PROJECT_NAME} PUBLIC
  "${PROJECT_BINARY_DIR}"
)

# add the libs for linking
set(LIBS ${LIBS} raylib)
set(LIBS ${LIBS} raylib_nuklear)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
# configure_file(alexa.png alexa.png COPYONLY)

cmake_print_variables(TARGET)
