#include "quant/timeseries/Models.hpp"

#include <cmath>

namespace quant::timeseries {

GARCHModel::GARCHModel(double omega, double alpha, double beta)
    : omega_(omega), alpha_(alpha), beta_(beta) {}

void GARCHModel::fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
                     const std::vector<Eigen::VectorXd>& static_features) {
    int n = static_cast<int>(y.size());
    if (n == 0) return;
    cond_vol_.resize(n);
    double var = 0.0;
    for (const auto& v : y.values()) var += v(0) * v(0);
    var /= n;
    cond_vol_(0) = std::sqrt(var);
    for (int i = 1; i < n; ++i) {
        double eps2 = y.values()[i - 1](0) * y.values()[i - 1](0);
        double sigma2 = omega_ + alpha_ * eps2 + beta_ * cond_vol_(i - 1) * cond_vol_(i - 1);
        cond_vol_(i) = std::sqrt(std::max(sigma2, 1e-12));
    }
    last_value_ = y.values().back();
}

Eigen::VectorXd GARCHModel::forecast(std::size_t horizon,
                                     const std::vector<Eigen::VectorXd>& future_static_features) const {
    Eigen::VectorXd out = Eigen::VectorXd::Zero(static_cast<int>(horizon));
    double prev_sigma2 = cond_vol_.size() > 0 ? cond_vol_.tail(1)(0) * cond_vol_.tail(1)(0) : omega_;
    double eps2 = last_value_.size() > 0 ? last_value_(0) * last_value_(0) : 0.0;
    for (std::size_t h = 0; h < horizon; ++h) {
        double sigma2 = omega_ + alpha_ * eps2 + beta_ * prev_sigma2;
        prev_sigma2 = sigma2;
        out(static_cast<int>(h)) = std::sqrt(std::max(sigma2, 1e-12));
        eps2 = 0.0; // expected future shock zero
    }
    return out;
}

} // namespace quant::timeseries

