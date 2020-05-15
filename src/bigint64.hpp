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

using uint128_t = __uint128_t;
using int128_t = __int128_t;

// Work fine on little-endian machine.
// Big-endian can be done by reversing the whole vector
// and doing everything in reversed order,
// so it will not be considered in the code.
template <>
class BigInt<uint128_t> {
    // data
    uint128_t* val_;
    uint64_t len_;  // actual used length
    uint64_t cap_;  // capacity
    uint128_t* end_;
    static constexpr uint64_t LIMB = 128;
    static constexpr uint64_t LOGLIMB = 7;
    static constexpr uint64_t MAX_CAP = uint64_t(1) << 63;

    // random device
    // inline static std::random_device rand_dev_;
    inline static std::random_device rand_dev_{};
    inline static std::mt19937 rand_gen_{rand_dev_()};
    inline static std::uniform_int_distribution<uint64_t> rand_{};
    // usage: rand_(rand_gen_)
    // inline static std::uniform_int_distribution<uint64_t> rand_;

    // bigint64_div.cpp
    static uint64_t DivDCore(BigInt& lhs, const BigInt& rhs, uint64_t v1,
                             uint64_t v2, uint64_t u1h, uint64_t u1l,
                             uint64_t u2, uint64_t bias, bool half_more);
    static BigInt<uint128_t> DivRNormal(BigInt lhs, const BigInt& rhs,
                                        BigInt* mod = nullptr);

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
    BigInt& RandomBits(uint64_t bitlen);
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
    // input from big-endian data
    explicit BigInt(const uint8_t* data, size_t size);
    explicit BigInt(const uint8_t* data, const uint8_t* end)
        : BigInt(data, end > data ? end - data : 0) {}
    // two's complement octet string (bytes), big-endian (network flow style)
    std::vector<uint8_t> Serialize() const;

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
    // note this is signed, to ensure the sign of remain is set correctly
    BigInt& DivEq64(int64_t rhs, int64_t* remain = nullptr);
    BigInt& operator/=(int64_t rhs);
    BigInt& operator%=(int64_t rhs);
    BigInt& DivEqD(const BigInt& rhs, BigInt* mod = nullptr);
    BigInt& operator/=(const BigInt& rhs);
    BigInt& operator%=(const BigInt& rhs);
    BigInt& DivEq(const BigInt& rhs, BigInt* mod = nullptr);
    // recursive
    BigInt& DivEqR(const BigInt& rhs, BigInt* mod = nullptr);
    static BigInt DivDBase(BigInt lhs, const BigInt& rhs,
                           BigInt* mod = nullptr);
    static BigInt DivRBase(BigInt lhs, const BigInt& rhs,
                           BigInt* mod = nullptr);

    // bigint64_mul.cpp
    BigInt& operator*=(uint64_t rhs);
    template <typename T>
    static void BitRevSort(T* a, uint64_t n);
    static void RMNT(int64_t* dest, uint64_t n, bool inv);
    BigInt& RMNTMulEq(const BigInt& rhs);
    static void MNT(CompMp* dest, uint64_t n, bool inv);
    BigInt& MNTMulEq(const BigInt& rhs);
    // deprecated
    BigInt& MulEqKaratsuba(const BigInt& rhs);
    BigInt& operator*=(const BigInt& rhs);
    BigInt& SquareEq();
    BigInt& RMNTMulEqUB(const BigInt& rhs);
    BigInt& PlainMulEq(const BigInt& rhs);
    static BigInt PlainMulBase(const BigInt& lhs, const BigInt& rhs);
    static BigInt RMNTMulUBBase(const BigInt& lhs, const BigInt& rhs);

    // bigint64_ext.cpp
    bool isProbablePrime() const;
    BigInt& ToNextPrime();
    friend BigInt PowMod(const BigInt& a, uint64_t p, const BigInt& n);
    friend BigInt PowMod(const BigInt& a, const BigInt& p, const BigInt& n);

