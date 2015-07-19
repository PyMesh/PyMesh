# General lib locations
LINK_DIRECTORIES(/opt/local/lib)

# Source files
FILE(GLOB SRC_FILES predicates.c)
FILE(GLOB INC_FILES predicates.h)

# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY(lib_predicates SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_predicates PROPERTIES OUTPUT_NAME "predicates")

IF(NOT WITHOUT_SWIG)
    SET(SWG_FILES PyPredicates.i)

    # Generate wrapper for python
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CPLUSPLUS ON)
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-includeall")
    SWIG_ADD_MODULE(PyPredicates python ${SWG_FILES} ${INC_FILES})
    SWIG_LINK_LIBRARIES(PyPredicates ${PYTHON_LIBRARIES} lib_predicates)

    # Boudle exe, lib, swig wrapper into a single target.
    # To build all: "make predicates"
    ADD_CUSTOM_TARGET(predicates DEPENDS lib_predicates _PyPredicates)
ELSE(NOT WITHOUT_SWIG)
    ADD_CUSTOM_TARGET(predicates DEPENDS lib_predicates)
ENDIF(NOT WITHOUT_SWIG)
