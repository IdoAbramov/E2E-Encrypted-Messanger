
MessageU App

Stateless Server-Client E2E encrypted messanger.

Client - C++ with Boost and CryptoPP libraries.

Server - Python 3.10.6.

**The messanger protocol is stateless and is encrypted end-to-end by:**
- using RSA encryption for delivering symmetric key.
- send and receive messages between clients, encrypted by AES symmetric key.

How to use (in windows):
Download the files both c++ and python.

**For C++ Client:**

Download the latest Boost library and add its path to Additional Include Directories.

Download the latest CryptoPP library:

  * Open cryptest.sln file and build it (win32 debug).
  
  * Add the main directory of CryptoPP to Additional Include Directories.
  
  * Add static library cryptlib.lib in the path - ./win32/output/debug to Linker->input->Additional Dependencies.
  
  * Change the runtime library to Multi-threaded Debug (/MTd).

Finally, compile the C++ client.

**For python, just hit Run.**
