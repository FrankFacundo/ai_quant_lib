#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include <map>
#include <memory>
#include <optional>

#include "quant/core/Date.hpp"
#include "quant/core/TimeSeries.hpp"
#include "quant/instruments/EuropeanOption.hpp"
#include "quant/instruments/BarrierOption.hpp"
#include "quant/instruments/VanillaSwap.hpp"
#include "quant/pricing/BlackScholes.hpp"
#include "quant/pricing/BarrierOption.hpp"
#include "quant/pricing/SABR.hpp"
#include "quant/backtest/Backtester.hpp"
#include "quant/utils/Volatility.hpp"
#include "quant/utils/Correlation.hpp"
#include "quant/utils/YieldTools.hpp"
#include "quant/timeseries/Models.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pyquant, m) {
    using namespace quant;

    py::class_<core::Date>(m, "Date")
        .def(py::init<int, unsigned, unsigned>())
        .def("to_string", &core::Date::to_string);

    py::class_<core::DateTime>(m, "DateTime")
        .def(py::init<int, unsigned, unsigned, unsigned, unsigned, unsigned>(),
             py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hh") = 0, py::arg("mm") = 0, py::arg("ss") = 0)
        .def("to_string", &core::DateTime::to_string);

    py::class_<core::TimeSeries<double>>(m, "TimeSeriesDouble")
        .def(py::init<>())
        .def("push_back", &core::TimeSeries<double>::push_back)
        .def("size", &core::TimeSeries<double>::size)
        .def("values", &core::TimeSeries<double>::values, py::return_value_policy::reference_internal);

    py::class_<instruments::EuropeanOption, instruments::Instrument>(m, "EuropeanOption")
        .def(py::init<instruments::OptionType, double, double, double, double, double, double>(),
             py::arg("type"), py::arg("spot"), py::arg("strike"), py::arg("maturity"), py::arg("rate"), py::arg("vol"), py::arg("dividend") = 0.0)
        .def("npv", &instruments::EuropeanOption::npv)
        .def_property_readonly("spot", &instruments::EuropeanOption::spot);

    py::enum_<instruments::OptionType>(m, "OptionType")
        .value("Call", instruments::OptionType::Call)
        .value("Put", instruments::OptionType::Put);

    py::class_<instruments::BarrierOption, instruments::Instrument>(m, "BarrierOption")
        .def(py::init<instruments::BarrierType, instruments::OptionType, double, double, double, double, double, double, double>())
        .def("npv", &instruments::BarrierOption::npv);

    py::enum_<instruments::BarrierType>(m, "BarrierType")
        .value("UpAndOut", instruments::BarrierType::UpAndOut)
        .value("DownAndOut", instruments::BarrierType::DownAndOut)
        .value("UpAndIn", instruments::BarrierType::UpAndIn)
        .value("DownAndIn", instruments::BarrierType::DownAndIn);

    py::class_<pricing::BlackScholesEuropeanEngine>(m, "BlackScholesEuropeanEngine")
        .def(py::init<const market::YieldCurve*>(), py::arg("curve") = nullptr)
        .def("price", py::overload_cast<const instruments::EuropeanOption&>(&pricing::BlackScholesEuropeanEngine::price, py::const_))
        .def("delta", &pricing::BlackScholesEuropeanEngine::delta)
        .def("gamma", &pricing::BlackScholesEuropeanEngine::gamma)
        .def("vega", &pricing::BlackScholesEuropeanEngine::vega)
        .def("theta", &pricing::BlackScholesEuropeanEngine::theta)
        .def("rho", &pricing::BlackScholesEuropeanEngine::rho);

    py::class_<pricing::BarrierOptionEngine>(m, "BarrierOptionEngine")
        .def(py::init<std::size_t>(), py::arg("steps") = 200)
        .def("price", py::overload_cast<const instruments::BarrierOption&>(&pricing::BarrierOptionEngine::price, py::const_));

    py::class_<market::YieldCurve>(m, "YieldCurve")
        .def(py::init<std::vector<double>, std::vector<double>>())
        .def("discount", &market::YieldCurve::discount)
        .def("zero_rate", &market::YieldCurve::zero_rate);

    py::class_<pricing::SABRModel>(m, "SABRModel")
        .def(py::init<double, double, double, double>())
        .def("implied_vol", &pricing::SABRModel::implied_vol);

    py::class_<pricing::SABREuropeanEngine>(m, "SABREuropeanEngine")
        .def(py::init<pricing::SABRModel, double>())
        .def("price", py::overload_cast<const instruments::EuropeanOption&>(&pricing::SABREuropeanEngine::price, py::const_));

    py::class_<timeseries::ARIMAModel>(m, "ARIMAModel")
        .def(py::init<int, int, int>())
        .def("fit", &timeseries::ARIMAModel::fit)
        .def("forecast", &timeseries::ARIMAModel::forecast);

    py::class_<timeseries::VARModel>(m, "VARModel")
        .def(py::init<int>())
        .def("fit", &timeseries::VARModel::fit)
        .def("forecast", &timeseries::VARModel::forecast);

    py::class_<timeseries::GARCHModel>(m, "GARCHModel")
        .def(py::init<double, double, double>(), py::arg("omega") = 0.01, py::arg("alpha") = 0.05, py::arg("beta") = 0.9)
        .def("fit", &timeseries::GARCHModel::fit)
        .def("forecast", &timeseries::GARCHModel::forecast);

    py::class_<timeseries::RandomForestRegressor>(m, "RandomForestRegressor")
        .def(py::init<int, int, double>())
        .def("fit", &timeseries::RandomForestRegressor::fit)
        .def("forecast", &timeseries::RandomForestRegressor::forecast);

    py::class_<timeseries::FeedForwardNN>(m, "FeedForwardNN")
        .def(py::init<std::vector<int>, double, int>())
        .def("fit", &timeseries::FeedForwardNN::fit)
        .def("forecast", &timeseries::FeedForwardNN::forecast);

    py::class_<timeseries::FXTimeSeriesModel>(m, "FXTimeSeriesModel")
        .def(py::init<>())
        .def("fit_returns", &timeseries::FXTimeSeriesModel::fit_returns)
        .def("forecast_vol", &timeseries::FXTimeSeriesModel::forecast_vol);

    py::class_<timeseries::EquityTimeSeriesModel>(m, "EquityTimeSeriesModel")
        .def(py::init<>())
        .def("fit_prices", &timeseries::EquityTimeSeriesModel::fit_prices)
        .def("forecast_prices", &timeseries::EquityTimeSeriesModel::forecast_prices);

    py::class_<timeseries::EnergyTimeSeriesModel>(m, "EnergyTimeSeriesModel")
        .def(py::init<>())
        .def("fit_series", &timeseries::EnergyTimeSeriesModel::fit_series)
        .def("forecast_prices", &timeseries::EnergyTimeSeriesModel::forecast_prices);

    py::class_<timeseries::CreditTimeSeriesModel>(m, "CreditTimeSeriesModel")
        .def(py::init<>())
        .def("fit_spreads", &timeseries::CreditTimeSeriesModel::fit_spreads)
        .def("forecast_spreads", &timeseries::CreditTimeSeriesModel::forecast_spreads);

    auto utils_mod = m.def_submodule("utils");
    utils_mod.def("correlation_matrix", &utils::correlation_matrix);
    utils_mod.def("rolling_correlation", &utils::rolling_correlation);
    py::class_<utils::MacroDashboard>(utils_mod, "MacroDashboard")
        .def(py::init<>())
        .def("add_series", &utils::MacroDashboard::add_series)
        .def("correlations", &utils::MacroDashboard::correlations)
        .def("beta", &utils::MacroDashboard::beta);

    auto bt = m.def_submodule("backtest");
    py::class_<backtest::Bar>(bt, "Bar")
        .def(py::init<core::DateTime, double, double, double, double, double>(),
             py::arg("time"), py::arg("open"), py::arg("high"), py::arg("low"), py::arg("close"), py::arg("volume"));

    py::class_<backtest::Trade>(bt, "Trade")
        .def_readonly("entry_time", &backtest::Trade::entry_time)
        .def_readonly("exit_time", &backtest::Trade::exit_time)
        .def_readonly("entry_price", &backtest::Trade::entry_price)
        .def_readonly("exit_price", &backtest::Trade::exit_price)
        .def_readonly("pnl", &backtest::Trade::pnl);

    py::class_<core::TimeSeries<backtest::Bar>>(bt, "BarSeries")
        .def(py::init<>())
        .def("push_back", &core::TimeSeries<backtest::Bar>::push_back)
        .def("values", &core::TimeSeries<backtest::Bar>::values, py::return_value_policy::reference_internal)
        .def("size", &core::TimeSeries<backtest::Bar>::size);

    py::class_<backtest::Portfolio>(bt, "Portfolio")
        .def(py::init<double>(), py::arg("cash") = 0.0)
        .def("update_position", &backtest::Portfolio::update_position, py::arg("asset"), py::arg("quantity"), py::arg("price"), py::arg("time") = std::nullopt)
        .def("market_value", &backtest::Portfolio::market_value);

    py::class_<backtest::BacktestStats>(bt, "BacktestStats")
        .def_readonly("cumulative_return", &backtest::BacktestStats::cumulative_return)
        .def_readonly("annualized_vol", &backtest::BacktestStats::annualized_vol)
        .def_readonly("sharpe", &backtest::BacktestStats::sharpe)
        .def_readonly("max_drawdown", &backtest::BacktestStats::max_drawdown);

    py::class_<backtest::BacktestResult>(bt, "BacktestResult")
        .def_readonly("equity_curve", &backtest::BacktestResult::equity_curve)
        .def_readonly("stats", &backtest::BacktestResult::stats)
        .def_readonly("trades", &backtest::BacktestResult::trades);

    py::class_<backtest::Strategy, std::shared_ptr<backtest::Strategy>>(bt, "Strategy");
    py::class_<backtest::MovingAverageCrossStrategy, backtest::Strategy>(bt, "MovingAverageCrossStrategy")
        .def(py::init<std::size_t, std::size_t, double, double, double>(),
             py::arg("short_window"), py::arg("long_window"), py::arg("qty"), py::arg("transaction_cost") = 0.0, py::arg("slippage") = 0.0);

    py::class_<backtest::Backtester>(bt, "Backtester")
        .def(py::init<std::map<std::string, core::TimeSeries<backtest::Bar>>, std::shared_ptr<backtest::Strategy>, backtest::Portfolio>())
        .def("run", &backtest::Backtester::run);

    m.def("realized_vol", &utils::realized_vol);
    m.def("realized_vol_series", &utils::realized_vol_series);
    m.def("implied_realized_spread", &utils::implied_realized_spread);
    m.def("spot_rates", &utils::spot_rates);
    m.def("forward_rates", &utils::forward_rates);
    m.def("funding_spread", &utils::funding_spread);
}
