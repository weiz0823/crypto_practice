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

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    // get random ready
    std::random_device rand_dev_;
    auto rand_gen_ = static_cast<std::mt19937>(rand_dev_());
    std::uniform_int_distribution<unsigned> rand_;
	rand_(rand_gen_);

    return 0;
}
