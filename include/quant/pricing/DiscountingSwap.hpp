#pragma once

#include "quant/instruments/VanillaSwap.hpp"
#include "quant/market/YieldCurve.hpp"
#include "quant/pricing/PricingEngine.hpp"

namespace quant::pricing {

class DiscountingSwapEngine : public PricingEngine {
public:
    double price(const quant::instruments::Instrument& inst) const override;
    double price(const quant::instruments::VanillaSwap& swap) const;
};

} // namespace quant::pricing

