#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

namespace hlp {

inline std::vector<std::string> splitString(const std::string& str,
                                            const std::string& delimiter,
                                            bool is_accept_empty = false) {
  std::vector<std::string> v{};
  if (delimiter.empty()) {
    return v;
  }

  std::string::size_type last{};
  std::string::size_type next{};

  std::string str_t{str};

  while ((next = str.find(delimiter, last)) != std::string::npos) {
    if (next > last || is_accept_empty) {
      v.push_back(str.substr(last, next - last));
    }
    last = next + delimiter.size();
  }

  if (str.length() > last || is_accept_empty) {
    v.push_back(str.substr(last));
  }
  return v;
}
inline const std::string& toNativePath(const std::string& strPath) {
  return strPath;
}

inline const std::string& toNativePath(const std::wstring& strPath) {
  return toNativePath(strPath);
}

inline int createPath(const std::string& path) {
  if (path.empty()) {
    std::cerr << "Path is empty\n";
    return -1;
  }

  auto os_path{ toNativePath(path) };
  if (os_path.back() != std::filesystem::path::preferred_separator) {
    os_path.push_back(std::filesystem::path::preferred_separator);
  }

  std::filesystem::path fs_path{ os_path };
  std::error_code err;
  std::filesystem::create_directories(fs_path, err);

  if (err) {
    std::cerr << "Error " << err.value() << " creating path " << os_path
              << " : " << err.message() << "\n";
    return -1;
  }

  std::cout << "Created path " << os_path << " successfully\n";
  
  return 0;
}

}  // namespace hlp
