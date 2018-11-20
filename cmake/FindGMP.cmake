# Try to find the GMP librairies
# GMP_FOUND - system has GMP lib
# GMP_INCLUDE_DIRS - the GMP include directory
# GMP_LIBRARIES - Libraries needed to use GMP

if (GMP_INCLUDE_DIRS AND GMP_LIBRARIES)
        # Already in cache, be silent
        set(GMP_FIND_QUIETLY TRUE)
endif (GMP_INCLUDE_DIRS AND GMP_LIBRARIES)

find_path(GMP_INCLUDE_DIRS NAMES gmp.h
    HINTS $ENV{GMP_INC} ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include)
find_library(GMP_LIBRARIES NAMES gmp libgmp libgmp-10
    HINTS $ENV{GMP_LIB} ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GMP DEFAULT_MSG GMP_INCLUDE_DIRS GMP_LIBRARIES)

mark_as_advanced(GMP_INCLUDE_DIRS GMP_LIBRARIES)
