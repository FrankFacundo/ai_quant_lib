#include "quant/timeseries/Models.hpp"
#include "quant/core/Exceptions.hpp"

#include <numeric>

namespace quant::timeseries {

ARIMAModel::ARIMAModel(int p, int d, int q) : p_(p), d_(d), q_(q) {}

void ARIMAModel::fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
                     const std::vector<Eigen::VectorXd>& static_features) {
    if (y.size() == 0) throw quant::core::DataError("Empty time series for ARIMA fit");
    int n = static_cast<int>(y.size());
    std::vector<double> diffed;
    diffed.reserve(n);
    for (const auto& v : y.values()) diffed.push_back(v(0));
    for (int i = 0; i < d_; ++i) {
        std::vector<double> tmp;
        for (std::size_t j = 1; j < diffed.size(); ++j) tmp.push_back(diffed[j] - diffed[j - 1]);
        diffed.swap(tmp);
    }
    int m = static_cast<int>(diffed.size());
    if (m <= p_) throw quant::core::DataError("Insufficient data for AR order");

    Eigen::MatrixXd X(m - p_, p_);
    Eigen::VectorXd yvec(m - p_);
    for (int i = p_; i < m; ++i) {
        for (int j = 0; j < p_; ++j) {
            X(i - p_, j) = diffed[i - j - 1];
        }
        yvec(i - p_) = diffed[i];
    }
    coefficients_ = (X.transpose() * X).ldlt().solve(X.transpose() * yvec);
    residuals_ = yvec - X * coefficients_;
    last_values_.resize(p_);
    for (int j = 0; j < p_; ++j) last_values_(j) = diffed[m - j - 1];
}

Eigen::VectorXd ARIMAModel::forecast(std::size_t horizon,
                                     const std::vector<Eigen::VectorXd>& future_static_features) const {
    Eigen::VectorXd result = Eigen::VectorXd::Zero(static_cast<int>(horizon));
    Eigen::VectorXd state = last_values_;
    for (std::size_t h = 0; h < horizon; ++h) {
        double pred = 0.0;
        for (int j = 0; j < coefficients_.size(); ++j) pred += coefficients_(j) * state(j);
        result(static_cast<int>(h)) = pred;
        // shift state
        for (int j = coefficients_.size() - 1; j > 0; --j) state(j) = state(j - 1);
        state(0) = pred;
    }
    return result;
}

double ARIMAModel::aic() const {
    double sigma2 = residuals_.squaredNorm() / residuals_.size();
    return 2 * coefficients_.size() + residuals_.size() * std::log(sigma2);
}

double ARIMAModel::bic() const {
    double sigma2 = residuals_.squaredNorm() / residuals_.size();
    return std::log(residuals_.size()) * coefficients_.size() + residuals_.size() * std::log(sigma2);
}

} // namespace quant::timeseries

