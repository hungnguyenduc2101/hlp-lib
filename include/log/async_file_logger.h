#pragma once

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include "hlp/date.h"
#include "hlp/non_copyable.h"

namespace hlp {
using StringPtr = std::shared_ptr<std::string>;
using StringPtrQueue = std::queue<StringPtr>;

class AsyncFileLogger : NonCopyable {
 public:
  AsyncFileLogger();
  ~AsyncFileLogger();

  void output(const char* msg, const uint64_t len);
  void flush();
  void startLogging();
  void setFileSizeLimit(uint64_t size_limit) {
    size_limit_ = size_limit;
  }
  void setMaxFiles(size_t max_files) {
    max_files_ = max_files;
  }
  void setSwitchOnLimitOnly(bool flag = true) {
    switchOnLimitOnly_ = flag;
  }
  void setFilename(const std::string& basename,
                   const std::string& extname = ".log",
                   const std::string& filepath = "./") {
    file_basename_ = basename;
    file_extname_ = (extname[0] == '.') ? extname : "." + extname;
    file_path_ = filepath;
    if (file_path_.empty())
      file_path_ = "./";
    if (file_path_[file_path_.length() - 1] != '/')
      file_path_ += "/";
  }

 protected:
  std::mutex mutex_;
  std::condition_variable cond_;
  std::unique_ptr<std::thread> thread_ptr_;
  StringPtr log_buf_ptr_;
  StringPtr next_buf_ptr_;
  StringPtrQueue write_buffers_;
  StringPtrQueue tmp_buffers_;

  void writeLogToFile(const StringPtr buf);
  void logThreadFunc();

  bool stop_flag_{ false };
  std::string file_path_{ "./" };
  std::string file_basename_{ "help" };
  std::string file_extname_{ ".log" };
  uint64_t size_limit_{ 20 * 1024 * 1024 };
  bool switchOnLimitOnly_{ false };
  size_t max_files_{ 0 };

  class LoggerFile : NonCopyable {
   public:
    LoggerFile(const std::string& file_path, const std::string& file_basename,
               const std::string& file_extname, bool switchOnLimitOnly = false,
               size_t max_files = 0);
    ~LoggerFile();
    void writeLog(const StringPtr buf);
    void flush();
    void open();
    void switchLog(bool open_new_one);
    uint64_t getLength();
    explicit operator bool() const {
      return fp_ != nullptr;
    }

   protected:
    void initFilenameQueue();
    void deleteOldFile();

    FILE* fp_{ nullptr };
    Date creation_date_;
    std::string file_path_;
    std::string file_basename_;
    std::string file_extname_;
    std::string fullname_;
    static uint64_t file_seq_;
    bool switchOnLimitOnly_{ false };
    size_t max_files_{ 0 };
    std::deque<std::string> filename_queue_;
  };
  std::unique_ptr<LoggerFile> log_file_ptr_;
  uint64_t lost_counter_{ 0 };
  void swapBuffer();
};

}  // namespace hlp
