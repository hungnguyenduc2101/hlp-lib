#pragma once

#include <memory>
#include "hlp/date.h"
#include "log_stream.h"
#include <iostream>

#define LOGGER_IF_(cond) for (int _r{ 0 }; _r == 0 && cond; ++_r)

namespace spdlog {
class logger;
}

namespace hlp {
enum LogLevel {
  TRACE = 0,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NUMBER_OF_LOG_LEVELS
};
class RawLogger : public NonCopyable {
 public:
  ~RawLogger();
  RawLogger& setIndex(int index) {
    index_ = index;
    return *this;
  }
  LogStream& stream() {
    return log_stream_;
  }

 private:
  LogStream log_stream_{};
  int index_{ -1 };
};
class Logger : public NonCopyable {
 public:
  enum LogLevel {
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUMBER_OF_LOG_LEVELS
  };
  class SourceFile {
   public:
    template <int N>
    SourceFile(const char (&arr)[N]) : data_(arr),
                                       size_(N - 1) {
      const char* slash = strrchr(data_, '/');
      if (slash) {
        data_ = slash + 1;
        size_ -= static_cast<int>(data_ - arr);
      }
    }

    explicit SourceFile(const char* filename = nullptr) : data_(filename) {
      if (!filename) {
        size_ = 0;
        return;
      }
      const char* slash = strrchr(filename, '/');
      if (slash) {
        data_ = slash + 1;
      }
      size_ -= static_cast<int>(strlen(data_));
    }
    const char* data_;
    int size_;
  };
  Logger(SourceFile file, int line);
  Logger(SourceFile file, int line, LogLevel level);
  Logger(SourceFile file, int line, bool is_sys_err);
  Logger(SourceFile file, int line, LogLevel level, const char* func);

  // LOG COMPACT <time><thread_id><level>
  Logger();
  Logger(LogLevel level);
  Logger(bool is_sys_err);

  void extracted();
  ~Logger();

  Logger& setIndex(int index) {
    index_ = index;
    return *this;
  }
  LogStream& stream() {
    return log_stream_;
  }

  static void setOutputFunction(
          std::function<void(const char* msg, const uint64_t len)> outputFunc,
          std::function<void()> flushFunc, int index = -1) {
    if (index < 0) {
      outputFunc_() = outputFunc;
      flushFunc_() = flushFunc;
    } else {
      outputFunc_(index) = outputFunc;
      flushFunc_(index) = flushFunc;
    }
  }

  static void setLogLevel(LogLevel level) {
    logLevel_() = level;
  }

  static LogLevel logLevel() {
    return logLevel_();
  }

  static bool displayLocalTime() {
    return displayLocalTime_();
  }

  static bool setDisplayLocalTime(bool showLocal) {
    displayLocalTime_() = showLocal;
    return showLocal;
  }

  static bool hasSpdLogSupport();
  static void enableSpdLog(int index,
                           std::shared_ptr<spdlog::logger> logger = {});
  static void enableSpdLog(std::shared_ptr<spdlog::logger> logger = {}) {
    return enableSpdLog(-1, logger);
  }
  static void disableSpdLog(int index);
  static void disableSpdLog() {
    return disableSpdLog(-1);
  }

  static std::shared_ptr<spdlog::logger> getSpdLogger(int index = -1);
  static std::shared_ptr<spdlog::logger> getDefaultSpdLogger(int index);

 protected:
  static void defaultOutputFunction(const char* msg, const uint64_t len) {
    fwrite(msg, sizeof(char), len, stdout);
  }
  static void defaultFlushFunction() {
    fflush(stdout);
  }

  static std::function<void(const char* msg, const uint64_t len)>&
  outputFunc_() {
    static std::function<void(const char* msg, const uint64_t len)> outputFunc =
            Logger::defaultOutputFunction;
    return outputFunc;
  }

  static bool isDefaultOutputFunc(size_t index = -1) {
    using FuncType = void (*)(const char*, const uint64_t);
    FuncType* target_ptr = index < 0 ? outputFunc_().target<FuncType>()
                                     : outputFunc_(index).target<FuncType>();
    return target_ptr != nullptr && *target_ptr == &defaultOutputFunction;
  }

  static std::function<void()>& flushFunc_() {
    static std::function<void()> flushFunc = Logger::defaultFlushFunction;
    return flushFunc;
  }

  static std::function<void(const char* msg, const uint64_t len)>&
  outputFunc_(size_t index) {
    static std::vector<std::function<void(const char* msg, const uint64_t len)>>
            outputFuncs;

    if (index < outputFuncs.size()) {
      return outputFuncs[index];
    }

    while (index >= outputFuncs.size()) {
      outputFuncs.emplace_back(outputFunc_());
    }
    return outputFuncs[index];
  }

  static std::function<void()>& flushFunc_(size_t index) {
    static std::vector<std::function<void()>> flushFuncs;
    if (index < flushFuncs.size()) {
      return flushFuncs[index];
    }
    while (index >= flushFuncs.size()) {
      flushFuncs.emplace_back(flushFunc_());
    }
    return flushFuncs[index];
  }

  static LogLevel& logLevel_() {
#ifdef RELEASE
    static LogLevel logLevel = LogLevel::INFO;
#else
    static LogLevel logLevel = LogLevel::DEBUG;

#endif
    return logLevel;
  }

  static bool& displayLocalTime_() {
    static bool showLocal{ false };
    return showLocal;
  }

  void formatTime();
  LogStream log_stream_;
  Date date_{ Date::now() };
  SourceFile source_file_;
  int file_line_;
  LogLevel log_level_;
  int index_{ -1 };
  const char* func_{ nullptr };
  size_t spdLogMessageOffset_{ 0 };

