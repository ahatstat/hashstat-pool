// Take your time. Try not to forget.  We never will. We're just a miner thread. 

#include "MinerThread.h"
#include "CBlock.h"
#include "hash/templates.h"	
#include "ApertureSerialComm.h"
#include "util_llh.h"

namespace Core
{
	MinerThread::MinerThread(unsigned int pData, std::string comPort, int baudRate)
	{
		m_unNonceCount = 0;
		m_unNonces[4] = {0};
		m_unBits = 0;
		m_pBLOCK = NULL;
		m_ThreadID = pData;
		m_bBlockFound =false; 
		m_unHashes = 0;
		m_pTHREAD = boost::thread(&MinerThread::SK1024Miner, this);
		total_mhashes_done = 0;
		m_nThroughput = 0;
		m_bBenchmark = false;
		m_bShutdown = false;
		m_sComPort = comPort;
		m_nBaudRate = baudRate;

	}

	MinerThread::MinerThread(const MinerThread& miner) 
	{
		m_pBLOCK = miner.GetBlock();
		m_ThreadID = miner.GetThreadID();
		m_bBlockFound = miner.GetIsBlockFound(); 
		m_bNewBlock = miner.GetIsNewBlock();
		m_bReady = miner.GetIsReady();
		m_unHashes = miner.GetHashes();
		m_nThroughput = miner.GetThroughput();
		m_sComPort = miner.GetComPort();
		m_nBaudRate = miner.GetBaudRate();
		m_pTHREAD = boost::thread(&MinerThread::SK1024Miner, this);
	}

	MinerThread& MinerThread::operator=(const MinerThread& miner)
	{
		m_pBLOCK = miner.GetBlock();
		m_ThreadID = miner.GetThreadID();
		m_bBlockFound = miner.GetIsBlockFound(); 
		m_bNewBlock = miner.GetIsNewBlock();
		m_bReady = miner.GetIsReady();
		m_unHashes = miner.GetHashes();
		m_nThroughput = miner.GetThroughput();
		m_sComPort = miner.GetComPort();
		m_nBaudRate = miner.GetBaudRate();
		m_pTHREAD = boost::thread(&MinerThread::SK1024Miner, this);

		return *this;
	}

	MinerThread::~MinerThread()
	{
	}


	//This is the main thread loop.
	void MinerThread::SK1024Miner()
	{
#ifdef WIN32
		SetThreadPriority(GetCurrentThread(), 2);
#endif
		//create serial port object.  This opens the serial port
		ApertureSerialComm com(m_sComPort, m_nBaudRate);
		com.disableHashing();

		//lambda convenience functions
		//send Key2 via com port
		auto sendKey2 = [&]() { 
			m_clLock.lock();
			std::string key2 = m_pBLOCK->GetKey2();
			m_clLock.unlock();
			return com.sendMessage(ApertureSerialComm::MessageTypes::KEY2, key2);
		};

		//send Message2 via com port
		auto sendMessage2 = [&]() {
			m_clLock.lock();
			std::string message2 = m_pBLOCK->GetMessage2();
			m_clLock.unlock();
			return com.sendMessage(ApertureSerialComm::MessageTypes::MESSAGE2, message2);
		};

		loop
		{
			try
			{
				if (m_bShutdown)
					break;

				/** Don't mine if the Connection Thread is Submitting Block or Receiving New. **/
				while(m_bNewBlock || m_bBlockFound || !m_pBLOCK)
					Sleep(10);					
		

				if (m_bBenchmark == true)
				{
					printf("\nBenchmark Mode - Should find a TEST block every 2^(32) hashes \"On Average\"\n\n");
					m_pBLOCK->SetBits(0x7d00ffff); //32 bits
					//target.SetCompact(0x7d00ffff); //32-bits
				}
				unsigned nHeight = m_pBLOCK->GetHeight();

				//std::cout << "Key2: " << m_pBLOCK->GetKey2() << "\n" << "Message2: " << m_pBLOCK->GetMessage2() << "\n";
				//serial port commands
				com.ping();
				m_unHashes += com.getDiff28HashCount(); //increment the hashcount since last block
				com.disableHashing();
				sendKey2();
				sendMessage2();
				com.setDifficulty(m_unBits);
				com.enableHashing();

				std::string response;
				//poll serial port for a found nonce
				uint64_t nNonce;

				while(!m_bNewBlock && nHeight == m_pBLOCK->GetHeight())
				{					
					if (m_bShutdown)
						break;

					try
					{
						response = com.cTimeOutSerialConnection.readStringUntil("}");
						if (response.length() > 1 && response.substr(1, 1) == "4")
						{
							//We found a nonce.
							//Convert from hex string to uint64
							std::istringstream converter(response.substr(3));
							converter >> std::hex >> nNonce;
							//Submit the block.
							m_clLock.lock();
							m_pBLOCK->SetNonce(nNonce);
							m_bBlockFound = true;
							m_clLock.unlock();
							//disable hashing
							//com.disableHashing();
							std::cout << "Found a Nonce: " << response.substr(3) << "\n";
							//m_bNewBlock = true; //need a new block
							break;
							
						}
					}
					catch (timeout_exception& e)
					{
						// Timeouts are normal.  Do nothing
						std::cout<<".";
					}
					catch (std::exception& e)
					{
						//Something unexpected happened
						printf("ERROR: %s\n", e.what());
					}
					
					Sleep(1);
					
				}
				
			}
			catch(std::exception& e)
			{ 
				printf("ERROR: %s\n", e.what()); 
			}
		}
	}


}