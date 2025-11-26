#include "quant/timeseries/Models.hpp"

namespace quant::timeseries {

FXTimeSeriesModel::FXTimeSeriesModel() : garch_(0.0001, 0.05, 0.9) {}

void FXTimeSeriesModel::fit_returns(const quant::core::TimeSeries<Eigen::VectorXd>& returns) {
    garch_.fit(returns);
}

Eigen::VectorXd FXTimeSeriesModel::forecast_vol(std::size_t horizon) const {
    return garch_.forecast(horizon);
}

EquityTimeSeriesModel::EquityTimeSeriesModel() : arima_(2, 1, 0) {}

void EquityTimeSeriesModel::fit_prices(const quant::core::TimeSeries<Eigen::VectorXd>& prices) {
    arima_.fit(prices);
}

Eigen::VectorXd EquityTimeSeriesModel::forecast_prices(std::size_t horizon) const {
    return arima_.forecast(horizon);
}

EnergyTimeSeriesModel::EnergyTimeSeriesModel() : var_(1) {}

void EnergyTimeSeriesModel::fit_series(const quant::core::TimeSeries<Eigen::VectorXd>& prices) {
    var_.fit(prices);
}

Eigen::VectorXd EnergyTimeSeriesModel::forecast_prices(std::size_t horizon) const {
    return var_.forecast(horizon);
}

CreditTimeSeriesModel::CreditTimeSeriesModel() : rf_(20, 3, 0.8) {}

void CreditTimeSeriesModel::fit_spreads(const quant::core::TimeSeries<Eigen::VectorXd>& spreads) {
    rf_.fit(spreads);
}

Eigen::VectorXd CreditTimeSeriesModel::forecast_spreads(std::size_t horizon) const {
    return rf_.forecast(horizon);
}

} // namespace quant::timeseries

