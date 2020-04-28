#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
namespace cryp {
// Standards: RFC 4251
inline void SerializeBool(std::vector<uint8_t>& dst, bool thing) {
    dst.push_back(thing);
}
inline void SerializeByte(std::vector<uint8_t>& dst, uint8_t thing) {
    dst.push_back(thing);
}
inline void SerializeInt32(std::vector<uint8_t>& dst, uint32_t thing) {
    dst.push_back(thing >> 24);
    dst.push_back(thing >> 16);
    dst.push_back(thing >> 8);
    dst.push_back(thing);
}
inline void SerializeInt64(std::vector<uint8_t>& dst, uint64_t thing) {
    for (int i = 56; i >= 0; i -= 8) dst.push_back(thing >> i);
}
void SerializeString(std::vector<uint8_t>& dst, const uint8_t* thing,
                     uint32_t size);
inline void SerializeString(std::vector<uint8_t>& dst,
                     std::vector<uint8_t> const& thing) {
    return SerializeString(dst, thing.data(), thing.size());
}
inline void SerializeString(std::vector<uint8_t>& dst,
                            const std::string& thing) {
    return SerializeString(dst, reinterpret_cast<const uint8_t*>(thing.data()),
                           thing.size());
}
inline void SerializeString(std::vector<uint8_t>& dst, const char* thing) {
    return SerializeString(dst, reinterpret_cast<const uint8_t*>(thing),
                           std::strlen(thing));
}
inline void SerializeString(std::vector<uint8_t>& dst, const char* thing,
                            uint32_t size) {
    return SerializeString(dst, reinterpret_cast<const uint8_t*>(thing), size);
}
void SerializeNameList(std::vector<uint8_t>& dst, const std::string* thing,
                       uint32_t size);

inline const uint8_t* DeserializeBool(bool* dst, const uint8_t* a) {
    *dst = *a;
    return ++a;
}
inline const uint8_t* DeserializeByte(bool* dst, const uint8_t* a) {
    *dst = *a;
    return ++a;
}
inline const uint8_t* DeserializeInt32(uint32_t* dst, const uint8_t* a) {
    *dst = *a;
    *dst = (*dst << 8) | *(++a);
    *dst = (*dst << 8) | *(++a);
    *dst = (*dst << 8) | *(++a);
    return ++a;
}
inline const uint8_t* DeserializeInt64(uint64_t* dst, const uint8_t* a) {
    *dst = *a;
    for (int i = 0; i < 7; ++i) *dst = (*dst << 8) | *(++a);
    return ++a;
}
const uint8_t* DeserializeString(const uint8_t** start_pos,
                                 const uint8_t** end_pos, const uint8_t* a);
}  // namespace cryp
