#pragma once
#include <complex>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>
#include <utility>
namespace calc {
// complex (mod Mersenne Prime)
class CompMp;
template <typename IntT>
class BigInt;

typedef __uint128_t uint128_t;
typedef __int128_t int128_t;

// Work fine on little-endian machine.
// Big-endian can be done by reversing the whole vector
// and doing everything in reversed order,
// so it will not be considered in the code.
template <>
class BigInt<uint128_t> {
    // data
    uint128_t* val_ = nullptr;
    uint64_t len_ = 2;  // actual used length
    uint64_t cap_ = 4;  // capacity
    uint128_t* end_ = nullptr;
    static constexpr uint64_t LIMB = 128;
    static constexpr uint64_t LOGLIMB = 7;
    static constexpr uint64_t MAX_CAP = uint64_t(1) << 63;

    // random device
    // NOLINTNEXTLINE: c++17 ok
    inline static std::random_device rand_dev_;
    // NOLINTNEXTLINE: c++17 ok
    inline static auto rand_gen_ = static_cast<std::mt19937>(rand_dev_());
    // usage: rand_(rand_gen_)
    // NOLINTNEXTLINE: c++17 ok
    inline static std::uniform_int_distribution<uint64_t> rand_;

    // bigint64_div.cpp
    uint64_t DivDCore(const BigInt& rhs, uint64_t v1, uint64_t v2, uint64_t u1h,
                      uint64_t u1l, uint64_t u2, uint64_t bias, bool half_more);
    void DivRNormal(const BigInt& rhs, BigInt* mod = nullptr);

    // bigint64_mul.cpp
    BigInt& RMNTMulEqGiven(const int64_t* src, uint64_t n, uint64_t rlen);

   public:
    // bigint64_basic.cpp
    explicit BigInt(int value = 0);
    explicit BigInt(uint64_t value);
    BigInt(const BigInt& rhs);
    BigInt(BigInt&& rhs) noexcept;
    virtual ~BigInt();
    BigInt& operator=(const BigInt& rhs);
    BigInt& operator=(BigInt&& rhs) noexcept;
    BigInt& GenRandom(uint64_t length, uint8_t fixed = 0);
    explicit operator int64_t() const;
    explicit operator bool() const;
    bool Sign() const;
    bool Parity() const;
    const uint128_t* Data() const;
    uint64_t Length() const;
    void ShrinkLen();
    void SetLen(uint64_t new_len, bool preserve_sign);
    // shrink size
    void Shrink();
    double log2() const;
    double log10() const;
    uint64_t TrailingZero() const;
    uint64_t BitLen() const;

    // bigint64_bit.cpp
    BigInt& ToBitInv();
    BigInt operator~() const;
    BigInt& operator&=(const BigInt& rhs);
    BigInt& operator|=(const BigInt& rhs);
    BigInt& operator^=(const BigInt& rhs);
    BigInt& operator<<=(uint64_t rhs);
    BigInt& operator>>=(uint64_t rhs);

    // bigint64_add.cpp
    BigInt& operator+=(uint64_t rhs);
    BigInt& operator+=(const BigInt& rhs);
    BigInt& BiasedAddEq(const BigInt& rhs, uint64_t bias,
                        bool half_more = false);
    BigInt& operator++();
    BigInt operator++(int);
    BigInt& operator-=(uint64_t rhs);
    BigInt& operator-=(const BigInt& rhs);
    BigInt& BiasedSubEq(const BigInt& rhs, uint64_t bias,
                        bool half_more = false);
    BigInt& operator--();
    BigInt operator--(int);
    BigInt& ToOpposite();
    BigInt operator-() const;
    BigInt& ToAbsolute();

    // bigint64_io.cpp
    void Print(int base = 10, int showbase = 1, bool uppercase = false,
               std::FILE* f = stdout) const;
    friend std::ostream& operator<<(std::ostream& out,
                                    const BigInt<uint128_t>& rhs);
    std::string ToString(int base = 10, int showbase = 1,
                         bool uppercase = false) const;
    explicit BigInt(const char* str, size_t base = 0);
    explicit BigInt(const std::string& str, size_t base = 0);

    // bigint64_compare.cpp
#ifdef __cpp_impl_three_way_comparison
    std::weak_ordering operator<=>(const BigInt& rhs) const;
#else
    int Compare(const BigInt& rhs) const;
#endif

