#include "quant/instruments/BarrierOption.hpp"

#include <cmath>

namespace quant::instruments {

namespace {
inline double norm_cdf(double x) { return 0.5 * std::erfc(-x / std::sqrt(2.0)); }
}

BarrierOption::BarrierOption(BarrierType type, OptionType opt_type, double spot, double strike, double maturity,
                             double rate, double vol, double barrier, double rebate)
    : barrier_type_(type), opt_type_(opt_type), spot_(spot), strike_(strike), maturity_(maturity),
      rate_(rate), vol_(vol), barrier_(barrier), rebate_(rebate) {}

double BarrierOption::npv() const {
    // Fallback to vanilla price ignoring barrier for standalone usage
    double d1 = (std::log(spot_ / strike_) + (rate_ + 0.5 * vol_ * vol_) * maturity_) / (vol_ * std::sqrt(maturity_));
    double d2 = d1 - vol_ * std::sqrt(maturity_);
    double df = std::exp(-rate_ * maturity_);
    if (opt_type_ == OptionType::Call) {
        return spot_ * norm_cdf(d1) - strike_ * df * norm_cdf(d2);
    }
    return strike_ * df * norm_cdf(-d2) - spot_ * norm_cdf(-d1);
}

} // namespace quant::instruments

