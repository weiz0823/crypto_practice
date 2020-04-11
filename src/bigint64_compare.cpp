#include "bigint64.hpp"
namespace calc {
#ifdef __cpp_impl_three_way_comparison
std::weak_ordering BigInt<uint128_t>::operator<=>(const BigInt& rhs) const {
    bool sign = Sign();
    auto cmp = len_ <=> rhs.len_;
    if (sign != rhs.Sign()) {
        if (sign)
            return std::strong_ordering::less;
        else
            return std::strong_ordering::greater;
    }
    auto it = end_;
    auto cit = rhs.end_;
    if (cmp < 0) {
        auto term = rhs.val_ + len_;
        if (sign) {
            do
                if (*(--cit) != -1) return std::strong_ordering::greater;
            while (cit != term);
        } else {
            do
                if (*(--cit)) return std::strong_ordering::less;
            while (cit != term);
        }
    } else if (cmp > 0) {
        auto term = val_ + rhs.len_;
        if (sign) {
            do
                if (*(--it) != -1) return std::strong_ordering::less;
            while (it != term);
        } else {
            do
                if (*(--it)) return std::strong_ordering::greater;
            while (it != term);
        }
    }
    do
        if ((cmp = *(--it) <=> *(--cit)) != 0) return cmp;
    while (it != val_);
    return std::strong_ordering::equal;
}
#else
int BigInt<uint128_t>::Compare(const BigInt& rhs) const {
    bool sign = Sign();
    if (sign != rhs.Sign()) {
        if (sign)
            return -1;
        else
            return 1;
    }
    auto it = end_;
    auto cit = rhs.end_;
    if (len_ < rhs.len_) {
        auto term = rhs.val_ + len_;
        if (sign) {
            do
                if (*(--cit) != uint128_t(-1)) return 1;
            while (cit != term);
        } else {
            do
                if (*(--cit)) return -1;
            while (cit != term);
        }
    } else if (len_ > rhs.len_) {
        auto term = val_ + rhs.len_;
        if (sign) {
            do
                if (*(--it) != uint128_t(-1)) return -1;
            while (it != term);
        } else {
            do
                if (*(--it)) return 1;
            while (it != term);
        }
    }
    do
        if (*(--it) != *(--cit)) {
            if (*it < *cit)
                return -1;
            else
                return 1;
        }
    while (it != val_);
    return 0;
}
#endif
}  // namespace calc
