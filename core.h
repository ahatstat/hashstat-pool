#ifndef COINSHIELD_LLP_CORE_H
#define COINSHIELD_LLP_CORE_H


#include "types.h"
//#include "bignum.h"
#include "hash/templates.h"
#include "CBlock.h"
#include "Miner.h"
#include "MinerThread.h"


namespace LLP
{
	
	
}

namespace Core
{
	

	class ServerConnection
	{
	public:
		LLP::Miner  *CLIENT;
		int nThreads, nTimeout;
		std::vector<MinerThread*> THREADS;
		LLP::Thread_t THREAD;
		LLP::Timer    TIMER;
		LLP::Timer	StartTimer;
		std::string   IP, PORT, m_szLogin;
		std::vector<std::string> comPorts;
		bool bBenchmark = false;

		ServerConnection(std::string ip, std::string port, std::string login, std::vector<std::string> comPorts, int nMaxTimeout, bool benchmark);
		void ResetThreads();
		unsigned long long Hashes();
		void ServerThread();
		static double GetDifficulty(unsigned int nBits, int nChannel);
	};
}

#endif