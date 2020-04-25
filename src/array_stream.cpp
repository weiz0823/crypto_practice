#include "array_stream.hpp"

#include <cstring>
namespace cryp {
uint64_t ArrayIstream::Read(uint8_t* buf, uint64_t count) {
    if (read_pos_ >= len_ || count == 0) {
        return 0;
    } else if (read_pos_ + count > len_) {
        count = len_ - read_pos_;
    }
    std::memcpy(buf, arr_ + read_pos_, count);
    read_pos_ += count;
    return count;
}
}  // namespace cryp
