#include "WindingNumberWithHierarchy.h"

#include <cmath>

#include <Misc/cl-helper.h>

#include "AABB.h"
#include "BoundingTree.h"
#include "winding_number.h"

#ifndef WINDING_NUMBER_OMP_MIN_VALUE
#  define WINDING_NUMBER_OMP_MIN_VALUE 1000
#endif

void WindingNumberWithHierarchy::init(const MatrixFr& V, const MatrixIr& F) {
    WindingNumber::init(V, F);
    m_hierarchy = HierarchyPtr(new Hierarchy(V, F));
    m_hierarchy->split_method = MEDIAN_ON_LONGEST_AXIS;
    m_hierarchy->grow();
    BoundingTree<Vector3F>::method = EXACT_WINDING_NUMBER_METHOD;
}

VectorF WindingNumberWithHierarchy::compute(const MatrixFr& P) {
    size_t num_points = P.rows();
    VectorF result(num_points);
//#pragma omp parallel for shared(num_points, result) default(none) if (num_points>WINDING_NUMBER_OMP_MIN_VALUE)
    for (size_t i=0; i<num_points; i++) {
        const Vector3F& p = P.row(i);
        result[i] = m_hierarchy->winding_number(p) / (4 * M_PI);
    }
    return result;
}
