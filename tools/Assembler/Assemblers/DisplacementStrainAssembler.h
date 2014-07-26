#pragma once

#include "Assembler.h"

class DisplacementStrainAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
