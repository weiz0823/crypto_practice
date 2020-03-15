#ifndef BIGINT_HPP
#define BIGINT_HPP
#include <cstring>
#include <iostream>
#include <random>
#include <utility>
namespace cryp {
// NOTE: "fixed length" version
// we are using it in number theory, so it will always be unsigned,
// unless sometimes represented by signed
template <unsigned LEN>
class BigUInt {
   public:
    // NO implicit type conversion allowed
    explicit BigUInt(const uint64_t val = 0);
    explicit BigUInt(const char* str, unsigned base = 0);
    explicit BigUInt(const std::string& str, unsigned base = 0)
        : BigUInt(str.c_str(), base) {}
    // copy constructor
    BigUInt(const BigUInt& rhs);
    // move constructor
    BigUInt(BigUInt&& rhs) noexcept;
    virtual ~BigUInt();
    // copy assignment
    BigUInt& operator=(const BigUInt& rhs);
    // move assignment, with possible memory reuse
    BigUInt& operator=(BigUInt&& rhs) noexcept;
    // copy/move operator, when memory reuse is not possible
    // [[deprecated]] BigUInt& operator=(BigUInt rhs) noexcept;
    // no memory reuse, all copy
    template <unsigned _LEN>
    explicit operator BigUInt<_LEN>() const;
    explicit operator unsigned() const;
    explicit operator bool() const;
    bool Sign() const;
    BigUInt& ToBitRev();
    BigUInt& operator+=(const BigUInt& rhs);
    BigUInt& operator-=(const BigUInt& rhs);
    // DON'T write +=1, for the cost of construct in fixed length is high
    BigUInt& operator++();  // ++i
    BigUInt& operator--();
    BigUInt operator++(int);  // i++
    BigUInt operator--(int);
    BigUInt& ToOpposite();
    BigUInt operator~() const;
    BigUInt operator-() const;
    BigUInt& operator&=(const BigUInt& rhs);
    BigUInt& operator|=(const BigUInt& rhs);
    BigUInt& operator^=(const BigUInt& rhs);
    BigUInt& operator<<=(int shift);
    BigUInt& operator>>=(int shift);
    void PrintBinU(std::FILE* f = stdout) const;
    void PrintHexU(std::FILE* f = stdout) const;
    // upper case 0X
    void PrintHEXU(std::FILE* f = stdout) const;
#ifdef __cpp_impl_three_way_comparison
    // DO LATER: three-way comparison in c++20
#else
#pragma message(                        \
    "No three way comparison support. " \
    "Consider compiling with c++20 if possible.")
    template <unsigned _LEN>
    // comparison permitted between different length
    int Compare(const BigUInt<_LEN>& rhs) const;
#endif
    // bigmul_div_mod.cpp
    BigUInt& operator*=(unsigned rhs);
    BigUInt& operator*=(const BigUInt& rhs);
    BigUInt& MulEq_Plain(const BigUInt& rhs);
    [[maybe_unused]] BigUInt& MulEq_Karatsuba(const BigUInt& rhs);
    // r is to store the remainder
    BigUInt& operator/=(unsigned rhs);
    BigUInt& operator/=(const BigUInt& rhs);
    BigUInt& operator%=(const BigUInt& rhs);
    BigUInt& DivEq_Basic(unsigned rhs, unsigned* r_ = nullptr);
    BigUInt& DivEq_Plain(const BigUInt& rhs, BigUInt* r_ = nullptr);
    unsigned Mod10() const;
    void GenRandom(unsigned len = LEN);
    // arbitrary base (2--32) I/O
    void Print(unsigned base = 10, std::FILE* f = stdout) const;
    unsigned Mod_Basic(unsigned rhs) const;
    // TODO(): write mul&div in bigmul_div_mod.cpp

   protected:
    // NOLINTNEXTLINE: c++17 ok
    inline static std::random_device rand_dev_;
    // NOLINTNEXTLINE: c++17 ok
    inline static auto rand_gen_ = static_cast<std::mt19937>(rand_dev_());
    // NOLINTNEXTLINE: c++17 ok
    inline static std::uniform_int_distribution<unsigned> rand_;
    unsigned* val_;

