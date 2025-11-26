#include "quant/market/VolSurface.hpp"

#include <algorithm>

namespace quant::market {

VolSurface::VolSurface(std::vector<double> strikes, std::vector<double> tenors, std::vector<std::vector<double>> vols)
    : strikes_(std::move(strikes)), tenors_(std::move(tenors)), vols_(std::move(vols)) {
    if (vols_.size() != strikes_.size()) throw quant::core::DataError("Vol grid size mismatch (strikes)");
    for (const auto& row : vols_) {
        if (row.size() != tenors_.size()) throw quant::core::DataError("Vol grid size mismatch (tenors)");
    }
}

double VolSurface::bilinear(double strike, double tenor) const {
    auto itK = std::upper_bound(strikes_.begin(), strikes_.end(), strike);
    auto itT = std::upper_bound(tenors_.begin(), tenors_.end(), tenor);
    std::size_t k1 = (itK == strikes_.begin()) ? 0 : static_cast<std::size_t>(std::distance(strikes_.begin(), itK) - 1);
    std::size_t k2 = (itK == strikes_.end()) ? strikes_.size() - 1 : k1 + 1;
    std::size_t t1 = (itT == tenors_.begin()) ? 0 : static_cast<std::size_t>(std::distance(tenors_.begin(), itT) - 1);
    std::size_t t2 = (itT == tenors_.end()) ? tenors_.size() - 1 : t1 + 1;

    double k_low = strikes_[k1], k_high = strikes_[k2];
    double t_low = tenors_[t1], t_high = tenors_[t2];
    double qk = (k_high == k_low) ? 0.0 : (strike - k_low) / (k_high - k_low);
    double qt = (t_high == t_low) ? 0.0 : (tenor - t_low) / (t_high - t_low);

    double v11 = vols_[k1][t1];
    double v12 = vols_[k1][t2];
    double v21 = vols_[k2][t1];
    double v22 = vols_[k2][t2];

    double v1 = v11 + qt * (v12 - v11);
    double v2 = v21 + qt * (v22 - v21);
    return v1 + qk * (v2 - v1);
}

double VolSurface::volatility(double strike, double tenor) const {
    if (strikes_.empty() || tenors_.empty()) throw quant::core::DataError("Empty vol surface");
    return bilinear(strike, tenor);
}

} // namespace quant::market

