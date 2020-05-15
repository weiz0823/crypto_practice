#pragma once
#include "bigint64.hpp"
#include "type_aliases.hpp"
namespace cryp {
using BI = calc::BigInt<calc::uint128_t>;
// Standard: ITU-T X.690: ASN.1 syntax
class ASN1 {
   public:
    enum ClassTag : ByteT {
        kUniversal = 0,
        kApplication = 0x40,
        kContext = 0x80,
        kPrivate = 0xc0
    };
    enum ValueEncodeType : ByteT { kPrimitive = 0, kConstructed = 0x20 };
    enum IDPrimitiveType {
        kEOC = 0,
        kBoolean = 0x1,
        kInteger = 0x2,
        kNull = 0x5,
        kOID = 0x6,
        kReal = 0x9,
        kEnumerate = 0xa,
        kRelativeOID = 0xd,
        kTime = 0xe,
        kDate = 0x1f,
        kTimeOfDay = 0x20,
        kDateTime = 0x21,
        kDuration = 0x22,
        kOIDIRI = 0x23,
        kRelativeOIDIRI = 0x24
    };
    enum IDConstructedType {
        kExternal = 0x8,
        kEmbeddedPDV = 0xb,
        kSequence = 0x10,
        kSet = 0x11
    };
    enum IDBothType {
        kBitString = 0x3,
        kOctetString = 0x4,
        kObjDescriptor = 0x7,
        kUTF8String = 0xc,
        kRESERVED = 0xf,
        kNumericString = 0x12,
        kPrintableString = 0x13,
        kT61String = 0x14,
        kVideotexString = 0x15,
        kIA5String = 0x16,
        kUTCTime = 0x17,
        kGeneralizedTime = 0x18,
        kGraphicString = 0x19,
        kVisibleString = 0x1a,
        kGeneralString = 0x1b,
        kUniversalString = 0x1c,
        kCharacterString = 0x1d,
        kBMPString = 0x1e
    };
    static BytesT EncodeIdentifier(int type);
    static BytesT EncodeIdentifier(IDConstructedType type) {
        auto v = EncodeIdentifier(static_cast<int>(type));
        v[0] |= kConstructed;
        return v;
    }
    static int DecodeIdentifier(int* dst, BytesT::const_iterator pos,
                                const BytesT::const_iterator& max_pos,
                                BytesT::const_iterator* end_pos = nullptr);
    // len=-1 is indefinite
    static BytesT EncodeLength(LenT len = -1);
    // -1 for indefinite
    static int DecodeLength(LenT* dst, BytesT::const_iterator pos,
                            const BytesT::const_iterator& max_pos,
                            BytesT::const_iterator* end_pos = nullptr);
    // basic types
    class EOC;
    class Boolean;
    class Integer;
    class Null;
    class OID;
    class OctetString;
    // wrapper types
    template <ClassTag TAG>
    class CustomWrapper;
    template <IDConstructedType TYPE>
    class BasicWrapper;
    using Universal = class CustomWrapper<kUniversal>;
    using Application = class CustomWrapper<kApplication>;
    using Context = class CustomWrapper<kContext>;
    using Private = class CustomWrapper<kPrivate>;
    using Sequence = class BasicWrapper<kSequence>;
    using Set = class BasicWrapper<kSet>;
};
class ASN1::EOC {
   protected:
    BytesT code_{kEOC, 0};

   public:
    void Encode(BytesT* dst) const { *dst += code_; }
    void EncodeImplicit(BytesT* dst) const { dst->push_back(0); }
    int DecodeImplicit(BytesT::const_iterator pos,
                       const BytesT::const_iterator& max_pos,
                       BytesT::const_iterator* end_pos = nullptr) const {
        if (end_pos) *end_pos = pos;
        if (pos >= max_pos) return -1;
        if (*pos) return -1;
        if (end_pos) *end_pos = pos + 1;
        return 0;
    }
    int Decode(BytesT::const_iterator pos,
               const BytesT::const_iterator& max_pos,
               BytesT::const_iterator* end_pos = nullptr) const {
        if (end_pos) *end_pos = pos;
        if (pos + 2 > max_pos) return -1;
        if (*pos != kEOC) return -1;
        if (*++pos) return -1;
        if (end_pos) *end_pos = pos + 2;
        return 0;
    }
};
inline const ASN1::EOC g_asn1_eoc;
class ASN1::Boolean {
   protected:
    BytesT code_;

