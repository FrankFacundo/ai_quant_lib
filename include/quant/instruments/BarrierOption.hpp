#pragma once

#include "quant/instruments/Instrument.hpp"
#include "quant/instruments/EuropeanOption.hpp"

namespace quant::instruments {

enum class BarrierType { UpAndOut, DownAndOut, UpAndIn, DownAndIn };

class BarrierOption : public Instrument {
public:
    BarrierOption(BarrierType type, OptionType opt_type, double spot, double strike, double maturity,
                  double rate, double vol, double barrier, double rebate = 0.0);

    BarrierType barrier_type() const { return barrier_type_; }
    OptionType option_type() const { return opt_type_; }
    double spot() const { return spot_; }
    double strike() const { return strike_; }
    double maturity() const { return maturity_; }
    double rate() const { return rate_; }
    double volatility() const { return vol_; }
    double barrier() const { return barrier_; }
    double rebate() const { return rebate_; }

    double npv() const override; // priced via internal closed-form for vanilla fallback

private:
    BarrierType barrier_type_;
    OptionType opt_type_;
    double spot_;
    double strike_;
    double maturity_;
    double rate_;
    double vol_;
    double barrier_;
    double rebate_;
};

} // namespace quant::instruments

