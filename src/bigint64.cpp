#include "bigint64.hpp"

#include "bigint64_add.cpp"
#include "bigint64_basic.cpp"
#include "bigint64_bit.cpp"
#include "bigint64_compare.cpp"
#include "bigint64_div.cpp"
#include "bigint64_io.cpp"
#include "bigint64_mul.cpp"
#include "bigint64_ext.cpp"
namespace calc {
BigInt<uint128_t> operator&(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs) {
    return lhs &= rhs;
}
BigInt<uint128_t> operator|(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs) {
    return lhs |= rhs;
}
BigInt<uint128_t> operator^(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs) {
    return lhs ^= rhs;
}
BigInt<uint128_t> operator<<(BigInt<uint128_t> lhs, uint64_t rhs) {
    return lhs <<= rhs;
}
BigInt<uint128_t> operator>>(BigInt<uint128_t> lhs, uint64_t rhs) {
    return lhs >>= rhs;
}
#ifndef __cpp_impl_three_way_comparison
bool operator<(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) < 0;
}
bool operator>(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) > 0;
}
bool operator<=(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) <= 0;
}
bool operator>=(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) >= 0;
}
bool operator==(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) == 0;
}
bool operator!=(const BigInt<uint128_t>& lhs, const BigInt<uint128_t>& rhs) {
    return lhs.Compare(rhs) != 0;
}
#endif
BigInt<uint128_t> operator+(BigInt<uint128_t> lhs, uint64_t rhs) {
    return lhs += rhs;
}
BigInt<uint128_t> operator+(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs) {
    return lhs += rhs;
}
BigInt<uint128_t> operator-(BigInt<uint128_t> lhs, uint64_t rhs) {
    return lhs -= rhs;
}
BigInt<uint128_t> operator-(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs) {
    return lhs -= rhs;
}
BigInt<uint128_t> BigInt<uint128_t>::MNTMul(BigInt lhs, const BigInt& rhs) {
    return lhs.MNTMulEq(rhs);
}
BigInt<uint128_t> BigInt<uint128_t>::RMNTMul(BigInt lhs, const BigInt& rhs) {
    return lhs.RMNTMulEq(rhs);
}
BigInt<uint128_t> BigInt<uint128_t>::PlainMul(BigInt lhs, const BigInt& rhs) {
    return lhs.PlainMulEq(rhs);
}
BigInt<uint128_t> BigInt<uint128_t>::MulKaratsuba(BigInt lhs,
                                                  const BigInt& rhs) {
    return lhs.MulEqKaratsuba(rhs);
}
BigInt<uint128_t> operator*(BigInt<uint128_t> lhs, uint64_t rhs) {
    return lhs *= rhs;
}
BigInt<uint128_t> BigInt<uint128_t>::DivD(BigInt lhs, const BigInt& rhs,
                                          BigInt* mod) {
    return lhs.DivEqD(rhs, mod);
}
BigInt<uint128_t> BigInt<uint128_t>::Div(BigInt lhs, const BigInt& rhs,
                                         BigInt* mod) {
    return lhs.DivEq(rhs, mod);
}
BigInt<uint128_t> operator*(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs) {
    return lhs *= rhs;
}
BigInt<uint128_t> operator/(BigInt<uint128_t> lhs, int64_t rhs) {
    return lhs.DivEq64(rhs, nullptr);
}
BigInt<uint128_t> operator/(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs) {
    return lhs.DivEq(rhs, nullptr);
}
int64_t operator%(BigInt<uint128_t> lhs, int64_t rhs) {
    int64_t rv;
    lhs.DivEq64(rhs, &rv);
    return rv;
}
BigInt<uint128_t> operator%(BigInt<uint128_t> lhs,
                            const BigInt<uint128_t>& rhs) {
    return lhs %= rhs;
}
BigInt<uint128_t> BigInt<uint128_t>::Square(BigInt lhs) {
    return lhs.SquareEq();
}
BigInt<uint128_t> BigInt<uint128_t>::RMNTMulUB(BigInt lhs, const BigInt& rhs) {
    return lhs.RMNTMulEqUB(rhs);
}
}  // namespace calc
