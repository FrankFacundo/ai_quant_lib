# API Overview

- `quant::core`
  - `Date`, `DateTime`, `Calendar`, `DayCountConvention`
  - `TimeSeries<T>` with lag/diff/rolling/resample helpers
  - `Matrix`, `Vector` aliases (Eigen)
- `quant::instruments`
  - `Instrument` base
  - `EuropeanOption`, `BarrierOption`, `VanillaSwap`
- `quant::market`
  - `YieldCurve` (discount/zero/forward), `VolSurface`
- `quant::pricing`
  - `PricingEngine` interface
  - `BlackScholesEuropeanEngine` (+Greeks)
  - `DiscountingSwapEngine`
  - `BarrierOptionEngine` (binomial)
  - `SABRModel`, `SABREuropeanEngine`
- `quant::risk`
  - Analytic Greeks helpers
  - `ScenarioEngine` for shocks/PnL
- `quant::timeseries`
  - Models: `ARIMAModel`, `VARModel`, `GARCHModel`, `RandomForestRegressor`, `FeedForwardNN`
  - Domain wrappers: `FXTimeSeriesModel`, `EquityTimeSeriesModel`, `EnergyTimeSeriesModel`, `CreditTimeSeriesModel`
- `quant::backtest`
  - `Bar`, `Portfolio`, `Strategy`, `MovingAverageCrossStrategy`, `Backtester`, `BacktestResult`
- `quant::utils`
  - Correlation/macro dashboard
  - Volatility tracker and implied-realized spread
  - Yield-curve spot/forward and funding spreads

