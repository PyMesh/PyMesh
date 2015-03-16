%{
#include <CGAL/AABBTree.h>
%}
%apply VectorF& ARGOUT { VectorF& squared_dists }
%apply VectorI& ARGOUT { VectorI& closest_face_indices }
%apply MatrixFr& ARGOUT { MatrixFr& closest_points }
%include "AABBTree.h"
