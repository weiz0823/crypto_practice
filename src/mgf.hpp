#pragma once
#include "type_aliases.hpp"
namespace cryp {
// mask generation function abstract class
class MaskGenFunc {
   public:
    // return value: 0) ok, other) fail
    virtual int Generate(const ByteT* src, LenT src_len, ByteT* dst,
                         LenT dst_len) = 0;
};
}  // namespace cryp
