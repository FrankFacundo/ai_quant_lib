#pragma once

#include "quant/core/TimeSeries.hpp"
#include "quant/core/LinearAlgebra.hpp"

#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace quant::backtest {

struct Bar {
    quant::core::DateTime time;
    double open{0.0};
    double high{0.0};
    double low{0.0};
    double close{0.0};
    double volume{0.0};

    Bar() = default;
    Bar(const quant::core::DateTime& t, double o, double h, double l, double c, double v)
        : time(t), open(o), high(h), low(l), close(c), volume(v) {}
};

struct Trade {
    quant::core::DateTime entry_time;
    quant::core::DateTime exit_time;
    double entry_price{0.0};
    double exit_price{0.0};
    double pnl{0.0};
};

class Portfolio {
public:
    explicit Portfolio(double cash = 0.0) : cash_(cash) {}

    void update_position(const std::string& asset, double quantity, double price,
                         std::optional<quant::core::DateTime> time = std::nullopt);
    double position(const std::string& asset) const;
    double market_value(const std::map<std::string, double>& prices) const;
    double cash() const { return cash_; }
    const std::vector<Trade>& trades() const { return trades_; }

private:
    double cash_{0.0};
    std::map<std::string, double> positions_;
    std::vector<Trade> trades_;
};

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual void on_bar(const std::string& asset, const Bar& bar, Portfolio& portfolio) = 0;
};

class MovingAverageCrossStrategy : public Strategy {
public:
    MovingAverageCrossStrategy(std::size_t short_window, std::size_t long_window, double qty,
                               double transaction_cost = 0.0, double slippage = 0.0);
    void on_bar(const std::string& asset, const Bar& bar, Portfolio& portfolio) override;

private:
    std::size_t short_window_;
    std::size_t long_window_;
    double quantity_;
    double transaction_cost_;
    double slippage_;
    std::map<std::string, std::deque<double>> history_;
};

struct BacktestStats {
    double cumulative_return{0.0};
    double annualized_vol{0.0};
    double sharpe{0.0};
    double max_drawdown{0.0};
};

struct BacktestResult {
    quant::core::TimeSeries<double> equity_curve;
    std::vector<Trade> trades;
    BacktestStats stats;
};

class Backtester {
public:
    Backtester(std::map<std::string, quant::core::TimeSeries<Bar>> data,
               std::shared_ptr<Strategy> strategy,
               Portfolio portfolio);

    BacktestResult run();

private:
    std::map<std::string, quant::core::TimeSeries<Bar>> data_;
    std::shared_ptr<Strategy> strategy_;
    Portfolio portfolio_;
};

BacktestStats compute_performance(const quant::core::TimeSeries<double>& equity);

} // namespace quant::backtest
