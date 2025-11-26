#include <gtest/gtest.h>
#include "quant/utils/Volatility.hpp"
#include "quant/utils/Correlation.hpp"

using namespace quant::utils;

TEST(Utils, RealizedVol) {
    quant::core::TimeSeries<double> ts;
    for (int i = 0; i < 30; ++i) ts.push_back(quant::core::DateTime(2020,1,(i % 28) + 1), 0.01);
    double rv = realized_vol(ts, 10);
    EXPECT_GT(rv, 0.0);
}

TEST(Utils, CorrelationMatrix) {
    quant::core::TimeSeries<double> a, b;
    for (int i = 0; i < 10; ++i) {
        a.push_back(quant::core::DateTime(2020,1,(i % 28) + 1), i);
        b.push_back(quant::core::DateTime(2020,1,(i % 28) + 1), i*2);
    }
    std::map<std::string, quant::core::TimeSeries<double>> series{{"a", a}, {"b", b}};
    auto corr = correlation_matrix(series);
    EXPECT_EQ(corr.rows(), 2);
    EXPECT_EQ(corr.cols(), 2);
}
