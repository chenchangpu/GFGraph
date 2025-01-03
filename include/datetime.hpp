#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

constexpr uint64_t SECONDS_PER_DAY = 86400;
constexpr uint64_t SECONDS_PER_HOUR = 3600;
constexpr uint64_t SECONDS_PER_MINUTE = 60;

struct TimeRange {
    uint64_t seconds;

    TimeRange() = default;

    constexpr TimeRange(uint64_t secs = 0)
        : seconds(secs) {}

    static TimeRange FromMmSs(int minutes, int secs = 0) {
        return TimeRange(uint64_t(secs) + uint64_t(minutes) * SECONDS_PER_MINUTE);
    }

    static TimeRange FromHhMmSs(int hours, int minutes = 0, int secs = 0) {
        return TimeRange(uint64_t(secs) + uint64_t(minutes) * SECONDS_PER_MINUTE + 
            uint64_t(hours) * SECONDS_PER_HOUR);
    }

    static TimeRange FromDdHhMmSs(int days, int hours = 0, int minutes = 0, int secs = 0) {
        return TimeRange(uint64_t(secs) + uint64_t(minutes) * SECONDS_PER_MINUTE + 
            uint64_t(hours) * SECONDS_PER_HOUR + uint64_t(days) * SECONDS_PER_DAY);
    }

    uint64_t to_seconds() const {
        return seconds;
    }

    int to_minutes() const {
        return seconds / SECONDS_PER_MINUTE;
    }

    int to_hours() const {
        return seconds / SECONDS_PER_HOUR;
    }

    int to_days() const {
        return seconds / SECONDS_PER_DAY;
    }

    bool operator<(const TimeRange& rhs) const {
        return seconds < rhs.seconds;
    }

    bool operator>(const TimeRange& rhs) const {
        return rhs < *this;
    }

    bool operator<=(const TimeRange& rhs) const {
        return !(*this > rhs);
    }

    TimeRange operator+(const TimeRange& rhs) const {
        return TimeRange(seconds + rhs.seconds);
    }

    TimeRange& operator+=(const TimeRange& rhs) {
        seconds += rhs.seconds;
        return *this;
    }
};

struct DateTime {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

    DateTime() = default;

    constexpr DateTime(int y, int mon, int d, int h = 0, int min = 0, int s = 0)
        : year(y), month(mon), day(d), hour(h), minute(min), second(s) {}
    
    explicit DateTime(const std::string& str) {
        std::istringstream iss(str);
        char dash1, dash2, colon1, colon2;
        if (iss >> year >> dash1 >> month >> dash2 >> day >> 
            hour >> colon1 >> minute >> colon2 >> second) {
            if (dash1!= '-' || dash2!= '-' || colon1!= ':' || colon2!= ':') {
                throw std::invalid_argument("Invalid time string format, should be YYYY-MM-DD hh:mm:ss");
            }
        } else {
            throw std::invalid_argument("Invalid time string format, should be YYYY-MM-DD hh:mm:ss");
        }
        validate();
    }

    bool operator<(const DateTime& rhs) const {
        if (year != rhs.year) {
            return year < rhs.year;
        }
        if (month != rhs.month) {
            return month < rhs.month;
        }
        if (day != rhs.day) {
            return day < rhs.day;
        }
        if (hour != rhs.hour) {
            return hour < rhs.hour;
        }
        if (minute != rhs.minute) {
            return minute < rhs.minute;
        }
        return second < rhs.second;
    }

    bool operator>(const DateTime& rhs) const {
        return rhs < *this;
    }

    bool operator<=(const DateTime& rhs) const {
        return !(*this > rhs);
    }

    TimeRange operator- (const DateTime& rhs) const {
        auto lhs_secs = UnixSeconds();
        auto rhs_secs = rhs.UnixSeconds();
        assert(lhs_secs >= rhs_secs);
        return TimeRange(lhs_secs - rhs_secs);
    }