  friend class RawLogger;
};

#ifdef NLOG
#define LOG_TRACE                                                              \
  LOGGER_IF_(0)                                                                \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::TRACE, __func__)      \
          .stream()
#else
#define LOG_TRACE                                                              \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::TRACE)          \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::TRACE, __func__)      \
          .stream()
#define LOG_TRACE_TO(index)                                                    \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::TRACE)          \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::TRACE, __func__)      \
          .setIndex(index)                                                     \
          .stream()
#define LOG_TRACE_IF(cond)                                                     \
  LOGGER_IF_((hlp::Logger::logLevel() <= hlp::Logger::LogLevel::TRACE) &&      \
             (cond))                                                           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::TRACE, __func__)      \
          .stream()
#endif

#define LOG_DEBUG_COMPACT                                                      \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::DEBUG)          \
  hlp::Logger(hlp::Logger::LogLevel::DEBUG).stream()
#define LOG_DEBUG                                                              \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::DEBUG)          \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::DEBUG, __func__)      \
          .stream()
#define LOG_DEBUG_COMPACT_TO(index)                                            \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::DEBUG)          \
  hlp::Logger(hlp::Logger::LogLevel::DEBUG).setIndex(index).stream()
#define LOG_DEBUG_TO(index)                                                    \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::DEBUG)          \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::DEBUG, __func__)      \
          .setIndex(index)                                                     \
          .stream()
#define LOG_DEBUG_IF(cond)                                                     \
  LOGGER_IF_((hlp::Logger::logLevel() <= hlp::Logger::LogLevel::DEBUG) &&      \
             (cond))                                                           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::DEBUG, __func__)      \
          .stream()

#define LOG_INFO_COMPACT                                                       \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::INFO)           \
  hlp::Logger(hlp::Logger::LogLevel::INFO).stream()
#define LOG_INFO                                                               \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::INFO)           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::INFO).stream()
#define LOG_INFO_COMPACT_TO(index)                                             \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::INFO)           \
  hlp::Logger(hlp::Logger::LogLevel::INFO).setIndex(index).stream()
#define LOG_INFO_TO(index)                                                     \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::INFO)           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::INFO)                 \
          .setIndex(index)                                                     \
          .stream()
#define LOG_INFO_IF(cond)                                                      \
  LOGGER_IF_((hlp::Logger::logLevel() <= hlp::Logger::LogLevel::INFO) &&       \
             (cond))                                                           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::INFO, __func__)       \
          .stream()

#define LOG_WARN_COMPACT                                                       \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::WARN)           \
  hlp::Logger(hlp::Logger::LogLevel::WARN).stream()
#define LOG_WARN                                                               \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::WARN)           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::WARN).stream()
#define LOG_WARN_COMPACT_TO(index)                                             \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::WARN)           \
  hlp::Logger(hlp::Logger::LogLevel::WARN).setIndex(index).stream()
#define LOG_WARN_TO(index)                                                     \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::WARN)           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::WARN)                 \
          .setIndex(index)                                                     \
          .stream()
#define LOG_WARN_IF(cond)                                                      \
  LOGGER_IF_((hlp::Logger::logLevel() <= hlp::Logger::LogLevel::WARN) &&       \
             (cond))                                                           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::WARN, __func__)       \
          .stream()

#define LOG_ERROR_COMPACT                                                      \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::ERROR)          \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::ERROR).stream()
#define LOG_ERROR                                                              \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::ERROR)          \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::ERROR).stream()
#define LOG_ERROR_COMPACT_TO(index)                                            \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::ERROR)          \
  hlp::Logger(hlp::Logger::LogLevel::ERROR).setIndex(index).stream()
#define LOG_ERROR_TO(index)                                                    \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::ERROR)          \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::ERROR)                \
          .setIndex(index)                                                     \
          .stream()
#define LOG_ERROR_IF(cond)                                                     \
  LOGGER_IF_((hlp::Logger::logLevel() <= hlp::Logger::LogLevel::ERROR) &&      \
             (cond))                                                           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::ERROR, __func__)      \
          .stream()

#define LOG_FATAL_COMPACT                                                      \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::FATAL)          \
  hlp::Logger(hlp::Logger::LogLevel::FATAL).stream()
#define LOG_FATAL                                                              \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::FATAL)          \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::FATAL).stream()
#define LOG_FATAL_COMPACT_TO(index)                                            \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::FATAL)          \
  hlp::Logger(hlp::Logger::LogLevel::FATAL).setIndex(index).stream()
#define LOG_FATAL_TO(index)                                                    \
  LOGGER_IF_(hlp::Logger::logLevel() <= hlp::Logger::LogLevel::FATAL)          \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::FATAL)                \
          .setIndex(index)                                                     \
          .stream()
#define LOG_FATAL_IF(cond)                                                     \
  LOGGER_IF_((hlp::Logger::logLevel() <= hlp::Logger::LogLevel::FATAL) &&      \
             (cond))                                                           \
  hlp::Logger(__FILE__, __LINE__, hlp::Logger::LogLevel::FATAL).stream()

#define LOG_SYSERR hlp::Logger(__FILE__, __LINE__, true).stream()
#define LOG_SYSERR_TO(index)                                                   \
  hlp::Logger(__FILE__, __LINE__, true).setIndex(index).stream()

#define LOG_RAW hlp::RawLogger().stream()
#define LOG_RAW_TO(index) hlp::RawLogger().setIndex(index).stream()

const char* strerror_tl(int savedErrno);

}  // namespace hlp
