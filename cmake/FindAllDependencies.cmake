# MKL
if (NOT MKL_FOUND)
    find_package(MKL)
endif (NOT MKL_FOUND)

# Include Eigen
if (NOT EIGEN_FOUND)
    find_package(Eigen REQUIRED)
endif (NOT EIGEN_FOUND)

# TBB
if (NOT TBB_FOUND)
    set(TBB_ROOT ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/)
    find_package(TBB REQUIRED)
endif (NOT TBB_FOUND)

# SpraseHash
if (NOT SPARSEHAHS_FOUND)
    find_package(SparseHash QUIET)
endif (NOT SPARSEHAHS_FOUND)

# GMP
if (NOT GMP_FOUND)
    find_package(GMP QUIET)
endif (NOT GMP_FOUND)

# MPFR
if (NOT MPFR_FOUND)
    find_package(MPFR QUIET)
endif (NOT MPFR_FOUND)

# CGAL
if (NOT CGAL_FOUND)
    set(CGAL_DIRS
        "${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/cmake/CGAL"
        "${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib64/cmake/CGAL"
    )
    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
    if (NOT Boost_FOUND)
        set(BOOST_ROOT ${PROJECT_SOURCE_DIR}/python/pymesh/third_party)
        find_package(Boost COMPONENTS atomic chrono date_time system thread)
    endif (NOT Boost_FOUND)
    find_package(CGAL QUIET PATHS ${CGAL_DIRS} NO_DEFAULT_PATH)
endif (NOT CGAL_FOUND)

# MMG
if (NOT Mmg_FOUND)
    if (DEFINED ENV{MMG_PATH})
        set(MMG_BUILD_DIR $ENV{MMG_PATH})
    ELSE (DEFINED ENV{MMG_PATH})
        set(MMG_BUILD_DIR ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/)
    endif (DEFINED ENV{MMG_PATH})
    find_package(Mmg QUIET)
endif ()

# Qhull
if (NOT QHULL_FOUND)
    set(QHULL_USE_STATIC TRUE)
    if (DEFINED ENV{QHULL_PATH})
        set(QHULL_ROOT $ENV{QHULL_PATH})
    endif (DEFINED ENV{QHULL_PATH})
    find_package(Qhull QUIET)
endif (NOT QHULL_FOUND)

# Quartet
if (NOT QUARTET_FOUND)
    find_package(Quartet QUIET)
endif (NOT QUARTET_FOUND)

# Triangle
if (NOT TRIANGLE_FOUND)
    find_package(Triangle QUIET)
endif (NOT TRIANGLE_FOUND)

# Tetgen
if (NOT TETGEN_FOUND)
    find_package(Tetgen QUIET)
endif (NOT TETGEN_FOUND)

# Cork
if (NOT CORK_FOUND)
    find_package(Cork QUIET)
endif(NOT CORK_FOUND)

# Clipper
if (NOT CLIPPER_FOUND)
    find_package(Clipper QUIET)
endif (NOT CLIPPER_FOUND)

# libigl
if (NOT libigl_FOUND)
    find_package(libigl QUIET)
endif ()

# Carve
if (NOT Carve_FOUND)
    find_package(Carve QUIET)
endif ()

# BSP
if (NOT BSP_FOUND)
    find_package(BSP QUIET)
endif (NOT BSP_FOUND)

# SuiteSparse
if (NOT Umfpack_FOUND)
    find_package(Umfpack QUIET)
endif ()

# Cholmod
if (NOT Cholmod_FOUND)
    find_package(Cholmod QUIET)
endif ()

# Metis
if (NOT Metis_FOUND)
    find_package(Metis QUIET)
endif ()

# tinyxml2
if (NOT TINYXML2_FOUND)
    find_package(tinyxml2 QUIET)
endif (NOT TINYXML2_FOUND)

# Lapack
if (NOT LAPACK_FOUND)
    find_package(LAPACK QUIET)
endif (NOT LAPACK_FOUND)

# Geogram
if (NOT GEOGRAM_FOUND)
    set(GEOGRAM_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/python/pymesh/third_party)
    find_package(Geogram QUIET)
endif (NOT GEOGRAM_FOUND)

# Draco
if (NOT draco_FOUND)
    find_package(Draco QUIET)
endif (NOT draco_FOUND)

# TetWild
if (NOT TetWild_FOUND)
    find_package(TetWild QUIET)
endif ()

# Fast winding number
if (NOT FastWindingNumber_FOUND)
    find_package(FastWindingNumber QUIET)
endif ()

if (NOT Jigsaw_FOUND)
    find_package(Jigsaw QUIET)
endif ()

# Nlohmann/json
if (NOT TARGET nlohmann_json::nlohmann_json)
    find_package(nlohmann_json QUIET REQUIRED
        HINTS ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/cmake/nlohmann_json)
endif()
