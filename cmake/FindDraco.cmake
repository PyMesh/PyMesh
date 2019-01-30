# Finddraco
#
# Locates draco and sets the following variables:
#
# draco_FOUND
# draco_INCLUDE_DIRS
# draco_LIBARY_DIRS
# draco_LIBRARIES
# draco_VERSION_STRING
#
# draco_FOUND is set to YES only when all other variables are successfully
# configured.

unset(draco_FOUND)
unset(draco_INCLUDE_DIRS)
unset(draco_LIBRARY_DIRS)
unset(draco_LIBRARIES)
unset(draco_VERSION_STRING)

mark_as_advanced(draco_FOUND)
mark_as_advanced(draco_INCLUDE_DIRS)
mark_as_advanced(draco_LIBRARY_DIRS)
mark_as_advanced(draco_LIBRARIES)
mark_as_advanced(draco_VERSION_STRING)

set(draco_version_file_no_prefix "draco/core/draco_version.h")

# Set draco_INCLUDE_DIRS
find_path(draco_INCLUDE_DIRS NAMES "${draco_version_file_no_prefix}"
    PATHS
    $ENV{DRACO_PATH}
    $ENV{DRACO_PATH}/include/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include
    /opt/local/include/
    /usr/local/include/
    /usr/lib/)

#  Extract the version string from draco_version.h.
if (draco_INCLUDE_DIRS)
  set(draco_version_file
      "${draco_INCLUDE_DIRS}/draco/core/draco_version.h")
  file(STRINGS "${draco_version_file}" draco_version
       REGEX "kDracoVersion")
  list(GET draco_version 0 draco_version)
  string(REPLACE "static const char kDracoVersion[] = " "" draco_version
         "${draco_version}")
  string(REPLACE ";" "" draco_version "${draco_version}")
  string(REPLACE "\"" "" draco_version "${draco_version}")
  set(draco_VERSION_STRING ${draco_version})
endif ()

# Find the library.
find_library(draco_LIBRARIES NAMES draco
    PATHS $ENV{DRACO_PATH}
    $ENV{DRACO_PATH}/lib
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

# Store path to library.
get_filename_component(draco_LIBRARY_DIRS ${draco_LIBRARIES} DIRECTORY)

if (draco_INCLUDE_DIRS AND draco_LIBRARY_DIRS AND draco_LIBRARIES AND
    draco_VERSION_STRING)
  set(draco_FOUND YES)
endif ()

