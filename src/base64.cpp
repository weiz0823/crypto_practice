#include "base64.hpp"
namespace cryp {
const char* Base64::Charset() { return charset_; }
int Base64::SetCharset(const char* str) {
    std::fill(decodetable_, decodetable_ + 128, 0);
    for (uint8_t i = 0; i < 64; ++i) {
        if (str[i] == '\0') return 1;
        charset_[i] = str[i];
        decodetable_[int8_t(str[i])] = i;
    }
    return 0;
}
void Base64::SetPad(const char ch) { pad_ = ch; }
std::string Base64::Encode(const uint8_t* data, size_t len) {
    std::string str;
    str.resize((len + 2) / 3 * 4, '\0');
    uint32_t t;
    size_t p = 0, i = 2;
    for (; i < len; i += 3, p += 4) {
        t = uint32_t((data[i - 2] << 16) | (data[i - 1] << 8) | data[i]);
        str[p] = charset_[(t & (63 << 18)) >> 18];
        str[p + 1] = charset_[(t & (63 << 12)) >> 12];
        str[p + 2] = charset_[(t & (63 << 6)) >> 6];
        str[p + 3] = charset_[t & 63];
    }
    // pad
    if (len % 3 == 2) {
        t = uint32_t((data[i - 2] << 16) | (data[i - 1] << 8));
        str[p] = charset_[(t & (63 << 18)) >> 18];
        str[p + 1] = charset_[(t & (63 << 12)) >> 12];
        str[p + 2] = charset_[(t & (63 << 6)) >> 6];
        str[p + 3] = pad_;
    } else if (len % 3 == 1) {
        t = uint32_t(data[i - 2] << 16);
        str[p] = charset_[(t & (63 << 18)) >> 18];
        str[p + 1] = charset_[(t & (63 << 12)) >> 12];
        str[p + 2] = pad_;
        str[p + 3] = pad_;
    }
    return str;
}
std::vector<uint8_t> Base64::Decode(const char* str, size_t len) {
    std::vector<uint8_t> v;
    v.reserve((len + 3) / 4 * 3);
    uint32_t t;
    size_t i = 3;
    for (; i < len; i += 4) {
        if (decodetable_[uint8_t(str[i - 3])] > 63 ||
            decodetable_[uint8_t(str[i - 2])] > 63) {
            len = i - 3;
            break;
        } else if (decodetable_[uint8_t(str[i - 1])] > 63) {
            len = i - 1;
            break;
        } else if (decodetable_[uint8_t(str[i])] > 63) {
            len = i;
            break;
        }
        t = uint32_t((decodetable_[uint8_t(str[i - 3])] << 18) |
                     (decodetable_[uint8_t(str[i - 2])] << 12) |
                     (decodetable_[uint8_t(str[i - 1])] << 6) |
                     decodetable_[uint8_t(str[i])]);
        v.push_back((t & (255 << 16)) >> 16);
        v.push_back((t & (255 << 8)) >> 8);
        v.push_back(t & 255);
    }
    // pad
    if ((len & 3) == 3) {
        t = uint32_t((decodetable_[uint8_t(str[i - 3])] << 18) |
                     (decodetable_[uint8_t(str[i - 2])] << 12) |
                     (decodetable_[uint8_t(str[i - 1])] << 6));
        v.push_back((t & (255 << 16)) >> 16);
        v.push_back((t & (255 << 8)) >> 8);
    } else if ((len & 3) == 2) {
        t = uint32_t((decodetable_[uint8_t(str[i - 3])] << 18) |
                     (decodetable_[uint8_t(str[i - 2])] << 12));
        v.push_back((t & (255 << 16)) >> 16);
    }
    return v;
}
}  // namespace cryp
