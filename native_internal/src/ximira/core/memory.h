
// ================================================================================
// Telegram Hub: @imguisource_bot / ImGui Source @ImGui_Source_FIXNEY / shit src 🧨 @il2cpp_src
// ================================================================================

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace xim::mem {

bool is_readable(const void* addr, std::size_t size);
bool is_writable(const void* addr, std::size_t size);

template <typename T>
inline bool read(uintptr_t addr, T& out) {
    if (!addr || !is_readable(reinterpret_cast<const void*>(addr), sizeof(T)))
        return false;
    std::memcpy(&out, reinterpret_cast<const void*>(addr), sizeof(T));
    return true;
}

template <typename T>
inline T read_or(uintptr_t addr, T fallback = T{}) {
    T v{}; return read(addr, v) ? v : fallback;
}

template <typename T>
inline bool read_at(uintptr_t base, std::size_t off, T& out) {
    return base ? read(base + off, out) : false;
}

template <typename T>
inline bool write(uintptr_t addr, const T& val) {
    if (!addr || !is_writable(reinterpret_cast<const void*>(addr), sizeof(T)))
        return false;
    std::memcpy(reinterpret_cast<void*>(addr), &val, sizeof(T));
    return true;
}

template <typename T>
inline bool write_at(uintptr_t base, std::size_t off, const T& val) {
    return base ? write(base + off, val) : false;
}

bool read_cstr(uintptr_t addr, char* out, std::size_t max_len);
bool read_il2cpp_string(uintptr_t str_obj, char* out, std::size_t max_len);

} // namespace xim::mem
