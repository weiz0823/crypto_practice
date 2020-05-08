#pragma once
#include "bin2text.hpp"
namespace cryp {
class HexPrint : public Bin2Text {
    char fmt_str[8] = "%02x";

   public:
    inline void SetUppercase() { fmt_str[3] = 'X'; }
    inline void SetLowercase() { fmt_str[3] = 'x'; }
    std::string Encode(const ByteT* data, LenT len) const override;
    BytesT Decode(const char* str, LenT len) const override;
};
}  // namespace cryp
