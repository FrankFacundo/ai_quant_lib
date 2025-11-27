#include "quant/timeseries/Models.hpp"

#include <algorithm>
#include <cmath>

namespace quant::timeseries {

RandomForestRegressor::RandomForestRegressor(int trees, int max_depth, double feature_subsample)
    : trees_(trees), max_depth_(max_depth), feature_subsample_(feature_subsample) {}

std::unique_ptr<RandomForestRegressor::Node> RandomForestRegressor::build_tree(
    const std::vector<Eigen::VectorXd>& X, const std::vector<double>& y, int depth, std::mt19937& gen) {
    auto node = std::make_unique<Node>();
    double mean = 0.0;
    for (double v : y) mean += v;
    mean /= y.size();
    node->value = mean;
    if (depth >= max_depth_ || y.size() < 3) return node;

    std::uniform_int_distribution<int> feature_dist(0, static_cast<int>(X[0].size() - 1));
    int feature = feature_dist(gen);
    node->feature = feature;
    std::vector<double> feature_values;
    feature_values.reserve(X.size());
    for (const auto& xi : X) feature_values.push_back(xi(feature));
    std::nth_element(feature_values.begin(), feature_values.begin() + feature_values.size() / 2, feature_values.end());
    double threshold = feature_values[feature_values.size() / 2];
    node->threshold = threshold;

    std::vector<Eigen::VectorXd> leftX, rightX;
    std::vector<double> lefty, righty;
    for (std::size_t i = 0; i < X.size(); ++i) {
        if (X[i](feature) <= threshold) {
            leftX.push_back(X[i]); lefty.push_back(y[i]);
        } else {
            rightX.push_back(X[i]); righty.push_back(y[i]);
        }
    }
    if (leftX.empty() || rightX.empty()) return node;
    node->left = build_tree(leftX, lefty, depth + 1, gen);
    node->right = build_tree(rightX, righty, depth + 1, gen);
    return node;
}

double RandomForestRegressor::predict_tree(const Node* node, const Eigen::VectorXd& x) const {
    if (!node->left || !node->right || node->feature < 0) return node->value;
    if (x(node->feature) <= node->threshold) return predict_tree(node->left.get(), x);
    return predict_tree(node->right.get(), x);
}

void RandomForestRegressor::fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
                                const std::vector<Eigen::VectorXd>& static_features) {
    if (y.size() < 2) return;
    std::vector<Eigen::VectorXd> X;
    std::vector<double> target;
    for (std::size_t i = 1; i < y.size(); ++i) {
        X.push_back(y.values()[i - 1]);
        target.push_back(y.values()[i](0));
    }
    forest_.clear();
    std::mt19937 gen(42);
    for (int t = 0; t < trees_; ++t) {
        forest_.push_back(build_tree(X, target, 0, gen));
    }
    feature_index_.assign(trees_, 0);
    if (!X.empty()) last_input_ = X.back();
    else last_input_.resize(0);
}

Eigen::VectorXd RandomForestRegressor::forecast(std::size_t horizon,
                                                const std::vector<Eigen::VectorXd>& future_static_features) const {
    Eigen::VectorXd out = Eigen::VectorXd::Zero(static_cast<int>(horizon));
    if (last_input_.size() == 0) return out;
    Eigen::VectorXd state = last_input_;
    for (std::size_t h = 0; h < horizon; ++h) {
        double pred = 0.0;
        for (const auto& tree : forest_) pred += predict_tree(tree.get(), state);
        pred /= forest_.empty() ? 1.0 : static_cast<double>(forest_.size());
        out(static_cast<int>(h)) = pred;
        state(0) = pred; // update simple state with predicted value
    }
    return out;
}

FeedForwardNN::FeedForwardNN(std::vector<int> layers, double lr, int epochs)
    : layers_(std::move(layers)), lr_(lr), epochs_(epochs) {}

void FeedForwardNN::fit(const quant::core::TimeSeries<Eigen::VectorXd>& y,
                        const std::vector<Eigen::VectorXd>& static_features) {
    if (y.size() < 2) return;
    int input_dim = static_cast<int>(y.values().front().size());
    layers_.front() = input_dim;
    layers_.back() = 1;
    weights_.clear();
    biases_.clear();
    std::mt19937 gen(123);
    std::normal_distribution<double> nd(0.0, 0.1);
    for (std::size_t l = 0; l + 1 < layers_.size(); ++l) {
        Eigen::MatrixXd W(layers_[l + 1], layers_[l]);
        Eigen::VectorXd b = Eigen::VectorXd::Zero(layers_[l + 1]);
        for (int i = 0; i < W.rows(); ++i) {
            for (int j = 0; j < W.cols(); ++j) W(i, j) = nd(gen);
        }
        weights_.push_back(W);
        biases_.push_back(b);
    }
    for (int epoch = 0; epoch < epochs_; ++epoch) {
        for (std::size_t i = 1; i < y.size(); ++i) {
            train_sample(y.values()[i - 1], Eigen::VectorXd::Constant(1, y.values()[i](0)));
        }
    }
    last_input_ = y.values().back();
}

Eigen::VectorXd FeedForwardNN::forecast(std::size_t horizon,
                                        const std::vector<Eigen::VectorXd>& future_static_features) const {
    Eigen::VectorXd out = Eigen::VectorXd::Zero(static_cast<int>(horizon));
    Eigen::VectorXd state = last_input_;
    for (std::size_t h = 0; h < horizon; ++h) {
        double pred = forward(state)(0);
        out(static_cast<int>(h)) = pred;
        state(0) = pred;
    }
    return out;
}

Eigen::VectorXd FeedForwardNN::forward(const Eigen::VectorXd& x) const {
    Eigen::VectorXd a = x;
    for (std::size_t l = 0; l < weights_.size(); ++l) {
        a = (weights_[l] * a + biases_[l]).unaryExpr([](double v) { return std::tanh(v); });
    }
    return a;
}

void FeedForwardNN::train_sample(const Eigen::VectorXd& x, const Eigen::VectorXd& target) {
    std::vector<Eigen::VectorXd> activations;
    std::vector<Eigen::VectorXd> zs;
    activations.push_back(x);
    Eigen::VectorXd a = x;
    for (std::size_t l = 0; l < weights_.size(); ++l) {
        Eigen::VectorXd z = weights_[l] * a + biases_[l];
        zs.push_back(z);
        a = z.unaryExpr([](double v) { return std::tanh(v); });
        activations.push_back(a);
    }
    Eigen::VectorXd delta = activations.back() - target;
    for (int l = static_cast<int>(weights_.size()) - 1; l >= 0; --l) {
        Eigen::VectorXd dz = zs[l].unaryExpr([](double v) { double t = std::tanh(v); return 1.0 - t * t; });
        Eigen::VectorXd grad = delta.cwiseProduct(dz);
        Eigen::MatrixXd wgrad = grad * activations[l].transpose();
        weights_[l] -= lr_ * wgrad;
        biases_[l] -= lr_ * grad;
        if (l > 0) {
            delta = weights_[l].transpose() * grad;
        }
    }
}

} // namespace quant::timeseries
