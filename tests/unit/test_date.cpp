#include <gtest/gtest.h>
#include "quant/core/Date.hpp"

using namespace quant::core;

TEST(DateTest, YearFraction) {
    Date d1(2023, 1, 1);
    Date d2(2023, 1, 31);
    double yf = year_fraction(d1, d2, DayCountConvention::ACT_365);
    EXPECT_NEAR(yf, 30.0 / 365.0, 1e-6);
}

TEST(DateTest, CalendarAdjust) {
    Calendar cal;
    Date sat(2023, 4, 1); // Saturday
    Date adjusted = cal.adjust(sat);
    EXPECT_NE(adjusted.day(), sat.day());
}