    // construct from raw data is not public
    explicit BigUInt(const unsigned* val, int len = LEN);
    [[maybe_unused]] BigUInt& MulEq_Karatsuba_Recursion(const BigUInt& rhs);

    template <unsigned>
    friend class BigUInt;
    struct Instanization;
};

// Google: prefer to define non-modifying binary operators as non-member func
template <unsigned LEN>
BigUInt<LEN> operator+(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs);
template <unsigned LEN>
BigUInt<LEN> operator-(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs);
template <unsigned LEN>
BigUInt<LEN> operator&(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs);
template <unsigned LEN>
BigUInt<LEN> operator|(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs);
template <unsigned LEN>
BigUInt<LEN> operator^(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs);
// bit shift
template <unsigned LEN>
BigUInt<LEN> operator<<(BigUInt<LEN> lhs, int shift);
template <unsigned LEN>
BigUInt<LEN> operator>>(BigUInt<LEN> lhs, int shift);
// comparison permitted between different length
template <unsigned LEN1, unsigned LEN2>
bool operator<(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs);
template <unsigned LEN1, unsigned LEN2>
bool operator>(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs);
template <unsigned LEN1, unsigned LEN2>
bool operator<=(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs);
template <unsigned LEN1, unsigned LEN2>
bool operator>=(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs);
template <unsigned LEN1, unsigned LEN2>
bool operator==(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs);
template <unsigned LEN1, unsigned LEN2>
bool operator!=(const BigUInt<LEN1>& lhs, const BigUInt<LEN2>& rhs);
// bigmul_div_mod.cpp
template <unsigned LEN>
BigUInt<LEN> operator*(BigUInt<LEN> lhs, unsigned rhs);
template <unsigned LEN>
BigUInt<LEN> operator/(BigUInt<LEN> lhs, unsigned rhs);
template <unsigned LEN>
BigUInt<LEN> operator*(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs);
template <unsigned LEN>
BigUInt<LEN> operator/(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs);
template <unsigned LEN>
BigUInt<LEN> operator%(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs);

// note: using is more modern than typedef
using uint128_t = BigUInt<4u>;
using uint256_t = BigUInt<8u>;
using uint512_t = BigUInt<16u>;
using uint1024_t = BigUInt<32u>;
using uint2048_t = BigUInt<64u>;
using uint4096_t = BigUInt<128u>;
using uint8192_t = BigUInt<256u>;

// explicit instantiation
extern template class BigUInt<0u>;
extern template class BigUInt<1u>;
extern template class BigUInt<2u>;
extern template class BigUInt<4u>;
extern template class BigUInt<8u>;
extern template class BigUInt<16u>;
extern template class BigUInt<32u>;
extern template class BigUInt<64u>;
extern template class BigUInt<128u>;
extern template class BigUInt<256u>;
// only a chain instantiated
extern template BigUInt<4u>::operator BigUInt<8u>() const;
extern template BigUInt<8u>::operator BigUInt<16u>() const;
extern template BigUInt<16u>::operator BigUInt<32u>() const;
extern template BigUInt<32u>::operator BigUInt<64u>() const;
extern template BigUInt<64u>::operator BigUInt<128u>() const;
extern template BigUInt<128u>::operator BigUInt<256u>() const;
extern template BigUInt<256u>::operator BigUInt<128u>() const;
extern template BigUInt<128u>::operator BigUInt<64u>() const;
extern template BigUInt<64u>::operator BigUInt<32u>() const;
extern template BigUInt<32u>::operator BigUInt<16u>() const;
extern template BigUInt<16u>::operator BigUInt<8u>() const;
extern template BigUInt<8u>::operator BigUInt<4u>() const;

