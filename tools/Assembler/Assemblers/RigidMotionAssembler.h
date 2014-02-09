#pragma once

#include "Assembler.h"
#include <Assembler/Math/ZSparseMatrix.h>

class RigidMotionAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
