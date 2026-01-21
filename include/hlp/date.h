#pragma once

#include <cstdint>
#include <string>

#define MICRO_SECONDS_PRE_SEC 1000000LL

namespace hlp {

class Date {
 public:
  Date() = default;
  ~Date() = default;

  explicit Date(int64_t microSec) : microSecondsSinceEpoch_(microSec) {
  }
  Date(uint32_t year, uint32_t month, uint32_t day, uint32_t hour = 0,
       uint32_t minute = 0, uint32_t second = 0, uint32_t microSecond = 0);

  static const Date date();
  static Date now() {
    return date();
  }

  static int64_t timezoneOffset() {
    static int64_t offset = -(
            Date::fromDbStringLocal("1970-01-03 00:00:00").secondsSinceEpoch() -
            2LL * 3600LL * 24LL);
    return offset;
  }
  const Date after(double second) const;
  const Date roundSecond() const;
  const Date roundDay() const;

  bool operator==(const Date& date) const {
    return microSecondsSinceEpoch_ == date.microSecondsSinceEpoch_;
  }

  bool operator!=(const Date& date) const {
    return microSecondsSinceEpoch_ != date.microSecondsSinceEpoch_;
  }

  bool operator<(const Date& date) const {
    return microSecondsSinceEpoch_ < date.microSecondsSinceEpoch_;
  }
  bool operator>(const Date& date) const {
    return microSecondsSinceEpoch_ > date.microSecondsSinceEpoch_;
  }
  bool operator>=(const Date& date) const {
    return microSecondsSinceEpoch_ >= date.microSecondsSinceEpoch_;
  }
  bool operator<=(const Date& date) const {
    return microSecondsSinceEpoch_ <= date.microSecondsSinceEpoch_;
  }
  int64_t microSecondsSinceEpoch() const {
    return microSecondsSinceEpoch_;
  }
  int64_t secondsSinceEpoch() const {
    return microSecondsSinceEpoch_ / MICRO_SECONDS_PRE_SEC;
  }

  struct tm tmStruct() const;

  std::string toFormattedString(bool is_show_us) const;
  std::string toFormattedStringLocal(bool is_show_us) const;
  std::string toCustomFormattedString(const std::string& fmtStr,
                                      bool is_show_us = false) const;
  std::string toCustomFormattedStringLocal(const std::string& fmtStr,
                                           bool is_show_us = false) const;
  std::string toDbString() const;
  std::string toDbStringLocal() const;

  static Date fromDbString(const std::string& datetime);
  static Date fromDbStringLocal(const std::string& datetime);

  void toCustomFormattedString(const std::string& fmtStr, char* str,
                               size_t len) const;

  bool isSameSecond(const Date& date) const {
    return microSecondsSinceEpoch_ / MICRO_SECONDS_PRE_SEC ==
           date.secondsSinceEpoch();
  }

  void swap(Date& that) {
    std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
  }

 private:
  int64_t microSecondsSinceEpoch_{};
};
}  // namespace hlp
