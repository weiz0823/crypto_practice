#include "bigint64.hpp"
namespace calc {
class CompMp {
    int64_t real_, imag_;

   public:
    explicit constexpr CompMp(int64_t real = 0, int64_t imag = 0)
        : real_(real), imag_(imag) {}
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
        int128_t t1 = real_, t2 = imag_, t3 = real_ + imag_;
        t1 *= rhs.real_;
        t2 *= rhs.imag_;
        t3 *= (rhs.real_ + rhs.imag_);

        t3 -= t1;
        t3 -= t2;

        t1 = (t1 & MP) + (t1 >> P);
        t2 = (t2 & MP) + (t2 >> P);
        t3 = (t3 & MP) + (t3 >> P);

        imag_ = t3;
        real_ = int64_t(t1) - int64_t(t2);

        real_ = (real_ & MP) + (real_ >> P);
        real_ = (real_ & MP) + (real_ >> P);
        imag_ = (imag_ & MP) + (imag_ >> P);
        imag_ = (imag_ & MP) + (imag_ >> P);
        return *this;
    }
    constexpr CompMp& operator*=(int64_t rhs) {
        int128_t t1 = real_, t2 = imag_;
        t1 *= rhs;
        t2 *= rhs;
        t1 = (t1 & MP) + (t1 >> P);
        t2 = (t2 & MP) + (t2 >> P);
        real_ = t1;
        imag_ = t2;

        real_ = (real_ & MP) + (real_ >> P);
        real_ = (real_ & MP) + (real_ >> P);
        imag_ = (imag_ & MP) + (imag_ >> P);
        imag_ = (imag_ & MP) + (imag_ >> P);
        return *this;
    }
    constexpr CompMp& ToConj() {
        imag_ = MP - imag_;
        return *this;
    }
    constexpr int64_t Real() const { return real_ >= MP ? real_ - MP : real_; }
    constexpr int64_t Imag() const { return imag_ >= MP ? imag_ - MP : imag_; }
    static constexpr int64_t P = 61;
    static constexpr int64_t MP = 2305843009213693951;
    static const CompMp WN[P];
    static constexpr int64_t ModMp(int128_t x) {
        x = (x & MP) + (x >> P);
        int64_t t = x;
        t = (t & MP) + (t >> P);
        t = (t & MP) + (t >> P);
        return t;
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
const CompMp CompMp::WN[CompMp::P] = {
    CompMp(2305843009213693950, 0),
    CompMp(0, 1),
    CompMp(1073741824, 1073741824),
    CompMp(612525257975972978, 22027337052962166),
    CompMp(83304533336094567, 1608519025533736705),
    CompMp(1605717535313600273, 477079714691672132),
    CompMp(1007278913389194978, 1605515517245405873),
    CompMp(668775414326009008, 171086507355679324),
    CompMp(223242169330631354, 1932816971808434683),
    CompMp(1311084444718765561, 1475847404606084430),
    CompMp(138973123619611617, 1424745856292818894),
    CompMp(866591109185045633, 1882362373379288092),
    CompMp(1477709895268633344, 1570711160879524820),
    CompMp(14314185882030745, 818535871397538520),
    CompMp(2224273544844914465, 355230632501047083),
    CompMp(1510466207055935382, 120042544849731353),
    CompMp(2277286914618426325, 401242637764531855),
    CompMp(1973234539278172120, 1244201103777839971),
    CompMp(2301577106826624115, 2067658591663656538),
    CompMp(7099537347279728, 315385352303489787),
    CompMp(1955212993688118119, 1399590101310979822),
    CompMp(1662087499102352953, 1275986840041108865),
    CompMp(2072965014761787807, 1110324018738685214),
    CompMp(1942532780955627238, 958466690593365994),
    CompMp(2240219762315661198, 249027728140371571),
    CompMp(634108736350871478, 974590857440464469),
    CompMp(249638158412668528, 753183807909889815),
    CompMp(1880511834100884989, 810833702593115019),
    CompMp(257174691547234593, 961832271599076546),
    CompMp(1271279323708198528, 160146159910001459),
    CompMp(2189771346815573502, 824396215253924279),
    CompMp(1822491602738317906, 534828482115481684),
    CompMp(2240613713718996008, 1020016769891850513),
    CompMp(870440071785122836, 1784561959406607252),
    CompMp(177969285982644532, 1976707438280408768),
    CompMp(151602767334192101, 1759172372483511734),
    CompMp(1283910259611844355, 2040025178495358529),
    CompMp(903525503626061739, 2278738702233177508),
    CompMp(1375477479693542069, 1208957706222349663),
    CompMp(211125430388711922, 1722910554284638678),
    CompMp(1815315585433471841, 1374473267435554189),
    CompMp(2047768286801579070, 1702617503735551823),
    CompMp(1561720520405271370, 651459739764292248),
    CompMp(1258097191868493986, 1163418701563332009),
    CompMp(779461586022059850, 469011596711728171),
    CompMp(1269080372986838410, 142987493057138902),
    CompMp(2072426017731382138, 114783867797282653),
    CompMp(2108385126724697221, 995118996692485496),
    CompMp(2176405693420701965, 786244673017883310),
    CompMp(2157027185465453147, 1862395006763389785),
    CompMp(233884707527343258, 2286988035322281465),
    CompMp(127673747150544569, 2115031009513532173),
    CompMp(1145785528240796099, 1183920336107108265),
    CompMp(2044342140422629675, 1156675284546784606),
    CompMp(981027961452405245, 877188789024058773),
    CompMp(298667577285977760, 750577264738041039),
    CompMp(4250272665676801, 1497280327205388844),
    CompMp(46099201, 63418700682167910),
    CompMp(4801, 70608338201365152),
    CompMp(49, 1177171008151898599),
    CompMp(5, 348301401736559255)};

BigInt<uint128_t>& BigInt<uint128_t>::operator*=(uint64_t rhs) {
    bool sign = Sign();
    if (sign) ToOpposite();
    BigInt<uint128_t> carry;
    carry.SetLen(len_ + 2, false);
    uint128_t* it = val_;
    uint128_t* rit = carry.val_;
    do {
        asm(R"(
	movq (%0), %%rax
	mulq %2
	movq %%rax, (%0)
	movq %%rdx, 8(%1)
	movq 8(%0), %%rax
	mulq %2
	movq %%rax, 8(%0)
	movq %%rdx, 16(%1)
)"
            : "+r"(it), "+r"(rit)
            : "g"(rhs)
            : "cc", "memory", "rax", "rdx");
        ++it;
        ++rit;
    } while (it < end_);
    SetLen(len_ + 2, false);
    *this += carry;
    if (sign) ToOpposite();
    ShrinkLen();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::RMNTMulEq(const BigInt& rhs) {
    // reinterpret as uint16
    if (rhs.Sign()) {
        RMNTMulEq(-rhs);
        return ToOpposite();
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    int64_t* v[2];
    uint64_t n = 1;
    auto it = reinterpret_cast<uint16_t*>(val_);
    auto cit = reinterpret_cast<uint16_t*>(rhs.val_);
    auto term = reinterpret_cast<uint16_t*>(end_);
    auto cterm = reinterpret_cast<uint16_t*>(rhs.end_);
    while (n < len_ + rhs.len_) n <<= 1;
    // 128/16=8
    n <<= 3;
    v[0] = new int64_t[n];
    v[1] = new int64_t[n];
    auto vit = v[0];
    for (; it < term; vit += 8, it += 8) {
        for (int j = 0; j < 8; j += 2) {
            *(vit + j) = *(it + j);
            *(vit + j + 1) = *(it + j + 1);
        }
    }
    std::fill(vit, v[0] + n, 0);
    vit = v[1];
    for (; cit < cterm; vit += 8, cit += 8) {
        for (int j = 0; j < 8; j += 2) {
            *(vit + j) = *(cit + j);
            *(vit + j + 1) = *(cit + j + 1);
        }
    }
    std::fill(vit, v[1] + n, 0);
    RMNT(v[0], n, false);
    RMNT(v[1], n, false);
    int64_t t1, t2, t3, t4;
    uint64_t n2 = n >> 1;
    v[0][0] = CompMp::ModMp(int128_t(v[0][0]) * v[1][0]);
    v[0][n2] = CompMp::ModMp(int128_t(v[0][n2]) * v[1][n2]);
    for (uint64_t i = 1; i < n2; ++i) {
        t1 = v[0][i];
        t2 = v[0][n - i];
        t3 = CompMp::ModMp(int128_t(v[1][i] + v[1][n - i]) << (CompMp::P - 1));
        t4 = t3 - v[1][i];
        t4 = (t4 & CompMp::MP) + (t4 >> CompMp::P);
        v[0][i] = CompMp::ModMp(int128_t(t1) * t3 - int128_t(t2) * t4);
        v[0][n - i] = CompMp::ModMp(int128_t(t2) * t3 + int128_t(t1) * t4);
    }
    RMNT(v[0], n, true);
    SetLen(len_ + rhs.len_ + 1, true);
    it = reinterpret_cast<uint16_t*>(val_);
    term = reinterpret_cast<uint16_t*>(end_ - 1);
    vit = v[0];
    uint64_t tmp = 0;
    for (; it != term; it += 8, vit += 8) {
        for (int j = 0; j < 8; j += 2) {
            if (*(vit + j) >= CompMp::MP) *(vit + j) -= CompMp::MP;
            tmp = (tmp >> 16) + uint64_t(*(vit + j));
            *(it + j) = uint16_t(tmp);
            if (*(vit + j + 1) >= CompMp::MP) *(vit + j + 1) -= CompMp::MP;
            tmp = (tmp >> 16) + uint64_t(*(vit + j + 1));
            *(it + j + 1) = uint16_t(tmp);
        }
    }
    if (sign) ToOpposite();
    ShrinkLen();
    delete[] v[0];
    delete[] v[1];
    return *this;
}
template <typename T>
void BigInt<uint128_t>::BitRevSort(T* a, uint64_t n) {
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
void BigInt<uint128_t>::RMNT(int64_t* dest, uint64_t n, bool inv) {
    using T = CompMp;
    // must ensure n is a power of 2
    uint64_t i = 0, j = 0;
    uint64_t step2, step3 = 1, step4;
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
    for (uint64_t step = 4; step <= n; step <<= 1, ++step3) {
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
                tmp = CompMp::ModMp(int128_t(real) * t2 + int128_t(imag) * t4);
                t = t1 - tmp;
                dest[i + j + step2] = (t & T::MP) + (t >> T::P);
                t = t1 + tmp;
                dest[i + j] = (t & T::MP) + (t >> T::P);
                tmp = CompMp::ModMp(int128_t(imag) * t2 - int128_t(real) * t4);
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
    uint64_t mov = T::P - step3;
    if (inv)
        for (i = 0; i < n; ++i) dest[i] = T::ModMp(int128_t(dest[i]) << mov);
}
BigInt<uint128_t>& BigInt<uint128_t>::MNTMulEq(const BigInt& rhs) {
    if (rhs.Sign()) {
        MNTMulEq(-rhs);
        ToOpposite();
        return *this;
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    using T = CompMp;
    T* v[2];
    uint64_t n = 1;
    auto it = reinterpret_cast<uint16_t*>(val_);
    auto cit = reinterpret_cast<uint16_t*>(rhs.val_);
    auto term = reinterpret_cast<uint16_t*>(end_);
    auto cterm = reinterpret_cast<uint16_t*>(rhs.end_);
    while (n < len_ + rhs.len_) n <<= 1;
    // 128/16=8
    n <<= 3;
    v[0] = new T[n];
    v[1] = new T[n];
    auto vit = v[0];
    for (; it < term; vit += 8, it += 8) {
        for (int j = 0; j < 8; j += 2) {
            *(vit + j) = T(*(it + j));
            *(vit + j + 1) = T(*(it + j + 1));
        }
    }
    std::fill(vit, v[0] + n, T());
    vit = v[1];
    for (; cit < cterm; vit += 8, cit += 8) {
        for (int j = 0; j < 8; j += 2) {
            *(vit + j) = T(*(cit + j));
            *(vit + j + 1) = T(*(cit + j + 1));
        }
    }
    std::fill(vit, v[1] + n, T());
    MNT(v[0], n, false);
    MNT(v[1], n, false);
    for (uint64_t i = 0; i < n; ++i) v[0][i] *= v[1][i];
    MNT(v[0], n, true);
    SetLen(len_ + rhs.len_ + 1, true);
    it = reinterpret_cast<uint16_t*>(val_);
    term = reinterpret_cast<uint16_t*>(end_ - 1);
    vit = v[0];
    uint64_t tmp = 0;
    for (; it != term; it += 8, vit += 8) {
        for (int j = 0; j < 8; j += 2) {
            tmp = (tmp >> 16) + uint64_t((vit + j)->Real());
            *(it + j) = uint16_t(tmp);
            tmp = (tmp >> 16) + uint64_t((vit + j + 1)->Real());
            *(it + j + 1) = uint16_t(tmp);
        }
    }
    if (sign) ToOpposite();
    ShrinkLen();
    delete[] v[0];
    delete[] v[1];
    return *this;
}
void BigInt<uint128_t>::MNT(CompMp* dest, uint64_t n, bool inv) {
    using T = CompMp;
    // must ensure n is a power of 2
    uint64_t i = 0, j = 0;
    uint64_t step2, step3 = 0;
    T omega_n, omega, tmp;
    BitRevSort(dest, n);
    for (uint64_t step = 2; step <= n; step <<= 1, ++step3) {
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
    int64_t mov = int64_t(1) << (T::P - step3);
    if (inv)
        for (i = 0; i < n; ++i) dest[i] *= mov;
}
BigInt<uint128_t>& BigInt<uint128_t>::MulEqKaratsuba(const BigInt& rhs) {
    // if (len_ <= 2 || rhs.len_ <= 2) return PlainMulEq(rhs);
    if (len_ <= 4 || rhs.len_ <= 4) return RMNTMulEq(rhs);
    if (rhs.Sign()) {
        MulEqKaratsuba(-rhs);
        ToOpposite();
        return *this;
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    size_t t = (std::max(rhs.len_, len_) + 1) >> 1;
    BigInt<uint128_t> a, b, c, d, x, y, z;
    if (t >= len_) {
        b.SetLen(len_, false);
        std::copy(val_, end_, b.val_);
    } else {
        b.SetLen(t + 1, false);
        std::copy(val_, val_ + t, b.val_);
        b.ShrinkLen();
        a.SetLen(len_ - t, false);
        std::copy(val_ + t, end_, a.val_);
    }
    if (t >= rhs.len_) {
        d.SetLen(rhs.len_, false);
        std::copy(rhs.val_, rhs.end_, d.val_);
    } else {
        d.SetLen(t + 1, false);
        std::copy(rhs.val_, rhs.val_ + t, d.val_);
        d.ShrinkLen();
        c.SetLen(rhs.len_ - t, false);
        std::copy(rhs.val_ + t, rhs.end_, c.val_);
    }
    if (t >= len_) {
        d.MulEqKaratsuba(b);
        c.MulEqKaratsuba(b);
        // d *= b;
        // c *= b;
        *this = std::move(d);
        BiasedAddEq(c, t);
    } else if (t >= rhs.len_) {
        a.MulEqKaratsuba(d);
        b.MulEqKaratsuba(d);
        // b *= d;
        // a *= d;
        *this = std::move(b);
        BiasedAddEq(a, t);
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
        *this = std::move(b);
        BiasedAddEq(a, t + t);
        BiasedAddEq(x, t);
    }
    if (sign) ToOpposite();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::PlainMulEq(const BigInt& rhs) {
    if (rhs.Sign()) {
        PlainMulEq(-rhs);
        return ToOpposite();
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    BigInt<uint128_t> save_this = std::move(*this);
    auto cit = reinterpret_cast<uint64_t*>(rhs.val_);
    uint64_t i = 0;
    auto cterm = reinterpret_cast<uint64_t*>(rhs.end_);
    if (!*(rhs.end_ - 1)) cterm -= 2;
    *this = BigInt<uint128_t>(0);
    do {
        BiasedAddEq(save_this * (*cit), i, false);
        ++cit;
        BiasedAddEq(save_this * (*cit), i, true);
        ++cit;
        ++i;
    } while (cit < cterm);
    if (sign) ToOpposite();
    ShrinkLen();
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::operator*=(const BigInt& rhs) {
    if (rhs.len_ <= 16 || len_ <= 16) {
        return PlainMulEq(rhs);
    } else if ((len_ << 2) < rhs.len_ || (rhs.len_ << 2) < len_) {
        return RMNTMulEqUB(rhs);
    } else {
        return RMNTMulEq(rhs);
    }
}
BigInt<uint128_t>& BigInt<uint128_t>::SquareEq() {
    // specialized version of RMNTMulEq
    if (Sign()) ToOpposite();
    int64_t* v;
    uint64_t n = 1;
    auto it = reinterpret_cast<uint16_t*>(val_);
    auto term = reinterpret_cast<uint16_t*>(end_);
    while (n < len_) n <<= 1;
    n <<= 4;
    v = new int64_t[n];
    auto vit = v;
    for (; it < term; vit += 8, it += 8) {
        for (int j = 0; j < 8; j += 2) {
            *(vit + j) = *(it + j);
            *(vit + j + 1) = *(it + j + 1);
        }
    }
    std::fill(vit, v + n, 0);
    RMNT(v, n, false);
    int64_t t1, t2, t3, t4;
    uint64_t n2 = n >> 1;
    v[0] = CompMp::ModMp(int128_t(v[0]) * v[0]);
    v[n2] = CompMp::ModMp(int128_t(v[n2]) * v[n2]);
    for (uint64_t i = 1; i < n2; ++i) {
        t1 = CompMp::ModMp(int128_t(v[i]) * v[i]);
        t2 = CompMp::ModMp(int128_t(v[n - i]) * v[n - i]);
        t3 = CompMp::ModMp(int128_t(v[i]) * v[n - i]);
        t1 = CompMp::ModMp(int128_t(t1 - t2) << (CompMp::P - 1));
        t4 = t3 + t1;
        v[i] = (t4 & CompMp::MP) + (t4 >> CompMp::P);
        t4 = t3 - t1;
        v[n - i] = (t4 & CompMp::MP) + (t4 >> CompMp::P);
    }
    RMNT(v, n, true);
    SetLen(len_ + len_ + 1, true);
    it = reinterpret_cast<uint16_t*>(val_);
    term = reinterpret_cast<uint16_t*>(end_ - 1);
    vit = v;
    uint64_t tmp = 0;
    for (; it != term; it += 8, vit += 8) {
        for (int j = 0; j < 8; j += 2) {
            if (*(vit + j) >= CompMp::MP) *(vit + j) -= CompMp::MP;
            tmp = (tmp >> 16) + uint64_t(*(vit + j));
            *(it + j) = uint16_t(tmp);
            if (*(vit + j + 1) >= CompMp::MP) *(vit + j + 1) -= CompMp::MP;
            tmp = (tmp >> 16) + uint64_t(*(vit + j + 1));
            *(it + j + 1) = uint16_t(tmp);
        }
    }
    ShrinkLen();
    delete[] v;
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::RMNTMulEqGiven(const int64_t* src,
                                                     uint64_t n,
                                                     uint64_t rlen) {
    // require len_ <= (length of the pre-converted number)
    bool sign = Sign();
    if (sign) ToOpposite();
    int64_t* v = new int64_t[n];
    auto it = reinterpret_cast<uint16_t*>(val_);
    auto term = reinterpret_cast<uint16_t*>(end_);
    auto vit = v;
    for (; it < term; vit += 8, it += 8) {
        for (int j = 0; j < 8; j += 2) {
            *(vit + j) = *(it + j);
            *(vit + j + 1) = *(it + j + 1);
        }
    }
    std::fill(vit, v + n, 0);
    RMNT(v, n, false);
    int64_t t1, t2, t3, t4;
    uint64_t n2 = n >> 1;
    v[0] = CompMp::ModMp(int128_t(v[0]) * src[0]);
    v[n2] = CompMp::ModMp(int128_t(v[n2]) * src[n2]);
    for (uint64_t i = 1; i < n2; ++i) {
        t1 = v[i];
        t2 = v[n - i];
        t3 = CompMp::ModMp(int128_t(src[i] + src[n - i]) << (CompMp::P - 1));
        t4 = t3 - src[i];
        t4 = (t4 & CompMp::MP) + (t4 >> CompMp::P);
        v[i] = CompMp::ModMp(int128_t(t1) * t3 - int128_t(t2) * t4);
        v[n - i] = CompMp::ModMp(int128_t(t2) * t3 + int128_t(t1) * t4);
    }
    RMNT(v, n, true);
    SetLen(len_ + rlen + 1, true);
    it = reinterpret_cast<uint16_t*>(val_);
    term = reinterpret_cast<uint16_t*>(end_ - 1);
    vit = v;
    uint64_t tmp = 0;
    for (; it < term; it += 8, vit += 8) {
        for (int j = 0; j < 8; j += 2) {
            if (*(vit + j) >= CompMp::MP) *(vit + j) -= CompMp::MP;
            tmp = (tmp >> 16) + uint64_t(*(vit + j));
            *(it + j) = uint16_t(tmp);
            if (*(vit + j + 1) >= CompMp::MP) *(vit + j + 1) -= CompMp::MP;
            tmp = (tmp >> 16) + uint64_t(*(vit + j + 1));
            *(it + j + 1) = uint16_t(tmp);
        }
    }
    if (sign) ToOpposite();
    ShrinkLen();
    delete[] v;
    return *this;
}
BigInt<uint128_t>& BigInt<uint128_t>::RMNTMulEqUB(const BigInt& rhs) {
    // reinterpret as uint16
    if (rhs.Sign()) {
        RMNTMulEqUB(-rhs);
        return ToOpposite();
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    int64_t* v;
    uint64_t n = 1;
    uint64_t k, i;
    BigInt<uint128_t> tmp_obj;
    if (len_ > rhs.len_) {
        auto cit = reinterpret_cast<uint16_t*>(rhs.val_);
        auto cterm = reinterpret_cast<uint16_t*>(rhs.end_);
        while (n <= rhs.len_) n <<= 1;
        n <<= 4;
        k = (len_ + rhs.len_ - 1) / rhs.len_;
        v = new int64_t[n];
        auto vit = v;
        for (; cit < cterm; vit += 8, cit += 8) {
            for (int j = 0; j < 8; j += 2) {
                *(vit + j) = *(cit + j);
                *(vit + j + 1) = *(cit + j + 1);
            }
        }
        std::fill(vit, v + n, 0);
        RMNT(v, n, false);
        BigInt<uint128_t> save_this = std::move(*this);
        auto sit = save_this.val_;
        auto sterm = save_this.end_;
        *this = BigInt<uint128_t>(0);
        for (i = 0; i < k - 1; ++i) {
            tmp_obj.SetLen(rhs.len_ + 1, false);
            *(tmp_obj.end_ - 1) = 0;
            std::copy(sit, sit + rhs.len_, tmp_obj.val_);
            sit += rhs.len_;
            tmp_obj.ShrinkLen();
            tmp_obj.RMNTMulEqGiven(v, n, rhs.len_);
            BiasedAddEq(tmp_obj, i * rhs.len_, false);
        }
        tmp_obj = BigInt<uint128_t>(0);
        tmp_obj.SetLen(sterm - sit + 1, false);
        *(tmp_obj.end_ - 1) = 0;
        std::copy(sit, sterm, tmp_obj.val_);
        tmp_obj.ShrinkLen();
        tmp_obj.RMNTMulEqGiven(v, n, rhs.len_);
        BiasedAddEq(tmp_obj, i * rhs.len_, false);
        delete[] v;
    } else {
        auto it = reinterpret_cast<uint16_t*>(val_);
        auto term = reinterpret_cast<uint16_t*>(end_);
        auto this_len = len_;
        while (n <= this_len) n <<= 1;
        n <<= 4;
        k = (rhs.len_ + this_len - 1) / this_len;
        v = new int64_t[n];
        auto vit = v;
        for (; it < term; vit += 8, it += 8) {
            for (int j = 0; j < 8; j += 2) {
                *(vit + j) = *(it + j);
                *(vit + j + 1) = *(it + j + 1);
            }
        }
        std::fill(vit, v + n, 0);
        RMNT(v, n, false);
        *this = BigInt<uint128_t>(0);
        auto cit = rhs.val_;
        auto cterm = rhs.end_;
        for (i = 0; i < k - 1; ++i) {
            tmp_obj.SetLen(this_len + 1, false);
            *(tmp_obj.end_ - 1) = 0;
            std::copy(cit, cit + this_len, tmp_obj.val_);
            cit += this_len;
            tmp_obj.ShrinkLen();
            tmp_obj.RMNTMulEqGiven(v, n, this_len);
            BiasedAddEq(tmp_obj, i * this_len, false);
        }
        tmp_obj = BigInt<uint128_t>(0);
        tmp_obj.SetLen(cterm - cit + 1, false);
        *(tmp_obj.end_ - 1) = 0;
        std::copy(cit, cterm, tmp_obj.val_);
        tmp_obj.ShrinkLen();
        tmp_obj.RMNTMulEqGiven(v, n, this_len);
        BiasedAddEq(tmp_obj, i * this_len, false);
        delete[] v;
    }
    if (sign) ToOpposite();
    ShrinkLen();
    return *this;
}
}  // namespace calc
