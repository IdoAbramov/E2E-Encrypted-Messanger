#include "Base64Wrapper.h"


std::string Base64Wrapper::encode(const std::string& str)
{
	std::string encoded;
	
	 // returns string "encoded" to store data, then store the encoded value of "str" string in it.
	CryptoPP::StringSource ss(str, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encoded)));
	
	return encoded;
}

std::string Base64Wrapper::decode(const std::string& str)
{
	std::string decoded;
	
	// returns string "decoded" to store data, then store the decoded value of "str" string in it.
	CryptoPP::StringSource ss(str, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));
	
	return decoded;
}
