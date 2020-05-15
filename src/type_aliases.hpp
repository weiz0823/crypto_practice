#pragma once
#include <cstdint>
#include <vector>
namespace cryp {
using ByteT = uint8_t;
using LenT = unsigned long long;
using BytesT = std::vector<ByteT>;
inline BytesT& operator+=(BytesT& lhs, const BytesT& rhs) {
    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return lhs;
}
inline BytesT operator+(const BytesT& lhs, const BytesT& rhs) {
    BytesT tmp = lhs;
    tmp.insert(tmp.end(), rhs.begin(), rhs.end());
    return tmp;
}
inline BytesT operator+(BytesT&& lhs, const BytesT& rhs) {
    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return std::move(lhs);
}
}  // namespace cryp
