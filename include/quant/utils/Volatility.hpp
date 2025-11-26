#pragma once

#include "quant/core/TimeSeries.hpp"

namespace quant::utils {

double realized_vol(const quant::core::TimeSeries<double>& returns, std::size_t window);
quant::core::TimeSeries<double> realized_vol_series(const quant::core::TimeSeries<double>& returns,
                                                   std::size_t window);

double implied_realized_spread(double implied_vol, double realized_vol);

} // namespace quant::utils

