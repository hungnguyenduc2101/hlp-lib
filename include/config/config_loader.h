#pragma once

#include "config_adapter_manager.h"
#include "hlp/non_copyable.h"
#include "hlp/path.h"
#include <iostream>
#include <string>

#if !defined(_WIN32)
#include <unistd.h>
#define os_access access
#else
#include <io.h>
#ifndef __MINGW32__
#define os_access _waccess
#define R_OK 04
#define W_OK 02
#else
#define os_access access
#endif
#endif

namespace hlp {
template <typename T>
class ConfigLoader : public NonCopyable {
 public:
  explicit ConfigLoader(const std::string& config_file) noexcept(false) {
    if (os_access(hlp::toNativePath(config_file).c_str(), R_OK)) {
      throw std::runtime_error("Config file " + config_file + " not found!");
    }
    if (os_access(hlp::toNativePath(config_file).c_str(), R_OK)) {
      throw std::runtime_error("No permission to read config file " +
                               config_file);
    }
    config_file_ = config_file;

    try {
      config_ = ConfigAdapterManager<T>::instance().getConfig(config_file);
    } catch (const std::exception& e) {
      throw std::runtime_error("Error read config file " + config_file + ": " +
                               e.what());
    }
  }

  T& getConfig() {
    return config_;
  }
  virtual void load() noexcept(false) = 0;

 private:
  std::string config_file_{};
  T config_{};
};
}  // namespace hlp
