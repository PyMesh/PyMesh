SET(JIGSAW_DIR ${PROJECT_SOURCE_DIR}/jigsaw)

ADD_LIBRARY(jigsaw STATIC ${JIGSAW_DIR}/src/jigsaw.cpp)
TARGET_COMPILE_FEATURES(jigsaw PRIVATE cxx_std_11)
TARGET_COMPILE_DEFINITIONS(jigsaw PRIVATE -D__lib_jigsaw)

INSTALL(TARGETS jigsaw
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin)
INSTALL(FILES
    ${JIGSAW_DIR}/inc/jigsaw_const.h
    ${JIGSAW_DIR}/inc/lib_jigsaw.h
    ${JIGSAW_DIR}/inc/jigsaw_msh_t.h
    ${JIGSAW_DIR}/inc/jigsaw_jig_t.h
    DESTINATION include)
