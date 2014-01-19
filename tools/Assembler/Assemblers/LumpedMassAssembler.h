#pragma once
#include "Assembler.h"
#include <Assembler/Math/ZSparseMatrix.h>

class LumpedMassAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
