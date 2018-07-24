################################################################################
# Find Geogram and build it as part of the current build
################################################################################

if(TARGET geogram)
	return()
endif()

################################################################################

if(THIRD_PARTY_DIR)
	set(GEOGRAM_SEARCH_PATHS ${THIRD_PARTY_DIR})
else()
	set(GEOGRAM_SEARCH_PATHS
		${GEOGRAM_INSTALL_PREFIX}
		"$ENV{GEOGRAM_INSTALL_PREFIX}"
		"/usr/local/"
		"$ENV{PROGRAMFILES}/Geogram"
		"$ENV{PROGRAMW6432}/Geogram"
		"$ENV{HOME}/.local/")
endif()

find_path(GEOGRAM_SOURCE_INCLUDE_DIR
		geogram/basic/common.h
		PATHS ${GEOGRAM_SEARCH_PATHS}
		PATH_SUFFIXES geogram/src/lib
)

set(GEOGRAM_ROOT ${GEOGRAM_SOURCE_INCLUDE_DIR}/../..)

################################################################################

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(VORPALINE_ARCH_64 TRUE CACHE BOOL "" FORCE)
	set(VORPALINE_PLATFORM Win-vs-generic CACHE STRING "" FORCE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(VORPALINE_PLATFORM Linux64-gcc-dynamic CACHE STRING "" FORCE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(VORPALINE_PLATFORM Darwin-clang-dynamic CACHE STRING "" FORCE)
endif()

option(GEOGRAM_WITH_GRAPHICS "Viewers and geogram_gfx library" OFF)
option(GEOGRAM_WITH_LEGACY_NUMERICS "Legacy numerical libraries" OFF)
option(GEOGRAM_WITH_HLBFGS "Non-linear solver (Yang Liu's HLBFGS)" ON)
option(GEOGRAM_WITH_TETGEN "Tetrahedral mesher (Hang Si's TetGen)" ON)
option(GEOGRAM_WITH_TRIANGLE "Triangle mesher (Jonathan Shewchuk's triangle)" ON)
option(GEOGRAM_WITH_EXPLORAGRAM "Experimental code (hexahedral meshing vpipeline and optimal transport)" ON)
option(GEOGRAM_WITH_LUA "Built-in LUA interpreter" OFF)
option(GEOGRAM_LIB_ONLY "Libraries only (no example programs/no viewer)" ON)
option(GEOGRAM_WITH_FPG "Predicate generator (Sylvain Pion's FPG)" OFF)
option(GEOGRAM_USE_SYSTEM_GLFW3 "Use the version of GLFW3 installed in the system if found" OFF)

################################################################################

add_subdirectory(${GEOGRAM_ROOT} geogram)
target_include_directories(geogram SYSTEM PUBLIC ${GEOGRAM_SOURCE_INCLUDE_DIR})

################################################################################

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# remove warning for multiply defined symbols (caused by multiple
	# instanciations of STL templates)
	#target_compile_options(geogram INTERFACE /wd4251)

	# remove all unused stuff from windows.h
	# target_compile_definitions(geogram INTERFACE -DWIN32_LEAN_AND_MEAN)
	# target_compile_definitions(geogram INTERFACE -DVC_EXTRALEAN)

	# do not define a min() and a max() macro, breaks
	# std::min() and std::max() !!
	target_compile_definitions(geogram INTERFACE -DNOMINMAX)

	# we want M_PI etc...
	target_compile_definitions(geogram INTERFACE -D_USE_MATH_DEFINES)

	if(NOT VORPALINE_BUILD_DYNAMIC)
		# If we use static library, we link with the static C++ runtime.
		foreach(config ${CMAKE_CONFIGURATION_TYPES})
			string(TOUPPER ${config} config)
			string(REPLACE /MD /MT CMAKE_C_FLAGS_${config} "${CMAKE_C_FLAGS_${config}}")
			string(REPLACE /MD /MT CMAKE_CXX_FLAGS_${config} "${CMAKE_CXX_FLAGS_${config}}")
		endforeach()
	endif()
endif()
