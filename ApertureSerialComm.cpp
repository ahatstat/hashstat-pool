#include "ApertureSerialComm.h"
#include <iostream>
#include <iomanip>



ApertureSerialComm::ApertureSerialComm(std::string comPortName, unsigned int baud) : port(comPortName), baud(baud)
{
	//set the serial port baud rate and open the port
	cTimeOutSerialConnection.open(comPortName, baud);
	//set serial port timeout
	cTimeOutSerialConnection.setTimeout(boost::posix_time::milliseconds(500));
}


bool ApertureSerialComm::sendCommand(Commands command)
{
	std::string commandStr, errStr;
	switch (command) {
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

	cTimeOutSerialConnection.writeString(commandStr);
	std::string response;
	try
	{
		response = cTimeOutSerialConnection.readStringUntil("}");
	}
	catch (timeout_exception& e)
	{
		//timeout reading from serial port
		std::cout << "Timeout waiting for response from serial port " + port + " from " + errStr + commandStr + " command.\n";
		return false;
	}
	catch (std::exception& e)
	{
		printf("ERROR: %s\n", e.what());
		return false;
	}

	if (response != "{K")
	{
		std::cout << "Unexpected response \"" + response + "\" to command " + errStr + commandStr + " from serial port " + port + "\n";
		return false;
	}
	return true;
}

bool ApertureSerialComm::writeReg(WriteRegisters writeReg, std::string regValue)
{
	std::string regStr, errStr;
	switch (writeReg) {
	case BITS:
		regStr = "{9" + regValue + "}";
		errStr = "BITS";
		break;

	default:
		regStr = "{0}";
		errStr = "Ping";
		break;
	}

	cTimeOutSerialConnection.writeString(regStr);
	std::string response;
	try
	{
		response = cTimeOutSerialConnection.readStringUntil("}");
	}
	catch (timeout_exception& e)
	{
		//timeout reading from serial port
		std::cout << "Timeout waiting for response from serial port " + port + " after writing register " + errStr + " " + regStr + "\n";
		return false;
	}
	catch (std::exception& e)
	{
		printf("ERROR: %s\n", e.what());
		return false;
	}

	if (response != "{K")
	{
		std::cout << "Unexpected response \"" + response + "\" after writing register " + errStr + " " + regStr + " from serial port " + port + "\n";
		return false;
	}
	return true;
}

std::string ApertureSerialComm::readReg(ReadRegisters readReg)
{
	std::string regStr, errStr;
	switch (readReg) {
	case GET_DIFF28_HASH_COUNT:
		regStr = "{7}";
		errStr = "GET_DIFF28_HASH_COUNT";
		break;
	default:
		regStr = "";
		errStr = "Error";
		break;
	}


	cTimeOutSerialConnection.writeString(regStr);
	std::string response;
	try
	{
		response = cTimeOutSerialConnection.readStringUntil("}");
	}
	catch (timeout_exception& e)
	{
		//timeout reading from serial port
		std::cout << "Timeout waiting for response from serial port " + port + " from " + errStr + regStr + " command.\n";
		return "";
	}
	catch (std::exception& e)
	{
		printf("ERROR: %s\n", e.what());
		return "";
	}

	if (response.length() == 0)
	{
		std::cout << "Unexpected empty response to register read " + errStr + regStr + " from serial port " + port + "\n";
		return "";
	}

	if (response.substr(0,1) != "{")
	{
		std::cout << "Unexpected response \"" + response + "\" to register read " + errStr + regStr + " from serial port " + port + "\n";
		return "";
	}
	return response.substr(1); //strip off the leading '}'
}

bool ApertureSerialComm::setDifficulty(unsigned int bits)
//set the target difficulty of the miner
{
	//convert bits to a hex string.
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(8);  //string must be 8 characters.
	stream << std::uppercase << std::hex << bits;  //hex string must be uppercase
	std::string strBits(stream.str());

	bool success = writeReg(BITS, strBits);

	return success;

}

unsigned int ApertureSerialComm::getDiff28HashCount()
{
	std::string s = readReg(GET_DIFF28_HASH_COUNT);
	if (s == "")
		return 0;
	//get only the last 8 characters
	std::string s_trunc = s;
	if (s.length() >= 8)
		s_trunc = s.substr(s.length() - 8);
	unsigned int x;
	std::stringstream ss;
	ss << std::hex << s_trunc;
	ss >> x;
	return x;
}


bool ApertureSerialComm::sendMessage(MessageTypes messageType, std::string message)
{
	std::string commandStr, errStr;
	switch (messageType) {
	case KEY2:
		commandStr = "{2";
		errStr = "Key2";
		break;
	case MESSAGE2:
		commandStr = "{3";
		errStr = "Message2";
		break;
	}

	cTimeOutSerialConnection.writeString(commandStr + message + "}");
	std::string response;
	try
	{
		response = cTimeOutSerialConnection.readStringUntil("}");
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

bool ApertureSerialComm::ping()
{
	return sendCommand(PING);
}

bool ApertureSerialComm::disableHashing()
{
	return sendCommand(DISABLE_HASH);
}

bool ApertureSerialComm::enableHashing()
{
	return sendCommand(ENABLE_HASH);
}
