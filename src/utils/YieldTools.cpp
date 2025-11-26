#include "quant/utils/YieldTools.hpp"

#include <vector>

namespace quant::utils {

std::vector<double> spot_rates(const quant::market::YieldCurve& curve, const std::vector<double>& times) {
    std::vector<double> out;
    out.reserve(times.size());
    for (double t : times) out.push_back(curve.zero_rate(t));
    return out;
}

std::vector<double> forward_rates(const quant::market::YieldCurve& curve, const std::vector<double>& times) {
    std::vector<double> out;
    if (times.size() < 2) return out;
    for (std::size_t i = 1; i < times.size(); ++i) {
        out.push_back(curve.forward_rate(times[i - 1], times[i]));
    }
    return out;
}

double funding_spread(const quant::market::YieldCurve& a, const quant::market::YieldCurve& b, double t) {
    return b.zero_rate(t) - a.zero_rate(t);
}

} // namespace quant::utils

