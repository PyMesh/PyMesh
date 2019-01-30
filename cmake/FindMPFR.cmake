# Try to find the GMP librairies
# MPFR_FOUND - system has GMP lib
# MPFR_INCLUDE_DIRS - the GMP include directory
# MPFR_LIBRARIES - Libraries needed to use GMP

find_path(MPFR_INCLUDE_DIRS NAMES mpfr.h
    HINTS $ENV{MPFR_INC} ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include)
find_library(MPFR_LIBRARIES NAMES mpfr libmpfr libmpfr-4
    HINTS $ENV{MPFR_LIB} ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MPFR DEFAULT_MSG MPFR_INCLUDE_DIRS MPFR_LIBRARIES)

mark_as_advanced(MPFR_INCLUDE_DIRS MPFR_LIBRARIES)
