#pragma once
#include <string>
#include <vector>
namespace cryp {
// binary to text encoding abstract class
class Bin2Text {
   public:
    virtual std::string Encode(const uint8_t* data, size_t len) = 0;
    virtual std::vector<uint8_t> Decode(const char* str, size_t len) = 0;
    // optional override
    inline virtual void Print(FILE* f, const uint8_t* data, size_t len) {
        auto&& str = Encode(data, len);
        std::fwrite(str.data(), 1, str.size(), f);
    }
    virtual ~Bin2Text() = default;
};
}  // namespace cryp
