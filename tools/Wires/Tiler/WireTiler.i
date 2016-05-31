%include Mesh.i
%include WireNetwork/WireNetwork.i
%include std_vector.i
namespace std {
    %template(WireVector) vector<std::shared_ptr<PyMesh::WireNetwork> >;
}
%{
#include <Wires/Tiler/WireTiler.h>
%}
%include "WireTiler.h"
