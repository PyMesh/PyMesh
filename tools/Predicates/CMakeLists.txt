# General lib locations
LINK_DIRECTORIES(/opt/local/lib)

# Source files
FILE(GLOB SRC_FILES predicates.c)
FILE(GLOB INC_FILES predicates.h)

# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY(lib_predicates STATIC ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_predicates PROPERTIES OUTPUT_NAME "predicates")

ADD_CUSTOM_TARGET(predicates DEPENDS lib_predicates)
