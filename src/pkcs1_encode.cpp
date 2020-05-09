#include "pkcs1_encode.hpp"
namespace cryp {
int EMEPKCS1::Encode(const ByteT* msg, LenT msg_len, ByteT* dst, LenT dst_len) {
    if (msg_len + 11 > dst_len) {
        std::fputs("Error(EMEPKCS1::Encode): message too long.", stderr);
        return 1;
    }
    dst[0] = 0;
    dst[1] = 2;
    LenT term = dst_len - msg_len - 1;
    LenT i = 2;
    std::uniform_int_distribution<uint8_t> rnd(1);  // [1, max]
    for (; i < term; ++i) dst[i] = rnd(g_rnd_engine32);
    dst[i++] = 0;
    std::copy(msg, msg + msg_len, dst + i);
    return 0;
}
int EMEPKCS1::Decode(const ByteT* encoded, LenT src_len, BytesT* dst) {
    if (src_len < 11) {
        std::fputs("Error(EMEPKCS1::Decode): encoded text too short.", stderr);
        return 1;
    }
    int rv = 0;
    if (encoded[0] != 0 || encoded[1] != 2) {
        std::fputs("Security warning(EMEPKCS1::Decode): prefix incorrect.",
                   stderr);
        rv |= 2;
    }
    LenT i = 2;
    while (i < src_len && encoded[i]) ++i;
    if (i >= src_len) {
        std::fputs("Security warning(EMEPKCS1::Decode): no end of padding.",
                   stderr);
        rv |= 4;
        dst->clear();
    } else {
        if (i < 10) {
            std::fputs("Security warning(EMEPKCS1::Decode): padding too short.",
                       stderr);
            rv |= 8;
        }
        ++i;
        dst->resize(src_len - i);
        std::copy(encoded + i, encoded + src_len, dst->begin());
    }
    return rv;
}
}  // namespace cryp
