#ifndef COINSHIELD_LLP_CORE_H
#define COINSHIELD_LLP_CORE_H

#include "types.h"
#include "TimeoutSerial.h"
#include <queue>


namespace LLP
{
	class Outbound : public Connection
	{
            std::string IP;
            std::string PORT;
            Service_t IO_SERVICE;
		
		
	public:
		/** Outgoing Client Connection Constructor **/
		Outbound(std::string ip, std::string port) : Connection(), IP(ip), PORT(port) { }
		
		bool Connect()
		{
			try
			{
				using boost::asio::ip::tcp;
				
				tcp::resolver 			  RESOLVER(IO_SERVICE);
				tcp::resolver::query      QUERY   (tcp::v4(), IP.c_str(), PORT.c_str());
				tcp::resolver::iterator   ADDRESS = RESOLVER.resolve(QUERY);
				
				this->SOCKET = Socket_t(new tcp::socket(IO_SERVICE));
				this->SOCKET -> connect(*ADDRESS, this->ERROR_HANDLE);
				
				if(Errors())
				{
					this->Disconnect();
					
					printf("Failed to Connect to Mining LLP Server...\n");
					return false;
				}
				
				this->CONNECTED = true;
				this->TIMER.Start();
				
				printf("Connected to %s:%s...\n", IP.c_str(), PORT.c_str());

				return true;
			}
			catch(...){ }
			
			this->CONNECTED = false;
			return false;
		}
		
	};
	
	
	class Miner : public Outbound
	{
	public:
		Miner(std::string ip, std::string port) : Outbound(ip, port){}
		
		enum
		{
			/** DATA PACKETS **/
			BLOCK_DATA   = 0,
			SUBMIT_BLOCK = 1,
			BLOCK_HEIGHT = 2,
			SET_CHANNEL  = 3,
					
			/** REQUEST PACKETS **/
			GET_BLOCK    = 129,
			GET_HEIGHT   = 130,
			
			/** RESPONSE PACKETS **/
			GOOD     = 200,
			FAIL     = 201,
					
			/** GENERIC **/
			PING     = 253,
			CLOSE    = 254
		};
		
		
		/** Current Newly Read Packet Access. **/
		inline Packet NewPacket() { return this->INCOMING; }
		
		
		/** Create a Packet with the Given Header. **/
		inline Packet GetPacket(unsigned char HEADER)
		{
			Packet PACKET;
			PACKET.HEADER = HEADER;
			
			return PACKET;
		}
		
		
		/** Get a new Block from the Pool Server. **/
		inline void GetBlock()    { this -> WritePacket(GetPacket(GET_BLOCK));   }
		
		
		/** Get the Current Height of the Coinshield Network. **/
		inline void GetHeight()    { this -> WritePacket(GetPacket(GET_HEIGHT)); }
		
		
		/** Set the Current Channel to Mine for in the LLP. **/
		inline void SetChannel(unsigned int nChannel)
		{
			Packet PACKET = GetPacket(SET_CHANNEL);
			PACKET.LENGTH = 4;
			PACKET.DATA   = uint2bytes(nChannel);
			
			this -> WritePacket(PACKET);  
		}
		
		
		/** Submit a Block to the Coinshield Network. **/
		inline void SubmitBlock(uint512 hashMerkleRoot, uint64 nNonce)
		{
			Packet PACKET = GetPacket(SUBMIT_BLOCK);
			PACKET.DATA = hashMerkleRoot.GetBytes();
			std::vector<unsigned char> NONCE  = uint2bytes64(nNonce);
			
			PACKET.DATA.insert(PACKET.DATA.end(), NONCE.begin(), NONCE.end());
			PACKET.LENGTH = 72;
			
			this -> WritePacket(PACKET);
		}
	};
	
}

namespace Core
{
	class ServerConnection;
	
        /*
	extern unsigned int *primes;
	extern unsigned int *inverses;

	extern unsigned int nBitArray_Size;
	extern mpz_t  zPrimorial;

	extern unsigned int prime_limit;
	extern unsigned int nPrimeLimit;
	extern unsigned int nPrimorialEndPrime;

	extern uint64 octuplet_origins[];
	
	void InitializePrimes();
	unsigned int SetBits(double nDiff);
	double GetPrimeDifficulty(CBigNum prime, int checks, std::vector<unsigned int>& vPrimes);
	double GetSieveDifficulty(CBigNum next, unsigned int clusterSize);
	unsigned int GetPrimeBits(CBigNum prime, int checks, std::vector<unsigned int>& vPrimes);
	unsigned int GetFractionalDifficulty(CBigNum composite);
	std::vector<unsigned int> Eratosthenes(int nSieveSize);
	bool DivisorCheck(CBigNum test);
	unsigned long PrimeSieve(CBigNum BaseHash, unsigned int nDifficulty, unsigned int nHeight);
	bool PrimeCheck(CBigNum test, int checks);
	CBigNum FermatTest(CBigNum n, CBigNum a);
	bool Miller_Rabin(CBigNum n, int checks);
	*/
	