    DateTime operator+ (const TimeRange& rhs) const {
        auto secs = UnixSeconds() + rhs.seconds;
        return DateTime::FromUnixSeconds(secs);
    }

    DateTime& operator+= (const TimeRange& rhs) {
        auto secs = UnixSeconds() + rhs.seconds;
        *this = DateTime::FromUnixSeconds(secs);
        return *this;
    }

    std::string ToString() const {
        std::ostringstream oss;
        oss << std::setw(4) << std::setfill('0') << year << "-"
            << std::setw(2) << std::setfill('0') << month << "-"
            << std::setw(2) << std::setfill('0') << day << " "
            << std::setw(2) << std::setfill('0') << hour << ":"
            << std::setw(2) << std::setfill('0') << minute << ":"
            << std::setw(2) << std::setfill('0') << second;
        return oss.str();
    }

    static bool is_leap_year(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    static int days_in_month(int year, int month) {
        static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month == 2 && is_leap_year(year)) {
            return 29;
        }
        return days[month - 1];
    }

    void validate() const {
        if (year < 1970) throw std::invalid_argument("Year must be >= 1970");
        if (month < 1 || month > 12) throw std::invalid_argument("Invalid month");
        if (day < 1 || day > days_in_month(year, month)) 
            throw std::invalid_argument("Invalid day");
        if (hour < 0 || hour > 23) throw std::invalid_argument("Invalid hour");
        if (minute < 0 || minute > 59) throw std::invalid_argument("Invalid minute");
        if (second < 0 || second > 59) throw std::invalid_argument("Invalid second");
    }

    uint64_t UnixSeconds() const {       
        uint64_t total_seconds = 0;

        // Calculate days since epoch (1970-01-01)
        for (int y = 1970; y < year; ++y) {
            total_seconds += (is_leap_year(y) ? 366 : 365) * SECONDS_PER_DAY;
        }

        // Add seconds for months in current year
        for (int m = 1; m < month; ++m) {
            total_seconds += days_in_month(year, m) * SECONDS_PER_DAY;
        }

        // Add seconds for days in current month
        total_seconds += (day - 1) * SECONDS_PER_DAY;

        // Add seconds for hours, minutes, and seconds
        total_seconds += hour * SECONDS_PER_HOUR;
        total_seconds += minute * SECONDS_PER_MINUTE;
        total_seconds += second;

        return total_seconds;
    }

    static DateTime FromUnixSeconds(uint64_t unix_seconds) {
        DateTime dt(1970, 1, 1);

        int days = unix_seconds / SECONDS_PER_DAY;
        unix_seconds %= SECONDS_PER_DAY;

        while (days > 0) {
            int days_this_year = is_leap_year(dt.year) ? 366 : 365;
            if (days >= days_this_year) {
                days -= days_this_year;
                dt.year++;
            } else {
                break;
            }
        }

        while (days > 0) {
            int days_this_month = days_in_month(dt.year, dt.month);
            if (days >= days_this_month) {
                days -= days_this_month;
                dt.month++;
            } else {
                break;
            }
        }

        dt.day += days;

        dt.hour = unix_seconds / SECONDS_PER_HOUR;
        unix_seconds %= SECONDS_PER_HOUR;

        dt.minute = unix_seconds / SECONDS_PER_MINUTE;
        dt.second = unix_seconds % SECONDS_PER_MINUTE;

        return dt;
    }
};

constexpr DateTime MIN_DATETIME(1970, 1, 1);
constexpr DateTime MAX_DATETIME(2999, 12, 31);

constexpr TimeRange TIMERANGE_DAY(SECONDS_PER_DAY);
constexpr TimeRange TIMERANGE_HOUR(SECONDS_PER_HOUR);
constexpr TimeRange TIMERANGE_MINUTE(SECONDS_PER_MINUTE);
constexpr TimeRange TIMERANGE_SECOND(1);