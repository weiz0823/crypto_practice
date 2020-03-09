#include "bigint.hpp"
namespace cryp {
template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator*=(unsigned rhs) {
    uint64_t t = 0;
    for (unsigned i = 0; i < LEN; ++i) {
        t += static_cast<uint64_t>(rhs) * val_[i];
        val_[i] = static_cast<unsigned>(t);
        t >>= 32;  // save overflow
    }
    return *this;
}

template <unsigned LEN>
BigUInt<LEN> operator*(BigUInt<LEN> lhs, unsigned rhs) {
    return lhs *= rhs;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::MulEq_Plain(const BigUInt<LEN>& rhs) {
    BigUInt<LEN> t = *this;  // make a copy
    *this = t * rhs.val_[0];
    for (unsigned i = 1; i < LEN; ++i)
        *this += (t * rhs.val_[i]) << static_cast<int>(32 * i);
    return *this;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::DivEq_Basic(unsigned rhs, unsigned* r_) {
    uint64_t r = 0;
    for (int i = LEN - 1; i >= 0; --i) {
        r <<= 32;
        r += val_[i];
        val_[i] = static_cast<unsigned>(r / rhs);
        r %= rhs;
    }
    if (r_ != nullptr) {
        *r_ = unsigned(r);
    }
    return *this;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator/=(unsigned rhs) {
    return DivEq_Basic(rhs);
}

template <unsigned LEN>
BigUInt<LEN> operator/(BigUInt<LEN> lhs, unsigned rhs) {
    return lhs /= rhs;
}

template <unsigned LEN>
unsigned BigUInt<LEN>::Mod10() const {
    // 2^32 = 6 mod 10
    // 6^n = 6 mod 10
    unsigned r = 0;
    for (int i = LEN - 1; i > 0; --i) r = (r + 6 * (val_[i] % 10)) % 10;
    return (r + val_[0] % 10) % 10;
}
}  // namespace cryp
