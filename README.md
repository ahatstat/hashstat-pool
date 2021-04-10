Nexus Hash Channel Miner.  Adapted from Videlicet's prime solo miner.
Connects to a pool for mining Nexus Hash channel.  The program communicates over a serial port to a compatible FPGA board running nexus hashing algorithm. 

To Run on Windows:
Edit config.ini to set the com port connected to the FPGA board.  

skminer.exe pooladdress port payoutAddress

for example:
skminer.exe us.nexuspool.ru 8333 2RcY8acdk258Hb5a63kq4ME7P1vtGDhyGQGXx8afc2QPwjzbv1h


Build Instructions
Dependencies:  boost
To build in windows using visual studio:
Download the latest boost and build from source (run bootstrap.bat followed by ./b2)
Change Visual Studio project C++ "additional include directories" and linker "additional library directories" to point to boost. The paths will be reported by the output of the boost build script.   
Build the project in Visual Studio.


