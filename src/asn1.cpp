#include "asn1.hpp"
namespace cryp {
BytesT ASN1::EncodeIdentifier(int type) {
    BytesT v;
    if (type < 0x1f) {
        v.push_back(type);
    } else {
        v.push_back(0x1f);
        ByteT reversed[5] = {0};
        int i = 5;
        while (type > 0) {
            reversed[--i] = type & 0x7f;
            type >>= 7;
        }
        for (; i < 4; ++i) v.push_back(0x80 | reversed[i]);
        v.push_back(reversed[i]);
    }
    return v;
}
int ASN1::DecodeIdentifier(int* dst, BytesT::const_iterator pos,
                           BytesT::const_iterator max_pos,
                           BytesT::const_iterator* end_pos) {
    if (pos >= max_pos) return -1;
    int value = *pos & 0x1f;
    if (value == 0x1f) {
        // long
        value = 0;
        while (++pos < max_pos && (*pos & 0x80)) {
            value |= *pos & 0x7f;
            value <<= 7;
        }
        if (pos >= max_pos) return -1;
        value |= *pos;
    }
    if (end_pos) *end_pos = ++pos;
    *dst = value;
    return 0;
}
// len=-1 is indefinite
BytesT ASN1::EncodeLength(LenT len) {
    BytesT v;
    if (len <= 0x7f) {
        // definite short
        v.push_back(len);
    } else if (len == -1) {
        // indefinite
        v.push_back(0x80);
    } else {
        // definite long
        v.push_back(0x80);
        ByteT reversed[8] = {0};
        int i = 8;
        while (len) {
            reversed[--i] = len;
            len >>= 8;
        }
        v[0] |= 8 - i;
        for (; i < 8; ++i) v.push_back(reversed[i]);
    }
    return v;
}
// -1 for indefinite
int ASN1::DecodeLength(LenT* dst, BytesT::const_iterator pos,
                       BytesT::const_iterator max_pos,
                       BytesT::const_iterator* end_pos) {
    if (pos >= max_pos) return -1;
    LenT value;
    if (*pos == 0x80) {
        // indefinite
        value = -1;
    } else if (*pos & 0x80) {
        // definite long
        value = 0;
        int len = *pos & 0x7f;
        for (int i = 0; i < len; ++i) {
            if (++pos >= max_pos) return -1;
            value |= *pos;
            value <<= 8;
        }
    } else {
        // definite short
        value = *pos;
    }
    if (end_pos) *end_pos = ++pos;
    *dst = value;
    return 0;
}

int ASN1::Boolean::DecodeImplicit(bool* value, BytesT::const_iterator pos,
                                  BytesT::const_iterator max_pos,
                                  BytesT::const_iterator* end_pos) const {
    if (end_pos) *end_pos = pos;
    if (pos + 2 > max_pos) return -1;
    if (*pos != 1) return -1;
    *value = *++pos;
    if (end_pos) *end_pos = ++pos;
    return 0;
}
int ASN1::Boolean::Decode(bool* value, BytesT::const_iterator pos,
                          BytesT::const_iterator max_pos,
                          BytesT::const_iterator* end_pos) const {
    if (end_pos) *end_pos = pos;
    if (pos + 3 > max_pos) return -1;
    if (*pos != kBoolean) return -1;
    if (*++pos != 1) return -1;
    *value = *++pos;
    if (end_pos) *end_pos = ++pos;
    return 0;
}

int ASN1::Integer::DecodeImplicit(BI* value, BytesT::const_iterator pos,
                                  BytesT::const_iterator max_pos,
                                  BytesT::const_iterator* end_pos) const {
    if (end_pos) *end_pos = pos;
    LenT len;
    if (DecodeLength(&len, pos, max_pos, &pos)) return -1;
    if (pos + len > max_pos) return -1;
    *value = BI(pos.base(), len);
    if (end_pos) *end_pos = pos + len;
    return 0;
}
int ASN1::Integer::Decode(BI* value, BytesT::const_iterator pos,
                          BytesT::const_iterator max_pos,
                          BytesT::const_iterator* end_pos) const {
    if (end_pos) *end_pos = pos;
    if (pos + 2 > max_pos) return -1;
    if (*pos != kInteger) return -1;
    ++pos;
    if (DecodeImplicit(value, pos, max_pos, &pos)) return -1;
    if (end_pos) *end_pos = pos;
    return 0;
}
void ASN1::Integer::Push(int value) {
    if (value == 0) {
        code_ += EncodeLength(1);
        code_.push_back(0);
        return;
    } else if (value == -1) {
        code_ += EncodeLength(1);
        code_.push_back(-1);
        return;
    }
    ByteT val[4];
    val[3] = value;
    value >>= 8;
    val[2] = value;
    value >>= 8;
    val[1] = value;
    value >>= 8;
    val[0] = value;
    int i = 0;
    if (value < 0) {
        while (val[i] == 0xff) ++i;
        if (!(val[i] >> 7)) --i;
    } else {
        while (val[i] == 0) ++i;
        if (val[i] >> 7) --i;
    }
    code_ += EncodeLength(4 - i);
    for (; i < 4; ++i) code_.push_back(val[i]);
}
void ASN1::Integer::Push(uint64_t value) {
    if (value == 0) {
        code_ += EncodeLength(1);
        code_.push_back(0);
        return;
    }
    ByteT val[9];
    for (int i = 8; i > 0; --i) {
        val[i] = value;
        value >>= 8;
    }
    val[0] = 0;
    int i = 1;
    while (val[i] == 0) ++i;
    if (val[i] >> 7) --i;
    code_ += EncodeLength(9 - i);
    for (; i < 9; ++i) code_.push_back(val[i]);
}

int ASN1::OctetString::DecodeImplicit(BytesT* value, BytesT::const_iterator pos,
                                      BytesT::const_iterator max_pos,
                                      BytesT::const_iterator* end_pos) const {
    if (end_pos) *end_pos = pos;
    LenT len;
    if (DecodeLength(&len, pos, max_pos, &pos)) return -1;
    if (pos + len > max_pos) return -1;
    value->clear();
    value->insert(value->end(), pos, pos + len);
    if (end_pos) *end_pos = pos + len;
    return 0;
}
int ASN1::OctetString::Decode(BytesT* value, BytesT::const_iterator pos,
                              BytesT::const_iterator max_pos,
                              BytesT::const_iterator* end_pos) const {
    if (end_pos) *end_pos = pos;
    if (pos + 2 > max_pos) return -1;
    if (*pos != kOctetString) return -1;
    ++pos;
    if (DecodeImplicit(value, pos, max_pos, &pos)) return -1;
    if (end_pos) *end_pos = pos;
    return 0;
}

void ASN1::OID::Push() {
    std::vector<uint8_t> v;
    if (id_.empty() || !std::isdigit(id_[0])) {
        code_ += EncodeLength(0);
        return;
    }
    size_t ps = 0, pe = id_.find('.');
    if (pe == id_.npos) {
        v.push_back(std::stoi(id_) * 40);
        code_ += EncodeLength(v.size());
        code_ += v;
        return;
    }
    unsigned tmp = std::stoi(id_.substr(ps, pe)) * 40;
    ByteT tmpv[6];
    int tmplen = 0;
    ps = pe + 1;
    pe = id_.find('.', ps);
    tmp += std::stoi(id_.substr(ps, pe));
    if (tmp < 0x80) {
        v.push_back(tmp);
    } else {
        tmplen = 0;
        tmpv[tmplen++] = tmp & 0x7f;
        while (tmp >= 0x80) {
            tmp >>= 7;
            tmpv[tmplen++] = 0x80 | (tmp & 0x7f);
        }
        do {
            v.push_back(tmpv[--tmplen]);
        } while (tmplen > 0);
    }
    while (pe != id_.npos) {
        ps = pe + 1;
        pe = id_.find('.', ps);
        tmp = std::stoi(id_.substr(ps, pe));
        if (tmp < 0x80) {
            v.push_back(tmp);
        } else {
            tmplen = 0;
            tmpv[tmplen++] = tmp & 0x7f;
            while (tmp >= 0x80) {
                tmp >>= 7;
                tmpv[tmplen++] = 0x80 | (tmp & 0x7f);
            }
            do {
                v.push_back(tmpv[--tmplen]);
            } while (tmplen > 0);
        }
    }
    code_ += EncodeLength(v.size());
    code_ += v;
}
int ASN1::OID::DecodeImplicit(BytesT::const_iterator pos,
                              BytesT::const_iterator max_pos,
                              BytesT::const_iterator* end_pos) {
    name_.clear();
    detail_.clear();
    if (end_pos) *end_pos = pos;
    LenT len;
    if (DecodeLength(&len, pos, max_pos, &pos)) return -1;
    if (pos + len > max_pos) return -1;
    unsigned tmp = 0;
    LenT p = 0;
    // first part
    while (p < len && (*pos & 0x80)) {
        tmp |= *pos & 0x7f;
        tmp <<= 7;
        ++pos;
        ++p;
    }
    if (p >= len) return -1;
    tmp |= *pos;
    ++pos;
    ++p;
    if (tmp >= 80) {
        id_ = "2.";
        id_ += std::to_string(tmp - 80);
    } else if (tmp >= 40) {
        id_ = "1.";
        id_ += std::to_string(tmp - 40);
    } else {
        id_ = "0.";
        id_ += std::to_string(tmp);
    }
    while (p < len) {
        tmp = 0;
        while (p < len && (*pos & 0x80)) {
            tmp |= *pos & 0x7f;
            tmp <<= 7;
            ++pos;
            ++p;
        }
        if (p >= len) return -1;
        tmp |= *pos;
        ++pos;
        ++p;
        id_ += '.';
        id_ += std::to_string(tmp - 80);
    }
    if (end_pos) *end_pos = pos;
    return 0;
}
int ASN1::OID::Decode(BytesT::const_iterator pos,
                      BytesT::const_iterator max_pos,
                      BytesT::const_iterator* end_pos) {
    if (end_pos) *end_pos = pos;
    if (pos + 2 > max_pos) return -1;
    if (*pos != kOID) return -1;
    ++pos;
    if (DecodeImplicit(pos, max_pos, &pos)) return -1;
    if (end_pos) *end_pos = pos;
    return 0;
}

template <ASN1::ClassTag TAG>
BytesT ASN1::CustomWrapper<TAG>::Wrap(int num, const BytesT& content) const {
    BytesT v = EncodeIdentifier(num);
    v[0] |= TAG;
    v[0] |= kConstructed;
    v += EncodeLength(content.size());
    return v;
}
template <ASN1::ClassTag TAG>
int ASN1::CustomWrapper<TAG>::Unwrap(int num, BytesT::const_iterator pos,
                                     BytesT::const_iterator max_pos,
                                     BytesT::const_iterator* begin_pos,
                                     BytesT::const_iterator* end_pos) const {
    if (pos + 2 > max_pos) return -1;
    if ((*pos & 0xc0) != TAG) return -1;
    if ((*pos & 0x20) != kConstructed) return -1;
    int id;
    if (DecodeIdentifier(&id, pos, max_pos, &pos)) return -1;
    if (id != num) return -1;
    LenT len;
    if (DecodeLength(&len, pos, max_pos, &pos)) return -1;
    if (pos + len > max_pos) return -1;
    *begin_pos = pos;
    *end_pos = pos + len;
    return 0;
}

template <ASN1::IDConstructedType TYPE>
BytesT ASN1::BasicWrapper<TYPE>::Wrap(const BytesT& content) const {
    BytesT v{kConstructed | TYPE};
    v += EncodeLength(content.size());
    return v;
}
template <ASN1::IDConstructedType TYPE>
int ASN1::BasicWrapper<TYPE>::Unwrap(BytesT::const_iterator pos,
                                     BytesT::const_iterator max_pos,
                                     BytesT::const_iterator* begin_pos,
                                     BytesT::const_iterator* end_pos) const {
    if (pos + 2 > max_pos) return -1;
    if (*pos != (kConstructed | TYPE)) return -1;
    ++pos;
    LenT len;
    if (DecodeLength(&len, pos, max_pos, &pos)) return -1;
    if (pos + len > max_pos) return -1;
    *begin_pos = pos;
    *end_pos = pos + len;
    return 0;
}
template class ASN1::CustomWrapper<ASN1::kUniversal>;
template class ASN1::CustomWrapper<ASN1::kApplication>;
template class ASN1::CustomWrapper<ASN1::kContext>;
template class ASN1::CustomWrapper<ASN1::kPrivate>;
template class ASN1::BasicWrapper<ASN1::kSequence>;
template class ASN1::BasicWrapper<ASN1::kSet>;
}  // namespace cryp
