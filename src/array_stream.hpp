#pragma once
#include <cstdint>
namespace cryp {
class ArrayIstream {
    const uint8_t* arr_;
    const uint64_t len_;
    uint64_t read_pos_ = 0;

   public:
    ArrayIstream(const uint8_t* array, uint64_t size)
        : arr_(array), len_(size) {}
    inline void Reset() { read_pos_ = 0; }
    uint64_t Read(uint8_t* buf, uint64_t count);
};
}  // namespace cryp
