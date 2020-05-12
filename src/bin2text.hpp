#pragma once
#include <string>
#include "type_aliases.hpp"
namespace cryp {
// binary to text encoding abstract class
class Bin2Text {
   public:
    virtual std::string Encode(const ByteT* data, LenT len) const = 0;
    virtual BytesT Decode(const char* str, LenT len) const = 0;
    // optional override
    inline void Print(FILE* f, const ByteT* data, LenT len,
                      LenT wrap_len = 0) const {
        auto&& str = Encode(data, len);
        if (wrap_len) {
            auto i = wrap_len;
            for (; i < str.size(); i += wrap_len) {
                std::fwrite(str.data() + i - wrap_len, 1, wrap_len, f);
                std::fputc('\n', f);
            }
            i -= wrap_len;
            if (i < str.size())
                std::fwrite(str.data() + i, 1, str.size() - i, f);
        } else {
            std::fwrite(str.data(), 1, str.size(), f);
        }
        std::fputc('\n', f);
    }
    inline void Print(FILE* f, const BytesT& v, LenT wrap_len = 0) const {
        Print(f, v.data(), v.size(), wrap_len);
    }
    virtual ~Bin2Text() = default;
};
}  // namespace cryp
