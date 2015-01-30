#include <sstream>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

template<typename T>
T MatrixUtils::vstack(const std::vector<T>& matrices) {
    if (matrices.empty()) {
        throw RuntimeError("Cannot vstack empty matrix array");
    }
    size_t num_rows=0;
    size_t num_cols=matrices[0].cols();
    for (const auto& mat : matrices) {
        num_rows += mat.rows();
        if (mat.cols() != num_cols) {
            std::stringstream err_msg;
            err_msg << "Cannot vstack because "
                << "matrix have different number of columns"
                << std::endl
                << "Expect " << num_cols << " but get "
                << mat.cols() << "columns";
            throw RuntimeError(err_msg.str());
        }
    }

    T result(num_rows, num_cols);
    size_t row_count = 0;
    for (const auto& mat : matrices) {
        result.block(row_count, 0, mat.rows(), mat.cols()) = mat;
        row_count += mat.rows();
    }
    assert(row_count == num_rows);
    return result;
}

