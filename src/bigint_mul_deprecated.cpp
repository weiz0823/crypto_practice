#include "bigint.hpp"
namespace calc {
// deprecated: slightly inefficient
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::FFTMulEq(const BigInt& rhs) {
    size_t min_len = std::max(len_, rhs.len_);
    if (min_len > 100000000) {
        // use long double, but still unable to handle very very big number
        auto tmp_obj = BigInt<uint8_t>(val_, len_);
        tmp_obj.FFTMulEqExt(BigInt<uint8_t>(rhs.val_, rhs.len_));
        *this = BigInt<IntT>(tmp_obj.val_, tmp_obj.len_);
        return *this;
    } else if (LIMB >= 32 && LIMB * min_len < 80000) {
        auto tmp_obj = BigInt<uint16_t>(val_, len_);
        tmp_obj.FFTMulEq(BigInt<uint16_t>(rhs.val_, rhs.len_));
        *this = BigInt<IntT>(tmp_obj.val_, tmp_obj.len_);
        return *this;
    } else if (LIMB >= 16 && LIMB * min_len >= 80000) {
        auto tmp_obj = BigInt<uint8_t>(val_, len_);
        tmp_obj.FFTMulEq(BigInt<uint8_t>(rhs.val_, rhs.len_));
        *this = BigInt<IntT>(tmp_obj.val_, tmp_obj.len_);
        return *this;
    }
    if (rhs.Sign()) {
        FFTMulEq(-rhs);
        ToOpposite();
        return *this;
    }
    using T = std::complex<double>;
    T* v[2];
    size_t n = 1, i = 0;
    bool sign = Sign() ^ rhs.Sign();
    if (Sign()) ToOpposite();
    while (n < len_ + rhs.len_) n <<= 1;
    v[0] = new T[n];
    v[1] = new T[n];
    for (; i < len_; ++i) v[0][i] = val_[i];
    for (; i < n; ++i) v[0][i] = 0;
    for (i = 0; i < rhs.len_; ++i) v[1][i] = rhs.val_[i];
    for (; i < n; ++i) v[1][i] = 0;
    FFT(v[0], n, false);
    FFT(v[1], n, false);
    for (i = 0; i < n; ++i) v[0][i] *= v[1][i];
    FFT(v[0], n, true);
    SetLen(len_ + rhs.len_ + 1, true);
    uint64_t tmp = 0;
    for (i = 0; i < len_ - 1; ++i) {
        tmp =
            (tmp >> LIMB) + static_cast<uint64_t>(std::lround(v[0][i].real()));
        val_[i] = static_cast<IntT>(tmp);
    }
    if (sign) ToOpposite();
    ShrinkLen();
    delete[] v[0];
    delete[] v[1];
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::NMNTMulEq(const BigInt& rhs) {
    size_t min_len = std::max(len_, rhs.len_);
    if (LIMB * min_len >= 262144) {
        return FFTMulEq(rhs);
    } else if (LIMB >= 16) {
        auto tmp_obj = BigInt<uint8_t>(val_, len_);
        tmp_obj.NMNTMulEq(BigInt<uint8_t>(rhs.val_, rhs.len_));
        *this = BigInt<IntT>(tmp_obj.val_, tmp_obj.len_);
        return *this;
    }
    if (rhs.Sign()) {
        NMNTMulEq(-rhs);
        ToOpposite();
        return *this;
    }
    using T = CompMp;
    T* v[2];
    size_t n = 1, i = 0;
    bool sign = Sign() ^ rhs.Sign();
    if (Sign()) ToOpposite();
    while (n < len_ + rhs.len_) n <<= 1;
    v[0] = new T[n];
    v[1] = new T[n];
    for (; i < len_; ++i) v[0][i] = T(val_[i]);
    // for (; i < n; ++i) v[0][i] = T(0);
    for (i = 0; i < rhs.len_; ++i) v[1][i] = T(rhs.val_[i]);
    // for (; i < n; ++i) v[1][i] = T(0);
    NMNT(v[0], n, false);
    NMNT(v[1], n, false);
    for (i = 0; i < n; ++i) v[0][i] *= v[1][i];
    NMNT(v[0], n, true);
    SetLen(len_ + rhs.len_ + 1, true);
    uint64_t tmp = 0;
    for (i = 0; i < len_ - 1; ++i) {
        tmp = (tmp >> LIMB) + static_cast<uint64_t>(v[0][i].Real());
        val_[i] = static_cast<IntT>(tmp);
    }
    if (sign) ToOpposite();
    ShrinkLen();
    delete[] v[0];
    delete[] v[1];
    return *this;
}
// deprecated: inefficient
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::MulEqKaratsuba(const BigInt& rhs) {
    if (len_ <= 2 || rhs.len_ <= 2) return PlainMulEq(rhs);
    if (len_ <= 10 || rhs.len_ <= 10) return *this *= rhs;
    if (rhs.Sign()) {
        MulEqKaratsuba(-rhs);
        ToOpposite();
        return *this;
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    size_t t = (std::max(rhs.len_, len_) + 1) >> 1;
    BigInt<IntT> a, b, c, d, x, y, z;
    if (t >= len_) {
        b = BigInt<IntT>(val_, len_);
    } else {
        b = BigInt<IntT>(val_, t);
        if (b.Sign()) b.SetLen(b.len_ + 1, false);
        a = BigInt<IntT>(val_ + t, len_ - t);
    }
    if (t >= rhs.len_) {
        d = BigInt<IntT>(rhs.val_, rhs.len_);
    } else {
        d = BigInt<IntT>(rhs.val_, t);
        if (d.Sign()) d.SetLen(d.len_ + 1, false);
        c = BigInt<IntT>(rhs.val_ + t, rhs.len_ - t);
    }
    if (t >= len_) {
        d.MulEqKaratsuba(b);
        c.MulEqKaratsuba(b);
        // d *= b;
        // c *= b;
        c <<= t * LIMB;
        *this = std::move(c) + d;
    } else if (t >= rhs.len_) {
        a.MulEqKaratsuba(d);
        b.MulEqKaratsuba(d);
        // b *= d;
        // a *= d;
        a <<= t * LIMB;
        *this = std::move(a) + b;
    } else {
        x = a + b;
        x.MulEqKaratsuba(c + d);
        a.MulEqKaratsuba(c);
        b.MulEqKaratsuba(d);
        // x = (a + b) * (c + d);
        // a *= c;
        // b *= d;
        x -= a;
        x -= b;
        a <<= t * LIMB * 2;
        x <<= t * LIMB;
        *this = std::move(a) + x + b;
    }
    if (sign) ToOpposite();
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::MulEqToomCook3(const BigInt& rhs) {
    if (len_ <= 10 || rhs.len_ <= 10) return *this *= rhs;
    // avoid call with len/rhs.len>3/2 or <2/3
    if (len_ <= 6 || rhs.len_ <= 6) return PlainMulEq(rhs);
    if (rhs.Sign()) {
        MulEqToomCook3(-rhs);
        ToOpposite();
        return *this;
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    size_t t = (std::max(rhs.len_, len_) + 2) / 3;
    BigInt<IntT> p[3], q[3], a[3], b[3];
    if (t >= len_) {
        p[0] = BigInt<IntT>(val_, len_);
    } else if (t + t >= len_) {
        p[0] = BigInt<IntT>(val_, t);
        if (p[0].Sign()) p[0].SetLen(p[0].len_ + 1, false);
        p[1] = BigInt<IntT>(val_ + t, len_ - t);
    } else {
        p[0] = BigInt<IntT>(val_, t);
        if (p[0].Sign()) p[0].SetLen(p[0].len_ + 1, false);
        p[1] = BigInt<IntT>(val_ + t, t);
        if (p[1].Sign()) p[1].SetLen(p[1].len_ + 1, false);
        p[2] = BigInt<IntT>(val_ + t + t, len_ - t - t);
    }
    if (t >= rhs.len_) {
        q[0] = BigInt<IntT>(rhs.val_, rhs.len_);
    } else if (t + t >= rhs.len_) {
        q[0] = BigInt<IntT>(rhs.val_, t);
        if (q[0].Sign()) q[0].SetLen(q[0].len_ + 1, false);
        q[1] = BigInt<IntT>(rhs.val_ + t, rhs.len_ - t);
    } else {
        q[0] = BigInt<IntT>(rhs.val_, t);
        if (q[0].Sign()) q[0].SetLen(q[0].len_ + 1, false);
        q[1] = BigInt<IntT>(rhs.val_ + t, t);
        if (q[1].Sign()) q[1].SetLen(q[1].len_ + 1, false);
        q[2] = BigInt<IntT>(rhs.val_ + t + t, rhs.len_ - t - t);
    }
    size_t mov = t * LIMB;
    if (t >= len_) {
        q[0] *= p[0];
        q[1] *= p[0];
        q[2] *= p[0];
        q[1] <<= mov;
        q[2] <<= mov + mov;
        *this = std::move(q[2]) + q[1] + q[0];
    } else if (t >= rhs.len_) {
        p[0] *= q[0];
        p[1] *= q[0];
        p[2] *= q[0];
        p[1] <<= mov;
        p[2] <<= mov + mov;
        *this = std::move(p[2]) + p[1] + p[0];
    } else {
        a[0] = p[0] + p[2];
        a[1] = a[0] - p[1];
        a[0] += p[1];
        a[2] = a[1] - p[1];
        a[2] += p[2];
        a[2] += p[2];
        a[2] += p[2];
        b[0] = q[0] + q[2];
        b[1] = b[0] - q[1];
        b[0] += q[1];
        b[2] = b[1] - q[1];
        b[2] += q[2];
        b[2] += q[2];
        b[2] += q[2];

        // p[0].MulEqToomCook3(q[0]);
        // p[2].MulEqToomCook3(q[2]);
        // a[0].MulEqToomCook3(a[0]);
        // a[1].MulEqToomCook3(a[1]);
        // a[2].MulEqToomCook3(a[2]);
        p[0] *= q[0];
        p[2] *= q[2];
        a[0] *= b[0];
        a[1] *= b[1];
        a[2] *= b[2];

        b[2] = (std::move(a[2]) - a[0]) / IntT(3);
        b[0] = std::move(a[0]) - a[1];
        b[0] >>= 1;
        b[1] = std::move(a[1]) - p[0];
        b[2] = ((b[1] - b[2]) >> 1) + p[2] + p[2];
        b[1] += b[0];
        b[1] -= p[2];
        b[0] -= b[2];

        p[2] <<= mov << 2;
        b[2] <<= mov * 3;
        b[1] <<= mov + mov;
        b[0] <<= mov;
        *this = std::move(p[2]) + b[2] + b[1] + b[0] + p[0];
    }
    if (sign) ToOpposite();
    return *this;
}

// non-modifying
template <typename IntT>
BigInt<IntT> BigInt<IntT>::FFTMul(BigInt lhs, const BigInt& rhs) {
    return lhs.FFTMulEq(rhs);
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::NMNTMul(BigInt lhs, const BigInt& rhs) {
    return lhs.NMNTMulEq(rhs);
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::MulKaratsuba(BigInt lhs, const BigInt& rhs) {
    return lhs.MulEqKaratsuba(rhs);
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::MulToomCook3(BigInt lhs, const BigInt& rhs) {
    return lhs.MulEqToomCook3(rhs);
}

// FFT
template <typename IntT>
void BigInt<IntT>::FFT(std::complex<double>* dest, size_t n, bool inv) {
    using T = std::complex<double>;
    // must ensure n is a power of 2
    double PI;
    if (inv)
        PI = -std::acos(-1.0);
    else
        PI = std::acos(-1.0);
    size_t i = 0, j = 0;
    size_t step2;
    T omega_n, omega, tmp;
    BitRevSort(dest, n);
    for (size_t step = 2; step <= n; step <<= 1) {
        step2 = step >> 1;
        omega_n = T(std::cos(PI / step2), std::sin(PI / step2));
        omega = T(1.0, 0.0);
        for (j = 0; j < step2; ++j) {
            for (i = 0; i < n; i += step) {
                tmp = omega * dest[i + j + step2];
                dest[i + j + step2] = dest[i + j] - tmp;
                dest[i + j] += tmp;
            }
            omega *= omega_n;
        }
    }
    if (inv)
        for (i = 0; i < n; ++i) dest[i] /= n;
}
template <typename IntT>
void BigInt<IntT>::NMNT(CompMp* dest, size_t n, bool inv) {
    using T = CompMp;
    // must ensure n is a power of 2
    size_t i = 0, j = 0;
    size_t step2, step3 = 0;
    T omega_n, omega, tmp;
    BitRevSort(dest, n);
    for (size_t step = 2; step <= n; step <<= 1, ++step3) {
        step2 = step >> 1;
        omega_n = T::WN[step3];
        if (inv) omega_n.ToConj();
        omega = T(1, 0);
        for (j = 0; j < step2; ++j) {
            for (i = 0; i < n; i += step) {
                tmp = omega * dest[i + j + step2];
                dest[i + j + step2] = dest[i + j] - tmp;
                dest[i + j] += tmp;
            }
            omega *= omega_n;
        }
    }
    --step3;
    if (inv)
        for (i = 0; i < n; ++i) dest[i] *= T::ModInv[step3];
}
}  // namespace calc
