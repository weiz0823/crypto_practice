#ifndef RSA_HPP
#define RSA_HPP
#include "bigint.hpp"

namespace cryp {
using BI = calc::BigInt<uint16_t>;
class RSA {
    BI p, q, m, d;
    BI n, e = BI(65537);

   public:
    const BI& GetN();
    const BI& GetE();
    void KeyGen();
};
}  // namespace cryp

#endif /* RSA_HPP */