    // inline functions
    static BigInt RMNTMul(BigInt lhs, const BigInt& rhs);
    static BigInt MNTMul(BigInt lhs, const BigInt& rhs);
    // deprecated
    static BigInt MulKaratsuba(BigInt lhs, const BigInt& rhs);
    static BigInt PlainMul(const BigInt& lhs, const BigInt& rhs);
    static BigInt PlainMul(const BigInt& lhs, BigInt&& rhs);
    static BigInt PlainMul(BigInt&& lhs, const BigInt& rhs);
    static BigInt PlainMul(BigInt&& lhs, BigInt&& rhs);
    static BigInt RMNTMulUB(const BigInt& lhs, const BigInt& rhs);
    static BigInt RMNTMulUB(const BigInt& lhs, BigInt&& rhs);
    static BigInt RMNTMulUB(BigInt&& lhs, const BigInt& rhs);
    static BigInt RMNTMulUB(BigInt&& lhs, BigInt&& rhs);
    static BigInt DivD(const BigInt& lhs, const BigInt& rhs,
                       BigInt* mod = nullptr);
    static BigInt DivD(const BigInt& lhs, BigInt&& rhs, BigInt* mod = nullptr);
    static BigInt DivD(BigInt&& lhs, const BigInt& rhs, BigInt* mod = nullptr);
    static BigInt DivD(BigInt&& lhs, BigInt&& rhs, BigInt* mod = nullptr);
    static BigInt DivR(const BigInt& lhs, const BigInt& rhs,
                       BigInt* mod = nullptr);
    static BigInt DivR(const BigInt& lhs, BigInt&& rhs, BigInt* mod = nullptr);
    static BigInt DivR(BigInt&& lhs, const BigInt& rhs, BigInt* mod = nullptr);
    static BigInt DivR(BigInt&& lhs, BigInt&& rhs, BigInt* mod = nullptr);
    static BigInt Div64(BigInt lhs, int64_t rhs, int64_t* mod = nullptr);
    static BigInt Div(BigInt lhs, const BigInt& rhs, BigInt* mod = nullptr);
    static BigInt Square(BigInt lhs);
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

// inline functions
// basic
inline BigInt<uint128_t>::~BigInt() { delete[] val_; }
inline const uint128_t* BigInt<uint128_t>::Data() const { return val_; }
inline uint64_t BigInt<uint128_t>::Length() const { return len_; }
inline bool BigInt<uint128_t>::Sign() const {
    return *(end_ - 1) >> (LIMB - 1);
}
inline bool BigInt<uint128_t>::Parity() const { return *val_ & 1; }
inline double BigInt<uint128_t>::log10() const {
    return log2() / std::log2(10.0);
}

// bit
inline BigInt<uint128_t>& BigInt<uint128_t>::ToBitInv() {
    uint128_t* it = val_;
    do
        *it = ~*it;
    while (++it != end_);
    return *this;
}
inline BigInt<uint128_t> BigInt<uint128_t>::operator~() const {
    auto obj = *this;
    return obj.ToBitInv();
}
inline BigInt<uint128_t> operator&(const BigInt<uint128_t>& lhs,
                                   const BigInt<uint128_t>& rhs) {
    auto tmp = lhs;
    tmp &= rhs;
    return tmp;
}
inline BigInt<uint128_t> operator&(BigInt<uint128_t>&& lhs,
                                   const BigInt<uint128_t>& rhs) {
    lhs &= rhs;
    return std::move(lhs);
}
inline BigInt<uint128_t> operator&(const BigInt<uint128_t>& lhs,
                                   BigInt<uint128_t>&& rhs) {
    return std::move(rhs) & lhs;
}
inline BigInt<uint128_t> operator&(BigInt<uint128_t>&& lhs,
                                   BigInt<uint128_t>&& rhs) {
    return std::move(lhs) & rhs;
}
inline BigInt<uint128_t> operator|(const BigInt<uint128_t>& lhs,
                                   const BigInt<uint128_t>& rhs) {
    auto tmp = lhs;
    tmp |= rhs;
    return tmp;
}
inline BigInt<uint128_t> operator|(BigInt<uint128_t>&& lhs,
                                   const BigInt<uint128_t>& rhs) {
    lhs |= rhs;
    return std::move(lhs);
}
inline BigInt<uint128_t> operator|(const BigInt<uint128_t>& lhs,
                                   BigInt<uint128_t>&& rhs) {
    return std::move(rhs) | lhs;
}
inline BigInt<uint128_t> operator|(BigInt<uint128_t>&& lhs,
                                   BigInt<uint128_t>&& rhs) {
    return std::move(lhs) | rhs;
}
inline BigInt<uint128_t> operator^(const BigInt<uint128_t>& lhs,
                                   const BigInt<uint128_t>& rhs) {
    auto tmp = lhs;
    tmp ^= rhs;
    return tmp;
}
inline BigInt<uint128_t> operator^(BigInt<uint128_t>&& lhs,
                                   const BigInt<uint128_t>& rhs) {
    lhs ^= rhs;
    return std::move(lhs);
}
inline BigInt<uint128_t> operator^(const BigInt<uint128_t>& lhs,
                                   BigInt<uint128_t>&& rhs) {
    return std::move(rhs) ^ lhs;
}
inline BigInt<uint128_t> operator^(BigInt<uint128_t>&& lhs,
                                   BigInt<uint128_t>&& rhs) {
    return std::move(lhs) ^ rhs;
}
inline BigInt<uint128_t> operator<<(BigInt<uint128_t> lhs, uint64_t rhs) {
    lhs <<= rhs;
    return lhs;
}
inline BigInt<uint128_t> operator>>(BigInt<uint128_t> lhs, uint64_t rhs) {
    lhs >>= rhs;
    return lhs;
}

// cmp
#ifndef __cpp_impl_three_way_comparison
inline bool operator<(const BigInt<uint128_t>& lhs,
                      const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) < 0;
}
inline bool operator>(const BigInt<uint128_t>& lhs,
                      const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) > 0;
}
inline bool operator<=(const BigInt<uint128_t>& lhs,
                       const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) <= 0;
}
inline bool operator>=(const BigInt<uint128_t>& lhs,
                       const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) >= 0;
}
inline bool operator==(const BigInt<uint128_t>& lhs,
                       const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) == 0;
}
inline bool operator!=(const BigInt<uint128_t>& lhs,
                       const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) != 0;
}
#endif

