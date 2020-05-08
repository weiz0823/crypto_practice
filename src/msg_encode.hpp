#pragma once
#include "type_aliases.hpp"
namespace cryp {
// message encoding method abstract class
class MsgEncode {
   protected:
    LenT emlen_;

   public:
    explicit MsgEncode(LenT emlen = 0) : emlen_(emlen) {}
    virtual int Encode(const ByteT* msg, LenT msg_len, ByteT* dst,
                       LenT dst_len) = 0;
    virtual int Decode(const ByteT* encoded, LenT src_len, BytesT* dst) = 0;
};
}  // namespace cryp
