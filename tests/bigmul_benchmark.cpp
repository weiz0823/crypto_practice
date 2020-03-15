#include <chrono>
#include <iostream>
#include <string>
#include "../src/bigint.hpp"
// using namespace std;

int PutInfo(const std::string& str) {
    return std::printf("\033[1m\033[30mInfo\033[0m: %s\n", str.c_str());
}
int PutOK(const std::string& str) {
    return std::printf("\033[32mOK\033[0m: %s\n", str.c_str());
}
int PutWarning(const std::string& str) {
    return std::fprintf(stderr, "\033[1m\033[35mWarning\033[0m: %s\n",
                        str.c_str());
}
int PutError(const std::string& str) {
    return std::fprintf(stderr, "\033[1m\033[31mError\033[0m: %s\n",
                        str.c_str());
}

void timeit_start() {}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    // get random ready
    // std::random_device rand_dev_;
    // auto rand_gen_ = static_cast<std::mt19937>(rand_dev_());
    // std::uniform_int_distribution<unsigned> rand_;

    auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto dur = end_time - start_time;  // nanoseconds, integer
    constexpr int MAX_TEST_COUNT = 1024;
    int TEST_COUNT;

    TEST_COUNT = 128;
    cryp::uint8192_t num[MAX_TEST_COUNT + 1], res(1u);
    cryp::uint8192_t alt[MAX_TEST_COUNT + 1];

    for (int i = 0; i <= TEST_COUNT; ++i) num[i].GenRandom();
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) num[i].MulEq_Plain(num[i + 1]);
    end_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) res &= num[i];
    dur = end_time - start_time;  // nanoseconds, integer
    std::printf(
        "%d test examples done on 8192bit plain multiplication. Time: %.3lfms. "
        "Average: %.3lfms. [",
        TEST_COUNT, dur.count() / 1e6, dur.count() / 1e6 / TEST_COUNT);
    res.Print();
    std::puts("]");

    TEST_COUNT = 128;
    for (int i = 0; i <= TEST_COUNT; ++i) num[i].GenRandom();
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) num[i].MulEq_Karatsuba(num[i + 1]);
    end_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) res &= num[i];
    dur = end_time - start_time;  // nanoseconds, integer
    std::printf(
        "%d test examples done on 8192bit Karatsuba multiplication. Time: "
        "%.3lfms. Average: %.3lfms. [",
        TEST_COUNT, dur.count() / 1e6, dur.count() / 1e6 / TEST_COUNT);
    res.Print();
    std::puts("]");

    TEST_COUNT = 32;
    for (int i = 0; i <= TEST_COUNT; ++i) num[i].GenRandom();
    for (int i = 0; i <= TEST_COUNT; ++i) alt[i].GenRandom(128);
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) num[i].DivEq_Plain(alt[i]);
    end_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) res &= num[i];
    dur = end_time - start_time;  // nanoseconds, integer
    std::printf(
        "%d test examples done on 8192bit/4096bit plain division. Time: "
        "%.3lfms. Average: %.3lfms. [",
        TEST_COUNT, dur.count() / 1e6, dur.count() / 1e6 / TEST_COUNT);
    res.Print();
    std::puts("]");

    TEST_COUNT = 32;
    for (int i = 0; i <= TEST_COUNT; ++i) num[i].GenRandom();
    for (int i = 0; i <= TEST_COUNT; ++i) alt[i].GenRandom(4);
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) num[i].DivEq_Plain(alt[i]);
    end_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) res &= num[i];
    dur = end_time - start_time;  // nanoseconds, integer
    std::printf(
        "%d test examples done on 8192bit/128bit plain division. Time: "
        "%.3lfms. Average: %.3lfms. [",
        TEST_COUNT, dur.count() / 1e6, dur.count() / 1e6 / TEST_COUNT);
    res.Print();
    std::puts("]");
    PutInfo("The speed is because the division is done bit by bit.");

    TEST_COUNT = 32;
    for (int i = 0; i <= TEST_COUNT; ++i) num[i].GenRandom();
    for (int i = 0; i <= TEST_COUNT; ++i) alt[i].GenRandom(200);
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) num[i].DivEq_Plain(alt[i]);
    end_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) res &= num[i];
    dur = end_time - start_time;  // nanoseconds, integer
    std::printf(
        "%d test examples done on 8192bit/6400bit plain division. Time: "
        "%.3lfms. Average: %.3lfms. [",
        TEST_COUNT, dur.count() / 1e6, dur.count() / 1e6 / TEST_COUNT);
    res.Print();
    std::puts("]");
    return 0;
}
