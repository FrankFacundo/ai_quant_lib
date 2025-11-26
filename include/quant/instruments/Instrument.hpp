#pragma once

namespace quant::instruments {

class Instrument {
public:
    virtual ~Instrument() = default;
    virtual double npv() const = 0;
};

} // namespace quant::instruments

