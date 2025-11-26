#pragma once

#include "quant/instruments/BarrierOption.hpp"
#include "quant/pricing/PricingEngine.hpp"

namespace quant::pricing {

class BarrierOptionEngine : public PricingEngine {
public:
    BarrierOptionEngine(std::size_t steps = 200) : steps_(steps) {}

    double price(const quant::instruments::Instrument& inst) const override;
    double price(const quant::instruments::BarrierOption& opt) const;

private:
    std::size_t steps_;
};

} // namespace quant::pricing

