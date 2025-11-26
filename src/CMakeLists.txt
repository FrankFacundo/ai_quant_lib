set(QUANTLIB_SOURCES
  core/Date.cpp
  core/TimeSeries.cpp
  core/Exceptions.cpp
  instruments/EuropeanOption.cpp
  instruments/BarrierOption.cpp
  instruments/VanillaSwap.cpp
  pricing/BlackScholes.cpp
  pricing/DiscountingSwap.cpp
  pricing/BarrierOption.cpp
  pricing/SABR.cpp
  market/YieldCurve.cpp
  market/VolSurface.cpp
  risk/Greeks.cpp
  risk/Scenario.cpp
  backtest/Backtester.cpp
  timeseries/ARIMA.cpp
  timeseries/VAR.cpp
  timeseries/GARCH.cpp
  timeseries/MLModels.cpp
  timeseries/DomainModels.cpp
  utils/Correlation.cpp
  utils/VolTracker.cpp
  utils/YieldTools.cpp
)

add_library(quantlib ${QUANTLIB_SOURCES})
add_library(quant::quantlib ALIAS quantlib)

find_package(Eigen3 REQUIRED)

target_include_directories(quantlib PUBLIC
  ${PROJECT_SOURCE_DIR}/include
  ${EIGEN3_INCLUDE_DIR}
)

target_link_libraries(quantlib PUBLIC Eigen3::Eigen)

target_compile_features(quantlib PUBLIC cxx_std_20)
