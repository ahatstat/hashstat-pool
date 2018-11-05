#ifndef COINSHIELD_UTIL_H
#define COINSHIELD_UTIL_H

//#include <boost/date_time/posix_time/posix_time.hpp>
#include "hash/templates.h"

#ifdef WIN32
	typedef int pid_t;
#else

#endif


#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#define MAX_THREADS 18446744073709551615lu  


#define loop                for(;;)
#define BEGIN(a)            ((char*)&(a))
#define END(a)              ((char*)&((&(a))[1]))
//#define Sleep(a)            boost::this_thread::sleep(boost::posix_time::milliseconds(a))
#define Sleep(a)			std::this_thread::sleep_for(std::chrono::milliseconds(a));
#define LOCK(a)             boost::lock_guard<boost::mutex> lock(a)





/** Convert a 32 bit Unsigned Integer to Byte Vector using Bitwise Shifts. **/
inline std::vector<unsigned char> uint2bytes(unsigned int UINT)
{
	std::vector<unsigned char> BYTES(4, 0);
	BYTES[0] = UINT >> 24;
	BYTES[1] = UINT >> 16;
	BYTES[2] = UINT >> 8;
	BYTES[3] = UINT;
				
	return BYTES;
}
			
			
/** Convert a byte stream into unsigned integer 32 bit. **/	
inline unsigned int bytes2uint(std::vector<unsigned char> BYTES, int nOffset = 0) { return (BYTES[0 + nOffset] << 24) + (BYTES[1 + nOffset] << 16) + (BYTES[2 + nOffset] << 8) + BYTES[3 + nOffset]; }
			
			
/** Convert a 64 bit Unsigned Integer to Byte Vector using Bitwise Shifts. **/
inline std::vector<unsigned char> uint2bytes64(uint64 UINT)
{
	std::vector<unsigned char> INTS[2];
	INTS[0] = uint2bytes((unsigned int) UINT);
	INTS[1] = uint2bytes((unsigned int) (UINT >> 32));
				
	std::vector<unsigned char> BYTES;
	BYTES.insert(BYTES.end(), INTS[0].begin(), INTS[0].end());
	BYTES.insert(BYTES.end(), INTS[1].begin(), INTS[1].end());
				
	return BYTES;
}

			
/** Convert a byte Vector into unsigned integer 64 bit. **/
inline uint64 bytes2uint64(std::vector<unsigned char> BYTES) { return (bytes2uint(BYTES) | ((uint64)bytes2uint(BYTES, 4) << 32)); }



/** Convert Standard String into Byte Vector. **/
inline std::vector<unsigned char> string2bytes(std::string STRING)
{
	std::vector<unsigned char> BYTES(STRING.begin(), STRING.end());
	return BYTES;
}


/** Convert Byte Vector into Standard String. **/
inline std::string bytes2string(std::vector<unsigned char> BYTES)
{
	std::string STRING(BYTES.begin(), BYTES.end());
	return STRING;
}



#endif
