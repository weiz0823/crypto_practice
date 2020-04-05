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
BigInt<IntT>& BigInt<IntT>::ToNextPrime() {
    bool sign = Sign();
    if (sign) ToOpposite();
    if (!Parity()) ++*this;
    constexpr uint32_t TOT1 = 30, TOT2 = 15, TOT3 = 480;
    // mod 2*3*5*7*11=2310
    uint64_t d[TOT3 + 1] = {
        1,    13,   17,   19,   23,   29,   31,   37,   41,   43,   47,   53,
        59,   61,   67,   71,   73,   79,   83,   89,   97,   101,  103,  107,
        109,  113,  127,  131,  137,  139,  149,  151,  157,  163,  167,  169,
        173,  179,  181,  191,  193,  197,  199,  211,  221,  223,  227,  229,
        233,  239,  241,  247,  251,  257,  263,  269,  271,  277,  281,  283,
        289,  293,  299,  307,  311,  313,  317,  323,  331,  337,  347,  349,
        353,  359,  361,  367,  373,  377,  379,  383,  389,  391,  397,  401,
        403,  409,  419,  421,  431,  433,  437,  439,  443,  449,  457,  461,
        463,  467,  479,  481,  487,  491,  493,  499,  503,  509,  521,  523,
        527,  529,  533,  541,  547,  551,  557,  559,  563,  569,  571,  577,
        587,  589,  593,  599,  601,  607,  611,  613,  617,  619,  629,  631,
        641,  643,  647,  653,  659,  661,  667,  673,  677,  683,  689,  691,
        697,  701,  703,  709,  713,  719,  727,  731,  733,  739,  743,  751,
        757,  761,  767,  769,  773,  779,  787,  793,  797,  799,  809,  811,
        817,  821,  823,  827,  829,  839,  841,  851,  853,  857,  859,  863,
        871,  877,  881,  883,  887,  893,  899,  901,  907,  911,  919,  923,
        929,  937,  941,  943,  947,  949,  953,  961,  967,  971,  977,  983,
        989,  991,  997,  1003, 1007, 1009, 1013, 1019, 1021, 1027, 1031, 1033,
        1037, 1039, 1049, 1051, 1061, 1063, 1069, 1073, 1079, 1081, 1087, 1091,
        1093, 1097, 1103, 1109, 1117, 1121, 1123, 1129, 1139, 1147, 1151, 1153,
        1157, 1159, 1163, 1171, 1181, 1187, 1189, 1193, 1201, 1207, 1213, 1217,
        1219, 1223, 1229, 1231, 1237, 1241, 1247, 1249, 1259, 1261, 1271, 1273,
        1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1313, 1319, 1321,
        1327, 1333, 1339, 1343, 1349, 1357, 1361, 1363, 1367, 1369, 1373, 1381,
        1387, 1391, 1399, 1403, 1409, 1411, 1417, 1423, 1427, 1429, 1433, 1439,
        1447, 1451, 1453, 1457, 1459, 1469, 1471, 1481, 1483, 1487, 1489, 1493,
        1499, 1501, 1511, 1513, 1517, 1523, 1531, 1537, 1541, 1543, 1549, 1553,
        1559, 1567, 1571, 1577, 1579, 1583, 1591, 1597, 1601, 1607, 1609, 1613,
        1619, 1621, 1627, 1633, 1637, 1643, 1649, 1651, 1657, 1663, 1667, 1669,
        1679, 1681, 1691, 1693, 1697, 1699, 1703, 1709, 1711, 1717, 1721, 1723,
        1733, 1739, 1741, 1747, 1751, 1753, 1759, 1763, 1769, 1777, 1781, 1783,
        1787, 1789, 1801, 1807, 1811, 1817, 1819, 1823, 1829, 1831, 1843, 1847,
        1849, 1853, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1891, 1901, 1907,
        1909, 1913, 1919, 1921, 1927, 1931, 1933, 1937, 1943, 1949, 1951, 1957,
        1961, 1963, 1973, 1979, 1987, 1993, 1997, 1999, 2003, 2011, 2017, 2021,
        2027, 2029, 2033, 2039, 2041, 2047, 2053, 2059, 2063, 2069, 2071, 2077,
        2081, 2083, 2087, 2089, 2099, 2111, 2113, 2117, 2119, 2129, 2131, 2137,
        2141, 2143, 2147, 2153, 2159, 2161, 2171, 2173, 2179, 2183, 2197, 2201,
        2203, 2207, 2209, 2213, 2221, 2227, 2231, 2237, 2239, 2243, 2249, 2251,
        2257, 2263, 2267, 2269, 2273, 2279, 2281, 2287, 2291, 2293, 2297, 2309,
        2311};
    IntT b[TOT1] = {13,  17,  19,  23,  29,  31,  37,  41,  43,  47,
                    53,  59,  61,  67,  71,  73,  79,  83,  89,  97,
                    101, 103, 107, 109, 113, 127, 131, 137, 139, 149};
    IntT c[TOT1];
    BigInt<IntT> a[TOT2] = {
        BigInt<IntT>(3),  BigInt<IntT>(5),  BigInt<IntT>(7),  BigInt<IntT>(11),
        BigInt<IntT>(13), BigInt<IntT>(17), BigInt<IntT>(19), BigInt<IntT>(23),
        BigInt<IntT>(29), BigInt<IntT>(31), BigInt<IntT>(37), BigInt<IntT>(41),
        BigInt<IntT>(43), BigInt<IntT>(47), BigInt<IntT>(53)};
    uint32_t i, j, l, l0;
    size_t k, s;
    BigInt<IntT> t, cond1, tmp;
    BigInt<IntT> cond2(-1);
    for (i = 0; i < TOT1; ++i) c[i] = *this % b[i];
    uint64_t init_mod = static_cast<uint64_t>(*this % BigInt<IntT>(2310));
    for (l = 0; l < TOT3; ++l)
        if (init_mod <= d[l]) break;
    if (init_mod < d[l]) *this += BigInt<IntT>(d[l] - init_mod);
    l0 = l;
    bool flag, composite_flag = true;
    while (composite_flag) {
        composite_flag = false;
        flag = true;
        while (flag) {
            flag = false;
            for (i = 0; i < TOT1; ++i)
                if (!c[i]) {
                    ++l;
                    for (j = 0; j < TOT1; ++j) {
                        c[j] += d[l] - d[l - 1];
                        if (c[j] >= b[j]) c[j] -= b[j];
                    }
                    if (l >= TOT3) l = 0;
                    flag = true;
                    break;
                }
        }
        if (l >= l0)
            *this += BigInt<IntT>(d[l] - d[l0]);
        else
            *this += BigInt<IntT>(d[l] + 2310u - d[l0]);
        t = *this;
        cond1 = t >> 1;
        --t;
        s = t.TrailingZero();
        t >>= s;
        for (auto& x : a) {
            tmp = PowMod(x, t, *this);
            if (tmp > cond1) tmp -= *this;
            if (tmp == BigInt<IntT>(1) || tmp == cond2) continue;
            for (k = 0; k < s - 1; ++k) {
                tmp.Square();
                tmp %= *this;
                if (tmp > cond1) tmp -= *this;
                if (tmp == cond2) break;
            }
            if (k == s - 1) {
                composite_flag = true;
                break;
            }
        }
        l0 = l;
        ++l;
        for (j = 0; j < TOT1; ++j) {
            c[j] += d[l] - d[l - 1];
            if (c[j] >= b[j]) c[j] -= b[j];
        }
        if (l >= TOT3) l = 0;
    }
    if (sign) ToOpposite();
    return *this;
}
template <typename IntT>
bool BigInt<IntT>::isProbablePrime() const {
    if (Sign()) {
        BigInt<IntT> tmp_obj = -*this;
        return tmp_obj.isProbablePrime();
    }
    constexpr uint8_t TOT1 = 24, TOT2 = 15;
    // Miller-Rabin primality test
    BigInt<IntT> a[TOT2] = {
        BigInt<IntT>(3),  BigInt<IntT>(5),  BigInt<IntT>(7),  BigInt<IntT>(11),
        BigInt<IntT>(13), BigInt<IntT>(17), BigInt<IntT>(19), BigInt<IntT>(23),
        BigInt<IntT>(29), BigInt<IntT>(31), BigInt<IntT>(37), BigInt<IntT>(41),
        BigInt<IntT>(43), BigInt<IntT>(47), BigInt<IntT>(53)};
    IntT b[TOT1] = {3,  5,  7,  11, 13, 17, 19, 23, 29, 31, 37, 41,
                    43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
    int cmp = Compare(BigInt<IntT>(2));
    if (cmp < 0)
        return false;
    else if (cmp == 0)
        return true;
    else if (!Parity())
        return false;
    for (auto x : b)
        if (!(*this % x)) return false;
    size_t i;
    BigInt<IntT> t(*this);
    BigInt<IntT> cond1(t >> 1);
    --t;
    BigInt<IntT> cond2(-1);
    size_t s = t.TrailingZero();
    t >>= s;
    BigInt<IntT> tmp;
    for (auto& x : a) {
        tmp = PowMod(x, t, *this);
        if (tmp > cond1) tmp -= *this;
        if (tmp == BigInt<IntT>(1) || tmp == cond2) continue;
        for (i = 0; i < s - 1; ++i) {
            tmp.Square();
            tmp %= *this;
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
    if (!p) return BigInt<IntT>(1);
    uint64_t mask = 1ul << 63ul;
    for (; mask; mask >>= 1)
        if (p & mask) break;
    BigInt<IntT> result(a);
    mask >>= 1;
    for (; mask; mask >>= 1) {
        result.Square();
        if (p & mask) result *= a;
    }
    return result;
}
template <typename IntT>
BigInt<IntT> PowMod(const BigInt<IntT>& a, const BigInt<IntT>& p,
                    const BigInt<IntT>& n) {
    if (p.Sign()) return BigInt<IntT>(0);
    if (!p) return BigInt<IntT>(1);
    if (a > n) return PowMod(a % n, p, n);
    IntT mask = IntT(1) << (a.LIMB - 1);
    for (; mask; mask >>= 1)
        if (p.val_[p.len_ - 1] & mask) break;
    BigInt<IntT> result(1);
    for (size_t i = p.len_ - 1; i != size_t(-1); --i) {
        for (; mask; mask >>= 1) {
            result.Square();
            if (p.val_[i] & mask) result *= a;
            if (result.len_ > n.len_) result %= n;
        }
        mask = IntT(1) << (a.LIMB - 1);
    }
    return std::move(result) % n;
}
template <typename IntT>
BigInt<IntT> PowMod(const BigInt<IntT>& a, uint64_t p, const BigInt<IntT>& n) {
    if (!p) return BigInt<IntT>(1);
    if (a > n) return PowMod(a % n, p, n);
    uint64_t mask = 1ul << 63ul;
    for (; mask; mask >>= 1)
        if (p & mask) break;
    BigInt<IntT> result(a);
    mask >>= 1;
    for (; mask; mask >>= 1) {
        result.Square();
        if (p & mask) result *= a;
        if (result.len_ > n.len_) result %= n;
    }
    return std::move(result) % n;
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
