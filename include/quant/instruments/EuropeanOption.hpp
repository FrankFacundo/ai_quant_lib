#pragma once

#include "quant/core/Date.hpp"
#include "quant/instruments/Instrument.hpp"

#include <optional>
#include <string>

namespace quant::market { class YieldCurve; class VolSurface; }

namespace quant::instruments {

enum class OptionType { Call, Put };

class EuropeanOption : public Instrument {
public:
    EuropeanOption() = default;
    EuropeanOption(OptionType type, double spot, double strike, double maturity,
                   double rate, double vol, double dividend = 0.0);

    double spot() const { return spot_; }
    double strike() const { return strike_; }
    double maturity() const { return maturity_; }
    double rate() const { return rate_; }
    double dividend() const { return dividend_; }
    double volatility() const { return vol_; }
    OptionType option_type() const { return type_; }

    void set_vol_surface(const market::VolSurface* surface) { vol_surface_ = surface; }
    void set_yield_curve(const market::YieldCurve* curve) { curve_ = curve; }

    double npv() const override;

private:
    OptionType type_{OptionType::Call};
    double spot_{0.0};
    double strike_{0.0};
    double maturity_{0.0};
    double rate_{0.0};
    double vol_{0.0};
    double dividend_{0.0};
    const market::VolSurface* vol_surface_{nullptr};
    const market::YieldCurve* curve_{nullptr};
};

} // namespace quant::instruments

