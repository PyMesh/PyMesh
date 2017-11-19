CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

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

INCLUDE(Settings.cmake)

# Add pybind11
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/third_party/pybind11)

# Process code in each subdirectories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tools)
ADD_SUBDIRECTORY(python)

ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)
