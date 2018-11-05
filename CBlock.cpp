#include "CBlock.h"
#include "util_llh.h"
#include <iostream>

namespace Core
{
	CBlock::CBlock()
	{
		this->m_unVersion		= 0;
		this->m_hashPrevBlock	= 0;
		this->m_hashMerkleRoot	= 0;
		this->m_unHeight		= 0;
		this->m_unBits			= 0;
		this->m_ullNonce		= 0;
	}

	CBlock::CBlock(const CBlock& block)
	{
		this->m_unVersion		= block.GetVersion();
		this->m_hashPrevBlock	= block.GetPrevBlock();
		this->m_hashMerkleRoot	= block.GetMerkleRoot();
		this->m_unHeight		= block.GetHeight();
		this->m_unBits			= block.GetBits();
		this->m_ullNonce		= block.GetNonce();
	}

	CBlock& CBlock::operator=(const CBlock& block)
	{
		this->m_unVersion		= block.GetVersion();
		this->m_hashPrevBlock	= block.GetPrevBlock();
		this->m_hashMerkleRoot	= block.GetMerkleRoot();
		this->m_unHeight		= block.GetHeight();
		this->m_unBits			= block.GetBits();
		this->m_ullNonce		= block.GetNonce();

		return *this;
	}

	CBlock::~CBlock()
	{
	}

	void CBlock::printBlock()
	{
		std::cout << "Height: " << GetHeight() << std::endl;
		std::cout << "Version: " << GetVersion() << std::endl;
		std::cout << "Bits: " << std::hex << GetBits() << " -> Difficulty: " << std::dec << GetDifficulty(GetBits()) << std::endl;
		std::cout << "Nonce: " << GetNonce() << std::endl;
		std::cout << "Prev Block Hash: " << GetPrevBlock().ToString() << std::endl;
		std::cout << "Merkle Root: " << GetMerkleRoot().ToString() << std::endl;
	}

	uint1024 CBlock::GetHash()
	{ 
		return SK1024(BEGIN(m_unVersion), END(m_ullNonce)); 
	}
	const unsigned int* CBlock::GetData()
	{
		static unsigned char pblank[1];
		unsigned char* pbegin= (unsigned char *)&m_unVersion;
		unsigned char* pend = (unsigned char *)&m_ullNonce;
		u08b_t *msg = (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]);
		unsigned int * pData = (unsigned int*)msg;
        return pData;
	}

	const std::string CBlock::GetKey2()
	//Get the Key IV for input to the miner
	{
		return SKEINKEY2(BEGIN(m_unVersion), END(m_ullNonce));
	}

	const std::string CBlock::GetMessage2()
	//get the portion of the message IV used as input to the miner
	{
		return SKEINMESSAGE2(BEGIN(m_unVersion), END(m_ullNonce));
	}

	double GetDifficulty(unsigned int nBits, int nChannel)
	{
		/** Prime Channel is just Decimal Held in Integer
		Multiplied and Divided by Significant Digits. **/
		if (nChannel == 1)
			return nBits / 10000000.0;

		/** Get the Proportion of the Bits First. **/
		double dDiff =
			(double)0x0000ffff / (double)(nBits & 0x00ffffff);

		/** Calculate where on Compact Scale Difficulty is. **/
		int nShift = nBits >> 24;

		/** Shift down if Position on Compact Scale is above 124. **/
		while (nShift > 124)
		{
			dDiff = dDiff / 256.0;
			nShift--;
		}

		/** Shift up if Position on Compact Scale is below 124. **/
		while (nShift < 124)
		{
			dDiff = dDiff * 256.0;
			nShift++;
		}

		/** Offset the number by 64 to give larger starting reference. **/
		return dDiff * ((nChannel == 2) ? 64 : 1024 * 1024 * 256);
	}

	double GetDifficultyLog2(double diff)
	{
		return 34 + log2(diff);
	}
	
}