#pragma once

#include "quant/core/Exceptions.hpp"

#include <vector>

namespace quant::market {

class VolSurface {
public:
    VolSurface() = default;
    VolSurface(std::vector<double> strikes, std::vector<double> tenors, std::vector<std::vector<double>> vols);

    double volatility(double strike, double tenor) const;

    const std::vector<double>& strikes() const { return strikes_; }
    const std::vector<double>& tenors() const { return tenors_; }
    const std::vector<std::vector<double>>& vols() const { return vols_; }

private:
    double bilinear(double strike, double tenor) const;

    std::vector<double> strikes_;
    std::vector<double> tenors_;
    std::vector<std::vector<double>> vols_; // vols_[i][j] strike i, tenor j
};

} // namespace quant::market