   public:
    explicit Boolean(bool value = false) : code_({kBoolean, 1, 0}) {
        Set(value);
    }
    void Set(bool value) { code_.back() = value ? -1 : 0; }
    void Encode(BytesT* dst) const { *dst += code_; }
    void EncodeImplicit(BytesT* dst) const {
        dst->insert(dst->end(), code_.begin() + 1, code_.end());
    }
    int DecodeImplicit(bool* value, BytesT::const_iterator pos,
                       const BytesT::const_iterator& max_pos,
                       BytesT::const_iterator* end_pos = nullptr) const;
    int Decode(bool* value, BytesT::const_iterator pos,
               const BytesT::const_iterator& max_pos,
               BytesT::const_iterator* end_pos = nullptr) const;
};
class ASN1::Integer {
   protected:
    BytesT code_{kInteger};
    LenT idlen_ = 1;
    void Push(int value);
    void Push(uint64_t value);
    void Push(const BI& value) {
        BytesT v = value.Serialize();
        code_ += EncodeLength(v.size());
        code_ += v;
    }

   public:
    explicit Integer(int value = 0) { Push(value); }
    explicit Integer(uint64_t value) { Push(value); }
    explicit Integer(const BI& value) { Push(value); }
    void Set(int value) {
        code_.erase(code_.begin() + idlen_, code_.end());
        Push(value);
    }
    void Set(uint64_t value) {
        code_.erase(code_.begin() + idlen_, code_.end());
        Push(value);
    }
    void Set(const BI& value) {
        code_.erase(code_.begin() + idlen_, code_.end());
        Push(value);
    }
    void Encode(BytesT* dst) const { *dst += code_; }
    void EncodeImplicit(BytesT* dst) const {
        dst->insert(dst->end(), code_.begin() + idlen_, code_.end());
    }
    int DecodeImplicit(BI* value, BytesT::const_iterator pos,
                       const BytesT::const_iterator& max_pos,
                       BytesT::const_iterator* end_pos = nullptr) const;
    int Decode(BI* value, BytesT::const_iterator pos,
               const BytesT::const_iterator& max_pos,
               BytesT::const_iterator* end_pos = nullptr) const;
};
class ASN1::OctetString {
   protected:
    BytesT code_{kOctetString};
    LenT idlen_ = 1;
    void Push(const BytesT& value) {
        code_ += EncodeLength(value.size());
        code_ += value;
    }

   public:
    explicit OctetString(const BytesT& value) { Push(value); }
    void Set(const BytesT& value) {
        code_.erase(code_.begin() + idlen_, code_.end());
        Push(value);
    }
    void Encode(BytesT* dst) const { *dst += code_; }
    void EncodeImplicit(BytesT* dst) const {
        dst->insert(dst->end(), code_.begin() + idlen_, code_.end());
    }
    int DecodeImplicit(BytesT* value, BytesT::const_iterator pos,
                       const BytesT::const_iterator& max_pos,
                       BytesT::const_iterator* end_pos = nullptr) const;
    int Decode(BytesT* value, BytesT::const_iterator pos,
               const BytesT::const_iterator& max_pos,
               BytesT::const_iterator* end_pos = nullptr) const;
};
class ASN1::Null {
   protected:
    BytesT code_{kNull, 0};

   public:
    void Encode(BytesT* dst) const { *dst += code_; }
    void EncodeImplicit(BytesT* dst) const { dst->push_back(0); }
    int DecodeImplicit(BytesT::const_iterator pos,
                       const BytesT::const_iterator& max_pos,
                       BytesT::const_iterator* end_pos = nullptr) const {
        if (end_pos) *end_pos = pos;
        if (pos >= max_pos) return -1;
        if (*pos) return -1;
        if (end_pos) *end_pos = pos + 1;
        return 0;
    }
    int Decode(BytesT::const_iterator pos,
               const BytesT::const_iterator& max_pos,
               BytesT::const_iterator* end_pos = nullptr) const {
        if (end_pos) *end_pos = pos;
        if (pos + 2 > max_pos) return -1;
        if (*pos != kNull) return -1;
        if (*++pos) return -1;
        if (end_pos) *end_pos = pos + 2;
        return 0;
    }
};
inline const ASN1::Null g_asn1_null;
class ASN1::OID {
    std::string name_;
    std::string id_;
    std::string detail_;
    BytesT code_{kOID};
    LenT idlen_{code_.size()};
    void Push();

