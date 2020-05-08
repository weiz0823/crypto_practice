#pragma once
#include <iostream>
#include <vector>
namespace cryp {
// oid class, header only
// Standard: ITU-T X.690: ASN.1 syntax
class OID {
    std::string name_;
    std::string id_;
    std::string detail_;

   public:
    inline explicit OID(std::string name, std::string id, std::string detail)
        : name_(std::move(name)),
          id_(std::move(id)),
          detail_(std::move(detail)) {}
    inline explicit OID(std::string name, const OID& base,
                        const std::string& extra_id,
                        const std::string& extra_detail)
        : name_(std::move(name)),
          id_(base.id_ + "." + extra_id),
          detail_(base.detail_ + "/" + extra_detail) {}
    inline const std::string& GetName() const { return name_; }
    inline const std::string& GetID() const { return id_; }
    inline const std::string& GetDetail() const { return detail_; }
    inline std::vector<uint8_t> DEREncode() const {
        std::vector<uint8_t> v;
        v.push_back(6);  // 6 = object identifier
        v.push_back(0);  // length, reserved
        if (id_.empty() || !std::isdigit(id_[0])) return v;
        size_t ps = 0, pe = id_.find('.');
        if (pe == id_.npos) {
            v.push_back(std::stoi(id_) * 40);
            return v;
        }
        unsigned tmp = std::stoi(id_.substr(ps, pe)) * 40;
        ps = pe + 1;
        pe = id_.find('.', ps);
        tmp += std::stoi(id_.substr(ps, pe));
        v.push_back(tmp);
        uint8_t tmpv[6];
        int tmplen = 0;
        while (pe != id_.npos) {
            ps = pe + 1;
            pe = id_.find('.', ps);
            tmp = std::stoi(id_.substr(ps, pe));
            if (tmp < 128) {
                v.push_back(tmp);
            } else {
                tmplen = 0;
                tmpv[tmplen++] = tmp & 127;
                while (tmp >= 128) {
                    tmp >>= 7;
                    tmpv[tmplen++] = 128 | (tmp & 127);
                }
                do {
                    v.push_back(tmpv[--tmplen]);
                } while (tmplen > 0);
            }
        }
        v[1] = v.size() - 2; // set length
        return v;
    }
};
inline const OID id_unknown("Unknown", "NULL", "Unknown");
}  // namespace cryp
