#pragma once

#include "hlp/non_copyable.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace hlp {
class Fmt {
 public:
  template <typename T>
  Fmt(const char* fmt, T value);

  const char* data() const {
    return buf_;
  }

  int length() const {
    return length_;
  }

 private:
  char buf_[48];
  int length_{};
};

namespace detail {
static constexpr size_t kSmallBuffer{ 4000 };
static constexpr size_t kLargeBuffer{ kSmallBuffer * 1000 };

inline const char digits[] = "9876543210123456789";
inline const char digits_hex[] = "0123456789ABCDEF";
inline const char* zero = digits + 9;

template <typename T>
size_t convert(char buf[], T value) {
  T i = value;
  char* p = buf;

  do {
    int lsd = static_cast<int>(i % 10);
    *p++ = zero[lsd];
    i /= 10;

  } while (i);

  if (value < 0) {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}
inline size_t convertHex(char buf[], uintptr_t value) {
  uintptr_t i = value;
  char* p = buf;

  do {
    int lsd = static_cast<int>(i % 16);
    *p++ = digits[lsd];
    i /= 16;

  } while (i);

  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

template <int SIZE>
class FixedBuffer : NonCopyable {
 public:
  FixedBuffer() : cur_(data_) {
    setCookie(cookieStart);
  }
  ~FixedBuffer() {
    setCookie(cookieEnd);
  }

  int avail() const {
    return static_cast<int>(end() - cur_);
  }

  bool append(const char* buf, size_t len) {
    if (static_cast<size_t>(avail()) > len) {
      memcpy(cur_, buf, len);
      cur_ += len;
      return true;
    }
    return false;
  }

  int length() const {
    return static_cast<int>(cur_ - data_);
  }

  char* current() const {
    return cur_;
  }

  void clear() {
    cur_ = data_;
  }

  void zeroBuffer() {
    memset(data_, 0, sizeof(data_));
  }

  const char* data() const {
    return data_;
  }
  const char* start() const {
    return data_;
  }
  const char* end() const {
    return data_ + sizeof(data_);
  }

  void add(size_t len) {
    cur_ += len;
  }

 private:
  const char* debugString() {
    *cur_ = '\0';
    return data_;
  }
  void setCookie(void (*cookie)()) {
    cookie_ = cookie;
  }
  static void cookieStart(){}
  static void cookieEnd(){}

  void (*cookie_)();
  char data_[SIZE];
  char* cur_{};
};
}  // namespace detail
class LogStream : NonCopyable {
  using sel = LogStream;
  using Buffer = detail::FixedBuffer<detail::kSmallBuffer>;

 public:
  void append(const char* data, size_t len) {
    if (ex_buffer_.empty()) {
      if (!buffer_.append(data, len)) {
        ex_buffer_.append(buffer_.start(), buffer_.length());
        ex_buffer_.append(data, len);
      }
    } else {
      ex_buffer_.append(data, len);
    }
  }

  const char* bufferData() const {
    if (ex_buffer_.empty()) {
      return buffer_.start();
    }
    return ex_buffer_.data();
  }

  size_t bufferLength() {
    if (ex_buffer_.empty()) {
      return buffer_.length();
    }
    return ex_buffer_.length();
  }

  void clearBuffer() {
    buffer_.clear();
    ex_buffer_.clear();
  }

  sel& operator<<(bool v) {
    append(v ? "1" : "0", 1);
    return *this;
  }
  sel& operator<<(char v) {
    append(&v, 1);
    return *this;
  }
  sel& operator<<(const char* str) {
    if (str) {
      append(str, strlen(str));
    } else {
      append("(null)", 6);
    }
    return *this;
  }
  sel& operator<<(char* str) {
    return operator<<(reinterpret_cast<const char*>(str));
  }
  sel& operator<<(const unsigned char* str) {
    return operator<<(reinterpret_cast<const char*>(str));
  }
  sel& operator<<(const std::string& v) {
    append(v.c_str(), v.size());
    return *this;
  }
  sel& operator<<(std::string_view v) {
    append(v.data(), v.size());
    return *this;
  }
  template <int N>
  sel& operator<<(const char (&buf)[N]) {
    assert(strnlen(buf, N) == N - 1);
    append(buf, N - 1);
    return *this;
  }

  sel& operator<<(const Fmt& v) {
    append(v.data(), v.length());
    return *this;
  }
  sel& operator<<(int v) {
    formatInteger(v);
    return *this;
  }
  sel& operator<<(unsigned int v) {
    formatInteger(v);
    return *this;
  }
  sel& operator<<(short v) {
    return operator<<(static_cast<int>(v));
  }
  sel& operator<<(unsigned short v) {
    return operator<<(static_cast<unsigned int>(v));
  }
  sel& operator<<(long v) {
    formatInteger(v);
    return *this;
  }
  sel& operator<<(unsigned long v) {
    formatInteger(v);
    return *this;
  }
  sel& operator<<(const long long& v) {
    formatInteger(v);
    return *this;
  }
  sel& operator<<(const unsigned long long& v) {
    formatInteger(v);
    return *this;
  }
  sel& operator<<(const double& v) {
    constexpr static int kMaxNumericSize = 32;
    if (ex_buffer_.empty()) {
      if (buffer_.avail() >= kMaxNumericSize) {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
        return *this;
      }
      ex_buffer_.append(buffer_.data(), buffer_.length());
    }
    auto old_len = ex_buffer_.length();
    ex_buffer_.resize(old_len + kMaxNumericSize);
    int len = snprintf(&ex_buffer_[old_len], kMaxNumericSize, "%.12g", v);
    ex_buffer_.resize(old_len + len);
    return *this;
  }
  sel& operator<<(const long double& v) {
    constexpr static int kMaxNumericSize = 48;
    if (ex_buffer_.empty()) {
      if (buffer_.avail() >= kMaxNumericSize) {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
        buffer_.add(len);
        return *this;
      }
      ex_buffer_.append(buffer_.data(), buffer_.length());
    }
    auto old_len = ex_buffer_.length();
    ex_buffer_.resize(old_len + kMaxNumericSize);
    int len = snprintf(&ex_buffer_[old_len], kMaxNumericSize, "%.12Lg", v);
    ex_buffer_.resize(old_len + len);
    return *this;
  }
  sel& operator<<(float& v) {
    return operator<<(static_cast<double>(v));
  }

  sel& operator<<(const void* p) {
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    constexpr static int kMaxNumericSize =
            std::numeric_limits<uintptr_t>::digits / 4 + 4;
    if (ex_buffer_.empty()) {
      if (buffer_.avail() >= kMaxNumericSize) {
        char* buf = buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = detail::convertHex(buf + 2, v);
        buffer_.add(len + 2);
        return *this;
      } else {
        ex_buffer_.append(buffer_.data(), buffer_.length());
      }
    }
    auto old_len = ex_buffer_.length();
    ex_buffer_.resize(old_len + kMaxNumericSize);
    char* buf = &ex_buffer_[old_len];
    buf[0] = '0';
    buf[1] = 'x';
    size_t len = detail::convertHex(buf + 2, v);
    ex_buffer_.resize(old_len + len + 2);
    return *this;
  }

 private:
  Buffer buffer_{};
  std::string ex_buffer_{};

  template <typename T>
  void formatInteger(T v) {
    static constexpr int kMaxNumericSize = std::numeric_limits<T>::digits10 + 4;
    if (ex_buffer_.empty()) {
      if (buffer_.avail() >= kMaxNumericSize) {
        size_t len = detail::convert(buffer_.current(), v);
        buffer_.add(len);
        return;
      }
      ex_buffer_.append(buffer_.start(), buffer_.length());
    }
    auto old_len = ex_buffer_.length();
    ex_buffer_.resize(old_len + kMaxNumericSize);
    size_t len = detail::convert(&ex_buffer_[old_len], v);
    ex_buffer_.resize(old_len + len);
  }
};
}  // namespace hlp
