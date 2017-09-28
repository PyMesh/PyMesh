CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

## Set compiler
#IF(APPLE)
#    SET(CMAKE_C_COMPILER "clang")
#    SET(CMAKE_CXX_COMPILER "clang++")
#ELSEIF(UNIX)
#    SET(CMAKE_C_COMPILER "gcc")
#    SET(CMAKE_CXX_COMPILER "g++")
#ELSEIF(WIN32)
#    # TODO: not how the code would work on Windows.
#ENDIF()

PROJECT(Mesh)

# Add pybind11
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/third_party/pybind11)

INCLUDE(Settings.cmake)

# Process code in each subdirectories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(python)

ADD_SUBDIRECTORY(tools EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)
