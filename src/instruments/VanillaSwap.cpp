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
        auto ymd = current.to_chrono();
        auto next = quant::core::Date(static_cast<int>((ymd.year() + std::chrono::months{months}).year()),
                                      static_cast<unsigned>((ymd.month() + std::chrono::months{months}).month()),
                                      current.day());
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

