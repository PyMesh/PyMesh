#pragma once
#include "Assembler.h"

class LumpedMassAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
