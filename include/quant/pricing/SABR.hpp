#pragma once

#include "quant/instruments/EuropeanOption.hpp"
#include "quant/pricing/PricingEngine.hpp"

namespace quant::pricing {

class SABRModel {
public:
    SABRModel(double alpha, double beta, double rho, double nu)
        : alpha_(alpha), beta_(beta), rho_(rho), nu_(nu) {}

    double implied_vol(double forward, double strike, double maturity) const;

private:
    double alpha_;
    double beta_;
    double rho_;
    double nu_;
};

class SABREuropeanEngine : public PricingEngine {
public:
    SABREuropeanEngine(SABRModel model, double discount_rate)
        : model_(std::move(model)), discount_rate_(discount_rate) {}

    double price(const quant::instruments::Instrument& inst) const override;
    double price(const quant::instruments::EuropeanOption& opt) const;

private:
    SABRModel model_;
    double discount_rate_;
};

} // namespace quant::pricing

