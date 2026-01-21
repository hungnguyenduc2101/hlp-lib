#pragma once

#include "config_adapter.h"
#include <stdexcept>
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace hlp {

template <typename T>
class ConfigAdapterManager {
 public:
  static ConfigAdapterManager& instance() {
    static ConfigAdapterManager instance;
    return instance;
  }
  void addAdapter(ConfigAdapter<T>* adapter, std::vector<std::string> types) {
    for (auto& type : types) {
      adapters_[type] = adapter;
    }
  }
  T getConfig(const std::string& file) const noexcept(false) {
    auto pos = file.find_last_of('.');
    if (pos == std::string::npos) {
      throw std::runtime_error(file + " invalid format.");
    }
    auto ext = file.substr(pos + 1);
    auto it = adapters_.find(ext);
    if (it == adapters_.end()) {
      throw std::runtime_error("No valid parser for this config file!");
    }
    return it->second->getConfig(file);
  }

 private:
  ConfigAdapterManager() {
  }
  std::map<std::string, ConfigAdapter<T>*> adapters_;
};
}  // namespace hlp
