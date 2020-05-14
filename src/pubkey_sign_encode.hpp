#pragma once
#include "type_aliases.hpp"
namespace cryp {
// message encoding method abstract class
class SignEncode {
   protected:
    LenT emlen_;

   public:
    explicit SignEncode(LenT emlen = 0) : emlen_(emlen) {}
    virtual int Encode(const ByteT* msg, LenT msg_len, ByteT* dst,
                       LenT dst_len) = 0;
    virtual int Verify(const ByteT* msg, LenT msg_len, const ByteT* encoded,
                       LenT src_len) = 0;
};
}  // namespace cryp
