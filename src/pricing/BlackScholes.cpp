#include "quant/pricing/BlackScholes.hpp"
#include "quant/core/Exceptions.hpp"

#include <cmath>

namespace quant::pricing {

namespace {
inline double norm_cdf(double x) { return 0.5 * std::erfc(-x / std::sqrt(2.0)); }
constexpr double INV_SQRT_2PI = 0.39894228040143267794;
inline double norm_pdf(double x) { return std::exp(-0.5 * x * x) * INV_SQRT_2PI; }
}

double BlackScholesEuropeanEngine::price(const quant::instruments::Instrument& inst) const {
    const auto* opt = dynamic_cast<const quant::instruments::EuropeanOption*>(&inst);
    if (!opt) throw quant::core::PricingError("Instrument is not EuropeanOption");
    return price(*opt);
}

double BlackScholesEuropeanEngine::price(const quant::instruments::EuropeanOption& opt) const {
    double r = opt.rate();
    double vol = opt.volatility();
    double q = opt.dividend();
    if (curve_) r = curve_->zero_rate(opt.maturity());
    double T = opt.maturity();
    if (T <= 0.0 || vol <= 0.0) return 0.0;
    double d1 = (std::log(opt.spot() / opt.strike()) + (r - q + 0.5 * vol * vol) * T) / (vol * std::sqrt(T));
    double d2 = d1 - vol * std::sqrt(T);
    double df = std::exp(-r * T);
    double qf = std::exp(-q * T);
    if (opt.option_type() == quant::instruments::OptionType::Call) {
        return opt.spot() * qf * norm_cdf(d1) - opt.strike() * df * norm_cdf(d2);
    }
    return opt.strike() * df * norm_cdf(-d2) - opt.spot() * qf * norm_cdf(-d1);
}

double BlackScholesEuropeanEngine::delta(const quant::instruments::EuropeanOption& opt) const {
    double r = opt.rate();
    double vol = opt.volatility();
    double q = opt.dividend();
    if (curve_) r = curve_->zero_rate(opt.maturity());
    double T = opt.maturity();
    double d1 = (std::log(opt.spot() / opt.strike()) + (r - q + 0.5 * vol * vol) * T) / (vol * std::sqrt(T));
    double qf = std::exp(-q * T);
    if (opt.option_type() == quant::instruments::OptionType::Call) return qf * norm_cdf(d1);
    return -qf * norm_cdf(-d1);
}

double BlackScholesEuropeanEngine::gamma(const quant::instruments::EuropeanOption& opt) const {
    double r = opt.rate();
    double vol = opt.volatility();
    double q = opt.dividend();
    if (curve_) r = curve_->zero_rate(opt.maturity());
    double T = opt.maturity();
    double d1 = (std::log(opt.spot() / opt.strike()) + (r - q + 0.5 * vol * vol) * T) / (vol * std::sqrt(T));
    double qf = std::exp(-q * T);
    return qf * norm_pdf(d1) / (opt.spot() * vol * std::sqrt(T));
}

double BlackScholesEuropeanEngine::vega(const quant::instruments::EuropeanOption& opt) const {
    double r = opt.rate();
    double vol = opt.volatility();
    double q = opt.dividend();
    if (curve_) r = curve_->zero_rate(opt.maturity());
    double T = opt.maturity();
    double d1 = (std::log(opt.spot() / opt.strike()) + (r - q + 0.5 * vol * vol) * T) / (vol * std::sqrt(T));
    double qf = std::exp(-q * T);
    return opt.spot() * qf * norm_pdf(d1) * std::sqrt(T);
}

double BlackScholesEuropeanEngine::theta(const quant::instruments::EuropeanOption& opt) const {
    double r = opt.rate();
    double vol = opt.volatility();
    double q = opt.dividend();
    if (curve_) r = curve_->zero_rate(opt.maturity());
    double T = opt.maturity();
    double d1 = (std::log(opt.spot() / opt.strike()) + (r - q + 0.5 * vol * vol) * T) / (vol * std::sqrt(T));
    double d2 = d1 - vol * std::sqrt(T);
    double qf = std::exp(-q * T);
    double df = std::exp(-r * T);
    double theta = - (opt.spot() * qf * norm_pdf(d1) * vol) / (2.0 * std::sqrt(T));
    if (opt.option_type() == quant::instruments::OptionType::Call) {
        theta += -r * opt.strike() * df * norm_cdf(d2) + q * opt.spot() * qf * norm_cdf(d1);
    } else {
        theta += r * opt.strike() * df * norm_cdf(-d2) - q * opt.spot() * qf * norm_cdf(-d1);
    }
    return theta;
}

double BlackScholesEuropeanEngine::rho(const quant::instruments::EuropeanOption& opt) const {
    double r = opt.rate();
    double vol = opt.volatility();
    double q = opt.dividend();
    if (curve_) r = curve_->zero_rate(opt.maturity());
    double T = opt.maturity();
    double d1 = (std::log(opt.spot() / opt.strike()) + (r - q + 0.5 * vol * vol) * T) / (vol * std::sqrt(T));
    double d2 = d1 - vol * std::sqrt(T);
    double df = std::exp(-r * T);
    if (opt.option_type() == quant::instruments::OptionType::Call) {
        return T * opt.strike() * df * norm_cdf(d2);
    }
    return -T * opt.strike() * df * norm_cdf(-d2);
}

} // namespace quant::pricing
