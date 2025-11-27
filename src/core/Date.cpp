#include "quant/core/Date.hpp"

#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace quant::core {

Date::Date(int y, unsigned m, unsigned d) : year_(y), month_(m), day_(d) {}

std::chrono::system_clock::time_point Date::to_time_point() const {
    std::tm tm{};
    tm.tm_year = year_ - 1900;
    tm.tm_mon = static_cast<int>(month_) - 1;
    tm.tm_mday = static_cast<int>(day_);
    tm.tm_isdst = -1;
    std::time_t t = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(t);
}

std::string Date::to_string() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << year_ << "-" << std::setw(2) << month_ << "-" << std::setw(2) << day_;
    return oss.str();
}

DateTime::DateTime() : tp_(std::chrono::system_clock::now()) {}

DateTime::DateTime(std::chrono::system_clock::time_point tp) : tp_(tp) {}

DateTime::DateTime(int y, unsigned m, unsigned d, unsigned hh, unsigned mm, unsigned ss) {
    std::tm tm{};
    tm.tm_year = y - 1900;
    tm.tm_mon = static_cast<int>(m) - 1;
    tm.tm_mday = static_cast<int>(d);
    tm.tm_hour = static_cast<int>(hh);
    tm.tm_min = static_cast<int>(mm);
    tm.tm_sec = static_cast<int>(ss);
    tm.tm_isdst = -1;
    std::time_t t = std::mktime(&tm);
    tp_ = std::chrono::system_clock::from_time_t(t);
}

Date DateTime::date() const {
    auto tt = std::chrono::system_clock::to_time_t(tp_);
    std::tm tm = *std::localtime(&tt);
    return Date(tm.tm_year + 1900, static_cast<unsigned>(tm.tm_mon + 1), static_cast<unsigned>(tm.tm_mday));
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
    auto tt = std::chrono::system_clock::to_time_t(d.to_time_point());
    std::tm tm = *std::localtime(&tt);
    int wday = tm.tm_wday; // 0 = Sunday
    if (wday == 0 || wday == 6) return false;
    return holidays_.find(d) == holidays_.end();
}

Date Calendar::adjust(const Date& d) const {
    Date adj = d;
    while (!is_business_day(adj)) {
        adj = advance(adj, 1);
    }
    return adj;
}

Date Calendar::advance(const Date& d, int days) const {
    auto tp = d.to_time_point() + std::chrono::hours{24 * days};
    auto tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&tt);
    return Date(tm.tm_year + 1900, static_cast<unsigned>(tm.tm_mon + 1), static_cast<unsigned>(tm.tm_mday));
}

long day_count(const Date& start, const Date& end, DayCountConvention conv) {
    auto delta = std::chrono::duration_cast<std::chrono::hours>(end.to_time_point() - start.to_time_point()).count() / 24;
    if (conv == DayCountConvention::THIRTY_360) {
        auto d1 = start.day();
        auto d2 = end.day();
        auto m1 = start.month();
        auto m2 = end.month();
        auto y1 = start.year();
        auto y2 = end.year();
        return (360 * (y2 - y1) + 30 * static_cast<long>(m2 - m1) + (static_cast<long>(d2) - static_cast<long>(d1)));
    }
    return delta;
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
