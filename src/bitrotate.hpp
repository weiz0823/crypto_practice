#pragma once
#include <cstdint>
namespace cryp {
constexpr uint32_t LeftRotate32(uint32_t x, uint32_t n) {
    return (x << n) | (x >> (32 - n));
}
constexpr uint32_t RightRotate32(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}
constexpr uint64_t LeftRotate64(uint64_t x, uint64_t n) {
    return (x << n) | (x >> (64 - n));
}
constexpr uint64_t RightRotate64(uint64_t x, uint64_t n) {
    return (x >> n) | (x << (64 - n));
}
}  // namespace cryp
