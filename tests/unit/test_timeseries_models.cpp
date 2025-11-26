#include <gtest/gtest.h>
#include "quant/timeseries/Models.hpp"

using namespace quant::timeseries;

TEST(ARIMA, Forecast) {
    quant::core::TimeSeries<Eigen::VectorXd> ts;
    double phi = 0.7;
    double x = 1.0;
    for (int i = 0; i < 50; ++i) {
        Eigen::VectorXd v(1);
        v(0) = x;
        ts.push_back(quant::core::DateTime(2020,1,(i % 28) + 1), v);
        x = phi * x;
    }
    ARIMAModel model(1,0,0);
    model.fit(ts);
    auto f = model.forecast(1);
    EXPECT_NEAR(f(0), ts.values().back()(0) * phi, 0.1);
}

TEST(VAR, Basic) {
    quant::core::TimeSeries<Eigen::VectorXd> ts;
    for (int i = 0; i < 30; ++i) {
        Eigen::VectorXd v(2);
        v << i, 2*i;
        ts.push_back(quant::core::DateTime(2020,1,(i % 28) + 1), v);
    }
    VARModel var(1);
    var.fit(ts);
    auto f = var.forecast(1);
    EXPECT_EQ(f.size(), 2);
}
