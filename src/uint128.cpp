#include "uint128.hpp"
namespace cryp {
constexpr uint128_t::operator bool() const {
    return val_[0] || val_[1] || val_[2] || val_[3];
}
constexpr uint128_t::operator uint8_t() const { return uint8_t(val_[0]); }
constexpr uint128_t::operator uint16_t() const { return uint16_t(val_[0]); }
constexpr uint128_t::operator uint32_t() const { return val_[0]; }
constexpr uint128_t::operator uint64_t() const {
    return uint64_t(val_[0]) | (uint64_t(val_[1]) << 32);
}
constexpr uint128_t& uint128_t::operator&=(const uint128_t& rhs) {
    val_[0] &= rhs.val_[0];
    val_[1] &= rhs.val_[1];
    val_[2] &= rhs.val_[2];
    val_[3] &= rhs.val_[3];
    return *this;
}
constexpr uint128_t& uint128_t::operator|=(const uint128_t& rhs) {
    val_[0] |= rhs.val_[0];
    val_[1] |= rhs.val_[1];
    val_[2] |= rhs.val_[2];
    val_[3] |= rhs.val_[3];
    return *this;
}
constexpr uint128_t& uint128_t::operator^=(const uint128_t& rhs) {
    val_[0] ^= rhs.val_[0];
    val_[1] ^= rhs.val_[1];
    val_[2] ^= rhs.val_[2];
    val_[3] ^= rhs.val_[3];
    return *this;
}
constexpr uint128_t& uint128_t::operator<<=(uint32_t rhs) {
    uint32_t r = rhs & 31;
    if (rhs >= 128) {
        val_[3] = val_[2] = val_[1] = val_[0] = 0;
    } else if (rhs >= 96) {
        val_[3] = val_[0] << r;
        val_[2] = val_[1] = val_[0] = 0;
    } else if (rhs >= 64) {
        if (r) {
            val_[3] = (val_[1] << r) | (val_[0] >> (32 - r));
            val_[2] = val_[0] << r;
        } else {
            val_[3] = val_[1];
            val_[2] = val_[0];
        }
        val_[1] = val_[0] = 0;
    } else if (rhs >= 32) {
        if (r) {
            val_[3] = (val_[2] << r) | (val_[1] >> (32 - r));
            val_[2] = (val_[1] << r) | (val_[0] >> (32 - r));
            val_[1] = val_[0] << r;
        } else {
            val_[3] = val_[2];
            val_[2] = val_[1];
            val_[1] = val_[0];
        }
        val_[0] = 0;
    } else if (rhs > 0) {
        val_[3] = (val_[3] << r) | (val_[2] >> (32 - r));
        val_[2] = (val_[2] << r) | (val_[1] >> (32 - r));
        val_[1] = (val_[1] << r) | (val_[0] >> (32 - r));
        val_[0] <<= r;
    }
    return *this;
}
constexpr uint128_t& uint128_t::operator>>=(uint32_t rhs) {
    uint32_t r = rhs & 31;
    if (rhs >= 128) {
        val_[0] = val_[1] = val_[2] = val_[3] = 0;
    } else if (rhs >= 96) {
        val_[0] = val_[3] >> r;
        val_[1] = val_[2] = val_[3] = 0;
    } else if (rhs >= 64) {
        if (r) {
            val_[0] = (val_[2] >> r) | (val_[3] << (32 - r));
            val_[1] = val_[3] >> r;
        } else {
            val_[0] = val_[2];
            val_[1] = val_[3];
        }
        val_[2] = val_[3] = 0;
    } else if (rhs >= 32) {
        if (r) {
            val_[0] = (val_[1] >> r) | (val_[2] << (32 - r));
            val_[1] = (val_[2] >> r) | (val_[3] << (32 - r));
            val_[2] = val_[3] >> r;
        } else {
            val_[0] = val_[1];
            val_[1] = val_[2];
            val_[2] = val_[3];
        }
        val_[3] = 0;
    } else if (rhs > 0) {
        val_[0] = (val_[0] >> r) | (val_[1] << (32 - r));
        val_[1] = (val_[1] >> r) | (val_[2] << (32 - r));
        val_[2] = (val_[2] >> r) | (val_[3] << (32 - r));
        val_[3] >>= r;
    }
    return *this;
}
constexpr uint128_t& uint128_t::ToBitInv() {
    val_[0] = ~val_[0];
    val_[1] = ~val_[1];
    val_[2] = ~val_[2];
    val_[3] = ~val_[3];
    return *this;
}
constexpr uint128_t& uint128_t::operator+=(const uint128_t& rhs) {
    bool overflow = false;
    for (uint8_t i = 0; i < 4; ++i) {
        val_[i] += rhs.val_[i] + overflow;
        if (val_[i] < rhs.val_[i] || (overflow && val_[i] <= rhs.val_[i]))
            overflow = true;
        else
            overflow = false;
    }
    return *this;
}
constexpr uint128_t& uint128_t::operator-=(const uint128_t& rhs) {
    // a-b=~(~a+b)
    ToBitInv();
    *this += rhs;
    ToBitInv();
    return *this;
}
constexpr uint128_t& uint128_t::operator++() {
    ++val_[0];
    if (val_[0]) return *this;
    ++val_[1];
    if (val_[1]) return *this;
    ++val_[2];
    if (val_[2]) return *this;
    ++val_[3];
    return *this;
}
constexpr uint128_t& uint128_t::operator--() {
    auto cond = uint32_t(-1);
    --val_[0];
    if (val_[0] != cond) return *this;
    --val_[1];
    if (val_[1] != cond) return *this;
    --val_[2];
    if (val_[2] != cond) return *this;
    --val_[3];
    return *this;
}
constexpr uint128_t uint128_t::operator++(int) {
    auto obj = *this;
    ++*this;
    return obj;
}
constexpr uint128_t uint128_t::operator--(int) {
    auto obj = *this;
    --*this;
    return obj;
}

constexpr uint128_t operator&(uint128_t lhs, const uint128_t& rhs) {
    return lhs &= rhs;
}
constexpr uint128_t operator|(uint128_t lhs, const uint128_t& rhs) {
    return lhs |= rhs;
}
constexpr uint128_t operator^(uint128_t lhs, const uint128_t& rhs) {
    return lhs ^= rhs;
}
constexpr uint8_t operator&(uint128_t lhs, uint8_t rhs) {
    return lhs.val_[0] & rhs;
}
constexpr uint128_t operator>>(uint128_t lhs, uint32_t rhs) {
    return lhs >>= rhs;
}
constexpr uint128_t operator<<(uint128_t lhs, uint32_t rhs) {
    return lhs <<= rhs;
}
constexpr uint128_t operator~(uint128_t lhs) { return lhs.ToBitInv(); }
constexpr uint128_t operator+(uint128_t lhs, const uint128_t& rhs) {
    return lhs += rhs;
}
constexpr uint128_t operator-(uint128_t lhs, const uint128_t& rhs) {
    return lhs -= rhs;
}
constexpr uint128_t operator-(uint128_t lhs) {
    lhs.ToBitInv();
    return ++lhs;
}
}  // namespace cryp