	class CBlock
	{
	public:

		/** Begin of Header.   BEGIN(nVersion) **/
		unsigned int  nVersion;
		uint1024 hashPrevBlock;
		uint512 hashMerkleRoot;
		unsigned int  nChannel;
		unsigned int   nHeight;
		unsigned int     nBits;
		uint64          nNonce;
		/** End of Header.     END(nNonce). 
			All the components to build an SK1024 Block Hash. **/
			
		CBlock()
		{
			nVersion       = 0;
			hashPrevBlock  = 0;
			hashMerkleRoot = 0;
			nChannel       = 0;
			nHeight        = 0;
			nBits          = 0;
			nNonce         = 0;
		}
			
		uint1024 GetHash() const
		{
			//return SK1024(BEGIN(nVersion), END(nBits));  //for prime channel
                    return SK1024(BEGIN(nVersion), END(nNonce)); // for hash channel
		}
                
                std::string GetKey2() const
		{
                    return SKEINKEY2(BEGIN(nVersion), END(nNonce));
		}
                
                std::string GetMessage2() const
		{
                    return SKEINMESSAGE2(BEGIN(nVersion), END(nNonce));
		}
               
                
                
		
		
	};
	
	
	
	/** Class to hold the basic data a Miner will use to build a Block.
		Used to allow one Wallet Connection for any amount of threads. **/
	class MinerThread
	{
	public:
            ServerConnection* cServerConnection;
            std::string port;
            unsigned baud;

            //each thread has its own com port connection
            TimeoutSerial* cSerialPortConnection;

            CBlock cBlock;
            unsigned int nDifficulty;

            bool fNewBlock, fBlockWaiting, fReadyToSend;
            LLP::Thread_t THREAD;
            boost::mutex MUTEX;

            MinerThread(ServerConnection* cConnection, std::string port, unsigned baud) 
                : cServerConnection(cConnection), port(port), baud(baud),
                fNewBlock(true), fBlockWaiting(false), fReadyToSend(false),
                THREAD(boost::bind(&MinerThread::HashMiner, this))  
            {
                cSerialPortConnection = new TimeoutSerial();
                cSerialPortConnection->open(port, baud);
                //set serial port timeout
                cSerialPortConnection->setTimeout(boost::posix_time::seconds(1));
            }
            
            enum Commands {PING, ENABLE_HASH, DISABLE_HASH};
            enum MessageTypes {KEY2, MESSAGE2};
            void HashMiner();
            bool sendCommand(Commands command);
            bool sendMessage(MessageTypes messageType, std::string message);
            bool ping();
            bool enableHashing();
            bool disableHashing();
            bool sendKey2();
            bool sendMessage2();
            bool readMessage();
                
        private:
            
               
	};
	
		/** Class to handle all the Connections via Mining LLP.
		Independent of Mining Threads for Higher Efficiency. **/
	class ServerConnection
	{
	public:
		LLP::Miner* CLIENT;
                std::string   IP, PORT;
                LLP::Timer    METER_TIMER;
		LLP::Timer    HEIGHT_TIMER;
                std::vector<std::string> comPorts;
		int nThreads, nTimeout;
		std::vector<MinerThread*> THREADS;
		LLP::Thread_t THREAD;
		
		
                
		
		boost::mutex    SUBMIT_MUTEX;

		std::queue<CBlock> SUBMIT_QUEUE;
		std::queue<CBlock> RESPONSE_QUEUE;
		
		ServerConnection(std::string ip, std::string port, std::vector<std::string> comPorts, int nMaxTimeout) 
                    : IP(ip), PORT(port), METER_TIMER(), HEIGHT_TIMER(), comPorts(comPorts), nThreads(static_cast<int>(comPorts.size())), nTimeout(nMaxTimeout), THREAD(boost::bind(&ServerConnection::ServerThread, this))
              
		{
			for(int nIndex = 0; nIndex < nThreads; nIndex++)
				THREADS.push_back(new MinerThread(this, comPorts[nIndex], 115200));
		}
		
		/** Reset the block on each of the Threads. **/
		void ResetThreads();
		void SubmitBlock(CBlock cBlock);
		void ServerThread();

	};
}



#endif
