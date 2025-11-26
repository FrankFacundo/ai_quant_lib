# Design Overview

The library is organized into modular namespaces (`quant::core`, `instruments`, `pricing`, `market`, `timeseries`, `backtest`, `risk`, `utils`) and built as a single CMake target `quantlib`. External dependencies (Eigen, GTest, pybind11) are resolved via `find_package` with optional `FetchContent` fallbacks in `cmake/Dependencies.cmake`.

**Core** contains low-level date/time utilities, day count conventions, calendars, linear algebra helpers, and a generic `TimeSeries<T>` container with lag/diff/rolling/resampling.

**Market/Instruments** provide market containers (`YieldCurve`, `VolSurface`, spots) and instrument abstractions (`Instrument`, `EuropeanOption`, `BarrierOption`, `VanillaSwap`).

**Pricing** hosts pluggable engines implementing the `PricingEngine` interface: Black–Scholes for European options (with Greeks), a discounting swap engine, a binomial barrier engine, and a SABR implied-vol-based pricer.

**Risk** implements analytic Greeks and a simple scenario engine that shocks rates/vols/spots and re-prices portfolios.

**Time series/ML** provide ARIMA, VAR, GARCH, RandomForestRegressor, and a feed-forward NN through a common `TimeSeriesModel` interface plus domain wrappers (FX, Equity, Energy, Credit).

**Backtesting** offers bars, a moving-average strategy, portfolio bookkeeping, performance statistics, and a driver to run strategies over multi-asset `TimeSeries` inputs.

**Utils** deliver correlation/macro dashboard analytics, volatility tracking, and yield-curve helpers (spot/forward, funding spreads).

Python bindings (`pybind11`) expose the major building blocks for interactive research, with examples in `python/examples` showing forecasting, backtesting, dashboards, and recipes.

