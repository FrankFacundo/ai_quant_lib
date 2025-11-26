#pragma once

#include <Eigen/Dense>

namespace quant::core {

using Matrix = Eigen::MatrixXd;
using Vector = Eigen::VectorXd;

inline Matrix covariance(const Matrix& data) {
    // rows: observations, cols: variables
    Matrix centered = data.rowwise() - data.colwise().mean();
    return (centered.transpose() * centered) / static_cast<double>(data.rows() - 1);
}

inline Matrix correlation(const Matrix& data) {
    Matrix cov = covariance(data);
    Eigen::VectorXd stddev = cov.diagonal().cwiseSqrt();
    Matrix corr = cov;
    for (int i = 0; i < corr.rows(); ++i) {
        for (int j = 0; j < corr.cols(); ++j) {
            corr(i, j) /= (stddev(i) * stddev(j) + 1e-12);
        }
    }
    return corr;
}

} // namespace quant::core