   public:
    explicit OID() = default;
    explicit OID(std::string name, std::string id, std::string detail)
        : name_(std::move(name)),
          id_(std::move(id)),
          detail_(std::move(detail)) {
        Push();
    }
    explicit OID(std::string name, const OID& base, const std::string& extra_id,
                 const std::string& extra_detail)
        : name_(std::move(name)),
          id_(base.id_ + "." + extra_id),
          detail_(base.detail_ + "/" + extra_detail) {
        Push();
    }
    const std::string& GetName() const { return name_; }
    const std::string& GetID() const { return id_; }
    const std::string& GetDetail() const { return detail_; }
    void Encode(BytesT* dst) const { *dst += code_; }
    void EncodeImplicit(BytesT* dst) const {
        dst->insert(dst->end(), code_.begin() + idlen_, code_.end());
    }
    // decode to this
    int DecodeImplicit(BytesT::const_iterator pos,
                       const BytesT::const_iterator& max_pos,
                       BytesT::const_iterator* end_pos = nullptr);
    int Decode(BytesT::const_iterator pos,
               const BytesT::const_iterator& max_pos,
               BytesT::const_iterator* end_pos = nullptr);
    bool operator==(const OID& rhs) const { return id_ == rhs.id_; }
    bool operator!=(const OID& rhs) const { return id_ != rhs.id_; }
};
inline const ASN1::OID id_unknown("unknown", "", "Unknown");
inline const ASN1::OID id_ber_encoding("basic-encoding", "2.1.1",
                                       "/Joint-ISO-ITU-T/ASN.1/Basic-Encoding");
inline const ASN1::OID id_cer_encoding(
    "basic-encoding", "2.1.2.0",
    "/Joint-ISO-ITU-T/ASN.1/BER-Derived/Canonical-Encoding");
inline const ASN1::OID id_der_encoding(
    "basic-encoding", "2.1.2.1",
    "/Joint-ISO-ITU-T/ASN.1/BER-Derived/Distinguished-Encoding");
// force abstract because it's just wrapper
template <ASN1::ClassTag TAG>
class ASN1::CustomWrapper {
   public:
    BytesT Wrap(int num, const BytesT& content) const;
    int Unwrap(int num, BytesT::const_iterator pos,
               const BytesT::const_iterator& max_pos,
               BytesT::const_iterator* begin_pos,
               BytesT::const_iterator* end_pos) const;
    virtual ~CustomWrapper() = 0;
};
template <ASN1::ClassTag TAG>
inline ASN1::CustomWrapper<TAG>::~CustomWrapper() = default;

template <ASN1::IDConstructedType TYPE>
class ASN1::BasicWrapper {
   public:
    BytesT Wrap(const BytesT& content) const;
    int Unwrap(BytesT::const_iterator pos,
               const BytesT::const_iterator& max_pos,
               BytesT::const_iterator* begin_pos,
               BytesT::const_iterator* end_pos) const;
    virtual ~BasicWrapper() = 0;
};
template <ASN1::IDConstructedType TYPE>
inline ASN1::BasicWrapper<TYPE>::~BasicWrapper() = default;

extern template class ASN1::CustomWrapper<ASN1::kUniversal>;
extern template class ASN1::CustomWrapper<ASN1::kApplication>;
extern template class ASN1::CustomWrapper<ASN1::kContext>;
extern template class ASN1::CustomWrapper<ASN1::kPrivate>;
extern template class ASN1::BasicWrapper<ASN1::kSequence>;
extern template class ASN1::BasicWrapper<ASN1::kSet>;
}  // namespace cryp
