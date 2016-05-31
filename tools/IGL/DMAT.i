%{
#include "DMAT.h"
%}
%include "std_string.i"
%include "DMAT.h"
%template(load_MatrixF_DMAT) PyMesh::DMAT::load_matrix<PyMesh::MatrixF>;
%template(load_MatrixFr_DMAT) PyMesh::DMAT::load_matrix<PyMesh::MatrixFr>;
%template(load_MatrixI_DMAT) PyMesh::DMAT::load_matrix<PyMesh::MatrixI>;
%template(load_MatrixIr_DMAT) PyMesh::DMAT::load_matrix<PyMesh::MatrixIr>;

%template(save_MatrixF_DMAT) PyMesh::DMAT::save_matrix<PyMesh::MatrixF>;
%template(save_MatrixFr_DMAT) PyMesh::DMAT::save_matrix<PyMesh::MatrixFr>;
%template(save_MatrixI_DMAT) PyMesh::DMAT::save_matrix<PyMesh::MatrixI>;
%template(save_MatrixIr_DMAT) PyMesh::DMAT::save_matrix<PyMesh::MatrixIr>;
