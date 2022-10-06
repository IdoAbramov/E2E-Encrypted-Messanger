#pragma once
/*
* @file Definitions.h
* @author Ido Abramov
*/

// Includes of the common headers used in the prorgam.
#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <exception>

// Using of Boost library for sockets.
#include <boost/asio.hpp>
#include <boost/algorithm/hex.hpp>

/* 
 * Using Wrappers of CryptoPP library for 
 * encryption and decryption (RSA and AES) 
 * and encode/decode of Base64.
 */
#include "AESWrapper.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"

namespace Constants {
	const std::string CLIENT_INFO_FILE_PATH = "my.info";
	const std::string SERVER_DATA_FILE_PATH = "server.info";
	const uint8_t VERSION = 2;
	const unsigned int NO_PAYLOAD = 0;
	const unsigned int NO_MESSAGE_CONTENT = 0;
	const unsigned int UUID_BYTES_LENGTH = 16;
	const unsigned int PUBLIC_KEY_LENGTH = 160;
	const unsigned int USERNAME_LENGTH = 255;
	const unsigned int MESSAGE_MAX_LENGTH = UINT_MAX;
	const unsigned int FILE_NAME_LEN = 12;
}

namespace Types {
	typedef std::array<uint8_t, Constants::UUID_BYTES_LENGTH> uuid_t;
	typedef std::array<uint8_t, Constants::USERNAME_LENGTH> username_t;
	typedef std::array<uint8_t, Constants::PUBLIC_KEY_LENGTH> pubkey_t;
}