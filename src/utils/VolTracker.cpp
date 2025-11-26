#include "quant/utils/Volatility.hpp"

#include <cmath>

namespace quant::utils {

double realized_vol(const quant::core::TimeSeries<double>& returns, std::size_t window) {
    if (returns.size() < window || window == 0) return 0.0;
    double mean = 0.0;
    for (std::size_t i = returns.size() - window; i < returns.size(); ++i) mean += returns.values()[i];
    mean /= window;
    double var = 0.0;
    for (std::size_t i = returns.size() - window; i < returns.size(); ++i) {
        double d = returns.values()[i] - mean;
        var += d * d;
    }
    return std::sqrt(var / window) * std::sqrt(252.0);
}

quant::core::TimeSeries<double> realized_vol_series(const quant::core::TimeSeries<double>& returns,
                                                   std::size_t window) {
    quant::core::TimeSeries<double> out;
    if (returns.size() < window || window == 0) return out;
    for (std::size_t i = window - 1; i < returns.size(); ++i) {
        double mean = 0.0;
        for (std::size_t j = i - window + 1; j <= i; ++j) mean += returns.values()[j];
        mean /= window;
        double var = 0.0;
        for (std::size_t j = i - window + 1; j <= i; ++j) {
            double d = returns.values()[j] - mean;
            var += d * d;
        }
        double rv = std::sqrt(var / window) * std::sqrt(252.0);
        out.push_back(returns.times()[i], rv);
    }
    return out;
}

double implied_realized_spread(double implied_vol, double realized_vol) {
    return implied_vol - realized_vol;
}

} // namespace quant::utils

