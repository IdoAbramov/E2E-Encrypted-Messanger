#pragma once

/*
* @file Utils.h
* @author Ido Abramov
*/

#include "Definitions.h"

class Utils {
public:
	
	static void showMenu(); // prints the menu
	static unsigned int getCommandInput(); // returns the users input.

	/*
	* Checks if the command is valid.
	* @param command - command number
	* @return true - if command exists as a valid command in the program (check UserCommands).
	* otherwise, returns false.
	*/
	static bool validateCommand(unsigned int command);

	/*
	* Converts a vector of values into hexadecimal string.
	* @param vec - the vector to convert.
	* @return - string of hexadecimal values.
	* @example - vec = {'\x0F','\x1E', '\x3C'} converted to string = "0F1E3C"
	*/
	static std::string hexify(const std::vector<uint8_t>& vec);

	/*
	* Converts a string which represents hexadecimal values of data into vector of values.
	* @param str - hexadecimal string to convert.
	* @return - vector of values.
	* @example - str = "0F1E3C" converted to vector = {'\x0F','\x1E', '\x3C'}
	*/
	static std::vector<uint8_t> unhexify(const std::string& str);

	/*
	* Converts uuid_t (for more info check "Defintions.h") into vector.
	* @param uuid - the uuid of the client as a uuid_t.
	* @return - the uuid of the client as a vector.
	*/
	static std::vector<uint8_t> convertUUIDToVector(Types::uuid_t& uuid);

	/*
	* Converts vector into uuid_t (for more info check "Defintions.h").
	* @param uuidVec - a uuid represented as a vector.
	* @return - the uuid as a uuid_t.
	*/
	static Types::uuid_t convertVectorToUUID(std::vector<uint8_t>& uuidVec);

	/*
	* Checks if a file exists in the given path.
	* @param filePath - the path which which will be checked.
	* @return true if file do exist. otherwise false.
	*/
	static bool isFileExist(std::string filePath);

	/*
	* Generates alphanumeric temporary file name with given length.
	* @param len - the length of the file name.
	* @return the generated file name.
	*/
	static std::string generateTempTextFileName(unsigned int len);
};


/*
* The user input commands available in the program.
*/
enum UserCommands {
	EXIT = 0,
	INSTRUCTIONS = 100,
	REGISTER = 110,
	GET_CLIENTS_LIST = 120,
	GET_PUBLIC_KEY = 130,
	GET_WAITING_MESSAGES = 140,
	SEND_TEXT_MESSAGE = 150,
	ASK_CLIENT_SYM_KEY = 151,
	SEND_MY_SYM_KEY = 152,
	SEND_TEXT_FILE = 160,
	WRONG_COMAMND = 999
};