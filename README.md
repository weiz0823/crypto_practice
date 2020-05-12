Crytography practice \[Work in progress\]
=========================================

Implement the wheels!

Features
--------

-   \[x\] Base 64 binary-to-text encoding. `make base64 MODE=release`. Completed on 2020-03-16.
-   \[x\] MD5 message digest. `make md5 MODE=release`. Completed on 2020-03-17.
-   \[x\] SHA-1 and SHA-2 secure hash algorithms family. `make sha1 sha2 MODE=release`. Completed on 2020-03-22.
-   \[x\] `uint128_t` basic support, in case extended precision needed (e.g. SHA-512). Completed on 2020-03-22.
-   \[x\] SHA-3 secure hash algorithms family. `make sha3 MODE=release`. Completed on 2020-03-26.
-   \[x\] Merge BigInt support from [KaitoD/CPP-OOP-CALC]. Completed on 2020-03-31.
-   \[ \] 文本比对。cyl

TODO
----

-   \[ \] RSA: RSASSA-PSS
-   \[ \] RSA: RSASSA-PKCS1-v1_5
(涉及可以考虑hard code或者调用ASN.1编码的部分，并且是旧的签名方式了，可以暂时不做或永远不做)
-   \[ \] ASN.1: 详见[ASN.1讨论]。
-   \[ \] 公钥算法: DSA（应该不难）
-   \[ \] 公钥算法: ECDSA/EdDSA（可以先尝试学习）。
-   \[ \] 伪随机数: mersenne twister（就是C++内置的mt19937，看上去不难）
-   \[ \] 伪随机数: SP 800-90A里面的三个算法。
-   \[ \] 对称加密: AES，这是个大项目，后面再拆解。
-   \[ \] MAC: HMAC
-   \[ \] KDF: HKDF
-   \[ \] 库目标: 哈希函数
-   \[ \] 库目标: 伪随机比特
-   \[ \] 库目标: 伪随机数（伪随机比特+均等映射）
-   \[ \] 库目标: 消息编码方式（公钥算法用）
-   \[ \] 库目标: 公钥算法
-   \[ \] 库目标: 对称加密算法
-   \[ \] 库目标: HMAC（基于哈希）
-   \[ \] 库目标: 数据的流编码（SSH & ASN.1）
-   \[ \] 库目标: 二进制数据可打印编码



References
----------

-   https://www.iso.org/committee/45306/x/catalogue/
-   https://en.wikipedia.org/wiki/Outline\_of\_cryptography

  [KaitoD/CPP-OOP-CALC]: https://github.com/KaitoD/CPP-OOP-CALC
  [ASN.1讨论]: https://github.com/weiz0823/crypto_practice/issues/3
