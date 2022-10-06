#include "Utils.h"

void Utils::showMenu() {
	std::cout << "\n------------------- MENU -------------------" << std::endl;
	std::cout << "MessageU client at your service.\n" << std::endl;
	std::cout << "[100] Instructions" << std::endl;
	std::cout << "[110] Register" << std::endl;
	std::cout << "[120] Request for clients list" << std::endl;
	std::cout << "[130] Request for public key" << std::endl;
	std::cout << "[140] Request for waiting messages" << std::endl;
	std::cout << "[150] Send a text message" << std::endl;
	std::cout << "[151] Send a request for symmetric key" << std::endl;
	std::cout << "[152] Send your symmetric key" << std::endl;
	std::cout << "[160] Send a text file" << std::endl;
	std::cout << "[0] Exit" << std::endl;
	std::cout << "---------------------------------------------\n" << std::endl;
}

unsigned int Utils::getCommandInput() {
	unsigned int command;
	std::string temp;
	std::cout << "Please enter the command number: " << std::endl;
	std::getline(std::cin, temp);
	std::stringstream parser(temp);
	if(parser >> command && 
	  (parser >> std::ws).eof()) {
		return command;
	}
	return UserCommands::WRONG_COMAMND;
}

bool Utils::validateCommand(unsigned int command) {
	if (command == UserCommands::INSTRUCTIONS ||
		command == UserCommands::REGISTER ||
		command == UserCommands::GET_CLIENTS_LIST ||
		command == UserCommands::GET_PUBLIC_KEY ||
		command == UserCommands::GET_WAITING_MESSAGES ||
		command == UserCommands::SEND_TEXT_MESSAGE ||
		command == UserCommands::ASK_CLIENT_SYM_KEY ||
		command == UserCommands::SEND_MY_SYM_KEY ||
		command == UserCommands::SEND_TEXT_FILE ||
		command == UserCommands::EXIT) {
		return true; 
	}
	return false;
}

std::string Utils::hexify(const std::vector<uint8_t>& vec) {
    std::string str;
    boost::algorithm::hex(vec.begin(), vec.end(), std::back_inserter(str));
    return str;
}

std::vector<uint8_t> Utils::unhexify(const std::string& str) {
    std::vector<uint8_t> hex = { 0 };
    boost::algorithm::unhex(str, std::back_inserter(hex));
    return hex;
}

std::vector<uint8_t> Utils::convertUUIDToVector(Types::uuid_t& uuid) {
    std::vector<uint8_t>uuidVec(uuid.begin(), uuid.end());
    return uuidVec;
}

Types::uuid_t Utils::convertVectorToUUID(std::vector<uint8_t>& uuidVec) {
    Types::uuid_t uuid;
    memcpy(&uuid, &uuidVec, Constants::UUID_BYTES_LENGTH);
    return uuid;
}

bool Utils::isFileExist(std::string filePath) {
    std::fstream file;
    file.open(filePath);
    if (file) {
        file.close(); 
        return true;
    }
    return false;
}


std::string Utils::generateTempTextFileName(unsigned int len) {

	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};

	std::string str(len, 0);
	std::generate_n(str.begin(), len, randchar);
	str.append(".txt");
	return str;
}
