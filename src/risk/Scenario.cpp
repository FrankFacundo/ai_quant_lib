#include "quant/risk/Scenario.hpp"
#include "quant/pricing/BlackScholes.hpp"
#include "quant/pricing/DiscountingSwap.hpp"
#include "quant/instruments/EuropeanOption.hpp"
#include "quant/instruments/VanillaSwap.hpp"

namespace quant::risk {

double ScenarioEngine::apply(const std::vector<std::shared_ptr<quant::instruments::Instrument>>& portfolio,
                             const ScenarioShock& shock) const {
    double base_value = 0.0;
    double shocked_value = 0.0;

    const auto* bs = dynamic_cast<const quant::pricing::BlackScholesEuropeanEngine*>(&engine_);
    const auto* swap_engine = dynamic_cast<const quant::pricing::DiscountingSwapEngine*>(&engine_);

    for (const auto& inst : portfolio) {
        if (bs) {
            auto* opt = dynamic_cast<quant::instruments::EuropeanOption*>(inst.get());
            if (opt) {
                base_value += bs->price(*opt);
                quant::instruments::EuropeanOption shocked(opt->option_type(), opt->spot() * (1.0 + shock.spot_shift),
                                                           opt->strike(), opt->maturity(),
                                                           opt->rate() + shock.rate_parallel_bp / 10000.0,
                                                           opt->volatility() * (1.0 + shock.vol_shift), opt->dividend());
                shocked_value += bs->price(shocked);
                continue;
            }
        }
        if (swap_engine) {
            auto* swap = dynamic_cast<quant::instruments::VanillaSwap*>(inst.get());
            if (swap && curve_) {
                base_value += swap_engine->price(*swap);
                auto times = curve_->times();
                auto rates = curve_->zero_rates();
                for (auto& r : rates) r += shock.rate_parallel_bp / 10000.0;
                quant::market::YieldCurve shocked_curve(times, rates);
                quant::instruments::VanillaSwap shocked_swap(swap->type(), swap->notional(), swap->fixed_rate(),
                                                             swap->fixed_schedule(), swap->float_schedule(),
                                                             swap->fixed_dcc(), swap->float_dcc(),
                                                             &shocked_curve, swap->float_spread());
                shocked_value += swap_engine->price(shocked_swap);
                continue;
            }
        }
        base_value += inst->npv();
        shocked_value += inst->npv();
    }
    return shocked_value - base_value;
}

} // namespace quant::risk
