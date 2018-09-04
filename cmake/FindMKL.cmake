# - Try to find the Intel Math Kernel Library
#   Forked from: https://github.com/openmeeg/openmeeg/blob/master/macros/FindMKL.cmake
# Once done this will define
#
# MKL_FOUND - system has MKL
# MKL_ROOT_DIR - path to the MKL base directory
# MKL_INCLUDE_DIR - the MKL include directory
# MKL_LIBRARIES - MKL libraries
#
# There are few sets of libraries:
# Array indexes modes:
# LP - 32 bit indexes of arrays
# ILP - 64 bit indexes of arrays
# Threading:
# SEQUENTIAL - no threading
# INTEL - Intel threading library
# GNU - GNU threading library
# MPI support
# NOMPI - no MPI support
# INTEL - Intel MPI library
# OPEN - Open MPI library
# SGI - SGI MPT Library

# linux
if(UNIX AND NOT APPLE)
    if(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(MKL_ARCH_DIR "intel64_lin")
    else()
        set(MKL_ARCH_DIR "32")
    endif()
endif()

# macos
if(APPLE)
    set(MKL_ARCH_DIR "em64t")
endif()

IF(FORCE_BUILD_32BITS)
    set(MKL_ARCH_DIR "32")
ENDIF()

if (WIN32)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        set(MKL_ARCH_DIR "intel64")
    else()
        set(MKL_ARCH_DIR "ia32")
    endif()
endif()

set (MKL_THREAD_VARIANTS SEQUENTIAL GNUTHREAD INTELTHREAD)
set (MKL_MODE_VARIANTS ILP LP)
set (MKL_MPI_VARIANTS NOMPI INTELMPI OPENMPI SGIMPT)

find_path(MKL_ROOT_DIR
    include/mkl_cblas.h
    PATHS
    $ENV{MKLDIR}
    /opt/intel/mkl/
    /opt/intel/mkl/*/
    /opt/intel/cmkl/*/
    /Library/Frameworks/Intel_MKL.framework/Versions/Current/lib/universal
    "Program Files (x86)/Intel/ComposerXE-2011/mkl"
)

SET(INTEL_ROOT_DIR "/opt/intel" CACHE PATH "Folder contains intel libs")

#MESSAGE("MKL_ROOT_DIR : ${MKL_ROOT_DIR}") # for debug
#MESSAGE("INTEL_ROOT_DIR: ${INTEL_ROOT_DIR}")

find_path(MKL_INCLUDE_DIR
  mkl_cblas.h
  PATHS
    ${MKL_ROOT_DIR}/include
    ${INCLUDE_INSTALL_DIR}
)

find_path(MKL_FFTW_INCLUDE_DIR
  fftw3.h
  PATH_SUFFIXES fftw
  PATHS
    ${MKL_ROOT_DIR}/include
    ${INCLUDE_INSTALL_DIR}
  NO_DEFAULT_PATH
)

find_library(MKL_RT_LIBRARY
  mkl_rt
  PATHS
    ${MKL_ROOT_DIR}/lib/${MKL_ARCH_DIR}
    ${MKL_ROOT_DIR}/lib/
)

set(MKL_LIBRARIES ${MKL_RT_LIBRARY})
#LINK_DIRECTORIES(${MKL_ROOT_DIR}/lib/${MKL_ARCH_DIR}) # hack
LINK_DIRECTORIES(${MKL_ROOT_DIR}/lib)
LINK_DIRECTORIES(${INTEL_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MKL DEFAULT_MSG MKL_INCLUDE_DIR MKL_LIBRARIES)

mark_as_advanced(MKL_INCLUDE_DIR MKL_LIBRARIES
    MKL_CORE_LIBRARY MKL_LP_LIBRARY MKL_ILP_LIBRARY
    MKL_SEQUENTIAL_LIBRARY MKL_INTELTHREAD_LIBRARY MKL_GNUTHREAD_LIBRARY
)
