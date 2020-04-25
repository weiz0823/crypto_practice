#include "randomart.hpp"
namespace cryp {
void RandomArt::Visualize(const uint8_t* src, uint32_t len, std::FILE* file) {
    static constexpr int OFFSET[9][4] = {
        {-18, -16, 16, 18}, {-1, 1, 16, 18},    {-18, -16, -1, 1},
        {-17, -16, 17, 18}, {-18, -17, 16, 17}, {0, 1, 17, 18},
        {-1, 0, 16, 17},    {-17, -16, 0, 1},   {-18, -17, -1, 0}};
    static constexpr char CHARSET[18] = " .o+=*BOX@%&#/^SE";
    uint8_t a_[153];
    std::memset(a_, 0, sizeof(a_));
    int pos_ = 76;
    int state_ = 0;
    int x, y;
    uint8_t tmp;
    for (uint32_t i = 0; i < len; ++i) {
        tmp = src[i];
        for (uint8_t j = 0; j < 4; ++j) {
            pos_ += OFFSET[state_][tmp & 3];
            ++a_[pos_];
            tmp >>= 2;
            // modify state
            state_ = 0;
            x = pos_ / 17;
            y = pos_ % 17;
            if (x == 0)
                state_ += 1;
            else if (x == 8)
                state_ += 2;
            if (y == 0)
                state_ += 4;
            else if (y == 16)
                state_ += 8;
            switch (state_) {
                case 0:
                    state_ = 0;
                    break;
                case 1:
                    state_ = 1;
                    break;
                case 2:
                    state_ = 2;
                    break;
                case 4:
                    state_ = 3;
                    break;
                case 8:
                    state_ = 4;
                    break;
                case 5:
                    state_ = 5;
                    break;
                case 9:
                    state_ = 6;
                    break;
                case 6:
                    state_ = 7;
                    break;
                case 10:
                    state_ = 8;
                    break;
            }
        }
    }
    a_[76] = 15;
    a_[pos_] = 16;
    char buf[21];
    buf[20] = '\0';
    buf[19] = '\n';
    buf[18] = buf[0] = '|';
    std::fwrite(first_line, 1, 20, file);
    for (x = 0; x < 9; ++x) {
        for (y = 0; y < 17; ++y) buf[y + 1] = CHARSET[a_[17 * x + y]];
        std::fwrite(buf, 1, 20, file);
    }
    std::fwrite(last_line, 1, 20, file);
}
}  // namespace cryp
