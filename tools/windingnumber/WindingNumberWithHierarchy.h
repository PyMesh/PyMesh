#pragma once
#include <tr1/memory>
#include "WindingNumber.h"
#include "AABB.h"

class WindingNumberWithHierarchy : public WindingNumber {
    public:
        virtual ~WindingNumberWithHierarchy() {}

    public:
        virtual void init(const MatrixFr& V, const MatrixIr& F);
        virtual VectorF compute(const MatrixFr& P);

    private:
        typedef AABB<Vector3F> Hierarchy;
        typedef std::tr1::shared_ptr<Hierarchy> HierarchyPtr;
        HierarchyPtr m_hierarchy;
};
