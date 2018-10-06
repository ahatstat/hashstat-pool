//Nexus Solo Miner for Aperture 

#include "core.h"

unsigned int nBestHeight = 0;
unsigned int nStartTimer = 0;

unsigned int nPrimes = 0;
unsigned int nChains = 0;
unsigned int nSieves = 0;
unsigned int nBlocks = 0;


unsigned int nDifficulty   = 0;


namespace Core
{
    
   
    bool MinerThread::sendCommand(Commands command)
    {
        std::string commandStr, errStr;
        switch (command){
            case PING: 
                commandStr = "{0}";
                errStr = "Ping";
                break;
            case ENABLE_HASH:
                commandStr = "{61}";
                errStr = "Enable_Hash";
                break;
            case DISABLE_HASH:
                commandStr = "{60}";
                errStr = "Disable_Hash";
                break;
            default:
                commandStr = "{0}";
                errStr = "Ping";
                break;
        }
        
        cSerialPortConnection->writeString(commandStr);
        std::string response;
        try
        {
            response = cSerialPortConnection->readStringUntil("}");
        }
        catch(timeout_exception& e)
        { 
            //timeout reading from serial port
            std::cout << "Timeout waiting for response from serial port " + port + " from " + errStr + commandStr + " command.\n"; 
            return false;
        } 
        catch(std::exception& e)
        {
            printf("ERROR: %s\n", e.what()); 
        }
        
        if (response != "{K")
        {
            std::cout << "Unexpected response \"" + response + "\" to command " + errStr + commandStr + " from serial port " + port + "\n";
            return false;
        }
        return true;
    }
    
    bool MinerThread::sendMessage(MessageTypes messageType, std::string message)
    {
        std::string commandStr, errStr;
        switch (messageType){
            case KEY2: 
                commandStr = "{2";
                errStr = "Key2";
                break;
            case MESSAGE2:
                commandStr = "{3";
                errStr = "Message2";
                break;
        }
        
        cSerialPortConnection->writeString(commandStr + message + "}");
        std::string response;
        try
        {
            response = cSerialPortConnection->readStringUntil("}");
        }
        catch(timeout_exception& e)
        { 
            //timeout reading from serial port
            std::cout << "Timeout waiting for response from serial port " + port + " after sending " + errStr + commandStr + message + "}.\n"; 
            return false;
        } 
        catch(std::exception& e)
        {
            printf("ERROR: %s\n", e.what()); 
        }
        
        if (response != "{K")
        {
            std::cout << "Unexpected response \"" + response + "\" to message " + errStr + commandStr + message + "} from serial port " + port + "\n";
            return false;
        }
        return true;
    }
    
    bool MinerThread::ping()
    {
        return sendCommand(PING);
    }
    
    bool MinerThread::disableHashing()
    {
        return sendCommand(DISABLE_HASH);
    }
    
    bool MinerThread::enableHashing()
    {
        return sendCommand(ENABLE_HASH);
    }
    
    bool MinerThread::sendKey2()
    {
        MUTEX.lock();
        std::string key2 = cBlock.GetKey2();                   
        MUTEX.unlock();
        return sendMessage(KEY2, key2);
    }
    
    bool MinerThread::sendMessage2()
    {
        MUTEX.lock();
        std::string message2 = cBlock.GetMessage2();                   
        MUTEX.unlock();
        return sendMessage(MESSAGE2, message2);
        
    }

