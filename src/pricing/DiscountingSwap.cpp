#include "quant/pricing/DiscountingSwap.hpp"
#include "quant/core/Exceptions.hpp"

namespace quant::pricing {

double DiscountingSwapEngine::price(const quant::instruments::Instrument& inst) const {
    const auto* swap = dynamic_cast<const quant::instruments::VanillaSwap*>(&inst);
    if (!swap) throw quant::core::PricingError("Instrument is not VanillaSwap");
    return price(*swap);
}

double DiscountingSwapEngine::price(const quant::instruments::VanillaSwap& swap) const {
    return swap.npv();
}

} // namespace quant::pricing

