#include "flow.hpp"
namespace cryp {
template <>
void SSHFlow::PushInt(bool thing) {
    // big-endian
    if (thing)
        data_.push_back(1);
    else
        data_.push_back(0);
    type_list_.push_back(kBOOL);
}
template <typename T>
void SSHFlow::PushInt(T thing) {
    constexpr int LEN = sizeof(T) << 3;
    // big-endian
    for (int i = LEN - 8; i >= 0; i -= 8) data_.push_back(thing >> i);
    if constexpr (LEN == 8) type_list_.push_back(DataType::kBYTE);
    if constexpr (LEN == 32) type_list_.push_back(DataType::kINT32);
    if constexpr (LEN == 64) type_list_.push_back(DataType::kINT64);
}
void SSHFlow::PushString(const uint8_t* thing, uint32_t size, DataType type) {
    data_.push_back(size >> 24);
    data_.push_back(size >> 16);
    data_.push_back(size >> 8);
    data_.push_back(size);
    if (!size) return;

    data_.resize(data_.size() + size);
    std::memcpy((data_.end() - size).base(), thing, size);
    type_list_.push_back(type);
}
void SSHFlow::PushNameList(const std::string* thing, uint32_t size) {
    if (!size) {
        data_.push_back(0);
        data_.push_back(0);
        data_.push_back(0);
        data_.push_back(0);
        return;
    }
    uint32_t tot_size = size - 1;
    for (int i = 0; i < size; ++i) tot_size += thing[i].size();
    data_.push_back(tot_size >> 24);
    data_.push_back(tot_size >> 16);
    data_.push_back(tot_size >> 8);
    data_.push_back(tot_size);

    uint32_t pos = data_.size();
    data_.resize(data_.size() + tot_size);
    std::memcpy(data_.data() + pos, thing[0].data(), thing[0].size());
    pos += thing[0].size();
    for (int i = 1; i < size; ++i) {
        data_.assign(pos++, uint8_t(','));
        std::memcpy(data_.data() + pos, thing[i].data(), thing[i].size());
        pos += thing[i].size();
    }
    type_list_.push_back(kNAMELIST);
}
uint32_t SSHFlow::SkipNext() {
    if (read_idx_ >= type_list_.size()) return read_idx_;
    auto type = NextType();
    if (type == kBOOL || type == kBYTE)
        read_pos_ += 1;
    else if (type == kINT32)
        read_pos_ += 4;
    else if (type == kINT64)
        read_pos_ += 8;
    else
        read_pos_ += 4 + BytesToInt32(data_.data() + read_pos_);
    return ++read_idx_;
}
std::vector<uint8_t> SSHFlow::GetNext() {
    auto type = NextType();
    ++read_idx_;
    if (type == kBOOL || type == kBYTE) {
        read_pos_ += 1;
        return std::vector<uint8_t>(data_.data() + read_pos_ - 1,
                                    data_.data() + read_pos_);
    } else if (type == kINT32) {
        read_pos_ += 4;
        return std::vector<uint8_t>(data_.data() + read_pos_ - 4,
                                    data_.data() + read_pos_);
    } else if (type == kINT64) {
        read_pos_ += 8;
        return std::vector<uint8_t>(data_.data() + read_pos_ - 8,
                                    data_.data() + read_pos_);
    } else {
        auto len = BytesToInt32(data_.data() + read_pos_);
        read_pos_ += 4 + len;
        return std::vector<uint8_t>(data_.data() + read_pos_ - len,
                                    data_.data() + read_pos_);
    }
}
uint32_t BytesToInt32(const uint8_t* a) {
    uint32_t x = *a;
    x = (x << 8) | *(++a);
    x = (x << 8) | *(++a);
    x = (x << 8) | *(++a);
    return x;
}
uint64_t BytesToInt64(const uint8_t* a) {
    uint64_t x = *a;
    for (int i = 0; i < 7; ++i) x = (x << 8) | *(++a);
    return x;
}

template void SSHFlow::PushInt(uint8_t);
template void SSHFlow::PushInt(uint32_t);
template void SSHFlow::PushInt(uint64_t);
template void SSHFlow::PushInt(int8_t);
template void SSHFlow::PushInt(int32_t);
template void SSHFlow::PushInt(int64_t);
}  // namespace cryp
