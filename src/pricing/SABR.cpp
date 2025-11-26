#include "quant/pricing/SABR.hpp"

#include <cmath>

namespace quant::pricing {

double SABRModel::implied_vol(double F, double K, double T) const {
    if (F <= 0.0 || K <= 0.0) return 0.0;
    if (F == K) {
        double num = alpha_;
        double den = std::pow(F, 1.0 - beta_);
        double term1 = ((1.0 - beta_) * (1.0 - beta_) * alpha_ * alpha_) / (24.0 * std::pow(den, 2.0));
        double term2 = (rho_ * beta_ * nu_ * alpha_) / (4.0 * den);
        double term3 = ((2.0 - 3.0 * rho_ * rho_) * nu_ * nu_) / 24.0;
        return (num / den) * (1.0 + (term1 + term2 + term3) * T);
    }
    double logFK = std::log(F / K);
    double FKbeta = std::pow(F * K, (1.0 - beta_) / 2.0);
    double z = (nu_ / alpha_) * FKbeta * logFK;
    double xz = 0.0;
    if (std::abs(z) < 1e-8) {
        xz = 1.0;
    } else {
        xz = std::log((std::sqrt(1.0 - 2.0 * rho_ * z + z * z) + z - rho_) / (1.0 - rho_));
    }
    double term1 = alpha_ / (FKbeta * (1.0 + std::pow(1.0 - beta_, 2.0) * logFK * logFK / 24.0 + std::pow(1.0 - beta_, 4.0) * std::pow(logFK, 4.0) / 1920.0));
    double term2 = z / xz;
    double term3 = 1.0 + ((std::pow(1.0 - beta_, 2.0) * alpha_ * alpha_) / (24.0 * std::pow(FKbeta, 2.0)) + (rho_ * beta_ * nu_ * alpha_) / (4.0 * FKbeta) + ((2.0 - 3.0 * rho_ * rho_) * nu_ * nu_) / 24.0) * T;
    return term1 * term2 * term3;
}

double SABREuropeanEngine::price(const quant::instruments::Instrument& inst) const {
    auto opt = dynamic_cast<const quant::instruments::EuropeanOption*>(&inst);
    if (!opt) throw std::invalid_argument("Instrument not EuropeanOption");
    return price(*opt);
}

double SABREuropeanEngine::price(const quant::instruments::EuropeanOption& opt) const {
    double T = opt.maturity();
    double F = opt.spot();
    double df = std::exp(-discount_rate_ * T);
    double vol = model_.implied_vol(F, opt.strike(), T);
    if (vol <= 0.0 || T <= 0.0) return 0.0;
    double d1 = (std::log(F / opt.strike()) + 0.5 * vol * vol * T) / (vol * std::sqrt(T));
    double d2 = d1 - vol * std::sqrt(T);
    auto norm_cdf = [](double x) { return 0.5 * std::erfc(-x / std::sqrt(2.0)); };
    if (opt.option_type() == quant::instruments::OptionType::Call) {
        return df * (F * norm_cdf(d1) - opt.strike() * norm_cdf(d2));
    }
    return df * (opt.strike() * norm_cdf(-d2) - F * norm_cdf(-d1));
}

} // namespace quant::pricing
