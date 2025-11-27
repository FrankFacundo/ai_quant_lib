#pragma once

#include <chrono>
#include <compare>
#include <set>
#include <string>
#include <vector>

namespace quant::core {

enum class DayCountConvention {
    ACT_365,
    ACT_360,
    THIRTY_360
};

class Date {
public:
    Date() = default;
    Date(int y, unsigned m, unsigned d);

    int year() const { return year_; }
    unsigned month() const { return month_; }
    unsigned day() const { return day_; }

    std::chrono::system_clock::time_point to_time_point() const;

    std::string to_string() const;

    auto operator<=>(const Date&) const = default;

private:
    int year_{};
    unsigned month_{};
    unsigned day_{};
};

class DateTime {
public:
    DateTime();
    explicit DateTime(std::chrono::system_clock::time_point tp);
    DateTime(int y, unsigned m, unsigned d, unsigned hh = 0, unsigned mm = 0, unsigned ss = 0);

    std::chrono::system_clock::time_point time_point() const { return tp_; }
    Date date() const;
    std::string to_string() const;

    auto operator<=>(const DateTime&) const = default;

private:
    std::chrono::system_clock::time_point tp_;
};

class Calendar {
public:
    Calendar() = default;
    explicit Calendar(std::set<Date> holidays) : holidays_(std::move(holidays)) {}

    bool is_business_day(const Date& d) const;
    Date adjust(const Date& d) const;
    Date advance(const Date& d, int days) const;
    void add_holiday(const Date& d) { holidays_.insert(d); }

private:
    std::set<Date> holidays_;
};

long day_count(const Date& start, const Date& end, DayCountConvention conv);
double year_fraction(const Date& start, const Date& end, DayCountConvention conv);

} // namespace quant::core
