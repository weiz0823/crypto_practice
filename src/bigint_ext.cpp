#include <cmath>

#include "bigint.hpp"
namespace calc {
// extended arithmetic
template <typename IntT>
double BigInt<IntT>::log2() const {
    constexpr size_t seg = 64 / LIMB;
    int64_t x = 0;
    size_t i = len_;
    while (i > 0 && i > len_ - seg) x = (x << LIMB) | val_[--i];
    return std::log2(double(x)) + i * LIMB;
}
template <typename IntT>
double BigInt<IntT>::log10() const {
    return log2() / std::log2(10.0);
}
template <typename IntT>
bool BigInt<IntT>::isProbablePrime() const {
    if (Sign()) {
        BigInt<IntT> tmp_obj = -*this;
        return tmp_obj.isProbablePrime();
    }
    // Miller-Rabin primality test
    BigInt<IntT> a[14] = {BigInt<IntT>(3),  BigInt<IntT>(5),  BigInt<IntT>(7),
                          BigInt<IntT>(11), BigInt<IntT>(13), BigInt<IntT>(17),
                          BigInt<IntT>(19), BigInt<IntT>(23), BigInt<IntT>(29),
                          BigInt<IntT>(31), BigInt<IntT>(37), BigInt<IntT>(41),
                          BigInt<IntT>(43), BigInt<IntT>(47)};
    int cmp = Compare(BigInt<IntT>(2));
    if (cmp < 0)
        return false;
    else if (cmp == 0)
        return true;
    else if (!Parity())
        return false;
    BigInt<IntT> t(*this);
    BigInt<IntT> cond1(t >> 1);
    --t;
    BigInt<IntT> cond2(-1);
    size_t s = t.TrailingZero();
    t >>= s;
    BigInt<IntT> tmp;
    size_t i;
    for (auto& x : a) {
        tmp = PowMod(x, t, *this);
        if (tmp > cond1) tmp -= *this;
        if (tmp == BigInt<IntT>(1) || tmp == cond2) continue;
        for (i = 0; i < s - 1; ++i) {
            tmp = tmp * tmp % *this;
            if (tmp > cond1) tmp -= *this;
            if (tmp == cond2) break;
        }
        if (i == s - 1) return false;
    }
    return true;
}

template <typename IntT>
BigInt<IntT> BigProduct(uint64_t a, uint64_t b) {
    if (a > b)
        return BigInt<IntT>(1);
    else if (a == b)
        return BigInt<IntT>(a);
    else if (a + 1 == b)
        return BigInt<IntT>(a) * BigInt<IntT>(b);
    else if (b - a < 8)
        return BigProduct<IntT>(a, a / 2 + b / 2) *
               BigProduct<IntT>(a / 2 + b / 2 + 1, b);
    else
        return BigProduct<IntT>(a, a / 4 + b / 4 * 3) *
               BigProduct<IntT>(a / 4 + b / 4 * 3 + 1, b);
}
template <typename IntT>
BigInt<IntT> Factorial(uint64_t n) {
    constexpr uint64_t fac[20] = {1ul,
                                  1ul,
                                  2ul,
                                  6ul,
                                  24ul,
                                  120ul,
                                  720ul,
                                  5040ul,
                                  40320ul,
                                  362880ul,
                                  3628800ul,
                                  39916800ul,
                                  479001600ul,
                                  6227020800ul,
                                  87178291200ul,
                                  1307674368000ul,
                                  20922789888000ul,
                                  355687428096000ul,
                                  6402373705728000ul,
                                  121645100408832000ul};
    if (n < 20) {
        return BigInt<IntT>(fac[n]);
    } else {
        return BigInt<IntT>(fac[19]) * BigProduct<IntT>(20, n);
    }
}
template <typename IntT>
BigInt<IntT> Power(const BigInt<IntT>& a, uint64_t p) {
    BigInt<IntT> tmp(a), result(1);
    if (!p) return BigInt<IntT>(1);
    uint64_t mask = 1;
    for (; mask <= p; mask <<= 1) {
        if (p & mask) result *= tmp;
        tmp = tmp * tmp;
    }
    return result;
}
template <typename IntT>
BigInt<IntT> PowMod(const BigInt<IntT>& a, const BigInt<IntT>& p,
                    const BigInt<IntT>& n) {
    BigInt<IntT> tmp(a), result(1);
    if (p.Sign()) return BigInt<IntT>(0);
    if (!p) return BigInt<IntT>(1);
    if (tmp > n) tmp %= n;
    IntT mask = 1;
    for (size_t i = 0; i < p.len_; ++i) {
        for (mask = 1; mask; mask <<= 1) {
            if (i == p.len_ - 1 && mask > p.val_[i]) break;
            if (p.val_[i] & mask) result *= tmp;
            tmp = tmp * tmp;
            if (tmp.len_ > n.len_) tmp %= n;
            if (result.len_ > n.len_) result %= n;
        }
    }
    return result;
}
template <typename IntT>
BigInt<IntT> PowMod(const BigInt<IntT>& a, uint64_t p, const BigInt<IntT>& n) {
    BigInt<IntT> tmp(a), result(1);
    if (!p) return BigInt<IntT>(1);
    if (tmp > n) tmp %= n;
    uint64_t mask = 1;
    for (; mask <= p; mask <<= 1) {
        if (p & mask) result *= tmp;
        tmp = tmp * tmp;
        if (tmp.len_ > n.len_) tmp %= n;
        if (result.len_ > n.len_) result %= n;
    }
    return result;
}
template <typename IntT>
BigInt<IntT> GcdBin(BigInt<IntT> a, BigInt<IntT> b) {
    a.ToAbsolute();
    b.ToAbsolute();
    size_t q1 = a.TrailingZero(), q2 = b.TrailingZero();
    int cmp;
    a >>= q1;
    b >>= q2;
    while (true) {
        cmp = a.Compare(b);
        if (cmp < 0) {
            b -= a;
            b >>= b.TrailingZero();
        } else if (cmp > 0) {
            a -= b;
            a >>= a.TrailingZero();
        } else {
            return a <<= std::min(q1, q2);
        }
    }
}
template <typename IntT>
BigInt<IntT> ExtGcdBin(BigInt<IntT> a, BigInt<IntT> b, BigInt<IntT>* x,
                       BigInt<IntT>* y) {
    a.ToAbsolute();
    b.ToAbsolute();
    // p*a0+q*b0=a, r*a0+s*b0=b
    BigInt<IntT> p(1), q(0), r(0), s(1);
    size_t t0 = std::min(a.TrailingZero(), b.TrailingZero());
    size_t t1, t2;
    bool swapped = false;
    int cmp;
    a >>= t0;
    b >>= t0;
    if (!a.Parity()) {
        std::swap(a, b);
        swapped = true;
    }
    BigInt<IntT> a0(a), b0(b);
    if (!b.Parity()) {
        r += b0;
        s -= a0;
        t1 = b.TrailingZero();
        t2 = s.TrailingZero();
        b >>= t1;
        while (t1 > t2) {
            r >>= t2;
            s >>= t2;
            if (r.Sign()) {
                r += b0;
                s -= a0;
            } else {
                r -= b0;
                s += a0;
            }
            t1 -= t2;
            t2 = s.TrailingZero();
        }
        r >>= t1;
        s >>= t1;
    }
    while (true) {
        cmp = a.Compare(b);
        if (cmp < 0) {
            b -= a;
            r -= p;
            s -= q;
            t1 = b.TrailingZero();
            t2 = s.TrailingZero();
            b >>= t1;
            while (t1 > t2) {
                r >>= t2;
                s >>= t2;
                if (r.Sign()) {
                    r += b0;
                    s -= a0;
                } else {
                    r -= b0;
                    s += a0;
                }
                t1 -= t2;
                t2 = s.TrailingZero();
            }
            r >>= t1;
            s >>= t1;
        } else if (cmp > 0) {
            a -= b;
            p -= r;
            q -= s;
            t1 = a.TrailingZero();
            t2 = q.TrailingZero();
            a >>= t1;
            while (t1 > t2) {
                p >>= t2;
                q >>= t2;
                if (p.Sign()) {
                    p += b0;
                    q -= a0;
                } else {
                    p -= b0;
                    q += a0;
                }
                t1 -= t2;
                t2 = q.TrailingZero();
            }
            p >>= t1;
            q >>= t1;
        } else {
            if (swapped) {
                if (x) *x = q;
                if (y) *y = p;
            } else {
                if (x) *x = p;
                if (y) *y = q;
            }
            return a <<= t0;
        }
    }
}
}  // namespace calc
