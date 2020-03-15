#ifndef BASE64_HPP
#define BASE64_HPP
#include <iostream>
#include <vector>
namespace cryp {
// base64 binary-to-text encoding
class Base64 {
    static char charset_[65];
    static uint8_t decodetable_[128];
    static char pad_;

   public:
    //@return 0) ok, other) not ok (str too short)
    static int SetCharset(const char* str);
    static const char* Charset();
    static void SetPad(const char ch);
    static std::string Encode(const uint8_t* data, size_t len);
    static std::vector<uint8_t> Decode(const char* str, size_t len);
};
}  // namespace cryp
#endif /* ifndef BASE64_HPP */
