# Find TetWild and build it as part of the current build.

IF (TARGET TetWild)
    return()
ENDIF (TARGET TetWild)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/fmt)

option(GTET_ISPC "Use ISPC" OFF)

SET(TETWILD_DIR ${PROJECT_SOURCE_DIR}/TetWild/)
SET(LIBIGL_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/libigl/include)
SET(SPDLOG_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/spdlog/include)
SET(MAIN_FILE ${TETWILD_DIR}/main.cpp)
IF (NOT BOOST_FOUND)
    FIND_PACKAGE(Boost REQUIRED COMPONENTS atomic chrono date_time system thread)
ENDIF (NOT BOOST_FOUND)
IF (NOT CGAL_FOUND)
    FIND_PACKAGE(CGAL)
    INCLUDE(${CGAL_USE_FILE})
ENDIF (NOT CGAL_FOUND)

set(SOURCE_FILES
    ${TETWILD_DIR}/include/tetwild/Args.h
    ${TETWILD_DIR}/include/tetwild/Exception.h
    ${TETWILD_DIR}/include/tetwild/Logger.h
    ${TETWILD_DIR}/include/tetwild/tetwild.h
    ${TETWILD_DIR}/src/tetwild/BSPSubdivision.cpp
    ${TETWILD_DIR}/src/tetwild/BSPSubdivision.h
    ${TETWILD_DIR}/src/tetwild/CGALTypes.h
    ${TETWILD_DIR}/src/tetwild/Common.cpp
    ${TETWILD_DIR}/src/tetwild/Common.h
    ${TETWILD_DIR}/src/tetwild/DelaunayTetrahedralization.cpp
    ${TETWILD_DIR}/src/tetwild/DelaunayTetrahedralization.h
    ${TETWILD_DIR}/src/tetwild/DistanceQuery.cpp
    ${TETWILD_DIR}/src/tetwild/DistanceQuery.h
    ${TETWILD_DIR}/src/tetwild/EdgeCollapser.cpp
    ${TETWILD_DIR}/src/tetwild/EdgeCollapser.h
    ${TETWILD_DIR}/src/tetwild/EdgeRemover.cpp
    ${TETWILD_DIR}/src/tetwild/EdgeRemover.h
    ${TETWILD_DIR}/src/tetwild/EdgeSplitter.cpp
    ${TETWILD_DIR}/src/tetwild/EdgeSplitter.h
    ${TETWILD_DIR}/src/tetwild/ForwardDecls.h
    ${TETWILD_DIR}/src/tetwild/InoutFiltering.cpp
    ${TETWILD_DIR}/src/tetwild/InoutFiltering.h
    ${TETWILD_DIR}/src/tetwild/LocalOperations.cpp
    ${TETWILD_DIR}/src/tetwild/LocalOperations.h
    ${TETWILD_DIR}/src/tetwild/Logger.cpp
    ${TETWILD_DIR}/src/tetwild/MeshConformer.cpp
    ${TETWILD_DIR}/src/tetwild/MeshConformer.h
    ${TETWILD_DIR}/src/tetwild/MeshRefinement.cpp
    ${TETWILD_DIR}/src/tetwild/MeshRefinement.h
    ${TETWILD_DIR}/src/tetwild/Preprocess.cpp
    ${TETWILD_DIR}/src/tetwild/Preprocess.h
    ${TETWILD_DIR}/src/tetwild/SimpleTetrahedralization.cpp
    ${TETWILD_DIR}/src/tetwild/SimpleTetrahedralization.h
    ${TETWILD_DIR}/src/tetwild/State.cpp
    ${TETWILD_DIR}/src/tetwild/State.h
    ${TETWILD_DIR}/src/tetwild/TetmeshElements.cpp
    ${TETWILD_DIR}/src/tetwild/TetmeshElements.h
    ${TETWILD_DIR}/src/tetwild/tetwild.cpp
    ${TETWILD_DIR}/src/tetwild/VertexSmoother.cpp
    ${TETWILD_DIR}/src/tetwild/VertexSmoother.h
    ${TETWILD_DIR}/src/tetwild/geogram/mesh_AABB.cpp
    ${TETWILD_DIR}/src/tetwild/geogram/mesh_AABB.h
    ${TETWILD_DIR}/extern/pymesh/MshLoader.cpp
    ${TETWILD_DIR}/extern/pymesh/MshSaver.cpp
)


ADD_LIBRARY(lib_tetwild SHARED ${SOURCE_FILES})
target_link_libraries(lib_tetwild geogram eigen CGAL Boost::system Boost::thread
    fmt-header-only)
target_include_directories(lib_tetwild SYSTEM
    PUBLIC
        ${TETWILD_DIR}/include/
        ${LIBIGL_INCLUDE_DIRS}
    PRIVATE
        ${SPDLOG_INCLUDE_DIRS}
        ${TETWILD_DIR}/src/
        ${TETWILD_DIR}/extern/)
set_target_properties(lib_tetwild PROPERTIES OUTPUT_NAME "TetWild")
target_compile_definitions(lib_tetwild PUBLIC -DCGAL_USE_GMP -DCGAL_DO_NOT_USE_BOOST_MP)

INSTALL(TARGETS lib_tetwild
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin)
INSTALL(DIRECTORY ${TETWILD_DIR}/include/ DESTINATION include)
