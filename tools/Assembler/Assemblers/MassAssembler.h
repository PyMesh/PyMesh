#pragma once
#include "Assembler.h"

class MassAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
