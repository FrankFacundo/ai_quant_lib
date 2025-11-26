#pragma once

#include "quant/core/Date.hpp"
#include "quant/core/Exceptions.hpp"
#include "quant/instruments/Instrument.hpp"

#include <vector>

namespace quant::market { class YieldCurve; }

namespace quant::instruments {

enum class SwapType { Payer, Receiver };

enum class Frequency { Annual = 1, SemiAnnual = 2, Quarterly = 4 };

struct Schedule {
    quant::core::Date start;
    quant::core::Date end;
    Frequency frequency{Frequency::Annual};
};

class VanillaSwap : public Instrument {
public:
    VanillaSwap(SwapType type,
                double notional,
                double fixed_rate,
                const Schedule& fixed_schedule,
                const Schedule& float_schedule,
                quant::core::DayCountConvention fixed_dcc,
                quant::core::DayCountConvention float_dcc,
                const market::YieldCurve* discount_curve,
                double float_spread = 0.0);

    double npv() const override;
    double fair_rate() const;

    const market::YieldCurve* discount_curve() const { return discount_curve_; }
    double notional() const { return notional_; }
    double fixed_rate() const { return fixed_rate_; }
    SwapType type() const { return type_; }
    const Schedule& fixed_schedule() const { return fixed_schedule_; }
    const Schedule& float_schedule() const { return float_schedule_; }
    quant::core::DayCountConvention fixed_dcc() const { return fixed_dcc_; }
    quant::core::DayCountConvention float_dcc() const { return float_dcc_; }
    double float_spread() const { return float_spread_; }

private:
    std::vector<quant::core::Date> build_dates(const Schedule& schedule) const;

    SwapType type_;
    double notional_;
    double fixed_rate_;
    Schedule fixed_schedule_;
    Schedule float_schedule_;
    quant::core::DayCountConvention fixed_dcc_;
    quant::core::DayCountConvention float_dcc_;
    const market::YieldCurve* discount_curve_;
    double float_spread_;
};

} // namespace quant::instruments
