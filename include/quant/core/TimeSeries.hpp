#pragma once

#include "quant/core/Date.hpp"
#include "quant/core/Exceptions.hpp"

#include <algorithm>
#include <functional>
#include <optional>
#include <span>
#include <vector>

namespace quant::core {

template <typename T>
class TimeSeries {
public:
    TimeSeries() = default;

    void push_back(const DateTime& t, const T& value) {
        times_.push_back(t);
        values_.push_back(value);
    }

    std::size_t size() const { return values_.size(); }
    const std::vector<DateTime>& times() const { return times_; }
    const std::vector<T>& values() const { return values_; }

    const T& at(std::size_t idx) const {
        if (idx >= values_.size()) {
            throw QuantError("TimeSeries index out of range");
        }
        return values_[idx];
    }

    TimeSeries<T> lag(std::size_t k) const {
        TimeSeries<T> out;
        if (k >= size()) return out;
        out.times_.assign(times_.begin() + k, times_.end());
        out.values_.assign(values_.begin(), values_.end() - static_cast<std::ptrdiff_t>(k));
        return out;
    }

    TimeSeries<T> diff() const {
        TimeSeries<T> out;
        if (size() < 2) return out;
        out.times_.assign(times_.begin() + 1, times_.end());
        for (std::size_t i = 1; i < size(); ++i) {
            out.values_.push_back(values_[i] - values_[i - 1]);
        }
        return out;
    }

    template <typename Aggregator>
    TimeSeries<T> rolling(std::size_t window, Aggregator agg) const {
        TimeSeries<T> out;
        if (window == 0 || size() < window) return out;
        for (std::size_t i = window - 1; i < size(); ++i) {
            out.times_.push_back(times_[i]);
            out.values_.push_back(agg(std::span<const T>(values_.data() + (i - window + 1), window)));
        }
        return out;
    }

    template <typename Aggregator>
    TimeSeries<T> resample(const std::vector<DateTime>& new_grid, Aggregator agg) const {
        TimeSeries<T> out;
        if (times_.empty()) return out;
        std::size_t idx = 0;
        for (const auto& t : new_grid) {
            std::vector<T> bucket;
            while (idx < times_.size() && times_[idx] <= t) {
                bucket.push_back(values_[idx]);
                ++idx;
            }
            if (!bucket.empty()) {
                out.times_.push_back(t);
                out.values_.push_back(agg(bucket));
            }
        }
        return out;
    }

    TimeSeries<T> head(std::size_t n) const {
        TimeSeries<T> out;
        if (n > size()) n = size();
        out.times_.assign(times_.begin(), times_.begin() + static_cast<std::ptrdiff_t>(n));
        out.values_.assign(values_.begin(), values_.begin() + static_cast<std::ptrdiff_t>(n));
        return out;
    }

private:
    std::vector<DateTime> times_;
    std::vector<T> values_;
};

} // namespace quant::core

