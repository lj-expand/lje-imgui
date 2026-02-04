#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <vector>

class Module {
public:
  Module(HMODULE handle)
    : base_(reinterpret_cast<uintptr_t>(handle)) {
    auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base_);
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base_ + dos->e_lfanew);
    size_ = nt->OptionalHeader.SizeOfImage;
  }

  Module(const char* name)
    : Module(GetModuleHandleA(name)) {}

  uintptr_t base() const { return base_; }
  size_t size() const { return size_; }

  uintptr_t scan(const char* pattern) const {
    auto [bytes, mask] = parse(pattern);
    auto data = reinterpret_cast<uint8_t*>(base_);

    for (size_t i = 0; i < size_ - bytes.size(); ++i) {
      bool found = true;
      for (size_t j = 0; j < bytes.size(); ++j) {
        if (mask[j] && data[i + j] != bytes[j]) {
          found = false;
          break;
        }
      }
      if (found) return base_ + i;
    }
    return 0;
  }

private:
  uintptr_t base_;
  size_t size_;

  static std::pair<std::vector<uint8_t>, std::vector<bool>> parse(const char* pattern) {
    std::vector<uint8_t> bytes;
    std::vector<bool> mask;

    for (const char* p = pattern; *p; ++p) {
      if (*p == ' ') continue;
      if (*p == '?') {
        bytes.push_back(0);
        mask.push_back(false);
        if (*(p + 1) == '?') ++p;
      } else {
        char byte[3] = { p[0], p[1], 0 };
        bytes.push_back(static_cast<uint8_t>(strtoul(byte, nullptr, 16)));
        mask.push_back(true);
        ++p;
      }
    }
    return { bytes, mask };
  }
};
