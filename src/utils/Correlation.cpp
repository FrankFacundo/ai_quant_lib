#include "quant/utils/Correlation.hpp"

#include <numeric>

namespace quant::utils {

quant::core::Matrix correlation_matrix(const std::map<std::string, quant::core::TimeSeries<double>>& series) {
    if (series.empty()) return {};
    std::size_t cols = series.size();
    std::size_t rows = series.begin()->second.size();
    quant::core::Matrix data(rows, cols);
    std::size_t col = 0;
    for (const auto& [name, ts] : series) {
        for (std::size_t i = 0; i < rows && i < ts.size(); ++i) {
            data(static_cast<int>(i), static_cast<int>(col)) = ts.values()[i];
        }
        ++col;
    }
    return quant::core::correlation(data);
}

quant::core::Matrix rolling_correlation(const quant::core::TimeSeries<double>& a,
                                       const quant::core::TimeSeries<double>& b,
                                       std::size_t window) {
    quant::core::Matrix out;
    if (a.size() < window || b.size() < window) return out;
    out.resize(static_cast<int>(a.size() - window + 1), 1);
    for (std::size_t i = window - 1; i < a.size() && i < b.size(); ++i) {
        double mean_a = 0.0, mean_b = 0.0;
        for (std::size_t j = i - window + 1; j <= i; ++j) {
            mean_a += a.values()[j];
            mean_b += b.values()[j];
        }
        mean_a /= window;
        mean_b /= window;
        double num = 0.0, var_a = 0.0, var_b = 0.0;
        for (std::size_t j = i - window + 1; j <= i; ++j) {
            double da = a.values()[j] - mean_a;
            double db = b.values()[j] - mean_b;
            num += da * db;
            var_a += da * da;
            var_b += db * db;
        }
        out(static_cast<int>(i - window + 1), 0) = num / std::sqrt(var_a * var_b + 1e-12);
    }
    return out;
}

void MacroDashboard::add_series(const std::string& name, const quant::core::TimeSeries<double>& ts) {
    series_[name] = ts;
}

quant::core::Matrix MacroDashboard::correlations() const {
    return correlation_matrix(series_);
}

quant::core::Vector MacroDashboard::beta(const std::string& name, const std::string& benchmark) const {
    auto it_a = series_.find(name);
    auto it_b = series_.find(benchmark);
    if (it_a == series_.end() || it_b == series_.end()) return {};
    const auto& a = it_a->second;
    const auto& b = it_b->second;
    std::size_t n = std::min(a.size(), b.size());
    double mean_a = 0.0, mean_b = 0.0;
    for (std::size_t i = 0; i < n; ++i) { mean_a += a.values()[i]; mean_b += b.values()[i]; }
    mean_a /= n; mean_b /= n;
    double cov = 0.0, var_b = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        double da = a.values()[i] - mean_a;
        double db = b.values()[i] - mean_b;
        cov += da * db;
        var_b += db * db;
    }
    double beta_val = var_b > 0 ? cov / var_b : 0.0;
    quant::core::Vector v(1);
    v(0) = beta_val;
    return v;
}

} // namespace quant::utils