// add
inline BigInt<uint128_t>& BigInt<uint128_t>::operator++() { return *this += 1; }
inline BigInt<uint128_t> BigInt<uint128_t>::operator++(int) {
    auto obj = *this;
    *this += 1;
    return obj;
}
inline BigInt<uint128_t>& BigInt<uint128_t>::operator--() { return *this -= 1; }
inline BigInt<uint128_t> BigInt<uint128_t>::operator--(int) {
    auto obj = *this;
    *this -= 1;
    return obj;
}
inline BigInt<uint128_t>& BigInt<uint128_t>::ToOpposite() {
    ToBitInv();
    return *this += 1;
}
inline BigInt<uint128_t>& BigInt<uint128_t>::ToAbsolute() {
    if (Sign())
        return ToOpposite();
    else
        return *this;
}
inline BigInt<uint128_t> BigInt<uint128_t>::operator-() const {
    auto obj = *this;
    return obj.ToOpposite();
}
inline BigInt<uint128_t>& BigInt<uint128_t>::operator+=(const BigInt& rhs) {
    return BiasedAddEq(rhs, 0, false);
}
inline BigInt<uint128_t>& BigInt<uint128_t>::operator-=(const BigInt& rhs) {
    return BiasedSubEq(rhs, 0, false);
}
inline BigInt<uint128_t> operator+(BigInt<uint128_t> lhs, uint64_t rhs) {
    lhs += rhs;
    return lhs;
}
inline BigInt<uint128_t> operator-(BigInt<uint128_t> lhs, uint64_t rhs) {
    lhs -= rhs;
    return lhs;
}
inline BigInt<uint128_t> operator+(const BigInt<uint128_t>& lhs,
                                   const BigInt<uint128_t>& rhs) {
    auto tmp = lhs;
    tmp += rhs;
    return tmp;
}
inline BigInt<uint128_t> operator+(BigInt<uint128_t>&& lhs,
                                   const BigInt<uint128_t>& rhs) {
    lhs += rhs;
    return std::move(lhs);
}
inline BigInt<uint128_t> operator+(const BigInt<uint128_t>& lhs,
                                   BigInt<uint128_t>&& rhs) {
    return std::move(rhs) + lhs;
}
inline BigInt<uint128_t> operator+(BigInt<uint128_t>&& lhs,
                                   BigInt<uint128_t>&& rhs) {
    return std::move(lhs) + rhs;
}
// substract is not swappable
inline BigInt<uint128_t> operator-(BigInt<uint128_t> lhs,
                                   const BigInt<uint128_t>& rhs) {
    lhs -= rhs;
    return lhs;
}

