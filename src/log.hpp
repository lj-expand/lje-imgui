#pragma once
#include <cstdio>

namespace logger {

template<typename... Args>
inline void info(const char* fmt, Args... args) {
  printf("[lje-imgui] ");
  printf(fmt, args...);
  printf("\n");
}

template<typename... Args>
inline void warn(const char* fmt, Args... args) {
  printf("[lje-imgui] WARN: ");
  printf(fmt, args...);
  printf("\n");
}

template<typename... Args>
inline void error(const char* fmt, Args... args) {
  printf("[lje-imgui] ERROR: ");
  printf(fmt, args...);
  printf("\n");
}

} // namespace logger
