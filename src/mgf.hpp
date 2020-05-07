#pragma once
#include <cstdint>
namespace cryp {
// mask generation function abstract class
class MaskGenFunc {
   public:
    // return value: 0) ok, other) fail
    virtual int Generate(const uint8_t* src, uint64_t src_len, uint8_t* dst,
                         uint64_t dst_len) = 0;
};
}  // namespace cryp
