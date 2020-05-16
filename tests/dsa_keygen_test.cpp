#include "../src/dsa.hpp"

#include <chrono>
#include <iostream>

#include "../src/base64.hpp"
int main() {
    cryp::DSADomain domain;
    cryp::DSAPrvKey prv_key;
    cryp::DSAPubKey pub_key;
    auto start_time = std::chrono::high_resolution_clock::now();
    domain.RandomGen(1024, 160, 3);
    cryp::DSA::KeygenRandom(&pub_key, &prv_key, domain, 1024, 160, 3);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    domain.RandomGen(2048, 224, 3);
    cryp::DSA::KeygenRandom(&pub_key, &prv_key, domain, 2048, 224, 3);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    domain.RandomGen(2048, 256, 3);
    cryp::DSA::KeygenRandom(&pub_key, &prv_key, domain, 2048, 256, 3);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    // std::puts("");
    // start_time = std::chrono::high_resolution_clock::now();
    // end_time = std::chrono::high_resolution_clock::now();
    // duration = end_time - start_time;
    // std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    return 0;
}
