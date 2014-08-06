%include "std_shared_ptr.i"
%shared_ptr(SparseSolver);
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
%pythonprepend SparseSolver::analyze_pattern(const ZSparseMatrix& matrix) %{
if len(args) == 1:
    M = coo2ZSparseMatrix(args[0]);
    args = tuple([M]);
%}
%pythonprepend SparseSolver::factorize(const ZSparseMatrix& matrix) %{
if len(args) == 1:
    M = coo2ZSparseMatrix(args[0]);
    args = tuple([M]);
%}
%include "SparseSolver.h"
