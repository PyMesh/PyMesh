.PHONY: all
.PHONY: mex
.PHONY: blank

CXX=g++

IGL=/Users/qingnanzhou/Research/sources/libigl/
IGL_LIB=-L$(IGL)/lib -ligl -liglmatlab -lgomp
IGL_INC=-I$(IGL)/include

EIGEN3_INC=-I/opt/local/include/eigen3 -I/opt/local/include/eigen3/unsupported

MATLAB_INC=-I$(MATLAB)/extern/include/ -I$(MATLAB)/simulink/include/
MATLAB_LIB=-L$(MATLAB)/bin/maci64 -lmx -lmat -lmex -lstdc++
MATLAB_CFLAGS=-DMATLAB_MEX_FILE -fno-common -fexceptions -fno-common \
	-fexceptions -isysroot \
	/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/ \
	-mmacosx-version-min=10.7  -DMX_COMPAT_32 
MATLAB_LNFLAGS=-syslibroot,/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/ \
	-mmacosx-version-min=10.7 \
	-bundle -Wl,-exported_symbols_list,$(MATLAB)/extern/lib/maci64/mexFunction.map \

all: winding_number

# fopenmp might not be helping

#CFLAGS += -g -Wall -Winvalid-pch -fopenmp
#CFLAGS += -O3 -Wall -DNDEBUG -Winvalid-pch -m64
CFLAGS += -O3 -Wall -DNDEBUG -Winvalid-pch -m64 -fopenmp

CPP_FILES=$(wildcard *.cpp)
OBJ_FILES=$(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

LIB+=$(IGL_LIB)
INC+=$(TETGEN_INC) $(IGL_INC) $(EIGEN3_INC)

mex: CFLAGS += -DMEX
mex: LIB+=$(MATLAB_LIB)
mex: INC+=$(MATLAB_INC)

winding_number: $(OBJ_FILES)
	${CXX} $(CFLAGS) -o $@ $^ $(LIB)

obj:
	mkdir -p obj

obj/%.o: %.cpp %.h
	${CXX} $(CFLAGS) -o $@ -c $< $(INC) 

# Need a special rule so that main.cpp gets built without its corresponding
# main.h header file
obj/%.o: %.cpp obj
	${CXX} $(CFLAGS) -o $@ -c $< $(INC)

clean:
	rm -f $(OBJ_FILES)
	rm -f winding_number

clean_output:
	rm *.off *.mesh *.edge *.face *.smesh *.node *.dmat *.ele
