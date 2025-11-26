#pragma once

#include "quant/market/YieldCurve.hpp"

#include <vector>

namespace quant::utils {

std::vector<double> spot_rates(const quant::market::YieldCurve& curve, const std::vector<double>& times);
std::vector<double> forward_rates(const quant::market::YieldCurve& curve, const std::vector<double>& times);
double funding_spread(const quant::market::YieldCurve& a, const quant::market::YieldCurve& b, double t);

} // namespace quant::utils

