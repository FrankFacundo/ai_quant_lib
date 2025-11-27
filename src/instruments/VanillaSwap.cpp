#include "quant/instruments/VanillaSwap.hpp"
#include "quant/market/YieldCurve.hpp"

#include <cmath>

using quant::core::Date;
using quant::core::DayCountConvention;
using quant::core::year_fraction;

namespace quant::instruments {

VanillaSwap::VanillaSwap(SwapType type,
                         double notional,
                         double fixed_rate,
                         const Schedule& fixed_schedule,
                         const Schedule& float_schedule,
                         DayCountConvention fixed_dcc,
                         DayCountConvention float_dcc,
                         const market::YieldCurve* discount_curve,
                         double float_spread)
    : type_(type), notional_(notional), fixed_rate_(fixed_rate), fixed_schedule_(fixed_schedule),
      float_schedule_(float_schedule), fixed_dcc_(fixed_dcc), float_dcc_(float_dcc),
      discount_curve_(discount_curve), float_spread_(float_spread) {}

std::vector<Date> VanillaSwap::build_dates(const Schedule& schedule) const {
    std::vector<Date> dates;
    auto start = schedule.start;
    auto end = schedule.end;
    dates.push_back(start);
    int months = 12 / static_cast<int>(schedule.frequency);
    Date current = start;
    while (current < end) {
        auto add_months = [](const Date& d, int m_add) {
            auto is_leap = [](int y) { return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); };
            int y = d.year();
            int m = static_cast<int>(d.month());
            int total = m + m_add;
            y += (total - 1) / 12;
            m = ((total - 1) % 12) + 1;
            unsigned day = d.day();
            static const unsigned dim[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
            unsigned max_day = (m == 2 && is_leap(y)) ? 29 : dim[m - 1];
            if (day > max_day) day = max_day;
            return Date(y, static_cast<unsigned>(m), day);
        };
        auto next = add_months(current, months);
        if (next > end) next = end;
        dates.push_back(next);
        current = next;
    }
    if (dates.back() != end) dates.push_back(end);
    return dates;
}

double VanillaSwap::npv() const {
    auto fixed_dates = build_dates(fixed_schedule_);
    auto float_dates = build_dates(float_schedule_);
    double fixed_leg = 0.0;
    for (std::size_t i = 1; i < fixed_dates.size(); ++i) {
        double tau = year_fraction(fixed_dates[i - 1], fixed_dates[i], fixed_dcc_);
        double df = discount_curve_->discount(year_fraction(fixed_schedule_.start, fixed_dates[i], fixed_dcc_));
        fixed_leg += notional_ * fixed_rate_ * tau * df;
    }
    double float_leg = 0.0;
    for (std::size_t i = 1; i < float_dates.size(); ++i) {
        double t1 = year_fraction(float_schedule_.start, float_dates[i - 1], float_dcc_);
        double t2 = year_fraction(float_schedule_.start, float_dates[i], float_dcc_);
        double forward = discount_curve_->forward_rate(t1, t2);
        double tau = t2 - t1;
        double df = discount_curve_->discount(t2);
        float_leg += notional_ * (forward + float_spread_) * tau * df;
    }
    double sign = (type_ == SwapType::Payer) ? 1.0 : -1.0;
    return sign * (fixed_leg - float_leg);
}

double VanillaSwap::fair_rate() const {
    auto fixed_dates = build_dates(fixed_schedule_);
    auto float_dates = build_dates(float_schedule_);
    double annuity = 0.0;
    for (std::size_t i = 1; i < fixed_dates.size(); ++i) {
        double tau = year_fraction(fixed_dates[i - 1], fixed_dates[i], fixed_dcc_);
        double t = year_fraction(fixed_schedule_.start, fixed_dates[i], fixed_dcc_);
        annuity += tau * discount_curve_->discount(t);
    }
    double float_leg = 0.0;
    for (std::size_t i = 1; i < float_dates.size(); ++i) {
        double t1 = year_fraction(float_schedule_.start, float_dates[i - 1], float_dcc_);
        double t2 = year_fraction(float_schedule_.start, float_dates[i], float_dcc_);
        double forward = discount_curve_->forward_rate(t1, t2);
        double tau = t2 - t1;
        double df = discount_curve_->discount(t2);
        float_leg += forward * tau * df;
    }
    if (annuity == 0.0) throw quant::core::QuantError("Zero annuity in fair rate computation");
    return (float_leg / annuity) - float_spread_;
}

} // namespace quant::instruments
