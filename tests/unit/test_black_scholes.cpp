#include <gtest/gtest.h>
#include "quant/pricing/BlackScholes.hpp"
#include "quant/instruments/EuropeanOption.hpp"

using namespace quant::instruments;
using namespace quant::pricing;

TEST(BlackScholes, PriceAndGreeks) {
    EuropeanOption opt(OptionType::Call, 100.0, 100.0, 1.0, 0.05, 0.2, 0.0);
    BlackScholesEuropeanEngine engine;
    double price = engine.price(opt);
    EXPECT_NEAR(price, 10.4506, 1e-3);
    EXPECT_NEAR(engine.delta(opt), 0.6368, 1e-3);
    EXPECT_NEAR(engine.gamma(opt), 0.0188, 1e-4);
    EXPECT_NEAR(engine.vega(opt) / 100.0, 0.375, 5e-2);
}

TEST(AdvancedPricers, BarrierAndSABR) {
    using namespace quant::instruments;
    BarrierOption barrier(BarrierType::UpAndOut, OptionType::Call, 100.0, 100.0, 1.0, 0.01, 0.2, 130.0, 0.0);
    quant::pricing::BarrierOptionEngine barrier_engine(100);
    double price_ko = barrier_engine.price(barrier);
    EXPECT_GT(price_ko, 0.0);

    quant::pricing::SABRModel model(0.3, 0.5, -0.2, 0.4);
    quant::pricing::SABREuropeanEngine sabr(model, 0.01);
    EuropeanOption opt(OptionType::Call, 100.0, 90.0, 1.0, 0.01, 0.0, 0.0);
    double sabr_price = sabr.price(opt);
    EXPECT_GT(sabr_price, 0.0);
}
