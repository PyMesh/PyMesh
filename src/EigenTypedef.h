#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>

typedef double Float;
typedef Eigen::VectorXd VectorF;
typedef Eigen::VectorXi VectorI;
typedef Eigen::Vector4d Vector4F;
typedef Eigen::Vector4i Vector4I;
typedef Eigen::Vector3d Vector3F;
typedef Eigen::Vector3i Vector3I;
typedef Eigen::Vector2d Vector2F;
typedef Eigen::Vector2i Vector2I;

typedef Eigen::MatrixXd MatrixF;
typedef Eigen::Matrix3d Matrix3F;
typedef Eigen::Matrix4d Matrix4F;
typedef Eigen::MatrixXi MatrixI;
typedef Eigen::Matrix3i Matrix3I;
typedef Eigen::Matrix4i Matrix4I;

typedef Eigen::Matrix<int  , Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixIr;
typedef Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixFr;
