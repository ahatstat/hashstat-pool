#pragma once

#include <iostream>
#include "TimeoutSerial.h"

class ApertureSerialComm
{
public:

	std::string port;
	unsigned baud;

	//each thread has its own com port connection
	TimeoutSerial cTimeOutSerialConnection;

	enum Commands { PING, ENABLE_HASH, DISABLE_HASH };
	enum ReadRegisters { GET_DIFF28_HASH_COUNT };
	enum WriteRegisters { BITS };
	enum MessageTypes { KEY2, MESSAGE2 };
	bool sendCommand(Commands command);
	std::string readReg(ReadRegisters readReg);
	bool writeReg(WriteRegisters writeReg, std::string regValue);
	bool setDifficulty(unsigned int diff);
	unsigned int getDiff28HashCount();
	bool sendMessage(MessageTypes messageType, std::string message);
	bool ping();
	bool enableHashing();
	bool disableHashing();

	//constructor
	ApertureSerialComm(std::string comPortName, unsigned int baud);

	


	
};

