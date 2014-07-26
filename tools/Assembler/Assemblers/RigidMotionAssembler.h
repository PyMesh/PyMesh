#pragma once

#include "Assembler.h"

class RigidMotionAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};
