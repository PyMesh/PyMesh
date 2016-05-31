%include <std_string.i>
%include <std_shared_ptr.i>
%shared_ptr(PyMesh::WireNetwork)
%include <Mesh.i>
%include <std_vector.i>
namespace std {
%template(vectorb) vector<bool>;
};
%{
#include <Wires/WireNetwork/WireNetwork.h>
%}
%include "WireNetwork.h"
