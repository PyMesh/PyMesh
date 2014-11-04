# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)

#ADD_SUBDIRECTORY(selfintersect)
#ADD_SUBDIRECTORY(windingnumber)
ADD_SUBDIRECTORY(MeshUtils)
#ADD_SUBDIRECTORY(tetgen)
ADD_SUBDIRECTORY(triangle)
ADD_SUBDIRECTORY(CGAL)
ADD_SUBDIRECTORY(ConvexHull)
ADD_SUBDIRECTORY(Assembler)
ADD_SUBDIRECTORY(EigenUtils)
ADD_SUBDIRECTORY(Envelope)
ADD_SUBDIRECTORY(SparseSolver)
ADD_SUBDIRECTORY(CSG)
ADD_SUBDIRECTORY(Wires)

ADD_CUSTOM_TARGET(tools_tests DEPENDS
    #selfintersect_tests
    #windingnumber_tests
    MeshUtils_tests
    #tetgen_tests
    triangle_tests
    cgal_tests
    convex_hull_tests
    assembler_tests
    EigenUtils_tests
    envelope_tests
    SparseSolver_tests
    csg_tests
    wires_tests)
