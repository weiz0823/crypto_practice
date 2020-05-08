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
    inline virtual void Print(FILE* f, const ByteT* data, LenT len) const {
        auto&& str = Encode(data, len);
        std::fwrite(str.data(), 1, str.size(), f);
    }
    inline virtual void Print(FILE* f, const BytesT& v) const {
        Print(f, v.data(), v.size());
    }
    virtual ~Bin2Text() = default;
};
}  // namespace cryp
