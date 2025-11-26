#include "quant/core/TimeSeries.hpp"
#include "quant/backtest/Backtester.hpp"
#include <functional>

namespace quant::core {

template class TimeSeries<double>;
template class TimeSeries<quant::backtest::Bar>;

template TimeSeries<double> TimeSeries<double>::rolling<std::function<double(std::span<const double>)>>(std::size_t, std::function<double(std::span<const double>)>) const;

template TimeSeries<double> TimeSeries<double>::resample<std::function<double(const std::vector<double>&)>>(const std::vector<DateTime>&, std::function<double(const std::vector<double>&)>) const;

} // namespace quant::core
