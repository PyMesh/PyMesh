%{
#include "DMAT.h"
%}
%include "std_string.i"
%include "DMAT.h"
%template(load_MatrixF_DMAT) DMAT::load_matrix<MatrixF>;
%template(load_MatrixFr_DMAT) DMAT::load_matrix<MatrixFr>;
%template(load_MatrixI_DMAT) DMAT::load_matrix<MatrixI>;
%template(load_MatrixIr_DMAT) DMAT::load_matrix<MatrixIr>;

%template(save_MatrixF_DMAT) DMAT::save_matrix<MatrixF>;
%template(save_MatrixFr_DMAT) DMAT::save_matrix<MatrixFr>;
%template(save_MatrixI_DMAT) DMAT::save_matrix<MatrixI>;
%template(save_MatrixIr_DMAT) DMAT::save_matrix<MatrixIr>;
