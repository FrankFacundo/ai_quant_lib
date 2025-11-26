#pragma once

#include "quant/instruments/Instrument.hpp"
#include "quant/pricing/PricingEngine.hpp"
#include "quant/market/YieldCurve.hpp"
#include "quant/market/VolSurface.hpp"

#include <memory>
#include <vector>

namespace quant::risk {

struct ScenarioShock {
    double rate_parallel_bp{0.0};
    double vol_shift{0.0};
    double spot_shift{0.0};
};

class ScenarioEngine {
public:
    ScenarioEngine(const quant::pricing::PricingEngine& engine,
                   quant::market::YieldCurve* curve,
                   quant::market::VolSurface* surface)
        : engine_(engine), curve_(curve), surface_(surface) {}

    double apply(const std::vector<std::shared_ptr<quant::instruments::Instrument>>& portfolio,
                 const ScenarioShock& shock) const;

private:
    const quant::pricing::PricingEngine& engine_;
    quant::market::YieldCurve* curve_;
    quant::market::VolSurface* surface_;
};

} // namespace quant::risk

