#pragma once
#include "Assembler.h"

class LaplacianAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
