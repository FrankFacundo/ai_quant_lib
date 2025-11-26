#pragma once

#include "quant/instruments/EuropeanOption.hpp"
#include "quant/market/YieldCurve.hpp"
#include "quant/pricing/PricingEngine.hpp"

namespace quant::pricing {

class BlackScholesEuropeanEngine : public PricingEngine {
public:
    BlackScholesEuropeanEngine(const quant::market::YieldCurve* curve = nullptr) : curve_(curve) {}
    double price(const quant::instruments::Instrument& inst) const override;

    double price(const quant::instruments::EuropeanOption& opt) const;
    double delta(const quant::instruments::EuropeanOption& opt) const;
    double gamma(const quant::instruments::EuropeanOption& opt) const;
    double vega(const quant::instruments::EuropeanOption& opt) const;
    double theta(const quant::instruments::EuropeanOption& opt) const;
    double rho(const quant::instruments::EuropeanOption& opt) const;

private:
    const quant::market::YieldCurve* curve_;
};

} // namespace quant::pricing

