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
    COMMAND ${PYTHON_EXECUTABLE} -c "import sysconfig, os; print(sysconfig.get_config_var('LIBPL'))"
    OUTPUT_VARIABLE PYTHON_LIBRARY_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(LIBNAME python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})
FIND_LIBRARY(PYTHON_LIBRARIES
    NAMES
    ${LIBNAME}
    ${LIBNAME}m
    PATHS
    ${PYTHON_LIBRARY_PREFIX}
    NO_DEFAULT_PATH # Avoid finding incorrect python libraires.
    )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Python
    "Python library is not found.  Please ensure python is installed"
    PYTHON_INCLUDE_DIRS
    PYTHON_LIBRARIES)
