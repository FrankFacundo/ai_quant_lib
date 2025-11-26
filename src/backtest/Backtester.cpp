#include "quant/backtest/Backtester.hpp"
#include <cmath>

namespace quant::backtest {

void Portfolio::update_position(const std::string& asset, double quantity, double price,
                                std::optional<quant::core::DateTime> time) {
    double prev = positions_[asset];
    double delta = quantity - prev;
    double trade_value = delta * price;
    cash_ -= trade_value;
    positions_[asset] = quantity;
    if (std::abs(delta) > 1e-9) {
        Trade t;
        t.entry_time = time.value_or(quant::core::DateTime());
        t.exit_time = t.entry_time;
        t.entry_price = price;
        t.exit_price = price;
        t.pnl = -trade_value;
        trades_.push_back(t);
    }
}

double Portfolio::position(const std::string& asset) const {
    auto it = positions_.find(asset);
    return it == positions_.end() ? 0.0 : it->second;
}

double Portfolio::market_value(const std::map<std::string, double>& prices) const {
    double mv = 0.0;
    for (const auto& [asset, qty] : positions_) {
        auto it = prices.find(asset);
        if (it != prices.end()) mv += qty * it->second;
    }
    return mv + cash_;
}

MovingAverageCrossStrategy::MovingAverageCrossStrategy(std::size_t short_window, std::size_t long_window, double qty,
                                                       double transaction_cost, double slippage)
    : short_window_(short_window), long_window_(long_window), quantity_(qty),
      transaction_cost_(transaction_cost), slippage_(slippage) {}

void MovingAverageCrossStrategy::on_bar(const std::string& asset, const Bar& bar, Portfolio& portfolio) {
    auto& deque = history_[asset];
    deque.push_back(bar.close);
    if (deque.size() > long_window_) deque.pop_front();
    if (deque.size() < long_window_) return;
    double short_ma = 0.0, long_ma = 0.0;
    for (std::size_t i = deque.size() - short_window_; i < deque.size(); ++i) short_ma += deque[i];
    short_ma /= static_cast<double>(short_window_);
    for (double v : deque) long_ma += v;
    long_ma /= static_cast<double>(deque.size());
    double current_pos = portfolio.position(asset);
    if (short_ma > long_ma && current_pos <= 0) {
        portfolio.update_position(asset, quantity_, bar.close * (1.0 + slippage_), bar.time);
    } else if (short_ma < long_ma && current_pos > 0) {
        portfolio.update_position(asset, 0.0, bar.close * (1.0 - slippage_), bar.time);
    }
}

Backtester::Backtester(std::map<std::string, quant::core::TimeSeries<Bar>> data,
                       std::shared_ptr<Strategy> strategy,
                       Portfolio portfolio)
    : data_(std::move(data)), strategy_(std::move(strategy)), portfolio_(portfolio) {}

BacktestResult Backtester::run() {
    BacktestResult res;
    if (data_.empty()) return res;
    std::size_t steps = data_.begin()->second.size();
    for (std::size_t i = 0; i < steps; ++i) {
        std::map<std::string, double> prices;
        for (auto& [asset, ts] : data_) {
            const auto& bar = ts.values()[i];
            strategy_->on_bar(asset, bar, portfolio_);
            prices[asset] = bar.close;
            if (res.equity_curve.size() == 0 || res.equity_curve.size() == i) {
                res.equity_curve.push_back(bar.time, portfolio_.market_value(prices));
            }
        }
    }
    res.stats = compute_performance(res.equity_curve);
    res.trades = portfolio_.trades();
    return res;
}

BacktestStats compute_performance(const quant::core::TimeSeries<double>& equity) {
    BacktestStats stats;
    if (equity.size() < 2) return stats;
    double start = equity.values().front();
    double end = equity.values().back();
    stats.cumulative_return = (end - start) / start;
    double mean = 0.0;
    for (double v : equity.values()) mean += v;
    mean /= equity.size();
    double var = 0.0;
    for (double v : equity.values()) {
        double diff = v - mean;
        var += diff * diff;
    }
    var /= equity.size();
    stats.annualized_vol = std::sqrt(var) * std::sqrt(252.0);
    stats.sharpe = stats.annualized_vol > 0 ? stats.cumulative_return / stats.annualized_vol : 0.0;
    double peak = equity.values().front();
    double max_dd = 0.0;
    for (double v : equity.values()) {
        if (v > peak) peak = v;
        double dd = (peak - v) / peak;
        if (dd > max_dd) max_dd = dd;
    }
    stats.max_drawdown = max_dd;
    return stats;
}

} // namespace quant::backtest
