#pragma once

#include "quant/core/LinearAlgebra.hpp"
#include "quant/core/TimeSeries.hpp"

#include <map>
#include <string>

namespace quant::utils {

quant::core::Matrix correlation_matrix(const std::map<std::string, quant::core::TimeSeries<double>>& series);
quant::core::Matrix rolling_correlation(const quant::core::TimeSeries<double>& a,
                                       const quant::core::TimeSeries<double>& b,
                                       std::size_t window);

class MacroDashboard {
public:
    void add_series(const std::string& name, const quant::core::TimeSeries<double>& ts);
    quant::core::Matrix correlations() const;
    quant::core::Vector beta(const std::string& name, const std::string& benchmark) const;

private:
    std::map<std::string, quant::core::TimeSeries<double>> series_;
};

} // namespace quant::utils

