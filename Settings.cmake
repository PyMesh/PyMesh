find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    message(STATUS "ccache enabled")
endif(CCACHE_FOUND)

# Set output directories
set(LIBRARY_OUTPUT_PATH    ${PROJECT_SOURCE_DIR}/python/pymesh/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
make_directory(${LIBRARY_OUTPUT_PATH})
make_directory(${EXECUTABLE_OUTPUT_PATH})

# Windows export symbols.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Include customized FindPackage scripts
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Set PIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include all libraries
cmake_policy(SET CMP0074 NEW)
find_package(AllDependencies)

# Options
option(PYMESH_USE_CARVE      "Enable Carve support"      ${CARVE_FOUND})
option(PYMESH_USE_CGAL       "Enable CGAL support"       ${CGAL_FOUND})
option(PYMESH_USE_CLIPPER    "Enable Clipper support"    ${CLIPPER_FOUND})
option(PYMESH_USE_CHOLMOD    "Enable Cholmod support"    ${CHOLMOD_FOUND})
option(PYMESH_USE_CORK       "Enable Cork support"       ${CORK_FOUND})
option(PYMESH_USE_DRACO      "Enable Draco support"      ${draco_FOUND})
option(PYMESH_USE_GEOGRAM    "Enable Geogram support"    ${GEOGRAM_FOUND})
option(PYMESH_USE_LIBIGL     "Enable libigl support"     ${LIBIGL_FOUND})
option(PYMESH_USE_METIS      "Enable Metis support"      ${METIS_FOUND})
option(PYMESH_USE_MMG        "Enable MMG support"        ${MMG_FOUND})
option(PYMESH_USE_MKL        "Enable Intel MKL support"  ${MKL_FOUND})
option(PYMESH_USE_QHULL      "Enable QHull support"      ${QHULL_FOUND})
option(PYMESH_USE_QUARTET    "Enable Quartet support"    ${QUARTET_FOUND})
option(PYMESH_USE_TETGEN     "Enable TetGen support"     ${TETGEN_FOUND})
option(PYMESH_USE_TETWILD    "Enable TetWild support"    ${TETWILD_FOUND})
option(PYMESH_USE_TINYXML2   "Enable TinyXML2 support"   ${TINYXML2_FOUND})
option(PYMESH_USE_TRIANGLE   "Enable Triangle support"   ${TRIANGLE_FOUND})
option(PYMESH_USE_UMFPACK    "Enable Umfpack support"    ${UMFPACK_FOUND})
option(PYMESH_USE_SPARSEHASH "Enable SparseHash support" ${SPARSEHASH_FOUND})
option(PYMESH_USE_FastWindingNumber "Enable Fast Winding Number support"
    ${FAST_WINDING_NUMBER_FOUND})
include(GenerateDependencyTargets)

# Need support for C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add pybind11
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/pybind11)

## Setup RPath
set(CMAKE_MACOSX_RPATH ON)
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/cmake/SetInstallRpath.py)
include(SetInstallRpath)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
