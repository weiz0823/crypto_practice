#pragma once
#include <iostream>
namespace cryp {
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
};
inline const OID id_unknown("Unknown", "NULL", "Unknown");
}  // namespace cryp
