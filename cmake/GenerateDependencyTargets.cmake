if (MKL_FOUND AND PYMESH_USE_MKL AND NOT TARGET PyMesh::third_party::MKL)
    add_library(PyMesh::third_party::MKL INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::MKL SYSTEM
        INTERFACE ${MKL_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::MKL
        INTERFACE ${MKL_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::MKL
        INTERFACE -DEIGEN_USE_MKL_ALL)
endif ()


if (EIGEN_FOUND AND NOT TARGET PyMesh::third_party::Eigen)
    add_library(PyMesh::third_party::Eigen INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Eigen SYSTEM
        INTERFACE ${EIGEN_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}/unsupported)
    target_compile_definitions(PyMesh::third_party::Eigen
        INTERFACE -DEIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET)
    if (TARGET PyMesh::third_party::MKL)
        target_link_libraries(PyMesh::third_party::Eigen
            INTERFACE PyMesh::third_party::MKL)
    endif()
endif ()


if (GMP_FOUND AND NOT TARGET PyMesh::third_party::GMP)
    add_library(PyMesh::third_party::GMP INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::GMP SYSTEM
        INTERFACE ${GMP_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::GMP
        INTERFACE ${GMP_LIBRARIES})
endif ()


if (MPFR_FOUND AND NOT TARGET PyMesh::third_party::MPFR)
    add_library(PyMesh::third_party::MPFR INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::MPFR SYSTEM
        INTERFACE ${MPFR_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::MPFR
        INTERFACE ${MPFR_LIBRARIES})
endif ()


if (TBB_FOUND AND NOT TARGET PyMesh::third_party::TBB)
    add_library(PyMesh::third_party::TBB INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::TBB SYSTEM
        INTERFACE ${TBB_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::TBB
        INTERFACE ${TBB_LIBRARIES})
endif ()


if (SPARSEHASH_FOUND AND PYMESH_USE_SPARSEHASH AND
        NOT TARGET PyMesh::third_party::SparseHash)
    add_library(PyMesh::third_party::SparseHash INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::SparseHash SYSTEM
        INTERFACE ${SPARSEHASH_INCLUDE_DIRS})
    target_compile_definitions(PyMesh::third_party::SparseHash
        INTERFACE -DUSE_SPARSEHASH)
endif ()


if (BSP_FOUND AND PYMESH_USE_BSP AND NOT TARGET PyMesh::third_party::BSP)
    add_library(PyMesh::third_party::BSP INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::BSP SYSTEM
        INTERFACE ${BSP_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::BSP
        INTERFACE ${BSP_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::BSP
        INTERFACE -DWITH_BSP)
endif ()


if (MMG_FOUND AND PYMESH_USE_MMG AND NOT TARGET PyMesh::third_party::MMG)
    add_library(PyMesh::third_party::MMG INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::MMG SYSTEM
        INTERFACE ${MMG_INCLUDE_DIR})
    target_link_libraries(PyMesh::third_party::MMG
        INTERFACE ${MMG_LIBRARY})
    target_compile_definitions(PyMesh::third_party::MMG
        INTERFACE -DWITH_MMG)
endif ()


if (QHULL_FOUND AND PYMESH_USE_QHULL AND NOT TARGET PyMesh::third_party::Qhull)
    add_library(PyMesh::third_party::Qhull INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Qhull SYSTEM
        INTERFACE ${QHULL_INCLUDE_DIRS} ${QHULL_INCLUDE_DIRS}/libqhull)
    target_link_libraries(PyMesh::third_party::Qhull
        INTERFACE ${QHULL_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::Qhull
        INTERFACE -DWITH_QHULL)
endif ()


if (QUARTET_FOUND AND PYMESH_USE_QUARTET AND NOT TARGET PyMesh::third_party::Quartet)
    add_library(PyMesh::third_party::Quartet INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Quartet SYSTEM
        INTERFACE ${QUARTET_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::Quartet
        INTERFACE ${QUARTET_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::Quartet
        INTERFACE -DWITH_QUARTET)
endif ()


if (TRIANGLE_FOUND AND PYMESH_USE_TRIANGLE AND NOT TARGET PyMesh::third_party::Triangle)
    add_library(PyMesh::third_party::Triangle INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Triangle SYSTEM
        INTERFACE ${TRIANGLE_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::Triangle
        INTERFACE ${TRIANGLE_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::Triangle
        INTERFACE -DWITH_TRIANGLE)
endif ()


if (TETGEN_FOUND AND PYMESH_USE_TETGEN AND NOT PyMesh::third_party::TetGen)
    add_library(PyMesh::third_party::TetGen INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::TetGen SYSTEM
        INTERFACE ${TETGEN_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::TetGen
        INTERFACE ${TETGEN_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::TetGen
        INTERFACE -DTETLIBRARY -DWITH_TETGEN)
endif ()


if (CORK_FOUND AND PYMESH_USE_CORK AND NOT TARGET PyMesh::third_party::Cork)
    add_library(PyMesh::third_party::Cork INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Cork SYSTEM
        INTERFACE ${CORK_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::Cork
        INTERFACE ${CORK_LIBRARIES} PyMesh::third_party::GMP)
    target_compile_definitions(PyMesh::third_party::Cork
        INTERFACE -DWITH_CORK)
endif ()


if (CLIPPER_FOUND AND PYMESH_USE_CLIPPER AND NOT TARGET PyMesh::third_party::Clipper)
    add_library(PyMesh::third_party::Clipper INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Clipper SYSTEM
        INTERFACE ${CLIPPER_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::Clipper
        INTERFACE ${CLIPPER_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::Clipper
        INTERFACE -DWITH_CLIPPER)
endif ()


if (CGAL_FOUND AND PYMESH_USE_CGAL
        AND TARGET PyMesh::third_party::GMP
        AND TARGET PyMesh::third_party::MPFR
        AND TARGET Boost::atomic
        AND TARGET Boost::chrono
        AND TARGET Boost::date_time
        AND TARGET Boost::thread
        AND TARGET Boost::system
        AND NOT TARGET PyMesh::third_party::CGAL)
    add_library(PyMesh::third_party::CGAL INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::CGAL SYSTEM
        INTERFACE ${CGAL_INCLUDE_DIRS} ${CGAL_3RD_PARTY_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::CGAL
        INTERFACE
            PyMesh::third_party::GMP
            PyMesh::third_party::MPFR
            Boost::atomic
            Boost::chrono
            Boost::date_time
            Boost::thread
            Boost::system
            ${CGAL_LIBRARIES}
            ${CGAL_3RD_PARTY_LIBRARIES}
    )
    target_compile_definitions(PyMesh::third_party::CGAL
        INTERFACE
            ${CGAL_DEFINITIONS}
            ${CGAL_3RD_PARTY_DEFINITIONS}
            -DWITH_CGAL
            -DWITH_CGAL_COREFINEMENT
            -DCGAL_CONCURRENT_MESH_3
    )
    if (CGAL_HEADER_ONLY)
        # It seems CGAL does not capture this flag in ${CGAL_DEFINITIONS}...
        target_compile_definitions(PyMesh::third_party::CGAL
            INTERFACE -DCGAL_HEADER_ONLY)
    endif ()
endif ()


if (LIBIGL_FOUND AND PYMESH_USE_LIBIGL AND NOT TARGET PyMesh::third_party::libigl)
    add_library(PyMesh::third_party::libigl INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::libigl SYSTEM
        INTERFACE ${LIBIGL_INCLUDE_DIRS})
    target_compile_definitions(PyMesh::third_party::libigl
        INTERFACE -DWITH_IGL)
    if (TARGET PyMesh::third_party::CGAL)
        target_link_libraries(PyMesh::third_party::libigl
            INTERFACE PyMesh::third_party::CGAL)
        target_compile_definitions(PyMesh::third_party::libigl
            INTERFACE -DWITH_IGL_AND_CGAL)
    endif ()
endif ()


if (CARVE_FOUND AND PYMESH_USE_CARVE AND NOT TARGET PyMesh::third_party::Carve)
    add_library(PyMesh::third_party::Carve INTERFACE IMPORTED)
    target_link_libraries(PyMesh::third_party::Carve
        INTERFACE ${CARVE_LIBRARIES})
    target_include_directories(PyMesh::third_party::Carve SYSTEM
        INTERFACE ${CARVE_INCLUDE_DIRS})
    target_compile_definitions(PyMesh::third_party::Carve
        INTERFACE -DWITH_CARVE)
endif ()


if (UMFPACK_FOUND AND PYMESH_USE_UMFPACK AND NOT TARGET PyMesh::third_party::Umfpack)
    add_library(PyMesh::third_party::Umfpack INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Umfpack SYSTEM
        INTERFACE ${UMFPACK_INCLUDES})
    target_link_libraries(PyMesh::third_party::Umfpack
        INTERFACE ${UMFPACK_LIBRARIES} ${AMD_LIBRARY} ${COLAMD_LIBRARY})
endif ()


if (CHOLMOD_FOUND AND PYMESH_USE_CHOLMOD AND NOT TARGET PyMesh::third_party::Cholmod)
    add_library(PyMesh::third_party::Cholmod INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Cholmod
        INTERFACE ${CHOLMOD_INCLUDES})
    target_link_libraries(PyMesh::third_party::Cholmod
        INTERFACE ${CHOLMOD_LIBRARIES} ${AMD_LIBRARY} ${COLAMD_LIBRARY})
endif ()


if (METIS_FOUND AND PYMESH_USE_METIS AND NOT TARGET PyMesh::third_party::Metis)
    add_library(PyMesh::third_party::Metis INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Metis SYSTEM
        INTERFACE ${METIS_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::Metis
        INTERFACE ${METIS_LIBRARIES})
endif ()


if (TINYXML2_FOUND AND PYMESH_USE_TINYXML2 AND NOT TARGET PyMesh::third_party::TinyXML2)
    add_library(PyMesh::third_party::TinyXML2 INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::TinyXML2 SYSTEM
        INTERFACE ${TINYXML2_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::TinyXML2
        INTERFACE ${TINYXML2_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::TinyXML2
        INTERFACE -DWITH_TINYXML2)
endif ()


if (GEOGRAM_FOUND AND PYMESH_USE_GEOGRAM AND NOT TARGET PyMesh::third_party::Geogram)
    add_library(PyMesh::third_party::Geogram INTERFACE IMPORTED)
    target_link_libraries(PyMesh::third_party::Geogram
        INTERFACE Geogram::geogram)
    target_compile_definitions(PyMesh::third_party::Geogram
        INTERFACE -DWITH_GEOGRAM)
endif ()


if (draco_FOUND AND PYMESH_USE_DRACO AND NOT TARGET PyMesh::third_party::Draco)
    add_library(PyMesh::third_party::Draco INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Draco SYSTEM
        INTERFACE ${draco_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::Draco
        INTERFACE ${draco_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::Draco
        INTERFACE -DWITH_DRACO)
endif ()


if (TETWILD_FOUND AND PYMESH_USE_TETWILD
        AND TARGET PyMesh::third_party::CGAL
        AND TARGET PyMesh::third_party::libigl
        AND TARGET PyMesh::third_party::Geogram
        AND NOT TARGET PyMesh::third_party::TetWild)
    add_library(PyMesh::third_party::TetWild INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::TetWild SYSTEM
        INTERFACE ${TETWILD_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::TetWild
        INTERFACE
            ${TETWILD_LIBRARY}
            PyMesh::third_party::CGAL
            PyMesh::third_party::libigl
            PyMesh::third_party::Geogram
    )
    target_compile_definitions(PyMesh::third_party::TetWild
        INTERFACE -DWITH_TETWILD)
endif ()


if (FAST_WINDING_NUMBER_FOUND AND PYMESH_USE_FastWindingNumber
        AND TARGET PyMesh::third_party::TBB
        AND NOT TARGET PyMesh::third_party::FastWindingNumber)
    add_library(PyMesh::third_party::FastWindingNumber INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::FastWindingNumber SYSTEM
        INTERFACE ${FAST_WINDING_NUMBER_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::FastWindingNumber
        INTERFACE ${FAST_WINDING_NUMBER_LIBRARIES} PyMesh::third_party::TBB)
    target_compile_definitions(PyMesh::third_party::FastWindingNumber
        INTERFACE -DWITH_FAST_WINDING_NUMBER)
endif ()

if (JIGSAW_FOUND AND NOT TARGET PyMesh::third_party::Jigsaw)
    add_library(PyMesh::third_party::Jigsaw INTERFACE IMPORTED)
    target_include_directories(PyMesh::third_party::Jigsaw SYSTEM
        INTERFACE ${JIGSAW_INCLUDE_DIRS})
    target_link_libraries(PyMesh::third_party::Jigsaw
        INTERFACE ${JIGSAW_LIBRARIES})
    target_compile_definitions(PyMesh::third_party::Jigsaw
        INTERFACE -DWITH_JIGSAW)
endif ()

if (TARGET nlohmann_json::nlohmann_json)
    add_library(PyMesh::third_party::json INTERFACE IMPORTED)
    target_link_libraries(PyMesh::third_party::json INTERFACE nlohmann_json)
endif()

