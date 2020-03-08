#ifndef BIGINT_HPP
#define BIGINT_HPP
#include <cstring>
#include <iostream>
namespace cryp {
// NOTE: "fixed length" version
template <int LEN>
class BigInt {
   public:
    // NO implicit type conversion allowed
    explicit BigInt(const int val = 0);
    explicit BigInt(const int64_t val);
    // TODO(str->bigint): after bit arithmetic support
    explicit BigInt(const char* str);
    BigInt(const BigInt& rhs);      // copy constructor
    BigInt(BigInt&& rhs) noexcept;  // move constructor
    virtual ~BigInt();
    BigInt& operator=(const BigInt& rhs);      // copy assignment
    BigInt& operator=(BigInt&& rhs) noexcept;  // move assignment
    explicit operator bool() const;
    template <int _LEN>
    friend void swap(BigInt<_LEN>& lhs, BigInt<_LEN>& rhs);
    bool Sign() const;
    BigInt& ToBitRev();
    BigInt& operator+=(const BigInt& rhs);
    BigInt& operator-=(const BigInt& rhs);
    // DON'T write +=1, for the cost of construct in fixed length is high
    BigInt& operator++();  // ++i
    BigInt& operator--();
    BigInt operator++(int);  // i++
    BigInt operator--(int);
    BigInt& ToOpposite();
    BigInt operator~() const;
    BigInt operator-() const;
    BigInt& operator&=(const BigInt& rhs);
    BigInt& operator|=(const BigInt& rhs);
    BigInt& operator^=(const BigInt& rhs);
    BigInt& operator<<=(int shift);
    BigInt& operator>>=(int shift);
    void PrintBinU(std::FILE* f = stdout);
    void PrintHexU(std::FILE* f = stdout);
    // upper case 0X
    void PrintHEXU(std::FILE* f = stdout);
#ifdef __cpp_impl_three_way_comparison
    // TODO(): three-way comparison in c++20
#else
#pragma message( \
    "\033[1m\033[35mWarning\033[0m: No three way comparison support. Consider compiling at least with c++20 draft (c++2a).")
	// TODO(): old style comparison (but comply with c++20)
#endif
    // TODO(): write mul&div in another file
    // TODO(): arbitrary base (2--32) I/O

   protected:
    unsigned* val_;
};

template <int LEN>
void swap(BigInt<LEN>& lhs, BigInt<LEN>& rhs);
template <int LEN>
BigInt<LEN> operator+(BigInt<LEN> lhs, const BigInt<LEN>& rhs);
template <int LEN>
BigInt<LEN> operator-(BigInt<LEN> lhs, const BigInt<LEN>& rhs);
template <int LEN>
BigInt<LEN> operator&(BigInt<LEN> lhs, const BigInt<LEN>& rhs);
template <int LEN>
BigInt<LEN> operator|(BigInt<LEN> lhs, const BigInt<LEN>& rhs);
template <int LEN>
BigInt<LEN> operator^(BigInt<LEN> lhs, const BigInt<LEN>& rhs);
// bit shift
template <int LEN>
BigInt<LEN> operator<<(BigInt<LEN> lhs, int shift);
template <int LEN>
BigInt<LEN> operator>>(BigInt<LEN> lhs, int shift);

// note: using is more modern than typedef
using int128_t = BigInt<4>;
using int256_t = BigInt<8>;
using int512_t = BigInt<16>;
using int1024_t = BigInt<32>;
using int2048_t = BigInt<64>;
using int4096_t = BigInt<128>;
using int8192_t = BigInt<256>;

// explicit instantiation
extern template class BigInt<4>;
extern template class BigInt<8>;
extern template class BigInt<16>;
extern template class BigInt<32>;
extern template class BigInt<64>;
extern template class BigInt<128>;
extern template class BigInt<256>;
}  // namespace cryp
#endif /* ifndef BIGINT_HPP */
