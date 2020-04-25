#include <vector>

#include "randomart.hpp"
int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::printf("Usage: %s hexnum\n", argv[0]);
        return 0;
    }
    uint8_t num = 0;
    std::vector<uint8_t> va;
    for (int i = 0; std::isxdigit(argv[1][i]) && std::isxdigit(argv[1][i + 1]);
         i += 2) {
        if (argv[1][i] <= '9')
            num = uint8_t((argv[1][i] - '0') << 4);
        else
            num = uint8_t((argv[1][i] - 'a' + 10) << 4);
        if (argv[1][i + 1] <= '9')
            num += uint8_t(argv[1][i + 1] - '0');
        else
            num += uint8_t(argv[1][i + 1] - 'a' + 10);
        va.push_back(num);
    }
    cryp::RandomArt ra;
    ra.Visualize(va.data(), va.size(), stdout);
}
