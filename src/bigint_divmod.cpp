#include <cassert>

#include "bigint.hpp"
namespace calc {
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator/=(IntT rhs) {
    BasicDivEq(rhs);
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator%=(IntT rhs) {
    if (rhs == 0) return *this;
    const uint64_t limb_mod = (1l << LIMB) % rhs;
    uint64_t cur_mod = 1;
    uint64_t tot = 0;
    bool sign = Sign();
    if (sign) ToOpposite();
    if (limb_mod == 0) {
        tot = val_[0] % rhs;
    } else {
        for (size_t i = 0; i < len_; ++i) {
            tot = (tot + cur_mod * val_[i]) % rhs;
            cur_mod = cur_mod * limb_mod % rhs;
        }
    }
    if (len_ > 1) std::fill(val_ + 1, val_ + len_, IntT(0));
    len_ = 2;
    val_[0] = IntT(tot);
    if (sign) ToOpposite();
    ShrinkLen();
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator/=(const BigInt& rhs) {
    if (len_ <= 64 / LIMB && rhs.len_ <= 64 / LIMB) {
        PlainDivEq(rhs, nullptr);
    } else if (rhs.len_ == 1) {
        BasicDivEq(rhs.val_[0], nullptr);
    } else if (len_ <= rhs.len_ || len_ / rhs.len_ > 8 ||
               len_ / (len_ - rhs.len_) > 8) {
        if constexpr (LIMB > 21)
            DivEqAlgA(rhs, nullptr);
        else
            DivEqAlgB(rhs, nullptr);
    } else {
        DivEqRecursive(rhs, nullptr);
    }
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::operator%=(const BigInt& rhs) {
    BigInt<IntT> mod;
    if (len_ <= 64 / LIMB && rhs.len_ <= 64 / LIMB) {
        PlainDivEq(rhs, &mod);
    } else if (len_ <= rhs.len_ || len_ / rhs.len_ > 8 ||
               len_ / (len_ - rhs.len_) > 8) {
        if constexpr (LIMB > 21)
            DivEqAlgA(rhs, &mod);
        else
            DivEqAlgB(rhs, &mod);
    } else {
        DivEqRecursive(rhs, &mod);
    }
    *this = std::move(mod);
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::BasicDivEq(IntT rhs, IntT* mod) {
    if (rhs == 0 || rhs == 1) return *this;
    uint64_t t = 0, r = 0;
    bool sign = Sign();
    if (sign) ToOpposite();
    IntT tmp_rhs = rhs, log_rhs = 0;
    while (!(tmp_rhs & 1)) {
        ++log_rhs;
        tmp_rhs >>= 1;
    }

    if (len_ == 1) {
        t = val_[0] % rhs;
        val_[0] /= rhs;
    } else if (tmp_rhs == 1) {
        *this >>= log_rhs;
    } else {
        for (size_t i = len_ - 1; i != size_t(-1); --i) {
            t = ((t - r * rhs) << LIMB) | val_[i];
            r = t / rhs;
            val_[i] = IntT(r);
        }
    }

    if (sign) ToOpposite();
    ShrinkLen();
    if (mod) {
        if (sign && t)
            *mod = IntT(rhs - t % rhs);
        else
            *mod = IntT(t % rhs);
    }
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::PlainDivEq(const BigInt& rhs, BigInt* mod) {
    if (rhs == BigInt<IntT>(0)) return *this;
    // must ensure len_*LIMB<=64
    bool sign = Sign();
    if (sign) ToOpposite();
    uint64_t x = 0, y = 0, z = 0, w = 0;
    size_t i;
    for (i = len_ - 1; i != size_t(-1); --i) x = (x << LIMB) | val_[i];
    for (i = rhs.len_ - 1; i != size_t(-1); --i) y = (y << LIMB) | rhs.val_[i];
    if (rhs.Sign()) y = 0 - y;
    z = x / y;
    w = x - z * y;
    i = 0;
    while (z) {
        val_[i++] = IntT(z);
        z >>= LIMB;
    }
    std::fill(val_ + i, val_ + len_, IntT(0));
    len_ = i ? i : 1;
    if (Sign()) SetLen(len_ + 1, false);
    if (sign != rhs.Sign()) ToOpposite();
    if (mod) {
        mod->SetLen(64 / LIMB, false);
        i = 0;
        while (w) {
            mod->val_[i++] = IntT(w);
            w >>= LIMB;
        }
        std::fill(mod->val_ + i, mod->val_ + mod->len_, IntT(0));
        mod->len_ = i ? i : 1;
        if (mod->Sign()) mod->SetLen(mod->len_ + 1, false);
        if (sign) mod->ToOpposite();
    }
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::DivEqAlgA(const BigInt& rhs, BigInt* mod) {
    if (rhs == BigInt<IntT>(0)) return *this;
    if (rhs.Sign()) {
        DivEqAlgA(-rhs, mod);
        ToOpposite();
        return *this;
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    size_t mov = 0;
    uint64_t q, r, u1, u2, v1, v2;
    constexpr uint64_t b = (1l << LIMB);
    size_t pos = rhs.len_ - 1;
    while (!rhs.val_[pos]) --pos;
    IntT test_tmp = rhs.val_[pos];
    while (test_tmp < (IntT(1) << (LIMB - 1))) {
        test_tmp <<= 1;
        ++mov;
    }
    if (*this < rhs) {
        if (mod) *mod = *this;
        std::fill(val_, val_ + len_, IntT(0));
        len_ = 1;
    } else if (rhs.len_ <= 64 / LIMB && len_ <= 64 / LIMB) {
        PlainDivEq(rhs, mod);
    } else if (rhs.len_ == 1) {
        // ensure non-negative
        if (mod) {
            BasicDivEq(rhs.val_[0], mod->val_);
            mod->SetLen(1, false);
        } else {
            BasicDivEq(rhs.val_[0], nullptr);
        }
    } else {
        BigInt<IntT> result;
        result.SetLen(len_ - pos + 1, true);
        if (mov) {
            v1 = IntT(rhs.val_[pos] << mov) |
                 (rhs.val_[pos - 1] >> (LIMB - mov));
            v2 = IntT(rhs.val_[pos - 1] << mov) |
                 (rhs.val_[pos - 2] >> (LIMB - mov));
        } else {
            v1 = rhs.val_[pos];
            v2 = rhs.val_[pos - 1];
        }
        u1 = val_[len_ - 1];
        for (size_t i = len_ - pos - 1; i != size_t(-1); --i) {
            if (mov) {
                u1 = (u1 << mov) | (val_[i + pos - 1] >> (LIMB - mov));
                u2 = IntT(val_[i + pos - 1] << mov) |
                     (val_[i + pos - 2] >> (LIMB - mov));
            } else {
                u2 = val_[i + pos - 1];
            }
            q = u1 / v1;
            if (q >= b) q = b - 1;
            r = u1 - q * v1;
            if (q * v2 > b * r + u2) {
                --q;
                r += v1;
            }
            *this -= (rhs * IntT(q)) << (i * LIMB);
            if (Sign()) {
                --q;
                *this += rhs << (i * LIMB);
            }
            result.val_[i] = static_cast<IntT>(q);

            u1 = (uint64_t(val_[i + pos]) << LIMB) | val_[i + pos - 1];
        }
        if (mod) *mod = std::move(*this);
        *this = std::move(result);
        if (sign) ToOpposite();
    }
    if (mod) {
        if (sign) mod->ToOpposite();
        mod->ShrinkLen();
    }
    ShrinkLen();
    return *this;
}
template <>
BigInt<uint32_t>& BigInt<uint32_t>::DivEqAlgB(const BigInt& rhs, BigInt* mod) {
    return DivEqAlgA(rhs, mod);
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::DivEqAlgB(const BigInt& rhs, BigInt* mod) {
    if (rhs == BigInt<IntT>(0)) return *this;
    if (rhs.Sign()) {
        DivEqAlgB(-rhs, mod);
        ToOpposite();
        return *this;
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    uint64_t q, u, v;
    constexpr uint64_t b = (1l << LIMB);
    if (*this < rhs) {
        if (mod) *mod = *this;
        std::fill(val_, val_ + len_, IntT(0));
        len_ = 1;
    } else if (rhs.len_ <= 64 / LIMB && len_ <= 64 / LIMB) {
        PlainDivEq(rhs, mod);
    } else if (rhs.len_ == 1) {
        // ensure non-negative
        if (mod) {
            BasicDivEq(rhs.val_[0], mod->val_);
            mod->SetLen(1, false);
        } else {
            BasicDivEq(rhs.val_[0], nullptr);
        }
    } else {
        size_t pos = rhs.len_ - 1;
        while (!rhs.val_[pos]) --pos;
        BigInt<IntT> result;
        result.SetLen(len_ - pos + 1, false);
        v = (uint64_t(rhs.val_[pos]) << LIMB) | rhs.val_[pos - 1];
        u = (uint64_t(val_[len_ - 1]) << LIMB) | val_[len_ - 2];
        for (size_t i = len_ - pos - 1; i != size_t(-1); --i) {
            q = u / v;
            if (q >= b) q = b - 1;
            *this -= (rhs * IntT(q)) << (i * LIMB);
            if (Sign()) {
                --q;
                *this += rhs << (i * LIMB);
            }
            result.val_[i] = static_cast<IntT>(q);
            u = (uint64_t(val_[i + pos]) << (2 * LIMB)) |
                (uint64_t(val_[i + pos - 1]) << LIMB) | val_[i + pos - 2];
        }
        if (mod) *mod = std::move(*this);
        *this = std::move(result);
        if (sign) ToOpposite();
    }
    if (mod) {
        if (sign) mod->ToOpposite();
        mod->ShrinkLen();
    }
    ShrinkLen();
    return *this;
}
template <typename IntT>
BigInt<IntT>& BigInt<IntT>::DivEqRecursive(const BigInt& rhs, BigInt* mod) {
    if (rhs == BigInt<IntT>(0)) return *this;
    if (rhs.Sign()) {
        DivEqRecursive(-rhs, mod);
        ToOpposite();
        return *this;
    }
    bool sign = Sign();
    if (sign) ToOpposite();
    if (*this < rhs) {
        if (mod) *mod = *this;
        std::fill(val_, val_ + len_, IntT(0));
        len_ = 1;
    } else if (rhs.len_ <= 64 / LIMB && len_ <= 64 / LIMB) {
        PlainDivEq(rhs, mod);
    } else if (rhs.len_ == 1) {
        // ensure non-negative
        if (mod) {
            BasicDivEq(rhs.val_[0], mod->val_);
            mod->SetLen(1, false);
        } else {
            BasicDivEq(rhs.val_[0], nullptr);
        }
    } else if (len_ <= rhs.len_ || len_ < 4 || rhs.len_ < 4 ||
               len_ / rhs.len_ > 8 || len_ / (len_ - rhs.len_) > 8) {
        DivEqAlgA(rhs, mod);
    } else {
        BigInt<IntT> result(0);
        size_t new_base = (rhs.len_ + 1) / 2;
        BigInt<IntT> b = BigInt<IntT>(1) << (new_base * LIMB);
        --b;
        BigInt<IntT> v(rhs.val_ + new_base, rhs.len_ - new_base);
        size_t mov = new_base * LIMB - v.BitLen();
        BigInt<IntT> new_rhs = rhs << mov;
        v = BigInt<IntT>(new_rhs.val_ + new_base, new_rhs.len_ - new_base);
        *this <<= mov;
        size_t new_len = len_ / new_base;
        size_t i = (new_len - 1) * new_base;
        BigInt<IntT> tmp = new_rhs << (i * LIMB);
        if (tmp <= *this) {
            result += BigInt<IntT>(1) << (i * LIMB);
            *this -= tmp;
        }
        BigInt<IntT> u(val_ + i, len_ - i);
        do {
            i -= new_base;
            u /= v;
            if (u > b) u = b;
            *this -= (new_rhs * u) << (i * LIMB);
            if (Sign()) {
                --u;
                *this += new_rhs << (i * LIMB);
            }
            if (Sign()) {
                --u;
                *this += new_rhs << (i * LIMB);
            }
            result += u << (i * LIMB);

            u = BigInt<IntT>(val_ + i, new_base + new_base);
            if (u.Sign()) u.SetLen(u.len_ + 1, false);
        } while (i);
        *this >>= mov;
        if (mod) *mod = std::move(*this);
        *this = std::move(result);
    }
    if (sign) ToOpposite();
    if (mod) {
        if (sign) mod->ToOpposite();
        mod->ShrinkLen();
    }
    ShrinkLen();
    return *this;
}

// non-modifying
template <typename IntT>
BigInt<IntT> BigInt<IntT>::BasicDiv(BigInt lhs, IntT rhs, IntT* mod) {
    return lhs.BasicDivEq(rhs, mod);
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::PlainDiv(BigInt lhs, const BigInt& rhs,
                                    BigInt* mod) {
    return lhs.PlainDivEq(rhs, mod);
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::DivAlgA(BigInt lhs, const BigInt& rhs, BigInt* mod) {
    return lhs.DivEqAlgA(rhs, mod);
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::DivAlgB(BigInt lhs, const BigInt& rhs, BigInt* mod) {
    return lhs.DivEqAlgB(rhs, mod);
}
template <typename IntT>
BigInt<IntT> BigInt<IntT>::DivRecursive(BigInt lhs, const BigInt& rhs,
                                        BigInt* mod) {
    return lhs.DivEqRecursive(rhs, mod);
}

// non-modifying binary operators
template <typename IntT>
BigInt<IntT> operator/(BigInt<IntT> lhs, IntT rhs) {
    return lhs /= rhs;
}
template <typename IntT>
BigInt<IntT> operator/(BigInt<IntT> lhs, const BigInt<IntT>& rhs) {
    return lhs /= rhs;
}
template <typename IntT>
IntT operator%(BigInt<IntT> lhs, IntT rhs) {
    if (rhs == 0) return 0;
    if (lhs.Sign()) return rhs - (-lhs) % rhs;
    const uint64_t limb_mod = (1l << lhs.LIMB) % rhs;
    uint64_t cur_mod = 1;
    uint64_t tot = 0;
    if (limb_mod == 0) {
        tot = lhs.val_[0] % rhs;
    } else {
        for (size_t i = 0; i < lhs.len_; ++i) {
            tot = (tot + cur_mod * lhs.val_[i]) % rhs;
            cur_mod = cur_mod * limb_mod % rhs;
        }
    }
    return IntT(tot);
}
template <typename IntT>
BigInt<IntT> operator%(BigInt<IntT> lhs, const BigInt<IntT>& rhs) {
    return lhs %= rhs;
}
}  // namespace calc