    // bigint64_div.cpp
    // note this is not unsigned, to ensure the sign of remain is set
    // correctly
    BigInt& DivEq64(int64_t rhs, int64_t* remain = nullptr);
    BigInt& operator/=(int64_t rhs);
    BigInt& operator%=(int64_t rhs);
    BigInt& DivEqD(const BigInt& rhs, BigInt* mod = nullptr);
    BigInt& operator/=(const BigInt& rhs);
    BigInt& operator%=(const BigInt& rhs);
    BigInt& DivEq(const BigInt& rhs, BigInt* mod = nullptr);
    // recursive
    BigInt& DivEqR(const BigInt& rhs, BigInt* mod = nullptr);

    // bigint64_mul.cpp
    BigInt& operator*=(uint64_t rhs);
    template <typename T>
    static void BitRevSort(T* a, uint64_t n);
    static void RMNT(int64_t* dest, uint64_t n, bool inv);
    BigInt& RMNTMulEq(const BigInt& rhs);
    static void MNT(CompMp* dest, uint64_t n, bool inv);
    BigInt& MNTMulEq(const BigInt& rhs);
    BigInt& MulEqKaratsuba(const BigInt& rhs);
    BigInt& operator*=(const BigInt& rhs);
    BigInt& SquareEq();
    BigInt& RMNTMulEqUB(const BigInt& rhs);
    BigInt& PlainMulEq(const BigInt& rhs);

    // bigint64_ext.cpp
    bool isProbablePrime() const;
    BigInt& ToNextPrime();
    friend BigInt PowMod(const BigInt& a, uint64_t p, const BigInt& n);
    friend BigInt PowMod(const BigInt& a, const BigInt& p, const BigInt& n);

    // bigint64.cpp
    static BigInt RMNTMul(BigInt lhs, const BigInt& rhs);
    static BigInt MNTMul(BigInt lhs, const BigInt& rhs);
    static BigInt MulKaratsuba(BigInt lhs, const BigInt& rhs);
    static BigInt PlainMul(BigInt lhs, const BigInt& rhs);
    static BigInt DivD(BigInt lhs, const BigInt& rhs, BigInt* mod = nullptr);
    static BigInt Div(BigInt lhs, const BigInt& rhs, BigInt* mod = nullptr);
    static BigInt Square(BigInt lhs);
    static BigInt RMNTMulUB(BigInt lhs, const BigInt& rhs);
    static BigInt DivR(BigInt lhs, const BigInt& rhs, BigInt* mod = nullptr);
};
// bigint64_io.cpp
std::ostream& operator<<(std::ostream& out, const BigInt<uint128_t>& rhs);
std::istream& operator>>(std::istream& in, BigInt<uint128_t>& rhs);

// bigint64_ext.cpp
BigInt<uint128_t> BigProduct(uint64_t a, uint64_t b);
BigInt<uint128_t> Factorial(uint64_t n);
BigInt<uint128_t> Power(const BigInt<uint128_t>& a, uint64_t p);
BigInt<uint128_t> PowMod(const BigInt<uint128_t>& a, uint64_t p,
                         const BigInt<uint128_t>& n);
BigInt<uint128_t> PowMod(const BigInt<uint128_t>& a, const BigInt<uint128_t>& p,
                         const BigInt<uint128_t>& n);
BigInt<uint128_t> GcdBin(BigInt<uint128_t> a, BigInt<uint128_t> b);
BigInt<uint128_t> ExtGcdBin(BigInt<uint128_t> a, BigInt<uint128_t> b,
                            BigInt<uint128_t>* x, BigInt<uint128_t>* y);

// bigint64.cpp
BigInt<uint128_t> operator&(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs);
BigInt<uint128_t> operator|(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs);
BigInt<uint128_t> operator^(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs);
BigInt<uint128_t> operator<<(BigInt<uint128_t> lhs, uint64_t rhs);
BigInt<uint128_t> operator>>(BigInt<uint128_t> lhs, uint64_t rhs);
BigInt<uint128_t> operator+(BigInt<uint128_t> lhs, uint64_t rhs);
BigInt<uint128_t> operator+(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs);
BigInt<uint128_t> operator-(BigInt<uint128_t> lhs, uint64_t rhs);
BigInt<uint128_t> operator-(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs);
BigInt<uint128_t> operator*(BigInt<uint128_t> lhs, uint64_t rhs);
BigInt<uint128_t> operator*(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs);
BigInt<uint128_t> operator/(BigInt<uint128_t> lhs, int64_t rhs);
BigInt<uint128_t> operator/(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs);
int64_t operator%(BigInt<uint128_t> lhs, int64_t rhs);
BigInt<uint128_t> operator%(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs);
#ifndef __cpp_impl_three_way_comparison
bool operator<(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs);
bool operator>(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs);
bool operator<=(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs);
bool operator>=(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs);
bool operator==(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs);
bool operator!=(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs);
#endif
}  // namespace calc
