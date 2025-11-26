#include "quant/instruments/EuropeanOption.hpp"
#include "quant/market/VolSurface.hpp"
#include "quant/market/YieldCurve.hpp"

#include <cmath>

namespace quant::instruments {

namespace {
inline double norm_cdf(double x) { return 0.5 * std::erfc(-x / std::sqrt(2.0)); }
}

EuropeanOption::EuropeanOption(OptionType type, double spot, double strike, double maturity,
                               double rate, double vol, double dividend)
    : type_(type), spot_(spot), strike_(strike), maturity_(maturity), rate_(rate), vol_(vol), dividend_(dividend) {}

double EuropeanOption::npv() const {
    double r = rate_;
    double sigma = vol_;
    if (curve_) {
        r = curve_->zero_rate(maturity_);
    }
    if (vol_surface_) {
        sigma = vol_surface_->volatility(strike_, maturity_);
    }
    if (maturity_ <= 0.0 || sigma <= 0.0) return 0.0;
    double d1 = (std::log(spot_ / strike_) + (r - dividend_ + 0.5 * sigma * sigma) * maturity_) / (sigma * std::sqrt(maturity_));
    double d2 = d1 - sigma * std::sqrt(maturity_);
    double df = std::exp(-r * maturity_);
    double qf = std::exp(-dividend_ * maturity_);
    if (type_ == OptionType::Call) {
        return spot_ * qf * norm_cdf(d1) - strike_ * df * norm_cdf(d2);
    }
    return strike_ * df * norm_cdf(-d2) - spot_ * qf * norm_cdf(-d1);
}

} // namespace quant::instruments

