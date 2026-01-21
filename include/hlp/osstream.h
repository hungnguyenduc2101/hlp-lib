#pragma once

#include <string>
#include <sstream>
#include <utility>

namespace hlp {
namespace internal {
template <typename T, typename = void>
struct CanConvertToString : std::false_type {};

template <typename T>
struct CanConvertToString<
        T, std::void_t<decltype(std::to_string(std::declval<T>()))>>
        : std::true_type {};
}  // namespace internal

class OSStream {
 public:
  OSStream() = default;

  void reserve(size_t size) {
    buffer_.reserve(size);
  }

  template <typename T>
  OSStream& operator<<(T&& value) {
    if constexpr (internal::CanConvertToString<T>::value) {
      buffer_.append(std::to_string(std::forward<T>(value)));
      return *this;
    } else {
      std::stringstream ss;
      ss << std::forward<T>(value);
      buffer_.append(ss.str());
      return *this;
    }
  }

  template <int N>
  OSStream& operator<<(const char (&buf)[N]) {
    buffer_.append(buf, N - 1);
    return *this;
  }

  OSStream& operator<<(const std::string_view& str) {
    buffer_.append(str.data(), str.length());
    return *this;
  }

  OSStream& operator<<(std::string_view&& str) {
    buffer_.append(str.data(), str.length());
    return *this;
  }

  OSStream& operator<<(const std::string& str) {
    buffer_.append(str);
    return *this;
  }

  OSStream& operator<<(std::string&& str) {
    buffer_.append(std::move(str));
    return *this;
  }

  OSStream& operator<<(const double& d) {
    std::stringstream ss;
    ss << d;
    buffer_.append(ss.str());
    return *this;
  }

  OSStream& operator<<(const float& f) {
    std::stringstream ss;
    ss << f;
    buffer_.append(ss.str());
    return *this;
  }

  OSStream& operator<<(double&& d) {
    std::stringstream ss;
    ss << d;
    buffer_.append(ss.str());
    return *this;
  }

  OSStream& operator<<(float&& f) {
    std::stringstream ss;
    ss << f;
    buffer_.append(ss.str());
    return *this;
  }

  std::string& str() {
    return buffer_;
  }

  const std::string& str() const {
    return buffer_;
  }

 private:
  std::string buffer_;
};
}  // namespace hlp
