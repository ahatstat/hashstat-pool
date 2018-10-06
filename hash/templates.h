/*******************************************************************************************
 
			Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++
   
 [Learn, Create, but do not Forge] Viz. http://www.opensource.org/licenses/mit-license.php
  
*******************************************************************************************/

#ifndef COINSHIELD_TEMPLATES_H
#define COINSHIELD_TEMPLATES_H

#include "uint1024.h"
#include "skein.h"
#include "KeccakHash.h"

/** Hashing template for Checksums **/
template<typename T1>
inline uint64 SK64(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1];
	
    uint64 skein;
	Skein_256_Ctxt_t ctx;
	Skein_256_Init  (&ctx, 64);
	Skein_256_Update(&ctx, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
	Skein_256_Final (&ctx, (unsigned char *)&skein);
	
    uint64 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize(&ctx_keccak, 1344, 256, 64, 0x06);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 64);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

/** Hashing template for Address Generation **/
inline uint256 SK256(const std::vector<unsigned char>& vch)
{
	//static unsigned char pblank[1];
	
    uint256 skein;
	Skein_256_Ctxt_t ctx;
	Skein_256_Init(&ctx, 256);
	Skein_256_Update(&ctx, (unsigned char *)&vch[0], vch.size());
	Skein_256_Final(&ctx, (unsigned char *)&skein);
	
    uint256 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize_SHA3_256(&ctx_keccak);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 256);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

/** Hashing template for Address Generation **/
template<typename T1>
inline uint256 SK256(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1];
	
    uint256 skein;
	Skein_256_Ctxt_t ctx;
	Skein_256_Init  (&ctx, 256);
	Skein_256_Update(&ctx, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
	Skein_256_Final (&ctx, (unsigned char *)&skein);
	
    uint256 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize_SHA3_256(&ctx_keccak);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 256);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

/** Hashing template for TX hash **/
template<typename T1>
inline uint512 SK512(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1];
	
    uint512 skein;
	Skein_512_Ctxt_t ctx;
	Skein_512_Init  (&ctx, 512);
	Skein_512_Update(&ctx, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
	Skein_512_Final (&ctx, (unsigned char *)&skein);
	
    uint512 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize_SHA3_512(&ctx_keccak);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 512);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

/** Hashing template for TX hash **/
template<typename T1, typename T2>
inline uint512 SK512(const T1 p1begin, const T1 p1end,
                    const T2 p2begin, const T2 p2end)
{
	static unsigned char pblank[1];
	
    uint512 skein;
	Skein_512_Ctxt_t ctx;
	Skein_512_Init  (&ctx, 512);
	Skein_512_Update(&ctx, (p1begin == p1end ? pblank : (unsigned char*)&p1begin[0]), (p1end - p1begin) * sizeof(p1begin[0]));
    Skein_512_Update(&ctx, (p2begin == p2end ? pblank : (unsigned char*)&p2begin[0]), (p2end - p2begin) * sizeof(p2begin[0]));
	Skein_512_Final (&ctx, (unsigned char *)&skein);
	
    uint512 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize_SHA3_512(&ctx_keccak);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 512);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

/** Hashing template for TX hash **/
template<typename T1, typename T2, typename T3>
inline uint512 SK512(const T1 p1begin, const T1 p1end,
                    const T2 p2begin, const T2 p2end,
                    const T3 p3begin, const T3 p3end)
{
	static unsigned char pblank[1];
	
    uint512 skein;
	Skein_512_Ctxt_t ctx;
	Skein_512_Init  (&ctx, 512);
	Skein_512_Update(&ctx, (p1begin == p1end ? pblank : (unsigned char*)&p1begin[0]), (p1end - p1begin) * sizeof(p1begin[0]));
    Skein_512_Update(&ctx, (p2begin == p2end ? pblank : (unsigned char*)&p2begin[0]), (p2end - p2begin) * sizeof(p2begin[0]));
    Skein_512_Update(&ctx, (p3begin == p3end ? pblank : (unsigned char*)&p3begin[0]), (p3end - p3begin) * sizeof(p3begin[0]));
	Skein_512_Final (&ctx, (unsigned char *)&skein);
	
    uint512 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize_SHA3_512(&ctx_keccak);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 512);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

