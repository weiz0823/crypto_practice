#include "bigint.hpp"
namespace calc {
class CompMp {
    int64_t real_, imag_;

   public:
    explicit constexpr CompMp(int64_t real = 0, int64_t imag = 0)
        : real_(real), imag_(imag) {}
    constexpr CompMp& ModEq() {
        real_ = (real_ & MP) + (real_ >> P);
        real_ = (real_ & MP) + (real_ >> P);
        real_ = (real_ & MP) + (real_ >> P);
        imag_ = (imag_ & MP) + (imag_ >> P);
        imag_ = (imag_ & MP) + (imag_ >> P);
        imag_ = (imag_ & MP) + (imag_ >> P);
        return *this;
    }
    constexpr CompMp& operator+=(const CompMp& rhs) {
        real_ += rhs.real_;
        imag_ += rhs.imag_;
        real_ = (real_ & MP) + (real_ >> P);
        imag_ = (imag_ & MP) + (imag_ >> P);
        return *this;
    }
    constexpr CompMp& operator-=(const CompMp& rhs) {
        real_ -= rhs.real_;
        imag_ -= rhs.imag_;
        real_ = (real_ & MP) + (real_ >> P);
        imag_ = (imag_ & MP) + (imag_ >> P);
        return *this;
    }
    constexpr CompMp& operator*=(const CompMp& rhs) {
        int64_t t2 = imag_ * rhs.imag_, t3 = real_ + imag_;
        t3 = (t3 & MP) + (t3 >> P);
        t3 *= (rhs.real_ + rhs.imag_);
        t3 = (t3 & MP) + (t3 >> P);
        real_ *= rhs.real_;
        imag_ = t3 - t2 - real_;
        real_ -= t2;
        ModEq();
        return *this;
    }
    constexpr CompMp& operator*=(int64_t rhs) {
        real_ *= rhs;
        imag_ *= rhs;
        ModEq();
        return *this;
    }
    constexpr CompMp& ToConj() {
        imag_ = MP - imag_;
        return *this;
    }
    constexpr int64_t Real() const { return real_ >= MP ? real_ - MP : real_; }
    constexpr int64_t Imag() const { return imag_ >= MP ? imag_ - MP : imag_; }
    static constexpr int64_t P = 31;
    static constexpr int64_t MP = 2147483647;
    static const CompMp WN[P];
    static constexpr int64_t ModInv[P] = {
        1073741824, 536870912, 268435456, 134217728, 67108864, 33554432,
        16777216,   8388608,   4194304,   2097152,   1048576,  524288,
        262144,     131072,    65536,     32768,     16384,    8192,
        4096,       2048,      1024,      512,       256,      128,
        64,         32,        16,        8,         4,        2,
        1

    };
    static constexpr CompMp ModMp(CompMp x) { return x.ModEq(); }
    static constexpr int64_t ModMp(int64_t x) {
        x = (x & MP) + (x >> P);
        x = (x & MP) + (x >> P);
        x = (x & MP) + (x >> P);
        return x;
    }
    friend constexpr CompMp operator+(CompMp x, const CompMp& y) {
        return x += y;
    }
    friend constexpr CompMp operator-(CompMp x, const CompMp& y) {
        return x -= y;
    }
    friend constexpr CompMp operator*(CompMp x, const CompMp& y) {
        return x *= y;
    }
};
const CompMp CompMp::WN[CompMp::P] = {CompMp(2147483646, 0),
                                      CompMp(0, 1),
                                      CompMp(32768, 32768),
                                      CompMp(1556715293, 978592373),
                                      CompMp(1241207368, 1179735656),
                                      CompMp(1641940819, 26164677),
                                      CompMp(206059115, 1935040570),
                                      CompMp(236104903, 1577470940),
                                      CompMp(430821412, 1152650470),
                                      CompMp(2118812954, 1778905319),
                                      CompMp(1007165928, 96763266),
                                      CompMp(1860715661, 513820856),
                                      CompMp(503566987, 691000908),
                                      CompMp(1733763612, 1375762489),
                                      CompMp(1556415982, 1095657803),
                                      CompMp(2011288684, 1058967642),
                                      CompMp(455787664, 555296581),
                                      CompMp(758674321, 160114785),
                                      CompMp(763747008, 1297503800),
                                      CompMp(1714604740, 1889070037),
                                      CompMp(1930613611, 768607679),
                                      CompMp(1281838047, 186019763),
                                      CompMp(1705978478, 781338124),
                                      CompMp(92950461, 1574605965),
                                      CompMp(329019877, 1113832336),
                                      CompMp(1975567928, 27121372),
                                      CompMp(948821812, 1003062899),
                                      CompMp(46099201, 1943377242),
                                      CompMp(4801, 1683836729),
                                      CompMp(49, 2142752556),
                                      CompMp(5, 643771985)};
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
        if (rhs.len_ / 2 < len_) {
            if constexpr (LIMB <= 8)
                return MNTMulEq(rhs);
            else
                return NFFTMulEq(rhs);
        }
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
        if (len_ / 2 < rhs.len_) {
            if constexpr (LIMB <= 8)
                return MNTMulEq(rhs);
            else
                return NFFTMulEq(rhs);
        }
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
BigInt<IntT>& BigInt<IntT>::Square() {
    if (len_ < 5) return *this = *this * *this;
    if (LIMB >= 32 && LIMB * len_ < 80000) {
        BigInt<uint16_t> tmp_obj;
        tmp_obj.MoveFrom(std::move(*this));
        tmp_obj.Square();
        return MoveFrom(std::move(tmp_obj));
    } else if (LIMB >= 16 && LIMB * len_ >= 80000) {
        BigInt<uint8_t> tmp_obj;
        tmp_obj.MoveFrom(std::move(*this));
        tmp_obj.Square();
        return MoveFrom(std::move(tmp_obj));
    }
    using T = double;
    T* v;
    size_t n = 1, i = 0;
    ToAbsolute();
    while (n < len_ + len_) n <<= 1;
    v = new T[n];
    for (; i < len_; ++i) v[i] = val_[i];
    for (; i < n; ++i) v[i] = 0;
    NFFT(v, n, false);
    T t1, t2, t3, t4;
    size_t n2 = n >> 1;
    v[0] *= v[0];
    v[n2] *= v[n2];
    for (i = 1; i < n2; ++i) {
        t1 = v[i];
        t2 = v[n - i];
        t3 = (v[i] + v[n - i]) / 2.0;
        t4 = (v[i] - v[n - i]) / 2.0;
        v[i] = t1 * t3 + t2 * t4;
        v[n - i] = t2 * t3 - t1 * t4;
    }
    NFFT(v, n, true);
    SetLen(len_ + len_ + 1, true);
    uint64_t tmp = 0;
    for (i = 0; i < len_ - 1; ++i) {
        tmp = (tmp >> LIMB) + static_cast<uint64_t>(std::lround(v[i]));
        val_[i] = static_cast<IntT>(tmp);
    }
    ShrinkLen();
    delete[] v;
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
    *this = std::move(result);
    if (sign) ToOpposite();
    ShrinkLen();
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::NFFTMulEq(const BigInt& rhs) {
    size_t min_len = std::max(len_, rhs.len_);
    if (LIMB >= 32 && LIMB * min_len < 80000) {
        BigInt<uint16_t> tmp_obj, tmp_rhs;
        tmp_obj.MoveFrom(std::move(*this));
        tmp_rhs.LinkAs(rhs);
        tmp_obj.NFFTMulEq(tmp_rhs);
        tmp_rhs.DetachLink(rhs);
        return MoveFrom(std::move(tmp_obj));
    } else if (LIMB >= 16 && LIMB * min_len >= 80000) {
        BigInt<uint8_t> tmp_obj, tmp_rhs;
        tmp_obj.MoveFrom(std::move(*this));
        tmp_rhs.LinkAs(rhs);
        tmp_obj.NFFTMulEq(tmp_rhs);
        tmp_rhs.DetachLink(rhs);
        return MoveFrom(std::move(tmp_obj));
    }
    if (rhs.Sign()) {
        NFFTMulEq(-rhs);
        ToOpposite();
        return *this;
    }
    using T = double;
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
    NFFT(v[0], n, false);
    NFFT(v[1], n, false);
    T t1, t2, t3, t4;
    size_t n2 = n >> 1;
    v[0][0] *= v[1][0];
    v[0][n2] *= v[1][n2];
    for (i = 1; i < n2; ++i) {
        t1 = v[0][i];
        t2 = v[0][n - i];
        t3 = (v[1][i] + v[1][n - i]) / 2.0;
        t4 = (v[1][i] - v[1][n - i]) / 2.0;
        v[0][i] = t1 * t3 + t2 * t4;
        v[0][n - i] = t2 * t3 - t1 * t4;
    }
    NFFT(v[0], n, true);
    SetLen(len_ + rhs.len_ + 1, true);
    uint64_t tmp = 0;
    for (i = 0; i < len_ - 1; ++i) {
        tmp = (tmp >> LIMB) + static_cast<uint64_t>(std::lround(v[0][i]));
        val_[i] = static_cast<IntT>(tmp);
    }
    if (sign) ToOpposite();
    ShrinkLen();
    delete[] v[0];
    delete[] v[1];
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::MNTMulEq(const BigInt& rhs) {
    size_t min_len = std::max(len_, rhs.len_);
    if (LIMB * min_len >= 262144) {
        return NFFTMulEq(rhs);
    } else if (LIMB >= 16) {
        BigInt<uint8_t> tmp_obj, tmp_rhs;
        tmp_obj.MoveFrom(std::move(*this));
        tmp_rhs.LinkAs(rhs);
        tmp_obj.NFFTMulEq(tmp_rhs);
        tmp_rhs.DetachLink(rhs);
        return MoveFrom(std::move(tmp_obj));
    }
    if (rhs.Sign()) {
        MNTMulEq(-rhs);
        ToOpposite();
        return *this;
    }
    using T = int64_t;
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
    MNT(v[0], n, false);
    MNT(v[1], n, false);
    T t1, t2, t3, t4;
    size_t n2 = n >> 1;
    v[0][0] = CompMp::ModMp(v[0][0] * v[1][0]);
    v[0][n2] = CompMp::ModMp(v[0][n2] * v[1][n2]);
    for (i = 1; i < n2; ++i) {
        t1 = v[0][i];
        t2 = v[0][n - i];
        t3 = CompMp::ModMp((v[1][i] + v[1][n - i]) << (CompMp::P - 1));
        t4 = t3 - v[1][i];
        t4 = (t4 & CompMp::MP) + (t4 >> CompMp::P);
        v[0][i] = CompMp::ModMp(t1 * t3 - t2 * t4);
        v[0][n - i] = CompMp::ModMp(t2 * t3 + t1 * t4);
    }
    MNT(v[0], n, true);
    SetLen(len_ + rhs.len_ + 1, true);
    uint64_t tmp = 0;
    for (i = 0; i < len_ - 1; ++i) {
        if (v[0][i] >= CompMp::MP) v[0][i] -= CompMp::MP;
        tmp = (tmp >> LIMB) + static_cast<uint64_t>(v[0][i]);
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
BigInt<IntT> BigInt<IntT>::NFFTMul(BigInt lhs, const BigInt& rhs) {
    return lhs.NFFTMulEq(rhs);
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::MNTMul(BigInt lhs, const BigInt& rhs) {
    return lhs.MNTMulEq(rhs);
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
void BigInt<IntT>::NFFT(double* dest, size_t n, bool inv) {
    using T = std::complex<double>;
    // must ensure n is a power of 2
    double PI = std::acos(-1.0);
    size_t i = 0, j = 0;
    size_t step2, step4;
    T omega_n, omega;
    double tmp, t1, t2, t3, t4;
    double real, imag;
    BitRevSort(dest, n);
    for (i = 0; i < n; i += 2) {
        t1 = dest[i];
        t2 = dest[i + 1];
        dest[i + 1] = t1 - t2;
        dest[i] = t1 + t2;
    }
    for (size_t step = 4; step <= n; step <<= 1) {
        step2 = step >> 1;
        step4 = step >> 2;
        omega_n = T(std::cos(PI / step2), std::sin(PI / step2));
        omega = omega_n;
        real = omega.real();
        imag = omega.imag();
        for (i = 0; i < n; i += step) {
            t1 = dest[i];
            t2 = dest[i + step2];
            t3 = dest[i + step4];
            t4 = dest[i + step2 + step4];
            dest[i + step2] = t1 - t2;
            dest[i] += t2;
            dest[i + step2 + step4] = t3 - t4;
            dest[i + step4] += t4;
        }
        for (j = 1; j < step4; ++j) {
            for (i = 0; i < n; i += step) {
                t1 = dest[i + j];
                t2 = dest[i + j + step2];
                t3 = dest[i + step2 - j];
                t4 = dest[i + step - j];
                tmp = real * t2 + imag * t4;
                dest[i + j + step2] = t1 - tmp;
                dest[i + j] += tmp;
                tmp = imag * t2 - real * t4;
                dest[i + step - j] = t3 - tmp;
                dest[i + step2 - j] += tmp;
            }
            omega *= omega_n;
            real = omega.real();
            imag = omega.imag();
        }
    }
    if (inv)
        for (i = 0; i < n; ++i) dest[i] /= n;
}
template <typename IntT>
void BigInt<IntT>::MNT(int64_t* dest, size_t n, bool inv) {
    using T = CompMp;
    // must ensure n is a power of 2
    size_t i = 0, j = 0;
    size_t step2, step3 = 1, step4;
    T omega_n, omega;
    int64_t real, imag;
    int64_t tmp, t;
    int64_t t1, t2, t3, t4;
    BitRevSort(dest, n);
    for (i = 0; i < n; i += 2) {
        t1 = dest[i];
        t2 = dest[i + 1];
        t = t1 - t2;
        dest[i + 1] = (t & T::MP) + (t >> T::P);
        t = t1 + t2;
        dest[i] = (t & T::MP) + (t >> T::P);
    }
    for (size_t step = 4; step <= n; step <<= 1, ++step3) {
        step2 = step >> 1;
        step4 = step >> 2;
        omega_n = T::WN[step3];
        omega = omega_n;
        real = omega.Real();
        imag = omega.Imag();
        for (i = 0; i < n; i += step) {
            t1 = dest[i];
            t2 = dest[i + step2];
            t3 = dest[i + step4];
            t4 = dest[i + step2 + step4];
            t = t1 - t2;
            dest[i + step2] = (t & T::MP) + (t >> T::P);
            t = t1 + t2;
            dest[i] = (t & T::MP) + (t >> T::P);
            t = t3 - t4;
            dest[i + step2 + step4] = (t & T::MP) + (t >> T::P);
            t = t3 + t4;
            dest[i + step4] = (t & T::MP) + (t >> T::P);
        }
        for (j = 1; j < step4; ++j) {
            for (i = 0; i < n; i += step) {
                t1 = dest[i + j];
                t2 = dest[i + j + step2];
                t3 = dest[i + step2 - j];
                t4 = dest[i + step - j];
                tmp = CompMp::ModMp(real * t2 + imag * t4);
                t = t1 - tmp;
                dest[i + j + step2] = (t & T::MP) + (t >> T::P);
                t = t1 + tmp;
                dest[i + j] = (t & T::MP) + (t >> T::P);
                tmp = CompMp::ModMp(imag * t2 - real * t4);
                t = t3 - tmp;
                dest[i + step - j] = (t & T::MP) + (t >> T::P);
                t = t3 + tmp;
                dest[i + step2 - j] = (t & T::MP) + (t >> T::P);
            }
            omega *= omega_n;
            real = omega.Real();
            imag = omega.Imag();
        }
    }
    size_t mov = T::P - step3;
    if (inv)
        for (i = 0; i < n; ++i) dest[i] = T::ModMp(dest[i] << mov);
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
