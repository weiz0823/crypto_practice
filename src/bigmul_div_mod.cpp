#include <cassert>
#include "bigint.hpp"
namespace cryp {
template <unsigned LEN>
BigUInt<LEN> operator*(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs) {
    return lhs *= rhs;
}
template <unsigned LEN>
BigUInt<LEN> operator/(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs) {
    return lhs /= rhs;
}
template <unsigned LEN>
BigUInt<LEN> operator%(BigUInt<LEN> lhs, const BigUInt<LEN>& rhs) {
    return lhs %= rhs;
}

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
BigUInt<LEN>& BigUInt<LEN>::operator*=(const BigUInt<LEN>& rhs) {
    return MulEq_Plain(rhs);
}

template <unsigned LEN>
[[maybe_unused]] BigUInt<LEN>& BigUInt<LEN>::MulEq_Karatsuba_Recursion(
    const BigUInt<LEN>& rhs) {
    // divide (radix-2) and conquer
    // (ax+b)(cx+d)=acx^2+((a+b)(c+d)-ac-bd)x+bd
    if (LEN & 1) {
        return MulEq_Plain(rhs);
    } else if (LEN == 4) {
        uint64_t a = val_[0], b = rhs.val_[1];
        uint64_t c = (a + val_[1]) * (rhs.val_[0] + b);
        a *= rhs.val_[0];
        b *= val_[1];
        val_[0] = static_cast<unsigned>(a);
        val_[1] = static_cast<unsigned>(c + (a >> 32));
        val_[2] = static_cast<unsigned>(b + (c >> 32));
        val_[3] = static_cast<unsigned>(b >> 32);
    } else {
        BigUInt<LEN / 2> a(val_ + LEN / 4, LEN / 4), b(val_, LEN / 4);
        BigUInt<LEN / 2> c(rhs.val_ + LEN / 4, LEN / 4), d(rhs.val_, LEN / 4);
        BigUInt<LEN / 2> t(a + b);
        t.MulEq_Karatsuba_Recursion(c + d);
        b.MulEq_Karatsuba_Recursion(d);
        a.MulEq_Karatsuba_Recursion(c);
        t -= a;
        t -= b;
        BigUInt<LEN> tmp(a.val_, LEN / 2);
        *this = tmp << (32 * LEN / 2);
        std::copy(t.val_, t.val_ + LEN / 2, tmp.val_);
        *this += tmp << (32 * LEN / 4);
        std::copy(b.val_, b.val_ + LEN / 2, tmp.val_);
        *this += tmp;
    }
    return *this;
}

template <unsigned LEN>
[[maybe_unused]] BigUInt<LEN>& BigUInt<LEN>::MulEq_Karatsuba(
    const BigUInt<LEN>& rhs) {
    // divide (radix-2) and conquer
    // (ax+b)(cx+d)=acx^2+((a+b)(c+d)-ac-bd)x+bd
    if (LEN == 1) {
        val_[0] *= rhs.val_[0];
    } else if (LEN == 2) {
        uint64_t a = val_[0], b = rhs.val_[1];
        uint64_t c = (a + val_[1]) * (rhs.val_[0] + b);
        a *= rhs.val_[0];
        b *= val_[1];
        val_[0] = static_cast<unsigned>(a);
        val_[1] = static_cast<unsigned>(c + (a >> 32));
    } else {
        BigUInt<LEN> a(val_ + LEN / 2, LEN / 2), b(val_, LEN / 2);
        BigUInt<LEN> c(rhs.val_ + LEN / 2, LEN / 2), d(rhs.val_, LEN / 2);
        BigUInt<LEN> t(a + b);
        t.MulEq_Karatsuba_Recursion(c + d);
        b.MulEq_Karatsuba_Recursion(d);
        a.MulEq_Karatsuba_Recursion(c);
        t -= a;
        t -= b;
        *this = (t << (32 * LEN / 2)) + b;
    }
    return *this;
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::DivEq_Basic(unsigned rhs, unsigned* r_) {
    uint64_t r = 0;
    for (int i = int(LEN) - 1; i >= 0; --i) {
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
BigUInt<LEN>& BigUInt<LEN>::operator/=(const BigUInt<LEN>& rhs) {
    return DivEq_Plain(rhs);
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::operator%=(const BigUInt<LEN>& rhs) {
    return DivEq_Plain(rhs, this);
}

template <unsigned LEN>
BigUInt<LEN>& BigUInt<LEN>::DivEq_Plain(const BigUInt<LEN>& rhs,
                                        BigUInt<LEN>* r_) {
    if (!rhs) {
        std::fprintf(stderr, "BigUInt<%u>: integer div 0 error!\n", LEN);
        std::fill(val_, val_ + LEN, -1);
        if (r_) *r_ = *this;
    }
    if (*this < rhs) {
        if (r_ != this) {
            if (r_) *r_ = std::move(*this);
            std::fill(val_, val_ + LEN, 0);
        }
    } else if (rhs < (BigUInt<LEN>(1) << 32)) {
        unsigned rr;
        DivEq_Basic(rhs.val_[0], &rr);
        *r_ = BigUInt<LEN>(rr);
    } else {
        bool flag = false;
        BigUInt<LEN / 2> q_high(val_ + LEN / 2, LEN / 2);
        if (LEN >= 4 && rhs < (BigUInt<LEN>(1) << (16 * LEN))) {
            flag = true;
            BigUInt<LEN / 2> trhs(rhs.val_, LEN / 2);
            BigUInt<LEN / 2> trr;
            q_high.DivEq_Plain(trhs, &trr);
            std::copy(trr.val_, trr.val_ + LEN / 2, val_ + LEN / 2);
        }
        unsigned l1 = LEN - 1, l2 = LEN - 1;
        BigUInt<LEN> q;
        while (val_[l1] == 0) --l1;
        while (rhs.val_[l2] == 0) --l2;
        BigUInt<LEN> tmp = rhs << int((l1 - l2) * 32);
        unsigned tt = tmp.val_[LEN - 1];
        int bitlen = 0;
        unsigned i = l2;
        if (tmp.val_[LEN - 1]) {
            ++i;
            while (tt) {
                tt >>= 1;
                ++bitlen;
            }
            tmp <<= (32 - bitlen);
            if (tmp <= *this) {
                ++q;
                *this -= tmp;
            }
            for (int j = bitlen; j < 32; ++j) {
                q <<= 1;
                tmp >>= 1;
                if (tmp <= *this) {
                    ++q;
                    *this -= tmp;
                }
            }
        } else {
            tmp <<= 32;
        }
        for (; i <= l1; ++i) {
            for (unsigned j = 0; j < 32; ++j) {
                q <<= 1;
                tmp >>= 1;
                if (tmp <= *this) {
                    ++q;
                    *this -= tmp;
                }
            }
        }
        if (flag)
            std::copy(q_high.val_, q_high.val_ + LEN / 2, q.val_ + LEN / 2);
        if (r_ != this) {
            if (r_) *r_ = std::move(*this);
            *this = std::move(q);
        }
    }
    return *this;
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
    for (int i = int(LEN) - 1; i > 0; --i) r = (r + 6 * (val_[i] % 10)) % 10;
    return (r + val_[0] % 10) % 10;
}
}  // namespace cryp
