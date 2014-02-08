#pragma once

#include "Assembler.h"
#include <Assembler/Math/ZSparseMatrix.h>

class DisplacementStrainAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);

    private:
        MatrixI get_entry_order(size_t dim);
};
