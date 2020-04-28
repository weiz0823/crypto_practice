#include "hexprint.hpp"
namespace cryp {
std::string HexPrint::Encode(const uint8_t* data, size_t len) const {
    size_t i = 0;
    std::string str;
    char buf[10];
    str.reserve(len + len);
    size_t len0 = len & 3;
    for (; i < len0; ++i) std::sprintf(buf + i + i, fmt_str, data[i]);
    str.append(buf, len0 + len0);
    for (; i < len; i += 4) {
        std::sprintf(buf, fmt_str, data[i]);
        std::sprintf(buf + 2, fmt_str, data[i + 1]);
        std::sprintf(buf + 4, fmt_str, data[i + 2]);
        std::sprintf(buf + 6, fmt_str, data[i + 3]);
        str.append(buf, 8);
    }
    return str;
}
std::vector<uint8_t> HexPrint::Decode(const char* str, size_t len) const {
    std::vector<uint8_t> v;
    v.reserve((len + 1) >> 1);
    uint8_t num = 0;
    for (size_t i = 1; i < len; i += 2) {
        if (!std::isxdigit(str[i - 1])) return v;
        if (str[i - 1] >= 'a')
            num = str[i - 1] - 'a' + 10;
        else if (str[i - 1] >= 'A')
            num = str[i - 1] - 'A' + 10;
        else
            num = str[i - 1] - '0';
        num <<= 4;
        if (!std::isxdigit(str[i])) {
            v.push_back(num);
            return v;
        }
        if (str[i] >= 'a')
            num |= str[i] - 'a' + 10;
        else if (str[i] >= 'A')
            num |= str[i] - 'A' + 10;
        else
            num |= str[i] - '0';
    }
    if ((len & 1) && std::isxdigit(str[len - 1])) {
        if (str[len - 1] >= 'a')
            num = str[len - 1] - 'a' + 10;
        else if (str[len - 1] >= 'A')
            num = str[len - 1] - 'A' + 10;
        else
            num = str[len - 1] - '0';
        num <<= 4;
        v.push_back(num);
    }
    return v;
}
}  // namespace cryp
