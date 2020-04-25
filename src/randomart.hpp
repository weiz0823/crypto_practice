#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>
namespace cryp {
class RandomArt {
   public:
    char first_line[21] = "+-----------------+\n";
    char last_line[21] = "+-----------------+\n";
    void Visualize(const uint8_t* src, uint32_t len, std::FILE* file = stdout);
};
}  // namespace cryp
