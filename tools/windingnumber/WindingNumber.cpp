#include "WindingNumber.h"
#include "AABB.h"
#include "BoundingTree.h"
#include "winding_number.h"

VectorF WindingNumber::compute(const MatrixF& V, const MatrixI& F, const MatrixF& P) {
    AABB<Vector3F> hier(V,F);
    hier.split_method = MEDIAN_ON_LONGEST_AXIS;
    hier.grow();
    BoundingTree<Vector3F>::method = EXACT_WINDING_NUMBER_METHOD;

    size_t num_points = P.rows();
    VectorF result(num_points);
    for (size_t i=0; i<num_points; i++) {
        const Vector3F& p = P.row(i);
        result[i] = hier.winding_number(p);
    }
    return result;
}
