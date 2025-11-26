#pragma once

#include "quant/instruments/EuropeanOption.hpp"

namespace quant::risk {

double black_scholes_price(const quant::instruments::EuropeanOption& opt);
double black_scholes_delta(const quant::instruments::EuropeanOption& opt);
double black_scholes_gamma(const quant::instruments::EuropeanOption& opt);
double black_scholes_vega(const quant::instruments::EuropeanOption& opt);
double black_scholes_theta(const quant::instruments::EuropeanOption& opt);
double black_scholes_rho(const quant::instruments::EuropeanOption& opt);

} // namespace quant::risk

