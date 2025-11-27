#pragma once

#include "quant/core/LinearAlgebra.hpp"
#include "quant/core/TimeSeries.hpp"

#include <optional>
#include <memory>
#include <random>
#include <vector>

namespace quant::timeseries {

class TimeSeriesModel {
public:
    virtual ~TimeSeriesModel() = default;
    virtual void fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
                     const std::vector<Eigen::VectorXd>& static_features = {}) = 0;
    virtual Eigen::VectorXd forecast(std::size_t horizon,
                                     const std::vector<Eigen::VectorXd>& future_static_features = {}) const = 0;
};

class AlphaModel {
public:
    virtual ~AlphaModel() = default;
    virtual Eigen::VectorXd compute_signal(const quant::core::DateTime& t,
                                           const std::vector<Eigen::VectorXd>& features) const = 0;
};

class ARIMAModel : public TimeSeriesModel {
public:
    ARIMAModel(int p, int d, int q = 0);
    void fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
             const std::vector<Eigen::VectorXd>& static_features = {}) override;
    Eigen::VectorXd forecast(std::size_t horizon,
                             const std::vector<Eigen::VectorXd>& future_static_features = {}) const override;

    const Eigen::VectorXd& residuals() const { return residuals_; }
    double aic() const;
    double bic() const;

private:
    int p_;
    int d_;
    int q_;
    Eigen::VectorXd coefficients_;
    Eigen::VectorXd residuals_;
    Eigen::VectorXd last_values_;
};

class VARModel : public TimeSeriesModel {
public:
    explicit VARModel(int lags);
    void fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
             const std::vector<Eigen::VectorXd>& static_features = {}) override;
    Eigen::VectorXd forecast(std::size_t horizon,
                             const std::vector<Eigen::VectorXd>& future_static_features = {}) const override;

    double aic() const;
    double bic() const;

private:
    int lags_;
    Eigen::MatrixXd coefficients_; // stacked [lags * dim x dim]
    Eigen::VectorXd residual_var_;
    Eigen::VectorXd last_values_;
    int dim_{0};
};

class GARCHModel : public TimeSeriesModel {
public:
    GARCHModel(double omega = 0.01, double alpha = 0.05, double beta = 0.9);
    void fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
             const std::vector<Eigen::VectorXd>& static_features = {}) override;
    Eigen::VectorXd forecast(std::size_t horizon,
                             const std::vector<Eigen::VectorXd>& future_static_features = {}) const override;
    const Eigen::VectorXd& conditional_vol() const { return cond_vol_; }

private:
    double omega_;
    double alpha_;
    double beta_;
    Eigen::VectorXd cond_vol_;
    Eigen::VectorXd last_value_;
};

class RandomForestRegressor : public TimeSeriesModel {
public:
    RandomForestRegressor(int trees = 10, int max_depth = 3, double feature_subsample = 0.8);
    void fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
             const std::vector<Eigen::VectorXd>& static_features = {}) override;
    Eigen::VectorXd forecast(std::size_t horizon,
                             const std::vector<Eigen::VectorXd>& future_static_features = {}) const override;

private:
    struct Node {
        int feature{-1};
        double threshold{0.0};
        double value{0.0};
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };

    std::unique_ptr<Node> build_tree(const std::vector<Eigen::VectorXd>& X,
                                     const std::vector<double>& y,
                                     int depth,
                                     std::mt19937& gen);
    double predict_tree(const Node* node, const Eigen::VectorXd& x) const;

    int trees_;
    int max_depth_;
    double feature_subsample_;
    std::vector<std::unique_ptr<Node>> forest_;
    std::vector<int> feature_index_; // features used for each tree root
    Eigen::VectorXd last_input_;
};

class FeedForwardNN : public TimeSeriesModel {
public:
    FeedForwardNN(std::vector<int> layers, double lr = 0.01, int epochs = 200);
    void fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
             const std::vector<Eigen::VectorXd>& static_features = {}) override;
    Eigen::VectorXd forecast(std::size_t horizon,
                             const std::vector<Eigen::VectorXd>& future_static_features = {}) const override;

private:
    Eigen::VectorXd forward(const Eigen::VectorXd& x) const;
    void train_sample(const Eigen::VectorXd& x, const Eigen::VectorXd& target);

    std::vector<int> layers_;
    double lr_;
    int epochs_;
    std::vector<Eigen::MatrixXd> weights_;
    std::vector<Eigen::VectorXd> biases_;
    Eigen::VectorXd last_input_;
};

class FXTimeSeriesModel {
public:
    FXTimeSeriesModel();
    void fit_returns(const quant::core::TimeSeries<Eigen::VectorXd>& returns);
    Eigen::VectorXd forecast_vol(std::size_t horizon) const;

private:
    GARCHModel garch_;
};

class EquityTimeSeriesModel {
public:
    EquityTimeSeriesModel();
    void fit_prices(const quant::core::TimeSeries<Eigen::VectorXd>& prices);
    Eigen::VectorXd forecast_prices(std::size_t horizon) const;

private:
    ARIMAModel arima_;
};

class EnergyTimeSeriesModel {
public:
    EnergyTimeSeriesModel();
    void fit_series(const quant::core::TimeSeries<Eigen::VectorXd>& prices);
    Eigen::VectorXd forecast_prices(std::size_t horizon) const;

private:
    VARModel var_;
};

class CreditTimeSeriesModel {
public:
    CreditTimeSeriesModel();
    void fit_spreads(const quant::core::TimeSeries<Eigen::VectorXd>& spreads);
    Eigen::VectorXd forecast_spreads(std::size_t horizon) const;

private:
    RandomForestRegressor rf_;
};

} // namespace quant::timeseries
