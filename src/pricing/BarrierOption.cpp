#include "quant/pricing/BarrierOption.hpp"
#include "quant/pricing/BlackScholes.hpp"
#include "quant/core/Exceptions.hpp"

#include <cmath>

namespace quant::pricing {

namespace {
inline bool hit_barrier(double price, double barrier, quant::instruments::BarrierType type) {
    if (type == quant::instruments::BarrierType::UpAndOut || type == quant::instruments::BarrierType::UpAndIn) {
        return price >= barrier;
    }
    return price <= barrier;
}
}

double BarrierOptionEngine::price(const quant::instruments::Instrument& inst) const {
    const auto* opt = dynamic_cast<const quant::instruments::BarrierOption*>(&inst);
    if (!opt) throw quant::core::PricingError("Instrument is not BarrierOption");
    return price(*opt);
}

double BarrierOptionEngine::price(const quant::instruments::BarrierOption& opt) const {
    using quant::instruments::BarrierType;
    // Knock-in via parity with knock-out and vanilla
    if (opt.barrier_type() == BarrierType::UpAndIn || opt.barrier_type() == BarrierType::DownAndIn) {
        quant::instruments::BarrierType ko =
            opt.barrier_type() == BarrierType::UpAndIn ? BarrierType::UpAndOut : BarrierType::DownAndOut;
        quant::instruments::BarrierOption ko_opt(ko, opt.option_type(), opt.spot(), opt.strike(), opt.maturity(),
                                                 opt.rate(), opt.volatility(), opt.barrier(), opt.rebate());
        BarrierOptionEngine ko_engine(steps_);
        double vanilla = BlackScholesEuropeanEngine().price(quant::instruments::EuropeanOption(
            opt.option_type(), opt.spot(), opt.strike(), opt.maturity(), opt.rate(), opt.volatility()));
        return vanilla - ko_engine.price(ko_opt);
    }

    std::size_t steps = steps_;
    double dt = opt.maturity() / static_cast<double>(steps);
    double u = std::exp(opt.volatility() * std::sqrt(dt));
    double d = 1.0 / u;
    double disc = std::exp(-opt.rate() * dt);
    double p = (std::exp(opt.rate() * dt) - d) / (u - d);

    std::vector<double> values(steps + 1);
    for (std::size_t i = 0; i <= steps; ++i) {
        double s = opt.spot() * std::pow(u, static_cast<double>(steps - i)) * std::pow(d, static_cast<double>(i));
        double payoff = 0.0;
        if (!hit_barrier(s, opt.barrier(), opt.barrier_type())) {
            if (opt.option_type() == quant::instruments::OptionType::Call) {
                payoff = std::max(s - opt.strike(), 0.0);
            } else {
                payoff = std::max(opt.strike() - s, 0.0);
            }
        } else {
            payoff = opt.rebate();
        }
        values[i] = payoff;
    }

    for (int step = static_cast<int>(steps) - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            double s = opt.spot() * std::pow(u, static_cast<double>(step - i)) * std::pow(d, static_cast<double>(i));
            double cont = disc * (p * values[i] + (1.0 - p) * values[i + 1]);
            if (hit_barrier(s, opt.barrier(), opt.barrier_type())) {
                values[i] = opt.rebate();
            } else {
                values[i] = cont;
            }
        }
    }
    return values.front();
}

} // namespace quant::pricing

