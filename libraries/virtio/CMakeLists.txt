cmake_minimum_required(VERSION 3.15)

#! Check every comment after the "#!"

#! CHANGE YOUR PROJECT NAME
set(PROJECT_NAME virtio-lib)
project(${PROJECT_NAME} C CXX) # project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

#! Options
set(WARNINGS_AS_ERRORS ON)
set(ENABLE_PVS_STUDIO OFF)

#! ENABLE_SANITIZERS is the option for the test builds!
#  Definitely enable it while developing.
#  Disable it for the production builds and before submitting for grading!
set(ENABLE_SANITIZERS OFF)

#! Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

#! Build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

#! Include headers
include_directories(include)

#! Project source compilation
add_library(${PROJECT_NAME} SHARED src/9p.c src/blk.c console.c)

#! set output directory (bin)
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#####################################
# Define ALL_TARGETS variable to use in PVS and Sanitizers
set(ALL_TARGETS ${PROJECT_NAME})