    /** Main Miner Thread. Bound to the class with boost. Each Miner thread manages one USB com port.**/
    /** Take your time. Try not to forget.  We never will. We're just a miner thread. **/
    void MinerThread::HashMiner()
    {
        loop
        {
			
            sleep(1);
            /** Assure that this thread stays idle when waiting for new block, or share submission. **/
            if(!fReadyToSend)
                continue;


            //printf("Height: %u\n", cBlock.nHeight);
            //printf("Channel: %u\n", cBlock.nChannel);
            //printf("Version: %u\n", cBlock.nVersion);
            //printf("Nonce: %llu\n", cBlock.nNonce);
            //printf("Bits: %u\n", cBlock.nBits);
            //printf("Hash Previous block: %s\n", cBlock.hashPrevBlock.ToString().c_str());
            //printf("Merkle: %s\n", cBlock.hashMerkleRoot.ToString().c_str());

            //printf("Hash: %s\n", cBlock.GetHash().ToString().c_str());
            std::cout << "Key2: " << cBlock.GetKey2() << "\n" << "Message2: " << cBlock.GetMessage2() << "\n";

            //serial port commands
            ping();
            disableHashing();
            sendKey2();
            sendMessage2();
            enableHashing();
            fReadyToSend = false;

            std::string response;
            //poll serial port for a found nonce
            while (!fReadyToSend)
            {
                try
                {
                    response = cSerialPortConnection->readStringUntil("}");
                    if (response.length()>1 && response.substr(1,1) == "4")
                    {
                        //We found a nonce.  Convert from hex string to uint64
                        std::istringstream converter(response.substr(3));
                        converter >> std::hex >> cBlock.nNonce;
                        //Submit the block.
                        cServerConnection->SubmitBlock(cBlock);
                        std::cout << "Found a Nonce: " << response.substr(3) << "\n";
                        disableHashing();
                        fNewBlock = true; //need a new block
                    }
                }
                catch(timeout_exception& e)
                { 
                    // Timeouts are normal.  Do nothing
                    //std::cout<<"Normal Timeout.\n";
                } 
                catch(std::exception& e)
                {
                    //Something unexpected happened
                    printf("ERROR: %s\n", e.what()); 
                }
        
                

                Sleep(1);
            }

        }
    }
	

