#include "quant/core/Date.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace quant::core {

Date::Date(int y, unsigned m, unsigned d) : year_(y), month_(m), day_(d) {}

std::chrono::year_month_day Date::to_chrono() const {
    return std::chrono::year{year_} / std::chrono::month{month_} / std::chrono::day{day_};
}

std::chrono::sys_days Date::to_sys_days() const {
    return std::chrono::sys_days{to_chrono()};
}

std::string Date::to_string() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << year_ << "-" << std::setw(2) << month_ << "-" << std::setw(2) << day_;
    return oss.str();
}

DateTime::DateTime() : tp_(std::chrono::system_clock::now()) {}

DateTime::DateTime(std::chrono::system_clock::time_point tp) : tp_(tp) {}

DateTime::DateTime(int y, unsigned m, unsigned d, unsigned hh, unsigned mm, unsigned ss) {
    std::chrono::sys_days day = std::chrono::sys_days{std::chrono::year{y} / std::chrono::month{m} / std::chrono::day{d}};
    tp_ = std::chrono::system_clock::time_point{day} + std::chrono::hours{hh} + std::chrono::minutes{mm} + std::chrono::seconds{ss};
}

Date DateTime::date() const {
    auto day = std::chrono::floor<std::chrono::days>(tp_);
    std::chrono::year_month_day ymd = std::chrono::year_month_day{day};
    return Date(static_cast<int>(ymd.year()), static_cast<unsigned>(ymd.month()), static_cast<unsigned>(ymd.day()));
}

std::string DateTime::to_string() const {
    auto tt = std::chrono::system_clock::to_time_t(tp_);
    std::tm tm = *std::gmtime(&tt);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                  tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(buf);
}

bool Calendar::is_business_day(const Date& d) const {
    auto wd = std::chrono::weekday{d.to_sys_days()};
    if (wd == std::chrono::Saturday || wd == std::chrono::Sunday) return false;
    return holidays_.find(d) == holidays_.end();
}

Date Calendar::adjust(const Date& d) const {
    Date adj = d;
    auto day = adj.to_sys_days();
    while (!is_business_day(adj)) {
        day += std::chrono::days{1};
        std::chrono::year_month_day ymd{day};
        adj = Date(static_cast<int>(ymd.year()), static_cast<unsigned>(ymd.month()), static_cast<unsigned>(ymd.day()));
    }
    return adj;
}

Date Calendar::advance(const Date& d, int days) const {
    auto day = d.to_sys_days() + std::chrono::days{days};
    std::chrono::year_month_day ymd{day};
    return Date(static_cast<int>(ymd.year()), static_cast<unsigned>(ymd.month()), static_cast<unsigned>(ymd.day()));
}

long day_count(const Date& start, const Date& end, DayCountConvention conv) {
    auto days = (end.to_sys_days() - start.to_sys_days()).count();
    if (conv == DayCountConvention::THIRTY_360) {
        auto d1 = start.day();
        auto d2 = end.day();
        auto m1 = start.month();
        auto m2 = end.month();
        auto y1 = start.year();
        auto y2 = end.year();
        return (360 * (y2 - y1) + 30 * static_cast<long>(m2 - m1) + (static_cast<long>(d2) - static_cast<long>(d1)));
    }
    return days;
}

double year_fraction(const Date& start, const Date& end, DayCountConvention conv) {
    double days = static_cast<double>(day_count(start, end, conv));
    switch (conv) {
    case DayCountConvention::ACT_360:
        return days / 360.0;
    case DayCountConvention::ACT_365:
        return days / 365.0;
    case DayCountConvention::THIRTY_360:
        return days / 360.0;
    }
    return days / 365.0;
}

} // namespace quant::core

