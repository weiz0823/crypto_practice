#pragma once
#include "bigint64.hpp"
#include "type_aliases.hpp"
namespace cryp {
using BI = calc::BigInt<calc::uint128_t>;
inline BytesT I2OSP(const BI& src, LenT dst_len) {
    auto v = src.Serialize();
    if (v.size() > dst_len)
        v.erase(v.begin(), v.begin() + v.size() - dst_len);
    else if (v.size() < dst_len)
        v.insert(v.begin(), dst_len - v.size(), 0);
    return v;
}
inline BI OS2IP(const ByteT* it_begin, const ByteT* it_end) {
    if (*it_begin >= 0x80) {
        LenT len = it_end - it_begin + 1;
        auto v = new ByteT[len];
        std::copy(it_begin, it_end, v + 1);
        v[0] = 0;
        BI m(v, v + len);
        delete[] v;
        return m;
    } else {
        return BI(it_begin, it_end);
    }
}
}  // namespace cryp