	/** Reset the block on each of the Threads. **/
	void ServerConnection::ResetThreads()
	{
            //std::cout << "Reset Threads.\n";
		/** Reset each individual flag to tell threads to stop mining. **/
		for(int nIndex = 0; nIndex < THREADS.size(); nIndex++)
		{
                        THREADS[nIndex]->MUTEX.lock();
			THREADS[nIndex]->fNewBlock      = true;
			THREADS[nIndex]->fBlockWaiting  = false;
                        THREADS[nIndex]->MUTEX.unlock();
		}
		
		/** Reset the Block Height Timer. **/
		HEIGHT_TIMER.Reset();
	}
	
		
	/** Add a Block to the Submit Queue. **/
	void ServerConnection::SubmitBlock(CBlock cBlock)
	{
		CLIENT->SubmitBlock(cBlock.hashMerkleRoot, cBlock.nNonce);
	}
	
		
	/** Main Connection Thread. Handles all the networking to allow
		Mining threads the most performance. **/
	void ServerConnection::ServerThread()
	{
		
		/** Don't begin until all mining threads are Created. **/
		while(THREADS.size() != nThreads)
			Sleep(1);
				
				
		/** Initialize the Server Connection. **/
		CLIENT = new LLP::Miner(IP, PORT);
			
				
		/** Initialize a Timer for the Hash Meter. **/
		METER_TIMER.Start();
		HEIGHT_TIMER.Start();

		loop
		{
			try
			{
				/** Run this thread at 100 Cycles per Second. **/
				Sleep(10);
					
					
				/** Attempt with best efforts to keep the Connection Alive. **/
				if(!CLIENT->Connected() || CLIENT->Errors() || CLIENT->Timeout(nTimeout))
				{
					ResetThreads();
					
					if(!CLIENT->Connect())
						continue;
					else
					{
						CLIENT->SetChannel(1);
						CLIENT->GetHeight();
					}
				}
				
				
				/** Check the Block Height every Second. **/
				if(HEIGHT_TIMER.ElapsedMilliseconds() > 1000)
				{
					HEIGHT_TIMER.Reset();
					CLIENT->GetHeight();
				}
				
				
				/** Show the Meter every 15 Seconds. **/
				if(METER_TIMER.Elapsed() > 15)
				{
					unsigned int SecondsElapsed = (unsigned int)time(0) - nStartTimer;
					//unsigned int nElapsed = METER_TIMER.Elapsed();
					//double PPS = (double) nPrimes / nElapsed;
					//double CPS = (double) nChains / nElapsed;
					//double CSD = (double) (nBlocks * 60.0) / (SecondsElapsed / 60.0);
					
					//nPrimes = 0;
					//nChains = 0;
					
					printf("[STATS] XXX.XX MH/s | A= R= | Difficulty | %02d:%02d:%02d\n", (SecondsElapsed/3600)%60, (SecondsElapsed/60)%60, (SecondsElapsed)%60);
					METER_TIMER.Reset();	
					
					//ResetThreads();
				}
				
				
				/** Check if there is work to do for each Miner Thread. **/
				for(int nIndex = 0; nIndex < THREADS.size(); nIndex++)
				{
					/** Attempt to get a new block from the Server if Thread needs One. **/
					if(THREADS[nIndex]->fNewBlock)
					{
                                                THREADS[nIndex]->MUTEX.lock();
						CLIENT->GetBlock();
						THREADS[nIndex]->fBlockWaiting = true;
						THREADS[nIndex]->fNewBlock = false;
                                                THREADS[nIndex]->MUTEX.unlock();
					}
				}
					
				CLIENT->ReadPacket();
				if(!CLIENT->PacketComplete())
					continue;
						
				/** Handle the New Packet, and Interpret its Data. **/
				LLP::Packet PACKET = CLIENT->NewPacket();
				CLIENT->ResetPacket();
							
							
				/** Output if a Share is Accepted. **/
				if(PACKET.HEADER == CLIENT->GOOD)
				{
					nBlocks++;
					printf("[MASTER] Nexus : Block ACCEPTED\n");
					
					ResetThreads();
				}
					
					
				/** Output if a Share is Rejected. **/
				else if(PACKET.HEADER == CLIENT->FAIL) 
				{
					printf("[MASTER] Nexus : Block REJECTED\n");
					
					ResetThreads();
				}
					
				/** Reset the Threads if a New Block came in. **/
				else if(PACKET.HEADER == CLIENT->BLOCK_HEIGHT)
				{
					unsigned int nHeight = bytes2uint(PACKET.DATA);
					if(nHeight > nBestHeight)
					{
						nBestHeight = nHeight;
						printf("[MASTER] Nexus : New Block %u.\n", nBestHeight);
							
						ResetThreads();
					}
				}
					
					
				/** Set the Block for the Thread if there is a New Block Packet. **/
				else if(PACKET.HEADER == CLIENT->BLOCK_DATA)
				{
					/** Search for a Thread waiting for a New Block to Supply its need. **/
					for(int nIndex = 0; nIndex < THREADS.size(); nIndex++)
					{
						if(THREADS[nIndex]->fBlockWaiting)
						{
							THREADS[nIndex]->MUTEX.lock();
							THREADS[nIndex]->cBlock.nVersion      = bytes2uint(std::vector<unsigned char>(PACKET.DATA.begin(), PACKET.DATA.begin() + 4));
							
							THREADS[nIndex]->cBlock.hashPrevBlock.SetBytes (std::vector<unsigned char>(PACKET.DATA.begin() + 4, PACKET.DATA.begin() + 132));
							THREADS[nIndex]->cBlock.hashMerkleRoot.SetBytes(std::vector<unsigned char>(PACKET.DATA.begin() + 132, PACKET.DATA.end() - 20));
							
							THREADS[nIndex]->cBlock.nChannel      = bytes2uint(std::vector<unsigned char>(PACKET.DATA.end() - 20, PACKET.DATA.end() - 16));
							THREADS[nIndex]->cBlock.nHeight       = bytes2uint(std::vector<unsigned char>(PACKET.DATA.end() - 16, PACKET.DATA.end() - 12));
							THREADS[nIndex]->cBlock.nBits         = bytes2uint(std::vector<unsigned char>(PACKET.DATA.end() - 12,  PACKET.DATA.end() - 8));
							//THREADS[nIndex]->cBlock.nNonce        = bytes2uint64(std::vector<unsigned char>(PACKET.DATA.end() - 8,  PACKET.DATA.end()));
							
                                                        
                                                        //set the starting nonce for each com port.  The default from the wallet is 1. We will use zero.
                                                        THREADS[nIndex]->cBlock.nNonce        = 0 + nIndex*exp2(52);
                                                        
                                                        //THREADS[nIndex]->cBlock.nVersion      = 4;
							//reverse the byte order from nxsorbitalscan
				//			THREADS[nIndex]->cBlock.hashPrevBlock.SetHex ("a793e4310d6957c758f287f462bebfbb562d25d3d8d79716a53304272d76faa68a09fc5e3a2d0005d55b1b651f401b9f482456f6c421512daf55f2f670135d02a544fad7631e4b715b0013dfc8968ee60898e8b50d8dda813e45e5e0186a3aed9e6f1d1162673e62fe393f0e9b4698c705cf93d5ca009ba2d201635402090000");
				//			THREADS[nIndex]->cBlock.hashMerkleRoot.SetHex("25303a6d34a2a89e91ecb814f50d16a62944417c4322b45038c79422419ad45f90bef6ebbe075baa0450f788d03e1940316ac2434f1cfd30cd0742fc58a4f531");
							//THREADS[nIndex]->cBlock.hashPrevBlock.SetHex ("00000902546301d2a29b00cad593cf05c798469b0e3f39fe623e6762111d6f9eed3a6a18e0e5453e81da8d0db5e89808e68e96c8df13005b714b1e63d7fa44a5025d1370f6f255af2d5121c4f65624489f1b401f651b5bd505002d3a5efc098aa6fa762d270433a51697d7d8d3252d56bbbfbe62f487f258c757690d31e493a7");
							//THREADS[nIndex]->cBlock.hashMerkleRoot.SetHex("31f5a458fc4207cd30fd1c4f43c26a3140193ed088f75004aa5b07beebf6be905fd49a412294c73850b422437c414429a6160df514b8ec919ea8a2346d3a3025");
							
                                                        
                                                        
							//THREADS[nIndex]->cBlock.nChannel      = 2;
							//THREADS[nIndex]->cBlock.nHeight       = 2023276;
							//THREADS[nIndex]->cBlock.nBits         = 0x7b032ed8;
							//THREADS[nIndex]->cBlock.nNonce        = 21155560019;
							
                                                        
                                                        
                                                        
                                                        
							
							if(THREADS[nIndex]->cBlock.nHeight < nBestHeight)
							{
								printf("[MASTER] Received Obsolete Block %u... Requesting New Block.\n", THREADS[nIndex]->cBlock.nHeight);
								CLIENT->GetBlock();
									
								break;
							}
							
							/** Set the Difficulty from most recent Block Received. **/
							nDifficulty = THREADS[nIndex]->cBlock.nBits;
								
							printf("[MASTER] Block %s Height = %u Received on Thread %u\n", THREADS[nIndex]->cBlock.hashMerkleRoot.ToString().substr(0, 20).c_str(), THREADS[nIndex]->cBlock.nHeight, nIndex);
							THREADS[nIndex]->fBlockWaiting = false;
                                                        THREADS[nIndex]->fReadyToSend = true;
                                                        THREADS[nIndex]->MUTEX.unlock();
								
							break;
						}
					}
				}
					
			}
			catch(std::exception& e)
			{
				printf("%s\n", e.what()); CLIENT = new LLP::Miner(IP, PORT); 
			}
		}
	}

}

