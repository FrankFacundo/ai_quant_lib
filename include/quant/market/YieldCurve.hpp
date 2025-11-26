#pragma once

#include "quant/core/Date.hpp"
#include "quant/core/Exceptions.hpp"

#include <vector>

namespace quant::market {

class YieldCurve {
public:
    YieldCurve() = default;
    YieldCurve(std::vector<double> times, std::vector<double> zero_rates);

    double discount(double t) const; // t in years
    double zero_rate(double t) const;
    double forward_rate(double t1, double t2) const;

    const std::vector<double>& times() const { return times_; }
    const std::vector<double>& zero_rates() const { return zero_rates_; }

private:
    double interpolate(double t) const;

    std::vector<double> times_;
    std::vector<double> zero_rates_;
};

} // namespace quant::market