// mul
inline BigInt<uint128_t> BigInt<uint128_t>::MNTMul(BigInt lhs,
                                                   const BigInt& rhs) {
    lhs.MNTMulEq(rhs);
    return lhs;
}
inline BigInt<uint128_t> BigInt<uint128_t>::RMNTMul(BigInt lhs,
                                                    const BigInt& rhs) {
    lhs.RMNTMulEq(rhs);
    return lhs;
}
inline BigInt<uint128_t> BigInt<uint128_t>::MulKaratsuba(BigInt lhs,
                                                         const BigInt& rhs) {
    lhs.MulEqKaratsuba(rhs);
    return lhs;
}
inline BigInt<uint128_t> operator*(BigInt<uint128_t> lhs, uint64_t rhs) {
    lhs *= rhs;
    return lhs;
}
inline BigInt<uint128_t> BigInt<uint128_t>::PlainMul(const BigInt& lhs,
                                                     BigInt&& rhs) {
    return PlainMul(std::move(rhs), lhs);
}
inline BigInt<uint128_t>& BigInt<uint128_t>::PlainMulEq(const BigInt& rhs) {
    return *this = PlainMul(std::move(*this), rhs);
}
inline BigInt<uint128_t> BigInt<uint128_t>::RMNTMulUB(const BigInt& lhs,
                                                      BigInt&& rhs) {
    return RMNTMulUB(std::move(rhs), lhs);
}
inline BigInt<uint128_t>& BigInt<uint128_t>::RMNTMulEqUB(const BigInt& rhs) {
    return *this = RMNTMulUB(std::move(*this), rhs);
}
inline BigInt<uint128_t>& BigInt<uint128_t>::operator*=(const BigInt& rhs) {
    if (rhs.len_ <= 16 || len_ <= 16) {
        return PlainMulEq(rhs);
    } else if ((len_ << 2) < rhs.len_ || (rhs.len_ << 2) < len_) {
        return RMNTMulEqUB(rhs);
    } else {
        return RMNTMulEq(rhs);
    }
}
inline BigInt<uint128_t> operator*(const BigInt<uint128_t>& lhs,
                                   const BigInt<uint128_t>& rhs) {
    if (rhs.Length() <= 16 || lhs.Length() <= 16) {
        return rhs.PlainMul(lhs, rhs);
    } else if ((lhs.Length() << 2) < rhs.Length() ||
               (rhs.Length() << 2) < lhs.Length()) {
        return rhs.RMNTMulUB(lhs, rhs);
    } else {
        return rhs.RMNTMul(lhs, rhs);
    }
}
inline BigInt<uint128_t> operator*(BigInt<uint128_t>&& lhs,
                                   const BigInt<uint128_t>& rhs) {
    if (rhs.Length() <= 16 || lhs.Length() <= 16) {
        return rhs.PlainMul(std::move(lhs), rhs);
    } else if ((lhs.Length() << 2) < rhs.Length() ||
               (rhs.Length() << 2) < lhs.Length()) {
        return rhs.RMNTMulUB(std::move(lhs), rhs);
    } else {
        return rhs.RMNTMul(std::move(lhs), rhs);
    }
}
inline BigInt<uint128_t> operator*(const BigInt<uint128_t>& lhs,
                                   BigInt<uint128_t>&& rhs) {
    return std::move(rhs) * lhs;
}
inline BigInt<uint128_t> operator*(BigInt<uint128_t>&& lhs,
                                   BigInt<uint128_t>&& rhs) {
    if (rhs.Length() <= 16 || lhs.Length() <= 16) {
        return rhs.PlainMul(std::move(lhs), std::move(rhs));
    } else if ((lhs.Length() << 2) < rhs.Length() ||
               (rhs.Length() << 2) < lhs.Length()) {
        return rhs.RMNTMulUB(std::move(lhs), std::move(rhs));
    } else {
        return rhs.RMNTMul(std::move(lhs), std::move(rhs));
    }
}

