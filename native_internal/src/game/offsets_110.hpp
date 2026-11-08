#pragma once
#include <cstdint>
// ARM64 addresses copied from user-supplied offsets.hpp for Standoff 2 1.1.0.
// These represent managed-function RVAs only; they have NOT been validated as
// native call signatures nor connected to Ximira's old object layouts.
namespace huntares::profile_110 {
inline constexpr char kGameVersion[] = "1.1.0";
inline constexpr char kTargetPackage[] = "com.axlebolt.standoff2";
inline constexpr std::uintptr_t kCameraGetMain = 0x6BE9604;
inline constexpr std::uintptr_t kComponentGetTransform = 0x791CF08;
inline constexpr std::uintptr_t kTransformGetPosition = 0x6937790;
inline constexpr bool kPlayerEspOffsetsValidated = false;
inline constexpr bool kLegacyUnityNativeInputHookValidated = false;
}
