#include "bigint.hpp"
namespace calc {
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator*=(IntT rhs) {
    if (rhs == 0) {
        std::fill(val_, val_ + len_, IntT(0));
        len_ = 1;
        return *this;
    } else if (rhs == 1) {
        return *this;
    }
    uint64_t t = 0;
    bool sign = Sign();
    if (sign) ToOpposite();

    IntT tmp_rhs = rhs, log_rhs = 0;
    while (!(tmp_rhs & 1)) {
        ++log_rhs;
        tmp_rhs >>= 1;
    }

    if (tmp_rhs == 1) {
        *this <<= log_rhs;
    } else {
        for (size_t i = 0; i < len_; ++i) {
            t = (t >> LIMB) + uint64_t(rhs) * val_[i];
            val_[i] = static_cast<IntT>(t);
        }
        t >>= LIMB;
        if (t) {
            SetLen(len_ + 1, true);
            val_[len_ - 1] = static_cast<IntT>(t);
        }
    }
    if (Sign()) SetLen(len_ + 1, false);

    if (sign) ToOpposite();
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator*=(const BigInt& rhs) {
    if (len_ < 4 || rhs.len_ < 4) {
        return PlainMulEq(rhs);
    } else if (len_ <= rhs.len_) {
        if (rhs.len_ / 2 < len_) return FFTMulEq(rhs);
        if (rhs.Sign()) {
            *this *= -rhs;
            ToOpposite();
        } else {
            size_t t = rhs.len_ / 2;
            BigInt<IntT> x(rhs.val_, t), y(rhs.val_ + t, rhs.len_ - t);
            x.SetLen(x.len_ + 1, false);
            x.ShrinkLen();
            y.ShrinkLen();
            y *= *this;
            *this *= x;
            *this += (std::move(y) << (t * LIMB));
        }
    } else {
        if (len_ / 2 < rhs.len_) return FFTMulEq(rhs);
        bool sign = Sign();
        if (sign) ToOpposite();
        size_t t = len_ / 2;
        BigInt<IntT> x(val_, t), y(val_ + t, len_ - t);
        x.SetLen(x.len_ + 1, false);
        x.ShrinkLen();
        y.ShrinkLen();
        x *= rhs;
        y *= rhs;
        *this = std::move(x);
        *this += (std::move(y) << (t * LIMB));
        if (sign) ToOpposite();
    }
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::PlainMulEq(const BigInt& rhs) {
    if (rhs.Sign()) {
        PlainMulEq(-rhs);
        ToOpposite();
        return *this;
    }
    BigInt<IntT> result;
    result.SetLen(len_ + rhs.len_ + 1, true);
    bool sign = Sign() ^ rhs.Sign();
    if (Sign()) ToOpposite();
    for (size_t i = 0; i < rhs.len_; ++i)
        result += ((*this) * rhs.val_[i]) << (LIMB * i);
    *this = result;
    if (sign) ToOpposite();
    ShrinkLen();
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::FFTMulEq(const BigInt& rhs) {
    size_t min_len = std::max(len_, rhs.len_);
    if (LIMB >= 32 && min_len < 2500) {
        auto tmp_obj = BigInt<uint16_t>(val_, len_);
        tmp_obj.FFTMulEq(BigInt<uint16_t>(rhs.val_, rhs.len_));
        *this = BigInt<IntT>(tmp_obj.val_, tmp_obj.len_);
        return *this;
    } else if ((LIMB >= 16 && min_len >= 5000) ||
               (LIMB >= 32 && min_len >= 2500)) {
        auto tmp_obj = BigInt<uint8_t>(val_, len_);
        tmp_obj.FFTMulEq(BigInt<uint8_t>(rhs.val_, rhs.len_));
        *this = BigInt<IntT>(tmp_obj.val_, tmp_obj.len_);
        return *this;
    } else if (min_len > 100000000) {
        // use long double, but still unable to handle very very big number
        auto tmp_obj = BigInt<uint8_t>(val_, len_);
        tmp_obj.FFTMulEqExt(BigInt<uint8_t>(rhs.val_, rhs.len_));
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
BigInt<IntT>& BigInt<IntT>::FFTMulEqExt(const BigInt& rhs) {
    if (rhs.Sign()) {
        FFTMulEqExt(-rhs);
        ToOpposite();
        return *this;
    }
    using T = std::complex<long double>;
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
    FFTExt(v[0], n, false);
    FFTExt(v[1], n, false);
    for (i = 0; i < n; ++i) v[0][i] *= v[1][i];
    FFTExt(v[0], n, true);
    SetLen(len_ + rhs.len_ + 1, true);
    uint64_t tmp = 0;
    for (i = 0; i < len_ - 1; ++i) {
        // note lroundl
        tmp =
            (tmp >> LIMB) + static_cast<uint64_t>(std::lroundl(v[0][i].real()));
        val_[i] = static_cast<IntT>(tmp);
    }
    if (sign) ToOpposite();
    ShrinkLen();
    delete[] v[0];
    delete[] v[1];
    return *this;
}

// non-modifying
template <typename IntT>
BigInt<IntT> BigInt<IntT>::PlainMul(BigInt lhs, const BigInt& rhs) {
    return lhs.PlainMulEq(rhs);
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::FFTMul(BigInt lhs, const BigInt& rhs) {
    return lhs.FFTMulEq(rhs);
}

// FFT
template <typename IntT>
template <typename T>
void BigInt<IntT>::BitRevSort(T* a, size_t n) {
    // must ensure n is a power of 2
    const uint64_t iv = n >> 1;
    uint64_t j = iv;
    uint64_t k;
    for (uint64_t i = 1; i < n - 1; ++i) {
        if (i < j) std::swap(a[i], a[j]);
        k = iv;
        // do a reversed increment on j
        while (k & j) {
            j ^= k;
            k >>= 1;
        }
        j ^= k;
    }
}
template <typename IntT>
void BigInt<IntT>::FFTExt(std::complex<long double>* dest, size_t n, bool inv) {
    using T = std::complex<long double>;
    // must ensure n is a power of 2
    long double PI;
    if (inv)
        PI = -std::acosl(-1.0l);
    else
        PI = std::acosl(-1.0l);
    size_t i = 0, j = 0;
    size_t step2;
    T omega_n, omega, tmp;
    BitRevSort(dest, n);
    for (size_t step = 2; step <= n; step <<= 1) {
        step2 = step >> 1;
        omega_n = T(std::cosl(PI / step2), std::sinl(PI / step2));
        omega = T(1.0l, 0.0l);
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

// non-modifying binary operators
template <typename IntT>
BigInt<IntT> operator*(BigInt<IntT> lhs, IntT rhs) {
    return lhs *= rhs;
}
template <typename IntT>
BigInt<IntT> operator*(BigInt<IntT> lhs, const BigInt<IntT>& rhs) {
    return lhs *= rhs;
}
}  // namespace calc
