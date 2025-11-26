#include <gtest/gtest.h>
#include "quant/instruments/VanillaSwap.hpp"
#include "quant/market/YieldCurve.hpp"
#include "quant/pricing/DiscountingSwap.hpp"

using namespace quant::instruments;
using namespace quant::market;
using namespace quant::pricing;
using quant::core::Date;
using quant::core::DayCountConvention;

TEST(Swap, ParRate) {
    std::vector<double> times{0.5, 1.0, 2.0, 5.0};
    std::vector<double> rates(times.size(), 0.02);
    YieldCurve curve(times, rates);
    Schedule sched{Date(2023,1,1), Date(2024,1,1), Frequency::Annual};
    VanillaSwap swap(SwapType::Payer, 1e6, 0.02, sched, sched, DayCountConvention::ACT_365, DayCountConvention::ACT_365, &curve, 0.0);
    DiscountingSwapEngine engine;
    double pv = engine.price(swap);
    EXPECT_NEAR(pv, 0.0, 1e-2);
}

