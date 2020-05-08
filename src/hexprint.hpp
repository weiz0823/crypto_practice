#pragma once
#include "bin2text.hpp"
namespace cryp {
class HexPrint : public Bin2Text {
    char fmt_str[8] = "%02x";

   public:
    inline void SetUppercase() { fmt_str[3] = 'X'; }
    inline void SetLowercase() { fmt_str[3] = 'x'; }
    std::string Encode(const uint8_t* data, size_t len) const override;
    std::vector<uint8_t> Decode(const char* str, size_t len) const override;
};
}  // namespace cryp
