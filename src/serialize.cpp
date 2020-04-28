#include "serialize.hpp"
namespace cryp {
void SerializeString(std::vector<uint8_t>& dst, const uint8_t* thing,
                     uint32_t size) {
    dst.push_back(size >> 24);
    dst.push_back(size >> 16);
    dst.push_back(size >> 8);
    dst.push_back(size);
    if (!size) return;

    dst.resize(dst.size() + size);
    std::memcpy((dst.end() - size).base(), thing, size);
}
void SerializeNameList(std::vector<uint8_t>& dst, const std::string* thing,
                       uint32_t size) {
    if (!size) {
        dst.push_back(0);
        dst.push_back(0);
        dst.push_back(0);
        dst.push_back(0);
        return;
    }
    uint32_t tot_size = size - 1;
    for (int i = 0; i < size; ++i) tot_size += thing[i].size();
    dst.push_back(tot_size >> 24);
    dst.push_back(tot_size >> 16);
    dst.push_back(tot_size >> 8);
    dst.push_back(tot_size);

    uint32_t pos = dst.size();
    dst.resize(dst.size() + tot_size);
    std::memcpy(dst.data() + pos, thing[0].data(), thing[0].size());
    pos += thing[0].size();
    for (int i = 1; i < size; ++i) {
        dst.assign(pos++, uint8_t(','));
        std::memcpy(dst.data() + pos, thing[i].data(), thing[i].size());
        pos += thing[i].size();
    }
}
const uint8_t* DeserializeString(const uint8_t** start_pos,
                                 const uint8_t** end_pos, const uint8_t* a) {
    uint32_t len;
    a = DeserializeInt32(&len, a);
    *start_pos = a;
    *end_pos = a + len;
    return a + len;
}
}  // namespace cryp
