#pragma once
#include <string>
#include <tr1/memory>

#include <Assembler/FESetting/FESetting.h>
#include <Assembler/Math/ZSparseMatrix.h>

class Assembler {
    public:
        typedef std::tr1::shared_ptr<Assembler> Ptr;
        typedef FESetting::Ptr FESettingPtr;
    public:
        static Ptr create(const std::string& matrix_name);

        virtual ZSparseMatrix assemble(FESettingPtr setting)=0;
};
