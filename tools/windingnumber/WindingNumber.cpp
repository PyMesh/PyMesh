#include "WindingNumber.h"

#include <cmath>

#include "AABB.h"
#include "BoundingTree.h"
#include "winding_number.h"

#ifndef WINDING_NUMBER_OMP_MIN_VALUE
#  define WINDING_NUMBER_OMP_MIN_VALUE 1000
#endif

WindingNumber::WindingNumber(const MatrixF& V, const MatrixI& F) : m_hierarchy(V, F){
    m_hierarchy.split_method = MEDIAN_ON_LONGEST_AXIS;
    m_hierarchy.grow();
    BoundingTree<Vector3F>::method = EXACT_WINDING_NUMBER_METHOD;
}

VectorF WindingNumber::compute(const MatrixF& P) {
    size_t num_points = P.rows();
    VectorF result(num_points);
//#pragma omp parallel for shared(num_points, result) default(none) if (num_points>WINDING_NUMBER_OMP_MIN_VALUE)
    for (size_t i=0; i<num_points; i++) {
        const Vector3F& p = P.row(i);
        result[i] = m_hierarchy.winding_number(p) / (4 * M_PI);
    }
    return result;
}

//VectorF WindingNumber::compute(const MatrixF& V, const MatrixI& F, const MatrixF& P) {
//    AABB<Vector3F> hier(V,F);
//    hier.split_method = MEDIAN_ON_LONGEST_AXIS;
//    hier.grow();
//    BoundingTree<Vector3F>::method = EXACT_WINDING_NUMBER_METHOD;
//
//    size_t num_points = P.rows();
//    VectorF result(num_points);
//    for (size_t i=0; i<num_points; i++) {
//        const Vector3F& p = P.row(i);
//        result[i] = hier.winding_number(p) / (4 * M_PI);
//    }
//    return result;
//}
