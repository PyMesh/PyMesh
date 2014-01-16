# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)

ADD_SUBDIRECTORY(selfintersect)
ADD_SUBDIRECTORY(windingnumber)
ADD_SUBDIRECTORY(MeshUtils)
ADD_SUBDIRECTORY(tetgen)
ADD_SUBDIRECTORY(CGAL)
ADD_SUBDIRECTORY(Assembler)

ADD_CUSTOM_TARGET(tools_tests DEPENDS
    selfintersect_tests
    windingnumber_tests
    MeshUtils_tests
    tetgen_tests
    cgal_tests
    assembler_tests)