template <unsigned LEN>
struct BigUInt<LEN>::Instanization {
    BigUInt<4u>* a4;
    BigUInt<8u>* a8;
    BigUInt<16u>* a16;
    BigUInt<32u>* a32;
    BigUInt<64u>* a64;
    BigUInt<128u>* a128;
    BigUInt<256u>* a256;
    Instanization() {
        *a4 + *a4;
        *a4 - *a4;
        *a4*(*a4);
        *a4 / *a4;
        // *a4 % *a4;
        *a4&* a4;
        *a4 | *a4;
        *a4 ^ *a4;
        *a4 * 1;
        *a4 / 1;
        *a4 << 1;
        *a4 >> 1;
        bool res;
        res = *a4 < *a4;
        res = *a4 > *a4;
        res = *a4 <= *a4;
        res = *a4 >= *a4;
        res = *a4 == *a4;
        res = *a4 != *a4;
        *a8 + *a8;
        *a8 - *a8;
        *a8*(*a8);
        *a8 / *a8;
        // *a8 % *a8;
        *a8&* a8;
        *a8 | *a8;
        *a8 ^ *a8;
        *a8 * 1;
        *a8 / 1;
        *a8 << 1;
        *a8 >> 1;
        res = *a8 < *a8;
        res = *a8 > *a8;
        res = *a8 <= *a8;
        res = *a8 >= *a8;
        res = *a8 == *a8;
        res = *a8 != *a8;
        *a16 + *a16;
        *a16 - *a16;
        *a16*(*a16);
        *a16 / *a16;
        // *a16 % *a16;
        *a16&* a16;
        *a16 | *a16;
        *a16 ^ *a16;
        *a16 * 1;
        *a16 / 1;
        *a16 << 1;
        *a16 >> 1;
        res = *a16 < *a16;
        res = *a16 > *a16;
        res = *a16 <= *a16;
        res = *a16 >= *a16;
        res = *a16 == *a16;
        res = *a16 != *a16;
        *a32 + *a32;
        *a32 - *a32;
        *a32*(*a32);
        *a32 / *a32;
        // *a32 % *a32;
        *a32&* a32;
        *a32 | *a32;
        *a32 ^ *a32;
        *a32 * 1;
        *a32 / 1;
        *a32 << 1;
        *a32 >> 1;
        res = *a32 < *a32;
        res = *a32 > *a32;
        res = *a32 <= *a32;
        res = *a32 >= *a32;
        res = *a32 == *a32;
        res = *a32 != *a32;
        *a64 + *a64;
        *a64 - *a64;
        *a64*(*a64);
        *a64 / *a64;
        // *a64 % *a64;
        *a64&* a64;
        *a64 | *a64;
        *a64 ^ *a64;
        *a64 * 1;
        *a64 / 1;
        *a64 << 1;
        *a64 >> 1;
        res = *a64 < *a64;
        res = *a64 > *a64;
        res = *a64 <= *a64;
        res = *a64 >= *a64;
        res = *a64 == *a64;
        res = *a64 != *a64;
        *a128 + *a128;
        *a128 - *a128;
        *a128*(*a128);
        *a128 / *a128;
        // *a128 % *a128;
        *a128&* a128;
        *a128 | *a128;
        *a128 ^ *a128;
        *a128 * 1;
        *a128 / 1;
        *a128 << 1;
        *a128 >> 1;
        res = *a128 < *a128;
        res = *a128 > *a128;
        res = *a128 <= *a128;
        res = *a128 >= *a128;
        res = *a128 == *a128;
        res = *a128 != *a128;
        *a256 + *a256;
        *a256 - *a256;
        *a256*(*a256);
        *a256 / *a256;
        // *a256 % *a256;
        *a256&* a256;
        *a256 | *a256;
        *a256 ^ *a256;
        *a256 * 1;
        *a256 / 1;
        *a256 << 1;
        *a256 >> 1;
        res = *a256 < *a256;
        res = *a256 > *a256;
        res = *a256 <= *a256;
        res = *a256 >= *a256;
        res = *a256 == *a256;
        res = *a256 != *a256;
    }
};
}  // namespace cryp
#endif /* ifndef BIGINT_HPP */
