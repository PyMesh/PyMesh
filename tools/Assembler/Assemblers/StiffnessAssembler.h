#pragma once
#include "Assembler.h"

class StiffnessAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
