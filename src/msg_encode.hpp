#pragma once
#include <cstdint>
namespace cryp {
// message encoding method abstract class
class MsgEncode {
   protected:
    uint64_t emlen_;

   public:
    virtual void Encode(const uint8_t* msg, uint64_t len, uint8_t* encoded,
                        uint64_t out_len) = 0;
    virtual void Decode(const uint8_t* encoded, uint64_t len, uint8_t* msg,
                        uint64_t out_len) = 0;
};
}  // namespace cryp
