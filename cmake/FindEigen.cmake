# - Find Eigen
# This module searches for the Eigen C++ library.
# 
# The module defines the following variables:
#  EIGEN_INCLUDE_DIRS, where to find Eigen/Core, etc.
#  EIGEN_FOUND, if false, do not try to use EIGEN.
#
# and the following target:
#  EIGEN::Eigen
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

# Create the PyMesh::Eigen target.
IF (EIGEN_FOUND AND NOT TARGET PyMesh::Eigen)
    ADD_LIBRARY(PyMesh::Eigen INTERFACE IMPORTED)
    TARGET_INCLUDE_DIRECTORIES(PyMesh::Eigen SYSTEM
        INTERFACE
            ${EIGEN_INCLUDE_DIRS}
            ${EIGEN_INCLUDE_DIRS}/unsupported
    )
    TARGET_COMPILE_DEFINITIONS(PyMesh::Eigen
        INTERFACE
            -DEIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
    )

    # Add MKL if found
    IF (MKL_FOUND)
        TARGET_LINK_LIBRARIES(PyMesh::Eigen
            INTERFACE
                PyMesh::MKL
        )
        TARGET_COMPILE_DEFINITIONS(PyMesh::Eigen
            INTERFACE
                -DEIGEN_USE_BLAS -DEIGEN_USE_LAPACKE
        )
    ENDIF (MKL_FOUND)
ENDIF (EIGEN_FOUND AND NOT TARGET PyMesh::Eigen)
