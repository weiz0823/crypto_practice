#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
namespace cryp {
enum DataType {
    kNULL,
    kBOOL,
    kBYTE,
    kINT32,
    kINT64,
    kMPINT,
    kSTRING,
    kNAMELIST
};
// Standards: RFC 4251
class SSHFlow {
    std::vector<uint8_t> data_;
    std::vector<DataType> type_list_;
    uint32_t read_pos_ = 0;
    uint32_t read_idx_ = 0;

   public:
    template <typename T>
    void PushInt(T thing);
    // push an octet string, no matter the real data type
    void PushString(const uint8_t* thing, uint32_t size,
                    DataType type = kSTRING);
    // comma-separated list of name
    void PushNameList(const std::string* thing, uint32_t size);
    inline const std::vector<uint8_t>& Data() const { return (data_); }
    inline void ResetRead() { read_idx_ = read_pos_ = 0; }
    inline void Clear() {
        data_.clear();
        type_list_.clear();
        ResetRead();
    }
    inline DataType NextType() const {
        if (read_idx_ < type_list_.size())
            return type_list_[read_idx_];
        else
            return kNULL;
    }
    // return the index after skip
    uint32_t SkipNext();
    // get and goto next
    std::vector<uint8_t> GetNext();
};
uint32_t BytesToInt32(const uint8_t* a);
uint64_t BytesToInt64(const uint8_t* a);
extern template void SSHFlow::PushInt(uint8_t);
extern template void SSHFlow::PushInt(uint32_t);
extern template void SSHFlow::PushInt(uint64_t);
extern template void SSHFlow::PushInt(int8_t);
extern template void SSHFlow::PushInt(int32_t);
extern template void SSHFlow::PushInt(int64_t);
}  // namespace cryp
