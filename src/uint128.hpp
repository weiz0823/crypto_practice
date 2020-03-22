#ifndef UINT128_H
#define UINT128_H
#include <iostream>

namespace cryp {
class uint128_t {
   private:
    uint32_t val_[4] = {0, 0, 0, 0};

   public:
    explicit constexpr uint128_t(int x) : val_{uint32_t(x), 0, 0, 0} {}
    explicit constexpr uint128_t(uint32_t x) : val_{x, 0, 0, 0} {}
    explicit constexpr uint128_t(uint64_t x)
        : val_{uint32_t(x), uint32_t(x >> 32), 0, 0} {}
    explicit constexpr operator bool() const;
    explicit constexpr operator uint8_t() const;
    explicit constexpr operator uint16_t() const;
    explicit constexpr operator uint32_t() const;
    explicit constexpr operator uint64_t() const;
    constexpr uint128_t& operator&=(const uint128_t& rhs);
    constexpr uint128_t& operator|=(const uint128_t& rhs);
    constexpr uint128_t& operator^=(const uint128_t& rhs);
    constexpr uint128_t& operator<<=(uint32_t rhs);
    constexpr uint128_t& operator>>=(uint32_t rhs);
    constexpr uint128_t& ToBitInv();
    constexpr uint128_t& operator+=(const uint128_t& rhs);
    constexpr uint128_t& operator-=(const uint128_t& rhs);
    constexpr uint128_t& operator++();
    constexpr uint128_t& operator--();
    constexpr uint128_t operator++(int);
    constexpr uint128_t operator--(int);
    friend constexpr uint8_t operator&(uint128_t lhs, uint8_t rhs);
};
constexpr uint128_t operator&(uint128_t lhs, const uint128_t& rhs);
constexpr uint128_t operator|(uint128_t lhs, const uint128_t& rhs);
constexpr uint128_t operator^(uint128_t lhs, const uint128_t& rhs);
constexpr uint8_t operator&(uint128_t lhs, uint8_t rhs);
constexpr uint128_t operator<<(uint128_t lhs, uint32_t rhs);
constexpr uint128_t operator>>(uint128_t lhs, uint32_t rhs);
constexpr uint128_t operator~(uint128_t lhs);
constexpr uint128_t operator+(uint128_t lhs, const uint128_t& rhs);
constexpr uint128_t operator-(uint128_t lhs, const uint128_t& rhs);
constexpr uint128_t operator-(uint128_t lhs);
}  // namespace cryp

#endif /* UINT128_H */
