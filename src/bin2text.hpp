#pragma once
#include <string>
#include <vector>
namespace cryp {
// binary to text encoding abstract class
class Bin2Text {
   public:
    virtual std::string Encode(const uint8_t* data, size_t len) const = 0;
    virtual std::vector<uint8_t> Decode(const char* str, size_t len) const = 0;
    // optional override
    inline virtual void Print(FILE* f, const uint8_t* data, size_t len) const {
        auto&& str = Encode(data, len);
        std::fwrite(str.data(), 1, str.size(), f);
    }
    inline virtual void Print(FILE* f, const std::vector<uint8_t>& v) const {
        Print(f, v.data(), v.size());
    }
    virtual ~Bin2Text() = default;
};
}  // namespace cryp
