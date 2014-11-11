# - Find Eigen
# This module searches for the Eigen C++ library.
# 
# The module defines the following variables:
#  Eigen_INCLUDE_DIR, where to find Eigen/Core, etc.
#  Eigen_FOUND, if false, do not try to use EIGEN.
#
# Variables used by this module, they can change the default behavior and need
# to be set before calling find_package:
#
#  EIGEN_ROOT_DIR - The prefered installation prefix when searching for Eigen
#

include(FindPackageHandleStandardArgs)

# Finds the include files directory
find_path(Eigen_INCLUDE_DIR 
    NAMES Eigen/Core
    HINTS "${EIGEN_ROOT_DIR}"
    PATHS
    $ENV{EIGEN_INC}
    $ENV{EIGEN_PATH}
    /opt/local/include
    /usr/local/include
    /usr/include
    PATH_SUFFIXES eigen3
    DOC "The directory where Eigen/Core resides"
    NO_DEFAULT_PATH
    )

if(Eigen_INCLUDE_DIR)
    mark_as_advanced(Eigen_INCLUDE_DIR)
endif()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Eigen DEFAULT_MSG Eigen_INCLUDE_DIR)
