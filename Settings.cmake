# Include directories to search for source.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Set build type.
#SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_BUILD_TYPE Release)

# Create 64 bits binary.  32 bits support is dropped.
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
SET(CMAKE_LIBRARY_PATH /opt/local/lib ${CMAKE_LIBRARY_PATH})

# Hide -Wdeprecated-declarations warnings
Include (CheckCXXCompilerFlag)

SET(MY_FLAGS "-Wno-deprecated-declarations")
Foreach (FLAG ${MY_FLAGS})
	Check_CXX_Compiler_Flag("${FLAG}" IS_SUPPORTED_${FLAG})
	If (IS_SUPPORTED_${FLAG})
		Set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
	Endif ()
Endforeach ()

# Set output directories
SET(LIBRARY_OUTPUT_PATH    ${PROJECT_SOURCE_DIR}/python/pymesh/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_SWIG_OUTDIR      ${PROJECT_SOURCE_DIR}/python/pymesh/swig)
MAKE_DIRECTORY(${LIBRARY_OUTPUT_PATH})
MAKE_DIRECTORY(${EXECUTABLE_OUTPUT_PATH})
MAKE_DIRECTORY(${CMAKE_SWIG_OUTDIR})

LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})
LINK_DIRECTORIES(/opt/local/lib)

# Include customized FindPackage scripts
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Set eigen wrapper path
SET(EIGEN_WRAPPER_PATH ${PROJECT_SOURCE_DIR}/eigen_wrapper)

# Disallow depreciated numpy calls
#ADD_DEFINITIONS(-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)

OPTION(WITHOUT_SWIG "Compile with building swig modules" OFF)
IF(WITHOUT_SWIG)
    MESSAGE("Python modules won't be built!!!")
ENDIF(WITHOUT_SWIG)

# Include all libraries
FIND_PACKAGE(AllDependencies)

# Setup RPath
SET(CMAKE_MACOSX_RPATH ON)
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/cmake/SetInstallRpath.py)
INCLUDE(SetInstallRpath)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