int main(int argc, char *argv[])
{

	if(argc < 3)
	{
		printf("Too Few Arguments. The Required Arguments are 'IP PORT'\n");
		printf("Default Arguments: Connection Timeout = 10 Seconds\n");
		printf("Format for Arguments is 'IP PORT TIMEOUT'\n");
		
		Sleep(10000);
		
		return 0;
	}
		
	std::string IP = argv[1];
	std::string PORT = argv[2];
	
	int nThreads = 1, nTimeout = 10;
        //use 1 thread per com port used.
	
	if(argc > 3)
		nTimeout = boost::lexical_cast<int>(argv[3]);
	
	printf("Nexus Hash Channel Solo Miner For Aperture XXX v0.1\n");
	//Sleep(2000);
        //Todo: 
        //Read config file for desired com ports and/or perform auto detect for connected devices
        // for now hardcode the list of com ports
	std::vector<std::string> comPorts{"/dev/ttyUSB0"};
        
	printf("Initializing Miner %s:%s Threads = %i Timeout = %i\n", IP.c_str(), PORT.c_str(), nThreads, nTimeout);
	
	nStartTimer = (unsigned int)time(0);
	
	Core::ServerConnection MINERS(IP, PORT, comPorts, nTimeout);
	loop { Sleep(10); }
	
	return 0;
}
