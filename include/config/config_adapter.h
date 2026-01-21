#pragma once

#include <memory>
#include <vector>
#include <string>
#include <fstream>

namespace hlp {
template <typename T>
class ConfigAdapter {
 public:
  virtual ~ConfigAdapter() = default;
  virtual T getConfig(const std::string& config_file) const noexcept(false) = 0;
  virtual std::vector<std::string> getExtensions() const = 0;
  static void createAdapterIns();
};

template <typename T>
using ConfigAdapterPtr = std::shared_ptr<ConfigAdapter<T>>;
}  // namespace hlp

#include "config_adapter_manager.h"

#define REGISTER_ADAPTER(adapter, param)                                       \
  hlp::adapter::adapter() {                                                    \
    hlp::ConfigAdapterManager<param>::instance().addAdapter(this,              \
                                                            getExtensions());  \
  }                                                                            \
  auto tmp_##adapter = hlp::adapter();

#define CREATE_ADAPTER(adapter, param, ext)                                    \
  class adapter : public hlp::ConfigAdapter<param> {                           \
   public:                                                                     \
    adapter() {                                                                \
      hlp ::ConfigAdapterManager<param>::instance().addAdapter(                \
              this, getExtensions());                                          \
    }                                                                          \
    ~adapter() override = default;                                             \
    param getConfig(const std::string& config_file) const                      \
            noexcept(false) override;                                          \
    std::vector<std::string> getExtensions() const override;                   \
  };                                                                           \
  auto tmp_##adapter = adapter();                                              \
  std::vector<std::string> adapter::getExtensions() const {                    \
    return { #ext };                                                           \
  }                                                                            \
  param adapter::getConfig(const std::string& config_file)                     \
          const noexcept(false) {                                              \
    std::ifstream file(config_file);                                           \
    param params;
#define ADAPTER_END()                                                          \
  return params;                                                               \
  }
