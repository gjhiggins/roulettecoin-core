// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_HASH_H
#define BITCOIN_HASH_H

#include <crypto/ripemd160.h>
#include <crypto/sha256.h>
#include <prevector.h>
#include <serialize.h>
#include <uint256.h>
#include <version.h>

#include <crypto/sphlib/sph_blake.h>
#include <crypto/sphlib/sph_bmw.h>
#include <crypto/sphlib/sph_cubehash.h>
#include <crypto/sphlib/sph_echo.h>
#include <crypto/sphlib/sph_fugue.h>
#include <crypto/sphlib/sph_groestl.h>
#include <crypto/sphlib/sph_hamsi.h>
#include <crypto/sphlib/sph_jh.h>
#include <crypto/sphlib/sph_keccak.h>
#include <crypto/sphlib/sph_luffa.h>
#include <crypto/sphlib/sph_sha2.h>
#include <crypto/sphlib/sph_shabal.h>
#include <crypto/sphlib/sph_shavite.h>
#include <crypto/sphlib/sph_simd.h>
#include <crypto/sphlib/sph_skein.h>
#include <crypto/sphlib/sph_types.h>
#include <crypto/sphlib/sph_whirlpool.h>


#include <vector>

typedef uint256 ChainCode;

/** A hasher class for Bitcoin's 256-bit hash (double SHA-256). */
class CHash256 {
private:
    CSHA256 sha;
public:
    static const size_t OUTPUT_SIZE = CSHA256::OUTPUT_SIZE;

    void Finalize(unsigned char hash[OUTPUT_SIZE]) {
        unsigned char buf[CSHA256::OUTPUT_SIZE];
        sha.Finalize(buf);
        sha.Reset().Write(buf, CSHA256::OUTPUT_SIZE).Finalize(hash);
    }

    CHash256& Write(const unsigned char *data, size_t len) {
        sha.Write(data, len);
        return *this;
    }

    CHash256& Reset() {
        sha.Reset();
        return *this;
    }
};

#ifdef GLOBALDEFINED
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL sph_blake512_context     z_blake;
GLOBAL sph_bmw512_context       z_bmw;
GLOBAL sph_groestl512_context   z_groestl;
GLOBAL sph_jh512_context        z_jh;
GLOBAL sph_keccak512_context    z_keccak;
GLOBAL sph_skein512_context     z_skein;
GLOBAL sph_luffa512_context     z_luffa;
GLOBAL sph_cubehash512_context  z_cubehash;
GLOBAL sph_shavite512_context   z_shavite;
GLOBAL sph_simd512_context      z_simd;
GLOBAL sph_echo512_context      z_echo;

#define fillz() do { \
    sph_blake512_init(&z_blake); \
    sph_bmw512_init(&z_bmw); \
    sph_groestl512_init(&z_groestl); \
    sph_jh512_init(&z_jh); \
    sph_keccak512_init(&z_keccak); \
    sph_skein512_init(&z_skein); \
    sph_luffa512_init(&z_luffa); \
    sph_cubehash512_init(&z_cubehash); \
    sph_shavite512_init(&z_shavite); \
    sph_simd512_init(&z_simd); \
    sph_echo512_init(&z_echo); \
} while (0)

#define ZBLAKE (memcpy(&ctx_blake, &z_blake, sizeof(z_blake)))
#define ZBMW (memcpy(&ctx_bmw, &z_bmw, sizeof(z_bmw)))
#define ZGROESTL (memcpy(&ctx_groestl, &z_groestl, sizeof(z_groestl)))
#define ZJH (memcpy(&ctx_jh, &z_jh, sizeof(z_jh)))
#define ZKECCAK (memcpy(&ctx_keccak, &z_keccak, sizeof(z_keccak)))
#define ZSKEIN (memcpy(&ctx_skein, &z_skein, sizeof(z_skein)))

/* ----------- Bitcoin Hash ------------------------------------------------- */
/** A hasher class for Bitcoin's 160-bit hash (SHA-256 + RIPEMD-160). */
class CHash160 {
private:
    CSHA256 sha;
public:
    static const size_t OUTPUT_SIZE = CRIPEMD160::OUTPUT_SIZE;

    void Finalize(unsigned char hash[OUTPUT_SIZE]) {
        unsigned char buf[CSHA256::OUTPUT_SIZE];
        sha.Finalize(buf);
        CRIPEMD160().Write(buf, CSHA256::OUTPUT_SIZE).Finalize(hash);
    }

    CHash160& Write(const unsigned char *data, size_t len) {
        sha.Write(data, len);
        return *this;
    }

    CHash160& Reset() {
        sha.Reset();
        return *this;
    }
};

