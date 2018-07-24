# Find TetWild and build it as part of the current build.

IF (TARGET TetWild)
    return()
ENDIF (TARGET TetWild)

option(GTET_ISPC "Use ISPC" OFF)

SET(TETWILD_DIR ${PROJECT_SOURCE_DIR}/TetWild/)
SET(LIBIGL_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/libigl/include)
SET(MAIN_FILE ${TETWILD_DIR}/main.cpp)
FIND_PACKAGE(CGAL REQUIRED Core)
INCLUDE(${CGAL_USE_FILE})

set(SOURCE_FILES
    ${TETWILD_DIR}/BSPElements.h
    ${TETWILD_DIR}/TetmeshElements.h
    ${TETWILD_DIR}/Preprocess.cpp
    ${TETWILD_DIR}/Preprocess.h
    ${TETWILD_DIR}/DelaunayTetrahedralization.cpp
    ${TETWILD_DIR}/DelaunayTetrahedralization.h
    ${TETWILD_DIR}/MeshConformer.cpp
    ${TETWILD_DIR}/MeshConformer.h
    ${TETWILD_DIR}/BSPSubdivision.cpp
    ${TETWILD_DIR}/BSPSubdivision.h
    ${TETWILD_DIR}/SimpleTetrahedralization.cpp
    ${TETWILD_DIR}/SimpleTetrahedralization.h
    ${TETWILD_DIR}/MeshRefinement.cpp
    ${TETWILD_DIR}/MeshRefinement.h
    ${TETWILD_DIR}/LocalOperations.cpp
    ${TETWILD_DIR}/LocalOperations.h
    ${TETWILD_DIR}/EdgeSplitter.cpp
    ${TETWILD_DIR}/EdgeSplitter.h
    ${TETWILD_DIR}/EdgeCollapser.cpp
    ${TETWILD_DIR}/EdgeCollapser.h
    ${TETWILD_DIR}/EdgeRemover.cpp
    ${TETWILD_DIR}/EdgeRemover.h
    ${TETWILD_DIR}/VertexSmoother.cpp
    ${TETWILD_DIR}/VertexSmoother.h
    ${TETWILD_DIR}/InoutFiltering.cpp
    ${TETWILD_DIR}/InoutFiltering.h
    ${TETWILD_DIR}/CLI11.hpp
    ${TETWILD_DIR}/pymesh/MshLoader.cpp
    ${TETWILD_DIR}/pymesh/MshLoader.h
    ${TETWILD_DIR}/pymesh/MshSaver.cpp
    ${TETWILD_DIR}/pymesh/MshSaver.h
    ${TETWILD_DIR}/pymesh/Exception.h
    ${TETWILD_DIR}/heads.cpp
    ${TETWILD_DIR}/heads.h
    ${TETWILD_DIR}/tetwild.h
    ${TETWILD_DIR}/tetwild.cpp
    )


SET(OBJ_FILES "")
if(GTET_ISPC)
	add_custom_command(OUTPUT energy_ispc.o
			COMMAND ispc ${CMAKE_SOURCE_DIR}/ispc/energy.ispc -h ${CMAKE_SOURCE_DIR}/ispc/energy.h -o energy_ispc.o DEPENDS ${CMAKE_SOURCE_DIR}/ispc/energy.ispc)
	add_definitions(-DGTET_ISPC)
    LIST(APPEND OBJ_FILES energy_ispc.o)
endif(GTET_ISPC)
ADD_LIBRARY(lib_tetwild SHARED ${SOURCE_FILES} ${OBJ_FILES})
target_link_libraries(lib_tetwild geogram eigen)
target_include_directories(lib_tetwild SYSTEM PUBLIC ${LIBIGL_INCLUDE_DIRS})
set_target_properties(lib_tetwild PROPERTIES OUTPUT_NAME "TetWild")

#add_executable(TetWild ${MAIN_FILE})
#target_link_libraries(TetWild lib_tetwild)

INSTALL(TARGETS lib_tetwild
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin)
INSTALL(FILES ${TETWILD_DIR}/tetwild.h
    DESTINATION include)
