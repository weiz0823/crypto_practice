#include "../src/rsa.hpp"

#include <chrono>
#include <iostream>
int main() {
    cryp::RSA r;
    auto start_time = std::chrono::high_resolution_clock::now();
	r.KeyGen(1024);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    r.KeyGen(1636);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    r.KeyGen(2048);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    r.KeyGen(3072);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    std::puts("");
    start_time = std::chrono::high_resolution_clock::now();
    r.KeyGen(4096);
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::printf("Time: %.3lfs\n", duration.count() / 1e9);
    return 0;
}
