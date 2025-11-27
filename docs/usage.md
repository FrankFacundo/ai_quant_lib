# Usage

## Build

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

If system packages for Eigen, GTest, or pybind11 are missing, enable bundled downloads:

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DQAI_USE_FETCH_EIGEN=ON -DQAI_USE_FETCH_GTEST=ON -DQAI_USE_FETCH_PYBIND=ON
```

## Tests

```
cd build
ctest
```

## Python bindings

Ensure Python dev headers are available. The bindings are built by default:

```
cmake -S . -B build -DBUILD_PYTHON=ON
cmake --build build
python -c "import sys; sys.path.append('build/python'); import pyquant; print(pyquant)"
PYTHONPATH="$(pwd)/build/python" python python/examples/recipes/funding_spread_recipe.py
```

## C++ usage

Link against target `quant::quantlib` and include headers from `include/quant`. Example:

```
#include "quant/pricing/BlackScholes.hpp"
...
quant::instruments::EuropeanOption opt(...);
quant::pricing::BlackScholesEuropeanEngine eng;
double pv = eng.price(opt);
```

## Python usage

See runnable scripts under `python/examples/`:

- `forecasting/forecast_demo.py`: ARIMA forecast
- `backtesting/backtest_demo.py`: moving-average strategy
- `dashboards/correlation_dashboard.py`: cross-asset correlation heatmap
- `recipes/volatility_recipe.py`, `yield_curve_recipe.py`, `funding_spread_recipe.py`
