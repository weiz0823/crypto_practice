#include "../src/dsa.hpp"
#include <chrono>
#include <iostream>
using cryp::BytesT;
using cryp::ByteT;
using cryp::LenT;
void DoTest(const cryp::DSAPubKey& pub_key, const cryp::DSAPrvKey& prv_key,
            const ByteT* data, LenT len) {
    std::puts("Start testing DSA signature...");
    cryp::BI r, s;
    prv_key.Sign(&r, &s, data, len);
    std::puts("DSA signature generation ends.");
    std::puts("Signature (hexadecimal integers):");
    std::puts("r =");
    r.Print(16);
    std::puts("");
    std::puts("s =");
    s.Print(16);
    std::puts("");
    if (pub_key.Verify(data, len, r, s)) {
        std::puts("DSA verification ends. Signature inconsistent.");
    } else {
        std::puts("DSA verification ends. Signature OK.");
    }
}
int main() {
    LenT l = 1024, n = 160;
    std::string str;
    std::printf("Please input message: ");
    std::getline(std::cin, str);
    cryp::DSADomain domain;
    cryp::DSAPrvKey prv_key;
    cryp::DSAPubKey pub_key;
    auto start_time = std::chrono::high_resolution_clock::now();
    domain.RandomGen(l, n, 1);
    cryp::DSA::KeygenRandom(&pub_key, &prv_key, domain, l, n, 1);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    std::printf("DSA-(%llu, %llu) key generation ends. Time: %.3lfs\n", l, n,
                duration.count() / 1e9);
    std::puts("");
    DoTest(pub_key, prv_key, reinterpret_cast<const ByteT*>(str.data()),
           str.size());
    std::puts("");

    l = 2048;
    n = 224;
    start_time = std::chrono::high_resolution_clock::now();
    domain.RandomGen(l, n, 1);
    cryp::DSA::KeygenRandom(&pub_key, &prv_key, domain, l, n, 1);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("DSA-(%llu, %llu) key generation ends. Time: %.3lfs\n", l, n,
                duration.count() / 1e9);
    std::puts("");
    DoTest(pub_key, prv_key, reinterpret_cast<const ByteT*>(str.data()),
           str.size());
    std::puts("");

    l = 2048;
    n = 256;
    start_time = std::chrono::high_resolution_clock::now();
    domain.RandomGen(l, n, 1);
    cryp::DSA::KeygenRandom(&pub_key, &prv_key, domain, l, n, 1);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("DSA-(%llu, %llu) key generation ends. Time: %.3lfs\n", l, n,
                duration.count() / 1e9);
    std::puts("");
    DoTest(pub_key, prv_key, reinterpret_cast<const ByteT*>(str.data()),
           str.size());
    return 0;
}
