//Nexus Pool Miner for Aperture Hardware

#include <iostream>
#include "boost/lexical_cast.hpp"
#include "INIReader.h"
#include "core.h"

#define PROGRAM_VERSION "0.1"
int main(int argc, char *argv[])
{
	printf("Nexus Hash Channel Pool Miner For Aperture XXX version: " PROGRAM_VERSION "\n");
	
	if(argc < 3)
	{
		printf("Too Few Arguments. The Required Arguments are 'SERVER PORT MINING_ADDRESS'\n");
		printf("Default Arguments: Connection Timeout = 10 Seconds\n");
		printf("Format for Arguments is 'SERVER PORT MINING_ADDRESS TIMEOUT TEST_MODE'\n");
		
		Sleep(10000);
		
		return 0;
	}
		
	std::string IP = argv[1];
	std::string PORT = argv[2];
	std::string LOGIN = argv[3];
	
	int nThreads = 1, nTimeout = 10;
        //use 1 thread per com port used.
	
	if(argc > 4)
		nTimeout = boost::lexical_cast<int>(argv[3]);

	bool bBenchmark = false;
	if (argc > 5)
		bBenchmark = boost::lexical_cast<bool>(argv[4]);
	
	
	//std::vector<std::string> comPorts{"/dev/ttyUSB1"};
	//std::vector<std::string> comPorts{ "COM3" };
	
	//read com ports from config.ini
	std::vector<std::string> comPorts;
	INIReader reader("config.ini");
	if (reader.ParseError() < 0) {
		std::cout << "Error loading configuration file..." << std::endl;
	}
	else
	{

		std::cout << "Loading configuration..." << std::endl;
		std::string defaultVal = "FAIL";
		//parse the config.ini file and get all the com ports
		for (int i = 0; i < 7; i++)  
		{
			std::string comPort = reader.Get("COM", "PORT" + std::to_string(i), defaultVal);
			if (comPort != defaultVal)
			{
				comPorts.push_back(comPort);
				std::cout << comPort << std::endl;
			}
		}
		
	}
        
	printf("Initializing Miner %s:%s COM Ports = %zi Timeout = %i\n", IP.c_str(), PORT.c_str(), comPorts.size(), nTimeout);
	printf("Payout Address: %s\n\n", LOGIN.c_str());
	Core::ServerConnection MINERS(IP, PORT, LOGIN, comPorts, nTimeout, bBenchmark);
	loop { Sleep(10); }
	
	return 0;
}
