#ifndef SHA3_HPP
#define SHA3_HPP
#include <iostream>

namespace cryp {
// secure hash function SHA-3
class SHA3 {
   private:
    static constexpr uint64_t LeftRotate64(uint64_t x, uint64_t n) {
        return (x << n) | (x >> (64 - n));
    }
    static void Keccak_Permutation(uint64_t a[5][5]);
    template <uint32_t CAP, uint8_t SUFFIX>
    static uint64_t Keccak(std::FILE* file, uint8_t* dest, uint32_t out_len);

   public:
    static uint64_t SHA224(std::FILE* file, uint8_t* dest);
    static uint64_t SHA256(std::FILE* file, uint8_t* dest);
    static uint64_t SHA384(std::FILE* file, uint8_t* dest);
    static uint64_t SHA512(std::FILE* file, uint8_t* dest);
    static uint64_t SHAKE128(std::FILE* file, uint8_t* dest, uint32_t out_len);
    static uint64_t SHAKE256(std::FILE* file, uint8_t* dest, uint32_t out_len);
    static uint64_t RawSHAKE128(std::FILE* file, uint8_t* dest,
                                uint32_t out_len);
    static uint64_t RawSHAKE256(std::FILE* file, uint8_t* dest,
                                uint32_t out_len);
};
}  // namespace cryp

#endif /* SHA3_HPP */
