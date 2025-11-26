#include "quant/timeseries/Models.hpp"
#include "quant/core/Exceptions.hpp"

namespace quant::timeseries {

VARModel::VARModel(int lags) : lags_(lags) {}

void VARModel::fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
                   const std::vector<Eigen::VectorXd>& static_features) {
    if (y.size() <= static_cast<std::size_t>(lags_)) throw quant::core::DataError("Insufficient data for VAR fit");
    dim_ = static_cast<int>(y.values().front().size());
    int n = static_cast<int>(y.size());
    int rows = n - lags_;
    Eigen::MatrixXd X(rows, dim_ * lags_);
    Eigen::MatrixXd Y(rows, dim_);

    for (int i = lags_; i < n; ++i) {
        for (int l = 0; l < lags_; ++l) {
            X.block(i - lags_, l * dim_, 1, dim_) = y.values()[i - l - 1].transpose();
        }
        Y.row(i - lags_) = y.values()[i].transpose();
    }
    Eigen::MatrixXd XtX = X.transpose() * X;
    coefficients_ = XtX.ldlt().solve(X.transpose() * Y);
    Eigen::MatrixXd residuals = Y - X * coefficients_;
    residual_var_.resize(dim_);
    for (int i = 0; i < dim_; ++i) {
        residual_var_(i) = residuals.col(i).squaredNorm() / residuals.rows();
    }
    last_values_ = Eigen::VectorXd(dim_ * lags_);
    for (int l = 0; l < lags_; ++l) {
        last_values_.segment(l * dim_, dim_) = y.values()[n - l - 1];
    }
}

Eigen::VectorXd VARModel::forecast(std::size_t horizon,
                                   const std::vector<Eigen::VectorXd>& future_static_features) const {
    Eigen::VectorXd state = last_values_;
    Eigen::VectorXd out = Eigen::VectorXd::Zero(static_cast<int>(horizon * dim_));
    for (std::size_t h = 0; h < horizon; ++h) {
        Eigen::VectorXd pred = Eigen::VectorXd::Zero(dim_);
        for (int l = 0; l < lags_; ++l) {
            Eigen::MatrixXd A = coefficients_.block(l * dim_, 0, dim_, dim_);
            pred += A * state.segment(l * dim_, dim_);
        }
        out.segment(static_cast<int>(h * dim_), dim_) = pred;
        // shift state
        for (int l = lags_ - 1; l > 0; --l) {
            state.segment(l * dim_, dim_) = state.segment((l - 1) * dim_, dim_);
        }
        state.segment(0, dim_) = pred;
    }
    return out;
}

double VARModel::aic() const {
    double loglik = 0.0;
    for (int i = 0; i < residual_var_.size(); ++i) {
        loglik -= 0.5 * std::log(residual_var_(i));
    }
    int k = coefficients_.rows() * coefficients_.cols();
    return -2 * loglik + 2 * k;
}

double VARModel::bic() const {
    double loglik = 0.0;
    for (int i = 0; i < residual_var_.size(); ++i) loglik -= 0.5 * std::log(residual_var_(i));
    int k = coefficients_.rows() * coefficients_.cols();
    return -2 * loglik + std::log(static_cast<double>(residual_var_.size())) * k;
}

} // namespace quant::timeseries

