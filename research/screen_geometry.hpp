
// ================================================================================
// TG: @imguisource_bot | @ImGui_Source_FIXNEY | @il2cpp_src
// ================================================================================

#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace screen_geometry {
struct Config {
    static constexpr std::uintptr_t ProjectRva=0x449bb0,ValidateRva=0x449d10,PrepareRva=0x449f30;
    static constexpr std::uintptr_t SkeletonBoundsRva=0x44523c,SimpleBoundsRva=0x4455e4;
    static constexpr std::size_t BoneCount=16,MinimumBones=6;
    static constexpr float NearW=.05f;
};
struct Vec2 {float x{},y{};};
struct Vec3 {float x{},y{},z{};};
struct Rect {Vec2 minimum{},maximum{};};
using Matrix=std::array<float,16>;
using Bones=std::array<Vec3,Config::BoneCount>;
struct Projection {Matrix combined{};Vec2 halfDisplay{};float maximumScale{};};
static_assert(sizeof(Projection)==0x4c);
static_assert(sizeof(Rect)==0x10);
static_assert(sizeof(Bones)==0xc0);
// Исходные матрицы column-major, combined row-major, как native.
Projection prepare(const Matrix&,const Matrix& localToWorld,Vec2 display) noexcept;
bool project(Vec3,const Matrix&,Vec2 display,Vec2& output) noexcept;
// UI scale задаётся одним снимком; native обращается к getter.
bool validate(const Rect&,Vec2 display,Vec2 uiScale) noexcept;
bool simple_bounds(float stroke,Vec3,const Matrix&,Vec2 display,Vec2 uiScale,Rect& output) noexcept;
bool skeleton_bounds(float padding,const Bones&,const Matrix&,Vec2 display,Vec2 uiScale,Rect& output) noexcept;
}
