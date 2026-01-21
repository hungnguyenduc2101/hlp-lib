#pragma once

namespace hlp {
  class NonCopyable {
    protected:
      NonCopyable() = default;
      ~NonCopyable() = default;

      NonCopyable(const NonCopyable&) = delete;
      NonCopyable& operator=(const NonCopyable&) = delete;

      NonCopyable(NonCopyable&&) noexcept(true) = default;
      NonCopyable& operator=(NonCopyable&&) noexcept(true) = default;
  };
}