/** Compute the 256-bit hash of an object. */
template<typename T1>
inline uint256 Hash(const T1 pbegin, const T1 pend)
{
    static const unsigned char pblank[1] = {};
    uint256 result;
    CHash256().Write(pbegin == pend ? pblank : (const unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0]))
              .Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of two objects. */
template<typename T1, typename T2>
inline uint256 Hash(const T1 p1begin, const T1 p1end,
                    const T2 p2begin, const T2 p2end) {
    static const unsigned char pblank[1] = {};
    uint256 result;
    CHash256().Write(p1begin == p1end ? pblank : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0]))
              .Write(p2begin == p2end ? pblank : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0]))
              .Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 160-bit hash an object. */
template<typename T1>
inline uint160 Hash160(const T1 pbegin, const T1 pend)
{
    static unsigned char pblank[1] = {};
    uint160 result;
    CHash160().Write(pbegin == pend ? pblank : (const unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0]))
              .Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 160-bit hash of a vector. */
inline uint160 Hash160(const std::vector<unsigned char>& vch)
{
    return Hash160(vch.begin(), vch.end());
}

/** Compute the 160-bit hash of a vector. */
template<unsigned int N>
inline uint160 Hash160(const prevector<N, unsigned char>& vch)
{
    return Hash160(vch.begin(), vch.end());
}

/** A writer stream (for serialization) that computes a 256-bit hash. */
class CHashWriter
{
private:
    CHash256 ctx;

    const int nType;
    const int nVersion;
public:

    CHashWriter(int nTypeIn, int nVersionIn) : nType(nTypeIn), nVersion(nVersionIn) {}

    int GetType() const { return nType; }
    int GetVersion() const { return nVersion; }

    void write(const char *pch, size_t size) {
        ctx.Write((const unsigned char*)pch, size);
    }

    // invalidates the object
    uint256 GetHash() {
        uint256 result;
        ctx.Finalize((unsigned char*)&result);
        return result;
    }

    template<typename T>
    CHashWriter& operator<<(const T& obj) {
        // Serialize to this stream
        ::Serialize(*this, obj);
        return (*this);
    }
};

/** Reads data from an underlying stream, while hashing the read data. */
template<typename Source>
class CHashVerifier : public CHashWriter
{
private:
    Source* source;

public:
    explicit CHashVerifier(Source* source_) : CHashWriter(source_->GetType(), source_->GetVersion()), source(source_) {}

    void read(char* pch, size_t nSize)
    {
        source->read(pch, nSize);
        this->write(pch, nSize);
    }

    void ignore(size_t nSize)
    {
        char data[1024];
        while (nSize > 0) {
            size_t now = std::min<size_t>(nSize, 1024);
            read(data, now);
            nSize -= now;
        }
    }

    template<typename T>
    CHashVerifier<Source>& operator>>(T& obj)
    {
        // Unserialize from this stream
        ::Unserialize(*this, obj);
        return (*this);
    }
};

/** Compute the 256-bit hash of an object's serialization. */
template<typename T>
uint256 SerializeHash(const T& obj, int nType=SER_GETHASH, int nVersion=PROTOCOL_VERSION)
{
    CHashWriter ss(nType, nVersion);
    ss << obj;
    return ss.GetHash();
}

unsigned int MurmurHash3(unsigned int nHashSeed, const std::vector<unsigned char>& vDataToHash);

void BIP32Hash(const ChainCode &chainCode, unsigned int nChild, unsigned char header, const unsigned char data[32], unsigned char output[64]);

/** SipHash-2-4 */
class CSipHasher
{
private:
    uint64_t v[4];
    uint64_t tmp;
    int count;

public:
    /** Construct a SipHash calculator initialized with 128-bit key (k0, k1) */
    CSipHasher(uint64_t k0, uint64_t k1);
    /** Hash a 64-bit integer worth of data
     *  It is treated as if this was the little-endian interpretation of 8 bytes.
     *  This function can only be used when a multiple of 8 bytes have been written so far.
     */
    CSipHasher& Write(uint64_t data);
    /** Hash arbitrary bytes. */
    CSipHasher& Write(const unsigned char* data, size_t size);
    /** Compute the 64-bit SipHash-2-4 of the data written so far. The object remains untouched. */
    uint64_t Finalize() const;
};

/** Optimized SipHash-2-4 implementation for uint256.
 *
 *  It is identical to:
 *    SipHasher(k0, k1)
 *      .Write(val.GetUint64(0))
 *      .Write(val.GetUint64(1))
 *      .Write(val.GetUint64(2))
 *      .Write(val.GetUint64(3))
 *      .Finalize()
 */
uint64_t SipHashUint256(uint64_t k0, uint64_t k1, const uint256& val);
uint64_t SipHashUint256Extra(uint64_t k0, uint64_t k1, const uint256& val, uint32_t extra);

