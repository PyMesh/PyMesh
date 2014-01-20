#pragma once
#include "Assembler.h"
#include <Assembler/Math/ZSparseMatrix.h>

class LaplacianAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
