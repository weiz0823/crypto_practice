#pragma once
#include "bin2text.hpp"
namespace cryp {
// base64 binary-to-text encoding
class Base64 : public Bin2Text {
    char charset_[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    ByteT decodetable_[128] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 62,  255, 255, 255, 63,  52,  53,  54,  55,
        56,  57,  58,  59,  60,  61,  255, 255, 255, 255, 255, 255, 255,
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,
        13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
        255, 255, 255, 255, 255, 255, 26,  27,  28,  29,  30,  31,  32,
        33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,
        46,  47,  48,  49,  50,  51,  255, 255, 255, 255, 255};
    char pad_ = '=';

   public:
    //@return 0) ok, other) not ok (str too short)
    int SetCharset(const char* str);
    const char* Charset() const;
    void SetPad(char ch);
    std::string Encode(const ByteT* data, LenT len) const override;
    BytesT Decode(const char* str, LenT len) const override;
};
inline Base64 g_base64;
}  // namespace cryp
