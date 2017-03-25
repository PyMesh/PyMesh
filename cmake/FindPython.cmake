# My hacked version of finding python that should work with virtualenv.
# The following variables are set
#
# PYTHON_FOUND
# PYTHON_INCLUDE_DIRS
# PYTHON_LIBRARIES
# PYTHON_VERSION_MAJOR
# PYTHON_VERSION_MINOR

FIND_PACKAGE(PythonInterp REQUIRED)

EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_config_var('INCLUDEPY'))"
    OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS
    OUTPUT_STRIP_TRAILING_WHITESPACE)

EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c "import sysconfig, os; print(os.path.join(sysconfig.get_config_var('LIBPL'), sysconfig.get_config_var('LIBRARY')))"
    OUTPUT_VARIABLE PYTHON_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE)



#EXECUTE_PROCESS(
#    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/prefix.py
#    OUTPUT_VARIABLE PYTHON_PREFIX
#    OUTPUT_STRIP_TRAILING_WHITESPACE)
#
#SET(LIBNAME python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})
#
## System wide python framework often are not the correct python.
#SET(CMAKE_FIND_FRAMEWORK LAST)
#FIND_PATH(PYTHON_INCLUDE_DIRS
#    NAMES Python.h
#    PATHS
#    ${PYTHON_PREFIX}/include
#    /opt/local/include/
#    /usr/local/include/
#    /usr/include/
#    PATH_SUFFIXES
#    ${LIBNAME}
#    ${LIBNAME}m)
#
#FIND_LIBRARY(PYTHON_LIBRARIES
#    NAMES
#    ${LIBNAME}
#    ${LIBNAME}m
#    PATHS
#    ${PYTHON_PREFIX}/lib
#    /opt/local/lib/
#    /usr/local/lib/
#    /usr/lib/
#    PATH_SUFFIXES
#    config
#    config-${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}
#    config-${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}m
#    ${LIBNAME}
#    ${LIBNAME}/config
#    ${LIBNAME}/config-${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}
#    ${LIBNAME}/config-${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}m
#    NO_DEFAULT_PATH # Avoid finding incorrect python libraires.
#    )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Python
    "Python library is not found.  Please ensure python is installed"
    PYTHON_INCLUDE_DIRS
    PYTHON_LIBRARIES)