/** Hashing template used for Private Keys **/
template<typename T1>
inline uint576 SK576(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1];
	
    uint576 skein;
	Skein1024_Ctxt_t ctx;
	Skein1024_Init(&ctx, 576);
	Skein1024_Update(&ctx, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
	Skein1024_Final(&ctx, (unsigned char *)&skein);
	
    uint576 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize(&ctx_keccak, 1024, 576, 576, 0x06);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 576);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
	return keccak;
}

/** Hashing template used to build Block Hashes **/
template<typename T1>
inline uint1024 SK1024(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1];
	
    uint1024 skein;
	Skein1024_Ctxt_t ctx;
	Skein1024_Init(&ctx, 1024);
	Skein1024_Update(&ctx, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
	Skein1024_Final(&ctx, (unsigned char *)&skein);
	
	uint1024 keccak;
	Keccak_HashInstance ctx_keccak;
	Keccak_HashInitialize(&ctx_keccak, 576, 1024, 1024, 0x05);
	Keccak_HashUpdate(&ctx_keccak, (unsigned char *)&skein, 1024);
	Keccak_HashFinal(&ctx_keccak, (unsigned char *)&keccak);
	
    return keccak;
}

//convert a number to a hex string
template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I)<<1) {
    static const char* digits = "0123456789ABCDEF";
    std::string rc(hex_len,'0');
    for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
        rc[i] = digits[(w>>j) & 0x0f];
    return rc;
}

/** Hashing template to get the key2 intermediate value **/
template<typename T1>
inline std::string SKEINKEY2(const T1 pbegin, const T1 pend)
{
    static unsigned char pblank[1];

    Skein1024_Ctxt_t ctx;
    Skein1024_Init(&ctx, 1024); //initialize the state using precalculated value
    //perform the 2nd round of skein
    Skein1024_Update(&ctx, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
    
    std::string key2; 
    unsigned int i;
    for (i=0; i<SKEIN1024_STATE_WORDS; i++)
    {
        //convert intermediate result to string.  
        key2.append(n2hexstr(ctx.X[i]));
    }
    //calculate the final word in the key
    uint64 finalWord;
    finalWord = ctx.X[ 0] ^ ctx.X[ 1] ^ ctx.X[ 2] ^ ctx.X[ 3] ^
                 ctx.X[ 4] ^ ctx.X[ 5] ^ ctx.X[ 6] ^ ctx.X[ 7] ^
                 ctx.X[ 8] ^ ctx.X[ 9] ^ ctx.X[10] ^ ctx.X[11] ^
                 ctx.X[12] ^ ctx.X[13] ^ ctx.X[14] ^ ctx.X[15] ^ SKEIN_KS_PARITY;
    
    key2.append(n2hexstr(finalWord));
    return key2;
    
}


/** Hashing template to get message to send to the miner **/
template<typename T1>
inline std::string SKEINMESSAGE2(const T1 pbegin, const T1 pend)
{
    static unsigned char pblank[1];

    Skein1024_Ctxt_t ctx;
    Skein1024_Init(&ctx, 1024); //initialize the state using precalculated value
    //perform the 2nd round of skein
    Skein1024_Update(&ctx, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]));
    
    constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    
    //initialize to all zeros
    std::string s(SKEIN1024_BLOCK_BYTES * 2, '0');
    
    int j, wordIndex;
    //rearrange the byte order and convert to ascii characters
    //only the first 88 bytes are valid. 
    for (int i = 0; i < ctx.h.bCnt; ++i) {
        wordIndex = i/8; //8 bytes per 64 bit word 
        j = 7-(i%8);
        s[2 * i]     = hexmap[(ctx.b[wordIndex*8+j] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[ctx.b[wordIndex*8+j] & 0x0F];
    }
    
    return s;
}


#endif
