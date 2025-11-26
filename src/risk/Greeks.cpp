#include "quant/risk/Greeks.hpp"

#include <cmath>

namespace quant::risk {

namespace {
inline double norm_cdf(double x) { return 0.5 * std::erfc(-x / std::sqrt(2.0)); }
constexpr double INV_SQRT_2PI = 0.39894228040143267794;
inline double norm_pdf(double x) { return std::exp(-0.5 * x * x) * INV_SQRT_2PI; }

inline void bs_params(const quant::instruments::EuropeanOption& opt, double& d1, double& d2, double& df, double& qf) {
    double r = opt.rate();
    double vol = opt.volatility();
    double q = opt.dividend();
    double T = opt.maturity();
    d1 = (std::log(opt.spot() / opt.strike()) + (r - q + 0.5 * vol * vol) * T) / (vol * std::sqrt(T));
    d2 = d1 - vol * std::sqrt(T);
    df = std::exp(-r * T);
    qf = std::exp(-q * T);
}
}

double black_scholes_price(const quant::instruments::EuropeanOption& opt) {
    double d1, d2, df, qf;
    bs_params(opt, d1, d2, df, qf);
    if (opt.option_type() == quant::instruments::OptionType::Call) {
        return opt.spot() * qf * norm_cdf(d1) - opt.strike() * df * norm_cdf(d2);
    }
    return opt.strike() * df * norm_cdf(-d2) - opt.spot() * qf * norm_cdf(-d1);
}

double black_scholes_delta(const quant::instruments::EuropeanOption& opt) {
    double d1, d2, df, qf;
    bs_params(opt, d1, d2, df, qf);
    return opt.option_type() == quant::instruments::OptionType::Call ? qf * norm_cdf(d1) : -qf * norm_cdf(-d1);
}

double black_scholes_gamma(const quant::instruments::EuropeanOption& opt) {
    double d1, d2, df, qf;
    bs_params(opt, d1, d2, df, qf);
    return qf * norm_pdf(d1) / (opt.spot() * opt.volatility() * std::sqrt(opt.maturity()));
}

double black_scholes_vega(const quant::instruments::EuropeanOption& opt) {
    double d1, d2, df, qf;
    bs_params(opt, d1, d2, df, qf);
    return opt.spot() * qf * norm_pdf(d1) * std::sqrt(opt.maturity());
}

double black_scholes_theta(const quant::instruments::EuropeanOption& opt) {
    double d1, d2, df, qf;
    bs_params(opt, d1, d2, df, qf);
    double term = -(opt.spot() * qf * norm_pdf(d1) * opt.volatility()) / (2.0 * std::sqrt(opt.maturity()));
    if (opt.option_type() == quant::instruments::OptionType::Call) {
        return term - opt.rate() * opt.strike() * df * norm_cdf(d2) + opt.dividend() * opt.spot() * qf * norm_cdf(d1);
    }
    return term + opt.rate() * opt.strike() * df * norm_cdf(-d2) - opt.dividend() * opt.spot() * qf * norm_cdf(-d1);
}

double black_scholes_rho(const quant::instruments::EuropeanOption& opt) {
    double d1, d2, df, qf;
    bs_params(opt, d1, d2, df, qf);
    if (opt.option_type() == quant::instruments::OptionType::Call) return opt.maturity() * opt.strike() * df * norm_cdf(d2);
    return -opt.maturity() * opt.strike() * df * norm_cdf(-d2);
}

} // namespace quant::risk
