#pragma once

#include <stdexcept>
#include <string>

namespace quant::core {

class QuantError : public std::runtime_error {
public:
    explicit QuantError(const std::string& msg) : std::runtime_error(msg) {}
};

class PricingError : public QuantError {
public:
    explicit PricingError(const std::string& msg) : QuantError(msg) {}
};

class DataError : public QuantError {
public:
    explicit DataError(const std::string& msg) : QuantError(msg) {}
};

} // namespace quant::core

