#include <gtest/gtest.h>
#include "quant/backtest/Backtester.hpp"

using namespace quant::backtest;

TEST(Backtest, Run) {
    std::map<std::string, quant::core::TimeSeries<Bar>> data;
    quant::core::TimeSeries<Bar> series;
    for (int i = 0; i < 50; ++i) {
        double price = 100.0 + i;
        Bar b{quant::core::DateTime(2020,1,(i % 28) + 1), price, price, price, price, 0.0};
        series.push_back(b.time, b);
    }
    data["EQ"] = series;
    auto strat = std::make_shared<MovingAverageCrossStrategy>(3, 5, 1);
    Portfolio pf(0.0);
    Backtester bt(data, strat, pf);
    auto res = bt.run();
    EXPECT_GT(res.equity_curve.size(), 0);
}
