#ifndef SHA2_HPP
#define SHA2_HPP
#include <iostream>

namespace cryp {
// secure hash function SHA-2
class SHA2 {
   private:
    static constexpr uint32_t RightRotate32(uint32_t x, uint32_t n) {
        return (x >> n) | (x << (32 - n));
    }
    static constexpr uint64_t RightRotate64(uint64_t x, uint64_t n) {
        return (x >> n) | (x << (64 - n));
    }
    static uint64_t SHA512t_IVGen(uint8_t* src, size_t len, uint64_t* dest);

   public:
    static uint64_t SHA224(std::FILE* file, uint8_t* dest);
    static uint64_t SHA256(std::FILE* file, uint8_t* dest);
    static uint64_t SHA384(std::FILE* file, uint8_t* dest);
    static uint64_t SHA512(std::FILE* file, uint8_t* dest);
    // generate initial value for SHA512/t
    static uint64_t SHA512t(std::FILE* file, uint8_t* dest, uint32_t t);
    static inline uint64_t SHA512_224(std::FILE* file, uint8_t* dest) {
        return SHA512t(file, dest, 224);
    }
    static inline uint64_t SHA512_256(std::FILE* file, uint8_t* dest) {
        return SHA512t(file, dest, 256);
    }
};
}  // namespace cryp

#endif /* SHA2_HPP */
