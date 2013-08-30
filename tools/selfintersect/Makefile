.PHONY: all
.PHONY: blank

CXX=g++

IGL=/Users/qingnanzhou/Research/sources/libigl/
IGL_LIB=-L$(IGL)/lib -ligl -liglmatlab
IGL_INC=-I$(IGL)/include

EIGEN3_INC=-I/opt/local/include/eigen3 -I/opt/local/include/eigen3/unsupported

CGAL=/opt/local/
CGAL_LIB=-L$(CGAL)/lib -lCGAL -lCGAL_Core -lgmp -lmpfr -lboost_thread-mt -lboost_system-mt
CGAL_INC=-I$(CGAL)/include -I/usr/include/
# This is absolutely necessary for Exact Construction
CGAL_FLAGS=-frounding-math 

all: selfintersect collapse_small_triangles

CXXFLAGS += -Os -Wall -DNDEBUG -Winvalid-pch -pipe ${CGAL_FLAGS}
# Uncomment the following line to produce a faster program, but one that only detects
# selfintersections exactly rather than also meshes them exactly
#CXXFLAGS += -DINEXACT_CONSTRUCTION

CPP_FILES=$(wildcard src/*.cpp)
OBJ_FILES=$(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

# headers to precompile
HPP_FILES=$(wildcard *.hpp)
PRE_FILES=$(addprefix gch/pre-,$(HPP_FILES))
# http://stackoverflow.com/a/3164874/148668
GCH_FILES=$(PRE_FILES:.hpp=.hpp.gch)
# remember to add precompiled header directory to includes
GCH_INC=-I./gch $(addprefix -include ,$(PRE_FILES))

LIB+=$(TETGEN_LIB) $(IGL_LIB) $(CGAL_LIB)
INC+=$(TETGEN_INC) $(IGL_INC) $(EIGEN3_INC) $(CGAL_INC)

selfintersect_main.o: selfintersect_main.cpp
	${CXX} ${CXXFLAGS} -c selfintersect_main.cpp $(INC) -Isrc -o selfintersect_main.o

selfintersect: selfintersect_main.o gch $(GCH_FILES) obj $(OBJ_FILES)
	${CXX} ${CXXFLAGS} -o $@ selfintersect_main.o $(OBJ_FILES) $(LIB)

collapse_small_triangles_main.o: collapse_small_triangles_main.cpp
	${CXX} ${CXXFLAGS} -c collapse_small_triangles_main.cpp $(INC) -Isrc -o collapse_small_triangles_main.o

collapse_small_triangles: collapse_small_triangles_main.o gch $(GCH_FILES) obj $(OBJ_FILES)
	${CXX} ${CXXFLAGS} -o collapse_small_triangles collapse_small_triangles_main.o $(OBJ_FILES) $(LIB)

gch:
	mkdir -p gch

obj:
	mkdir -p obj

obj/%.o: src/%.cpp src/%.h $(GCH_FILES) 
	${CXX} $(CXXFLAGS) -o $@ -c $< $(INC) 

# http://stackoverflow.com/a/3164874/148668
gch/pre-%.hpp.gch: %.hpp
	${CXX} $(CXXFLAGS) -o $@ -x c++-header -c $< $(INC)

# Need a special rule so that main.cpp gets built without its corresponding
# main.h header file
obj/%.o: %.cpp $(GCH_FILES) 
	${CXX} $(CXXFLAGS) -o $@ -c $< $(GCH_INC) $(INC)

clean:
	rm -f $(OBJ_FILES)
	rm -f $(GCH_FILES)
	rm -f selfintersect
	rm -f selfintersect_main.o
	rm -f collapse_small_triangles_main.o
	rm -f collapse_small_triangles
	rm -f collapse_small_triangles.o

clean_output:
	rm *.off *.mesh *.edge *.face *.smesh *.node *.dmat *.ele
