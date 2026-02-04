#pragma once
#include <MinHook.h>
#include <cstdint>

template<typename Fn>
class Hook {
public:
  Hook() = default;

  bool create(uintptr_t target, Fn detour) {
    target_ = reinterpret_cast<void*>(target);
    auto status = MH_CreateHook(target_, reinterpret_cast<void*>(detour), reinterpret_cast<void**>(&original_));
    return status == MH_OK;
  }

  bool enable() {
    return MH_EnableHook(target_) == MH_OK;
  }

  bool disable() {
    return MH_DisableHook(target_) == MH_OK;
  }

  bool remove() {
    return MH_RemoveHook(target_) == MH_OK;
  }

  Fn original() const { return original_; }

  template<typename... Args>
  auto call(Args... args) const {
    return original_(args...);
  }

private:
  void* target_ = nullptr;
  Fn original_ = nullptr;
};

namespace hook {

inline bool init() {
  return MH_Initialize() == MH_OK;
}

inline bool shutdown() {
  return MH_Uninitialize() == MH_OK;
}

} // namespace hook
