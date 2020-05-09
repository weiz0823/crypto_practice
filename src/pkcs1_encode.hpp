#pragma once
#include <iostream>
#include "msg_encode.hpp"
#include "random_engine.hpp"
namespace cryp {
class EMEPKCS1 final : public MsgEncode {
   public:
    int Encode(const ByteT* msg, LenT msg_len, ByteT* dst,
               LenT dst_len) override;
    int Decode(const ByteT* encoded, LenT src_len, BytesT* dst) override;
};
}  // namespace cryp
