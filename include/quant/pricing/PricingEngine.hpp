#pragma once

#include "quant/instruments/Instrument.hpp"

namespace quant::pricing {

class PricingEngine {
public:
    virtual ~PricingEngine() = default;
    virtual double price(const quant::instruments::Instrument& inst) const = 0;
};

} // namespace quant::pricing

