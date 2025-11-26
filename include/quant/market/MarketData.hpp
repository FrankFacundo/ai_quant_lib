#pragma once

#include "quant/market/YieldCurve.hpp"
#include "quant/market/VolSurface.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace quant::market {

class FXSpot {
public:
    FXSpot(std::string ccy_pair, double spot) : ccy_pair_(std::move(ccy_pair)), spot_(spot) {}
    double value() const { return spot_; }
    const std::string& pair() const { return ccy_pair_; }
private:
    std::string ccy_pair_;
    double spot_;
};

class EquitySpot {
public:
    EquitySpot(std::string ticker, double spot) : ticker_(std::move(ticker)), spot_(spot) {}
    double value() const { return spot_; }
    const std::string& ticker() const { return ticker_; }
private:
    std::string ticker_;
    double spot_;
};

class MarketData {
public:
    void add_yield_curve(const std::string& name, YieldCurve curve) { curves_[name] = std::move(curve); }
    void add_vol_surface(const std::string& name, VolSurface surface) { surfaces_[name] = std::move(surface); }

    const YieldCurve* yield_curve(const std::string& name) const {
        auto it = curves_.find(name);
        return it == curves_.end() ? nullptr : &it->second;
    }

    const VolSurface* vol_surface(const std::string& name) const {
        auto it = surfaces_.find(name);
        return it == surfaces_.end() ? nullptr : &it->second;
    }

private:
    std::unordered_map<std::string, YieldCurve> curves_;
    std::unordered_map<std::string, VolSurface> surfaces_;
};

} // namespace quant::market

