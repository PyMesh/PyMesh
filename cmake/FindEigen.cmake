# - Find Eigen
# This module searches for the Eigen C++ library.
# 
# The module defines the following variables:
#  EIGEN_INCLUDE_DIRS, where to find Eigen/Core, etc.
#  EIGEN_FOUND, if false, do not try to use EIGEN.
#
# Variables used by this module, they can change the default behavior and need
# to be set before calling find_package:
#
#  EIGEN_ROOT_DIR - The prefered installation prefix when searching for Eigen
#

include(FindPackageHandleStandardArgs)

# Finds the include files directory
find_path(EIGEN_INCLUDE_DIRS
    NAMES Eigen/Core
    HINTS "${EIGEN_ROOT_DIR}"
    PATHS
    $ENV{EIGEN_INC}
    $ENV{EIGEN_PATH}
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include
    /opt/local/include
    /usr/local/include
    /usr/include
    PATH_SUFFIXES eigen3
    DOC "The directory where Eigen/Core resides"
    NO_DEFAULT_PATH
    )

if(EIGEN_INCLUDE_DIRS)
    mark_as_advanced(EIGEN_INCLUDE_DIRS)
endif()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Eigen DEFAULT_MSG EIGEN_INCLUDE_DIRS)
