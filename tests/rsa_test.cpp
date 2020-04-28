#include "../src/rsa.hpp"

#include <chrono>
#include <iostream>

#include "../src/base64.hpp"
int main() {
    cryp::Base64 base64;
    cryp::RSAPrvKey prv_key;
    cryp::RSAPubKey pub_key;
    auto start_time = std::chrono::high_resolution_clock::now();
    cryp::RSA::KeyGen(&pub_key, &prv_key, 1024, 2);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    std::puts("Base64-encoded serialized public key:");
    pub_key.PrintKey(cryp::kSSH, base64);
    std::puts("");
    std::puts("Recovered key from serialized data:");
    auto v = pub_key.Serialize(cryp::kSSH);
    pub_key = cryp::RSAPubKey(v.data(), cryp::kSSH);
    pub_key.PrintInfo();
    std::puts("");
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    cryp::RSA::KeyGen(&pub_key, &prv_key, 2048, 2);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::puts("Base64-encoded serialized public key:");
    pub_key.PrintKey(cryp::kSSH, base64);
    std::puts("");
    std::puts("Recovered key from serialized data:");
    v = pub_key.Serialize(cryp::kSSH);
    pub_key = cryp::RSAPubKey(v.data(), cryp::kSSH);
    pub_key.PrintInfo();
    std::puts("");
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    cryp::RSA::KeyGen(&pub_key, &prv_key, 3072, 2);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::puts("Base64-encoded serialized public key:");
    pub_key.PrintKey(cryp::kSSH, base64);
    std::puts("");
    std::puts("Recovered key from serialized data:");
    v = pub_key.Serialize(cryp::kSSH);
    pub_key = cryp::RSAPubKey(v.data(), cryp::kSSH);
    pub_key.PrintInfo();
    std::puts("");
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    return 0;
}
