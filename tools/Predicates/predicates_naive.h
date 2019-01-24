/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */

#pragma once

namespace PyMesh {

template<typename T>
inline T orient2d_naive(T pa[2], T pb[2], T pc[2]) {
    return (pa[0]-pc[0]) * (pb[1]-pc[1]) - (pa[1]-pc[1]) * (pb[0]-pc[0]);
}

template<typename T>
inline T incircle_naive(T pa[2], T pb[2], T pc[2], T pd[2]) {
    const T A[2]{pa[0]-pd[0], pa[1]-pd[1]};
    const T B[2]{pb[0]-pd[0], pb[1]-pd[1]};
    const T C[2]{pc[0]-pd[0], pc[1]-pd[1]};
    return (A[0]*B[1] - A[1]*B[0]) * (C[0]*C[0] + C[1]*C[1]) -
           (A[0]*C[1] - A[1]*C[0]) * (B[0]*B[0] + B[1]*B[1]) +
           (B[0]*C[1] - B[1]*C[0]) * (A[0]*A[0] + A[1]*A[1]);
}

template <typename T>
inline T orient3d_naive(T pa[3], T pb[3], T pc[3], T pd[3]) {
    const T A[3]{pa[0]-pd[0], pa[1]-pd[1], pa[2]-pd[2]};
    const T B[3]{pb[0]-pd[0], pb[1]-pd[1], pb[2]-pd[2]};
    const T C[3]{pc[0]-pd[0], pc[1]-pd[1], pc[2]-pd[2]};

    return (A[0]*B[1]-A[1]*B[0]) * C[2] -
           (A[0]*C[1]-A[1]*C[0]) * B[2] +
           (B[0]*C[1]-B[1]*C[0]) * A[2];
}

template <typename T>
inline T insphere_naive(T pa[3], T pb[3], T pc[3], T pd[3], T pe[3]) {
    const T A[3]{pa[0]-pe[0], pa[1]-pe[1], pa[2]-pe[2]};
    const T B[3]{pb[0]-pe[0], pb[1]-pe[1], pb[2]-pe[2]};
    const T C[3]{pc[0]-pe[0], pc[1]-pe[1], pc[2]-pe[2]};
    const T D[3]{pd[0]-pe[0], pd[1]-pe[1], pd[2]-pe[2]};

    const T AB = A[0]*B[1] - A[1]*B[0];
    const T BC = B[0]*C[1] - B[1]*C[0];
    const T CD = C[0]*D[1] - C[1]*D[0];
    const T DA = D[0]*A[1] - D[1]*A[0];

    const T CA = C[0]*A[1] - C[1]*A[0];
    const T DB = D[0]*B[1] - D[1]*B[0];

    const T ABC =  AB*C[2] + BC*A[2] + CA*B[2];
    const T CDA =  CD*A[2] + DA*C[2] - CA*D[2];
    const T DAB =  DA*B[2] + AB*D[2] - DB*A[2];
    const T BCD =  BC*D[2] + CD*B[2] + DB*C[2];

    return -(BCD * (A[0]*A[0] + A[1]*A[1] + A[2]*A[2]) -
             CDA * (B[0]*B[0] + B[1]*B[1] + B[2]*B[2]) +
             DAB * (C[0]*C[0] + C[1]*C[1] + C[2]*C[2]) -
             ABC * (D[0]*D[0] + D[1]*D[1] + D[2]*D[2]));
}


}

