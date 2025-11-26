#include "quant/market/YieldCurve.hpp"

#include <algorithm>
#include <cmath>

namespace quant::market {

YieldCurve::YieldCurve(std::vector<double> times, std::vector<double> zero_rates)
    : times_(std::move(times)), zero_rates_(std::move(zero_rates)) {
    if (times_.size() != zero_rates_.size() || times_.empty()) {
        throw quant::core::DataError("Invalid yield curve inputs");
    }
}

double YieldCurve::interpolate(double t) const {
    if (t <= times_.front()) return zero_rates_.front();
    if (t >= times_.back()) return zero_rates_.back();
    auto it = std::upper_bound(times_.begin(), times_.end(), t);
    std::size_t idx = static_cast<std::size_t>(std::distance(times_.begin(), it) - 1);
    double t1 = times_[idx], t2 = times_[idx + 1];
    double r1 = zero_rates_[idx], r2 = zero_rates_[idx + 1];
    double w = (t - t1) / (t2 - t1);
    return r1 + w * (r2 - r1);
}

double YieldCurve::discount(double t) const {
    double r = interpolate(t);
    return std::exp(-r * t);
}

double YieldCurve::zero_rate(double t) const { return interpolate(t); }

double YieldCurve::forward_rate(double t1, double t2) const {
    if (t2 <= t1) return 0.0;
    double d1 = discount(t1);
    double d2 = discount(t2);
    return std::log(d1 / d2) / (t2 - t1);
}

} // namespace quant::market

