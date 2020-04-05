#include "rsa.hpp"
namespace cryp {
const BI& RSA::GetN() { return (n); }
const BI& RSA::GetE() { return (e); }
void RSA::KeyGen(int bit_len) {
    std::printf("Start RSA-%d key generation...\n", bit_len);
    std::puts("Begin looking for a prime p...");
    p.GenRandom(bit_len >> 6, 32);
    p.ToNextPrime();
    std::printf("Got %d-bit prime p\n", bit_len >> 1);
    std::puts("Begin looking for a prime q...");
    q.GenRandom(bit_len >> 6, 32);
    q.ToNextPrime();
    std::printf("Got %d-bit prime q\n", bit_len >> 1);
    n = p * q;
    m = calc::GcdBin(--p, --q);
    m = p * q / m;
    calc::ExtGcdBin<uint32_t>(e, m, &d, nullptr);
    if (d.Sign()) d += m;
    dp = d % p;
    dq = d % q;
    calc::ExtGcdBin<uint32_t>(++q, ++p, &qinv, nullptr);
    if (qinv.Sign()) qinv += p;
    std::puts("---public key section---");
    std::puts("n = pq =");
    n.Print();
    std::puts("");
    std::puts("Public exponent e =");
    e.Print();
    std::puts("");
    std::puts("---private key section---");
    std::puts("p =");
    p.Print();
    std::puts("");
    std::puts("q =");
    q.Print();
    std::puts("");
    std::puts("m = lambda(n) =");
    m.Print();
    std::puts("");
    std::puts("Private exponent d =");
    d.Print();
    std::puts("");
    std::puts("> additional information on private key");
    std::puts("dp = d mod p-1 =");
    dp.Print();
    std::puts("");
    std::puts("dq = d mod q-1 =");
    dq.Print();
    std::puts("");
    std::puts("qinv = q^-1 mod p =");
    qinv.Print();
    std::puts("");
    std::printf("---end RSA-%d key---\n", bit_len);
}
}  // namespace cryp
