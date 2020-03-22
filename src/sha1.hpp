#ifndef SHA1_HPP
#define SHA1_HPP
#include <array>
#include <iostream>

namespace cryp {
// secure hash function SHA-1
class SHA1 {
   private:
    static inline constexpr uint32_t LeftRotate(uint32_t x, uint32_t n) {
        return (x << n) | (x >> (32 - n));
    }

   public:
    static uint64_t Calculate(std::FILE* file, uint8_t* dest);
};
}  // namespace cryp

#endif /* SHA1_HPP */
