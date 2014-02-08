#pragma once

#include "Assembler.h"
#include <Assembler/Math/ZSparseMatrix.h>

class ElasticityTensorAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
