#include "../src/rsa.hpp"

#include <chrono>
#include <iostream>
int main() {
    cryp::RSAPrvKey prv_key;
    cryp::RSAPubKey pub_key;
    auto start_time = std::chrono::high_resolution_clock::now();
    cryp::RSA::KeyGen(&pub_key, &prv_key, 1024, 3);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    cryp::RSA::KeyGen(&pub_key, &prv_key, 2048, 3);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    cryp::RSA::KeyGen(&pub_key, &prv_key, 3072, 3);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    return 0;
}