// div
inline BigInt<uint128_t>& BigInt<uint128_t>::DivEqD(const BigInt& rhs,
                                                    BigInt* mod) {
    return *this = DivD(std::move(*this), rhs, mod);
}
inline BigInt<uint128_t>& BigInt<uint128_t>::DivEqR(const BigInt& rhs,
                                                    BigInt* mod) {
    return *this = DivR(std::move(*this), rhs, mod);
}
inline BigInt<uint128_t> BigInt<uint128_t>::Div64(BigInt lhs, int64_t rhs,
                                                  int64_t* mod) {
    lhs.DivEq64(rhs, mod);
    return lhs;
}
inline int64_t operator%(BigInt<uint128_t> lhs, int64_t rhs) {
    int64_t rv;
    lhs.DivEq64(rhs, &rv);
    return rv;
}
inline BigInt<uint128_t> operator%(BigInt<uint128_t> lhs,
                                   const BigInt<uint128_t>& rhs) {
    lhs %= rhs;
    return lhs;
}

inline BigInt<uint128_t>& BigInt<uint128_t>::operator/=(int64_t rhs) {
    return DivEq64(rhs, nullptr);
}
inline BigInt<uint128_t> operator/(BigInt<uint128_t> lhs, int64_t rhs) {
    lhs.DivEq64(rhs, nullptr);
    return lhs;
}
inline BigInt<uint128_t> BigInt<uint128_t>::Div(BigInt lhs, const BigInt& rhs,
                                                BigInt* mod) {
    return lhs.DivEq(rhs, mod);
}
inline BigInt<uint128_t> operator/(BigInt<uint128_t> lhs,
                                   const BigInt<uint128_t>& rhs) {
    return lhs.DivEq(rhs, nullptr);
}
inline BigInt<uint128_t>& BigInt<uint128_t>::operator/=(const BigInt& rhs) {
    return DivEq(rhs, nullptr);
}
inline BigInt<uint128_t>& BigInt<uint128_t>::operator%=(const BigInt& rhs) {
    BigInt<uint128_t> rv;
    DivEq(rhs, &rv);
    return *this = std::move(rv);
}
inline BigInt<uint128_t> BigInt<uint128_t>::Square(BigInt lhs) {
    return lhs.SquareEq();
}
inline BigInt<uint128_t>& BigInt<uint128_t>::RandomBits(uint64_t bitlen) {
    auto q = (bitlen + 127) >> 7, r = bitlen & 127;
    return GenRandom(q, r ? r : 128);
}
}  // namespace calc
