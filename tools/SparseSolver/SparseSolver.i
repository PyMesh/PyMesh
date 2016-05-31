%include "std_shared_ptr.i"
%shared_ptr(PyMesh::SparseSolver);
%{
#include "SparseSolver.h"
%}
%include Math/ZSparseMatrix.i
%pythonbegin %{
import numpy as np
import scipy.sparse
%}
%pythoncode %{
def csc2ZSparseMatrix(mat):
    mat = mat.tocsc();
    zmat = ZSparseMatrix();
    zmat.import_raw_csc(mat.shape[0], mat.shape[1],
            mat.indices, mat.indptr, mat.data);
    return zmat;

def coo2ZSparseMatrix(mat):
    mat = mat.tocoo();
    zmat = ZSparseMatrix();
    zmat.import_raw_coo(mat.shape[0], mat.shape[1], mat.row, mat.col, mat.data);
    return zmat;
%}
%pythonprepend PyMesh::SparseSolver::analyze_pattern(const PyMesh::ZSparseMatrix& matrix) %{
matrix = coo2ZSparseMatrix(matrix);
%}
%pythonprepend PyMesh::SparseSolver::factorize(const PyMesh::ZSparseMatrix& matrix) %{
matrix = coo2ZSparseMatrix(matrix);
%}
%include "SparseSolver.h"