template<typename T1>
inline uint256 RouletteHash(const T1 pbegin, const T1 pend)
{
    sph_blake512_context    ctx_blake;
    sph_bmw512_context      ctx_bmw;
    sph_cubehash512_context ctx_cubehash;
    sph_echo512_context     ctx_echo;
    sph_fugue512_context    ctx_fugue;
    sph_groestl512_context  ctx_groestl;
    sph_hamsi512_context    ctx_hamsi;
    sph_jh512_context       ctx_jh;
    sph_keccak512_context   ctx_keccak;
    sph_luffa512_context    ctx_luffa;
    sph_sha512_context      ctx_sha;
    sph_shabal512_context   ctx_shabal;
    sph_shavite512_context  ctx_shavite;
    sph_simd512_context     ctx_simd;
    sph_skein512_context    ctx_skein;
    sph_whirlpool_context   ctx_whirlpool1;

    unsigned char hash[64];  
    
    sph_sha512_init(&ctx_sha);
    sph_sha512(&ctx_sha, static_cast<const void*>(&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
    sph_sha512_close(&ctx_sha, hash);    

    for(unsigned i = 0; i < 16; i++)
    {
        unsigned hdec = hash[0] & 0xf;
        switch(hdec)
        {
            case 0:
                sph_blake512_init(&ctx_blake);   
                sph_blake512(&ctx_blake, hash, 64);
                sph_blake512_close(&ctx_blake, hash);        
                break;
            case 1:
                sph_bmw512_init(&ctx_bmw);   
                sph_bmw512(&ctx_bmw, hash, 64);
                sph_bmw512_close(&ctx_bmw, hash);        
                break;
            case 2:
                sph_cubehash512_init(&ctx_cubehash);
                sph_cubehash512(&ctx_cubehash, hash, 64);   
                sph_cubehash512_close(&ctx_cubehash, hash);  
                break;
            case 3:
                sph_echo512_init(&ctx_echo);
                sph_echo512(&ctx_echo, hash, 64);   
                sph_echo512_close(&ctx_echo, hash);    
                break;
            case 4:
                sph_fugue512_init(&ctx_fugue);
                sph_fugue512(&ctx_fugue, hash, 64);   
                sph_fugue512_close(&ctx_fugue, hash);    
                break;
            case 5:
                sph_groestl512_init(&ctx_groestl);   
                sph_groestl512(&ctx_groestl, hash, 64); 
                sph_groestl512_close(&ctx_groestl, hash);
                break;
            case 6:
                sph_hamsi512_init(&ctx_hamsi);   
                sph_hamsi512(&ctx_hamsi, hash, 64); 
                sph_hamsi512_close(&ctx_hamsi, hash);
                break;
            case 7:
                sph_jh512_init(&ctx_jh);     
                sph_jh512(&ctx_jh, hash, 64); 
                sph_jh512_close(&ctx_jh, hash);
                break;
            case 8:
                sph_keccak512_init(&ctx_keccak);     
                sph_keccak512(&ctx_keccak, hash, 64); 
                sph_keccak512_close(&ctx_keccak, hash);
                break;
            case 9:
                sph_luffa512_init(&ctx_luffa);
                sph_luffa512(&ctx_luffa, hash, 64);
                sph_luffa512_close(&ctx_luffa, hash);    
                break;
            case 10:
                sph_sha512_init(&ctx_sha);
                sph_sha512(&ctx_sha, hash, 64);
                sph_sha512_close(&ctx_sha, hash);    
                break;
            case 11:
                sph_shabal512_init(&ctx_shabal);
                sph_shabal512(&ctx_shabal, hash, 64);
                sph_shabal512_close(&ctx_shabal, hash);    
                break;
            case 12:
                sph_shavite512_init(&ctx_shavite);
                sph_shavite512(&ctx_shavite, hash, 64);   
                sph_shavite512_close(&ctx_shavite, hash);  
                break;
            case 13:
                sph_simd512_init(&ctx_simd);
                sph_simd512(&ctx_simd, hash, 64);   
                sph_simd512_close(&ctx_simd, hash); 
                break;
            case 14:
                sph_skein512_init(&ctx_skein);   
                sph_skein512(&ctx_skein, hash, 64); 
                sph_skein512_close(&ctx_skein, hash); 
                break;
            case 15:
                sph_whirlpool_init(&ctx_whirlpool1);
                sph_whirlpool(&ctx_whirlpool1, hash, 64);
                sph_whirlpool_close(&ctx_whirlpool1, hash);    
                break;
            default:
                break;
        }
    }

    return uint256(std::vector<unsigned char>(hash, hash + 32));
}

#endif // BITCOIN_HASH_H
