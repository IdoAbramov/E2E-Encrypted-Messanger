#include "MessageUClient.h"

void MessageUClient::printInstructions() {
	std::cout << "Welcome to MessageU - an E2E encrypted messanger.\n";
}

void MessageUClient::registerHandlder(Client* client) {
	
	std::string uname;
	std::cout << "Please enter your username:\n";
	std::getline(std::cin, uname);

	if (uname.length() > Constants::USERNAME_LENGTH) {
		std::cout << "Username length exceeds the maximum length.\n";
		std::cout << "Please try again with a username of maximum size of ";
		std::cout << (Constants::USERNAME_LENGTH-1) << " characters\n";
		return;
	}

	Types::uuid_t emptyUUID = { 0 };

	Request::RequestHeader reqHeader{emptyUUID,
					 Constants::VERSION,
					 Request::RequestCodes::REGISTER_REQ_CODE,
					 (Constants::USERNAME_LENGTH + Constants::PUBLIC_KEY_LENGTH)};

	std::vector<uint8_t> reqHeaderBuffer(reqHeader.buffer.begin(), 
					     reqHeader.buffer.end());

	// Generates both new public and private key for the client.
	RSAPrivateWrapper rsapriv;
	std::string clientPublicKey = rsapriv.getPublicKey();
	std::string clientPrivateKey = Base64Wrapper::encode(rsapriv.getPrivateKey());
	clientPrivateKey.erase(std::remove(clientPrivateKey.begin(), 
					   clientPrivateKey.end(), 
					   '\n'), 
			       clientPrivateKey.end());

	Types::username_t username = { 0 };
	std::copy(uname.begin(), 
		  uname.end(), 
		  username.data());

	Types::pubkey_t publicKey = { 0 };
	std::copy(clientPublicKey.begin(), 
		  clientPublicKey.end(), 
	   	  publicKey.data());

	Request::registerRequestPayload regReqPL{username, 
						 publicKey};

	std::vector<uint8_t> regReqPayloadBuffer(regReqPL.buffer.begin(), 
		                                 regReqPL.buffer.end());

	std::vector<uint8_t> respHeaderBuffer;
	std::vector<uint8_t> respPayloadBuffer;

	ServerCommunication::sendAndReceive(reqHeaderBuffer,
					    regReqPayloadBuffer, 
					    respHeaderBuffer, 
				            respPayloadBuffer);
	
	Response::ResponseHeader respHeader;
	respHeader.buffer = { 0 };
	std::copy(respHeaderBuffer.begin(),
		  respHeaderBuffer.end(),
		  respHeader.buffer.data());

	if (respHeader.responseHeaderData.code == Response::ResponseCodes::GENERAL_ERROR_RESP_CODE) {
		ServerCommunication::serverGeneralErrorHandler();
		return;
	}
	else if (respHeader.responseHeaderData.code != Response::ResponseCodes::REGISTER_SUCCESS_RESP_CODE) {
		ServerCommunication::serverUndefinedResponseHandler();
		return;
	}
		
	Response::registersuccessResponsePayload  regSuccessRespPL;
	regSuccessRespPL.buffer = { 0 };
	std::copy(respPayloadBuffer.begin(), 
		  respPayloadBuffer.end(), 
		  regSuccessRespPL.buffer.data());

	std::string hexUUID = Utils::hexify(Utils::convertUUIDToVector(regSuccessRespPL.regReqPayloadData.UUID));

	std::ofstream myInfoFile;
	myInfoFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

	try {
		myInfoFile.open(Constants::CLIENT_INFO_FILE_PATH);
		myInfoFile << uname << "\n";
		myInfoFile << hexUUID << "\n";
		myInfoFile << clientPrivateKey << "\n";
		myInfoFile.close();
	}
	catch (std::ofstream::failure const& e) {
		std::cout << "Error while trying to create Client's data file.\n";
		std::cout << e.what() << "\n";
		return;
	}

	client->setUUID(regSuccessRespPL.regReqPayloadData.UUID);
	client->setUsername(username);
	client->setPrivateKey(clientPrivateKey);
	client->setRegisterStatus(true);
	std::cout << uname << " registered successfuly. UUID: " << hexUUID << "\n";

	// Clears the data from the memory.
	std::fill(clientPrivateKey.begin(), 
		  clientPrivateKey.end(), 
		  0);
}

void MessageUClient::getClientsListHandler(Client* client) {
	
	Request::RequestHeader reqHeader{client->getUUID(),
					 Constants::VERSION,
					 Request::RequestCodes::USERS_LIST_REQ_CODE,
				   	 Constants::NO_PAYLOAD};
	
	std::vector<uint8_t> reqHeaderBuffer(reqHeader.buffer.begin(),
					     reqHeader.buffer.end());

	std::vector<uint8_t> reqEmptyPayload = { 0 };

	std::vector<uint8_t> respHeaderBuffer;
	std::vector<uint8_t> respPayloadBuffer;

	ServerCommunication::sendAndReceive(reqHeaderBuffer,
					    reqEmptyPayload, 
					    respHeaderBuffer, 
					    respPayloadBuffer);

	Response::ResponseHeader respHeader;
	respHeader.buffer = { 0 };
	std::copy(respHeaderBuffer.begin(), 
		  respHeaderBuffer.end(), 
		  respHeader.buffer.data());

	if (respHeader.responseHeaderData.code == Response::ResponseCodes::GENERAL_ERROR_RESP_CODE) {
		ServerCommunication::serverGeneralErrorHandler();
		return;
	}
	else if (respHeader.responseHeaderData.code != Response::ResponseCodes::USERS_LIST_RESP_CODE) {
		ServerCommunication::serverUndefinedResponseHandler();
		return;
	}

	Response::clientsListResponsePayload  clientRecordFromListPL;  

	uint32_t clientRecordSize = Constants::UUID_BYTES_LENGTH + Constants::USERNAME_LENGTH;

	uint32_t numberOfClients = respHeader.responseHeaderData.payloadSize / clientRecordSize;

	std::cout << "---------- CLIENTS LIST ----------\n\n";

	// Iterating through the response's payload on each client received.
	for (uint32_t clientRecordNum = 0;
	     clientRecordNum < numberOfClients;
	     clientRecordNum++) {

		clientRecordFromListPL.buffer = { 0 };

		// Gets the next client record.
		std::copy(respPayloadBuffer.begin() + (clientRecordNum * clientRecordSize),
			  respPayloadBuffer.begin() + ((clientRecordNum + 1) * clientRecordSize),
			  clientRecordFromListPL.buffer.data());

		client->addContact(clientRecordFromListPL.clntsListPayloadData.UUID,
				   clientRecordFromListPL.clntsListPayloadData.username);

		std::string hexUUID = Utils::hexify(Utils::convertUUIDToVector(clientRecordFromListPL.clntsListPayloadData.UUID));

		std::string username(clientRecordFromListPL.clntsListPayloadData.username.begin(),
				     clientRecordFromListPL.clntsListPayloadData.username.end());

		std::cout << "UUID: " << hexUUID << " Username: " << username << "\n";
	}
		
	std::cout << "\n---------- END OF CLIENTS LIST ----------\n";


	// Clears the data from the memory.
	std::fill(respPayloadBuffer.begin(), 
		  respPayloadBuffer.end(), 
		  0);
	std::fill(respHeader.buffer.begin(), 
		  respHeader.buffer.end(), 
		  0);
}

void MessageUClient::getPublicKeyHandler(Client* client) {

	std::string uname;
	std::cout << "Please enter the username which you ask for its public key:\n";
	std::getline(std::cin, uname);

	Types::username_t username = { 0 };
	std::copy(uname.begin(), 
		  uname.end(), 
		  username.data());
	
	if (!client->isClientExistInList(username)) {
		std::cout << "Error while trying to get user \'" << uname << "\' information.";
		std::cout << "Plesae check the username and try again.\n";
		return;
	}

	Types::uuid_t destUUID = client->getClientUUIDfromClientsList(username);

	if (client->contactHasPublicKey(destUUID)) {
		std::cout << "You already have user \'" << uname << "\' public key.\n";
		return;
	}

	Request::RequestHeader reqHeader{client->getUUID(),
					 Constants::VERSION,
					 Request::RequestCodes::GET_PUBLIC_KEY_REQ_CODE,
					 Constants::UUID_BYTES_LENGTH};
	
	std::vector<uint8_t> reqHeaderBuffer(reqHeader.buffer.begin(), 
		                             reqHeader.buffer.end());

	Request::getPublicKeyRequestPayload pubkeyReqPL{destUUID};

	std::vector<uint8_t> pubkeyReqPayloadBuffer(pubkeyReqPL.buffer.begin(), 
						    pubkeyReqPL.buffer.end());

	std::vector<uint8_t> respHeaderBuffer;
	std::vector<uint8_t> respPayloadBuffer;

	ServerCommunication::sendAndReceive(reqHeaderBuffer,
					    pubkeyReqPayloadBuffer, 
					    respHeaderBuffer, 
					    respPayloadBuffer);

	Response::ResponseHeader respHeader;
	respHeader.buffer = { 0 };
	std::copy(respHeaderBuffer.begin(), 
		  respHeaderBuffer.end(), 
		  respHeader.buffer.data());

	if (respHeader.responseHeaderData.code == Response::ResponseCodes::GENERAL_ERROR_RESP_CODE) {
		ServerCommunication::serverGeneralErrorHandler();
		return;
	}
	else if (respHeader.responseHeaderData.code != Response::ResponseCodes::PUBLIC_KEY_RESP_CODE) {
		ServerCommunication::serverUndefinedResponseHandler();
		return;
	}

	Response::publicKeyResponsePayload pubkeyRespPL;
	pubkeyRespPL.buffer = { 0 };
	std::copy(respPayloadBuffer.begin(), 
		  respPayloadBuffer.end(), 
		  pubkeyRespPL.buffer.data());
		
	std::string stringedPublicKey(pubkeyRespPL.pubkeyPayloadData.publicKey.begin(),
				      pubkeyRespPL.pubkeyPayloadData.publicKey.end());

	client->getContact(pubkeyRespPL.pubkeyPayloadData.UUID)->setPublicKey(stringedPublicKey);

	std::cout << "Public key of user \'" << uname << "\' received successfuly.\n";
}

void MessageUClient::getWaitingMessagesHandler(Client* client)  {

	Request::RequestHeader reqHeader{client->getUUID(),
					 Constants::VERSION,
					 Request::RequestCodes::PULL_WAITING_MESSAGES_REQ_CODE,
					 Constants::NO_PAYLOAD};

	std::vector<uint8_t> reqHeaderBuffer(reqHeader.buffer.begin(), 
		                             reqHeader.buffer.end());

	std::vector<uint8_t> requestEmptyPayload = { 0 };

	std::vector<uint8_t> respHeaderBuffer;
	std::vector<uint8_t> respPayloadBuffer;

	ServerCommunication::sendAndReceive(reqHeaderBuffer,
					    requestEmptyPayload, 
					    respHeaderBuffer, 
					    respPayloadBuffer);

	Response::ResponseHeader respHeader;
	respHeader.buffer = { 0 };
	std::copy(respHeaderBuffer.begin(), 
		  respHeaderBuffer.end(), 
		  respHeader.buffer.data());

	if (respHeader.responseHeaderData.code == Response::ResponseCodes::GENERAL_ERROR_RESP_CODE) {
		ServerCommunication::serverGeneralErrorHandler();
		return;
	}
	else if (respHeader.responseHeaderData.code != Response::ResponseCodes::PULL_WAITING_MESSAGES_RESP_CODE) {
		ServerCommunication::serverUndefinedResponseHandler();
		return;
	}

	if (respPayloadBuffer.size() == 0) {
		std::cout << "No waiting messages.\n";
		return;
	}

	// A struct which represents message record consisting receiving UUID, message's ID, type and size. 
	Message::MessageRecordHeader messageRecordHeader;
	messageRecordHeader.buffer = { 0 };
	std::vector<uint8_t>::iterator messageIterator;

	messageIterator = respPayloadBuffer.begin();

	// Iterating through the response's payload on each message received.
	while(messageIterator < respPayloadBuffer.end()) {

		std::copy(messageIterator, 
			  messageIterator + sizeof(Message::MessageRecordHeader),
			  messageRecordHeader.buffer.data());

		messageIterator += sizeof(Message::MessageRecordHeader);

		std::string messageContent(messageIterator, 
			                   messageIterator + messageRecordHeader.msgHeaderData.contentSize);

		messageIterator += messageRecordHeader.msgHeaderData.contentSize;

		std::cout << "----- Message -----\n";

		Types::username_t username;

		// Checks if the sender is a known contact.
		if (client->isContactExistInList(messageRecordHeader.msgHeaderData.destUUID)) {
			username = client->getContact(messageRecordHeader.msgHeaderData.destUUID)->getUsername();
			std::string uname(username.begin(), 
					  username.end());
			std::cout << "From: " << uname << "\n";
		} 
		else {
			std::cout << "From: Unknown user\n";
		}

		std::cout << "Content:\n";

		// Checks the message type and preform actions its value.
	
		if (messageRecordHeader.msgHeaderData.messageType == Message::MessageTypes::ASK_SYM_KEY) {
			std::cout << "Request for symmetric key.\n";
		} 

		else if (messageRecordHeader.msgHeaderData.messageType == Message::MessageTypes::SEND_SYM_KEY) {

			if (!client->isContactExistInList(messageRecordHeader.msgHeaderData.destUUID)) {
				std::cout << "Cannot decrypt symmetric key of unknown user.\n";
				return;
			}

			Types::uuid_t contactUUID = client->getClientUUIDfromClientsList(username);

			// Decrypt the message by the current client's private key.
			RSAPrivateWrapper rsapriv(Base64Wrapper::decode(client->getPrivateKey()));
			std::string decryptedMessage = rsapriv.decrypt(messageContent);

			// Sets the symmetric key to the sending client in the contacts list.
			client->getContact(contactUUID)->setSymmetricKey(decryptedMessage);
			std::cout << "Symmetric key received.\n";

			// Clears the data from the memory.
			std::fill(decryptedMessage.begin(), 
				      decryptedMessage.end(), 
				      0);
			} 

		else if (messageRecordHeader.msgHeaderData.messageType == Message::MessageTypes::SEND_TEXT_MSG) {

			if (!client->isContactExistInList(messageRecordHeader.msgHeaderData.destUUID)) {
				std::cout << "Cannot decrypt text message of unknown user.\n";
				return;
			}

			Types::uuid_t contactUUID = client->getClientUUIDfromClientsList(username);

			// Creates symmetric key buffer.
			unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];

			// Gets the sending client's symmetric key from contact list.
			std::string symmetricKey = client->getContact(contactUUID)->getSymmetricKey();

			std::copy(symmetricKey.begin(), 
				  symmetricKey.end(), 
				  key);

			// Creates AES by the key values.
			AESWrapper aes(key, AESWrapper::DEFAULT_KEYLENGTH);
				
			std::string decryptedMessage = aes.decrypt(messageContent.c_str(), 
								   messageContent.length());
			std::cout << decryptedMessage << "\n";

			// Clear the data from the memory.
			std::fill(symmetricKey.begin(), 
				  symmetricKey.end(), 
				  0);
			std::fill(decryptedMessage.begin(), 
				  decryptedMessage.end(), 
				  0);
		} 

		else if (messageRecordHeader.msgHeaderData.messageType == Message::MessageTypes::SEND_FILE) {

			if (!client->isContactExistInList(messageRecordHeader.msgHeaderData.destUUID)) {
				std::cout << "Cannot decrypt text message of unknown user.\n";
				return;
			}

			Types::uuid_t contactUUID = client->getClientUUIDfromClientsList(username);

			// Creates symmetric key buffer.
			unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];

			// Gets the sending client's symmetric key from contact list.
			std::string symmetricKey = client->getContact(contactUUID)->getSymmetricKey();

			std::copy(symmetricKey.begin(),
				  symmetricKey.end(),
				  key);

			// Creates AES by the key values.
			AESWrapper aes(key, AESWrapper::DEFAULT_KEYLENGTH);

			std::string decryptedFileData = aes.decrypt(messageContent.c_str(),
								    messageContent.length());

			// Generates random temp file name and saves the decrypred file content in it.
			std::string fileTempName = Utils::generateTempTextFileName(Constants::FILE_NAME_LEN);

			std::ofstream fileToWrite;
			fileToWrite.exceptions(std::ifstream::badbit | std::ifstream::failbit);

			try {
				fileToWrite.open(fileTempName, std::ios::binary);
				fileToWrite << decryptedFileData;
				fileToWrite.close();
			}
			catch (std::ofstream::failure const& e) {
				std::cout << "Error while trying to write the received file into the Client's folder.\n";
				std::cout << e.what() << "\n";
				return;
			}

			std::cout << "A new file <" << fileTempName << "> received and saved in the current directory.\n";

			// Clear the data from the memory.
			std::fill(symmetricKey.begin(),
				  symmetricKey.end(),
				  0);

			std::fill(decryptedFileData.begin(),
				  decryptedFileData.end(),
				  0);
		}	
		else {
			std::cout << "Current message's type is invalid.\n";
			continue;
		}

		std::cout << "----- End of message -----\n\n";

		std::fill(messageContent.begin(), 
		  	  messageContent.end(), 
			  0);
	}

	std::cout << "Done pulling waiting messages.\n";

	std::fill(messageRecordHeader.buffer.begin(), 
		  messageRecordHeader.buffer.end(), 
              	  0);
}

void MessageUClient::sendTextMessagesHandler(Client* client) {

	std::string uname;
	std::cout << "Please enter target username:\n";
	std::getline(std::cin, uname);

	Types::username_t username = { 0 };
	std::copy(uname.begin(), 
		  uname.end(), 
		  username.data());

	if (!client->isClientExistInList(username)) {
		std::cout << "Error while trying to get user \'" << uname << "\' information.";
		std::cout << "Plesae check the username and try again.\n";
		return;
	}

	Types::uuid_t destUUID = client->getClientUUIDfromClientsList(username);

	if (!client->contactHasSymmetricKey(destUUID)) {
		std::cout << "Error - user \'" << uname << "\' has no symmetric key yet.\n";
		std::cout << "Please ask target user for a symmetric key ";
		std::cout << "or send a new symmetric key to the target user.\n";
		return;
	}

	// Gets the target user Symmetric key.
	unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
	std::string symmetricKey = client->getContact(destUUID)->getSymmetricKey();
	std::copy(symmetricKey.begin(), 
		  symmetricKey.end(), 
		  key);

	AESWrapper aes(key, AESWrapper::DEFAULT_KEYLENGTH); 

	std::string messageContent;
	std::cout << "Please enter your message:\n";
	std::getline(std::cin, messageContent); 

	if (messageContent.length() > Constants::MESSAGE_MAX_LENGTH) {
		std::cout << "Message length exceeds the maximum length.";
		std::cout << "please split the messages and try again.\n";
		return;
	}

	std::string encryptedMessageContent = aes.encrypt(messageContent.c_str(), 
							  messageContent.length());

	Request::RequestHeader reqHeader{client->getUUID(),
					 Constants::VERSION,
					 Request::RequestCodes::SEND_MESSAGE_REQ_CODE,
					 (sizeof(Message::MessageHeader) + encryptedMessageContent.length())};

	std::vector<uint8_t> reqHeaderBuffer(reqHeader.buffer.begin(), 
		                             reqHeader.buffer.end());

	Message::MessageHeader msgHeader{destUUID,
					 Message::MessageTypes::SEND_TEXT_MSG,
					 encryptedMessageContent.length()};


	std::vector<uint8_t> msgHeaderBuffer(msgHeader.buffer.begin(), 
					     msgHeader.buffer.end());

	std::vector<uint8_t> msgContentBuffer(encryptedMessageContent.begin(), 
					      encryptedMessageContent.end());

	std::vector<uint8_t> reqPayloadBuffer(msgHeaderBuffer.begin(), 
					     msgHeaderBuffer.end());

	reqPayloadBuffer.insert(reqPayloadBuffer.end(), 
				msgContentBuffer.begin(), 
				msgContentBuffer.end());

	std::vector<uint8_t> respHeaderBuffer;
	std::vector<uint8_t> respPayloadBuffer;

	ServerCommunication::sendAndReceive(reqHeaderBuffer,
					    reqPayloadBuffer, 
					    respHeaderBuffer, 
					    respPayloadBuffer);

	Response::ResponseHeader respHeader;
	respHeader.buffer = { 0 };
	std::copy(respHeaderBuffer.begin(), 
		  respHeaderBuffer.end(), 
		  respHeader.buffer.data());

	if (respHeader.responseHeaderData.code == Response::ResponseCodes::GENERAL_ERROR_RESP_CODE) {
		ServerCommunication::serverGeneralErrorHandler();
		return;
	}
	else if (respHeader.responseHeaderData.code != Response::ResponseCodes::MESSAGE_SENT_RESP_CODE) {
		ServerCommunication::serverUndefinedResponseHandler();
		return;
	}

	std::cout << "Text message delivered successfuly.\n"; 


	// Clears the data from the memory.
	std::fill(symmetricKey.begin(), 
		  symmetricKey.end(), 
		  0);
	std::fill(encryptedMessageContent.begin(), 
		  encryptedMessageContent.end(), 
		  0);
	std::fill(msgContentBuffer.begin(), 
		  msgContentBuffer.end(), 
		  0);
	std::fill(reqPayloadBuffer.begin(), 
		  reqPayloadBuffer.end(), 
		  0);
	std::fill(respPayloadBuffer.begin(), 
		  respPayloadBuffer.end(), 
		  0);
}

void MessageUClient::askForSymmetricKeyHandler(Client* client) {

	std::string uname;
	std::cout << "Please enter destination username:\n";
	std::getline(std::cin, uname);

	Types::username_t username = { 0 };
	std::copy(uname.begin(), 
		  uname.end(), 
		  username.data());

	if (!client->isClientExistInList(username)) {
		std::cout << "Error while trying to get user \'" << uname << "\' information. ";
		std::cout << "Plesae check the username and try again.\n";
		return;
	}

	Types::uuid_t destUUID = client->getClientUUIDfromClientsList(username);

	Request::RequestHeader reqHeader{client->getUUID(),
					 Constants::VERSION,
					 Request::RequestCodes::SEND_MESSAGE_REQ_CODE,
					 sizeof(Message::MessageHeader)};
	
	std::vector<uint8_t> reqHeaderBuffer(reqHeader.buffer.begin(), 
					     reqHeader.buffer.end());
	
	Message::MessageHeader msgHeader{destUUID,
					 Message::MessageTypes::ASK_SYM_KEY,
					 Constants::NO_MESSAGE_CONTENT};

	std::vector<uint8_t> reqPayloadBuffer(msgHeader.buffer.begin(), 
		                              msgHeader.buffer.end());

	std::vector<uint8_t> respHeaderBuffer;
	std::vector<uint8_t> respPayloadBuffer;

	ServerCommunication::sendAndReceive(reqHeaderBuffer,
					    reqPayloadBuffer, 
					    respHeaderBuffer, 
					    respPayloadBuffer);

	Response::ResponseHeader respHeader;
	respHeader.buffer = { 0 };
	std::copy(respHeaderBuffer.begin(), 
		  respHeaderBuffer.end(), 
		  respHeader.buffer.data());

	if (respHeader.responseHeaderData.code == Response::ResponseCodes::GENERAL_ERROR_RESP_CODE) {
		ServerCommunication::serverGeneralErrorHandler();
		return;
	}
	else if(respHeader.responseHeaderData.code != Response::ResponseCodes::MESSAGE_SENT_RESP_CODE) {
		ServerCommunication::serverUndefinedResponseHandler();
		return;
	}
	
	std::cout << "Request for symmetric key delivered successfuly.\n";
}

void MessageUClient::sendSymmetricKeyHandler(Client* client) {

	std::string uname;
	std::cout << "Please enter destination username:\n";
	std::getline(std::cin, uname);

	Types::username_t username = { 0 };
	std::copy(uname.begin(), 
		  uname.end(), 
		  username.data());

	if (!client->isClientExistInList(username)) {
		std::cout << "Error while trying to get user \'" << uname << "\' information. ";
		std::cout << "Plesae check the username and try again.\n";
		return;
	}

	Types::uuid_t destUUID = client->getClientUUIDfromClientsList(username);

	if (!client->contactHasPublicKey(destUUID)) {
		std::cout << "Error - user \'" << uname << "\' has no public key. ";
		std::cout << "Please ask for public key from server before.\n";
		return;
	}

	if (client->contactHasSymmetricKey(destUUID)) {
		std::cout << "Error - user \'" << uname << "\' has a symmetric key already.";
		std::cout << "You can send a text message.\n";
		return;
	}

	// Creates symmetric key buffer.
	unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
	// Creates new symmetric key and converts it into a string.
	std::string symmetricKey(reinterpret_cast<const char*>(AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH)),
				 AESWrapper::DEFAULT_KEYLENGTH);

	// Encrypts the symmetric key with the target client's public key.
	RSAPublicWrapper rsapub(client->getContact(destUUID)->getPublicKey());
	std::string encryptedSymKey = rsapub.encrypt(symmetricKey);

	Request::RequestHeader reqHeader{client->getUUID(),
					Constants::VERSION,
					Request::RequestCodes::SEND_MESSAGE_REQ_CODE,
					(sizeof(Message::MessageHeader) + encryptedSymKey.length())};

	std::vector<uint8_t> reqHeaderBuffer(reqHeader.buffer.begin(), 
		                             reqHeader.buffer.end());

	Message::MessageHeader msgHeader{destUUID,
					 Message::MessageTypes::SEND_SYM_KEY,
					 encryptedSymKey.length()};

	std::vector<uint8_t> msgHeaderBuffer(msgHeader.buffer.begin(), 
		                             msgHeader.buffer.end());

	std::vector<uint8_t> msgContentBuffer(encryptedSymKey.begin(), 
		                              encryptedSymKey.end());

	std::vector<uint8_t> reqPayloadBuffer(msgHeaderBuffer.begin(), 
		                              msgHeaderBuffer.end());

	reqPayloadBuffer.insert(reqPayloadBuffer.end(), 
		                msgContentBuffer.begin(), 
		                msgContentBuffer.end());

	std::vector<uint8_t> respHeaderBuffer;
	std::vector<uint8_t> respPayloadBuffer;

	ServerCommunication::sendAndReceive(reqHeaderBuffer,
					    reqPayloadBuffer, 
					    respHeaderBuffer, 
					    respPayloadBuffer);

	Response::ResponseHeader respHeader;
	respHeader.buffer = { 0 };
	std::copy(respHeaderBuffer.begin(), 
		  respHeaderBuffer.end(), 
		  respHeader.buffer.data());

	if (respHeader.responseHeaderData.code == Response::ResponseCodes::GENERAL_ERROR_RESP_CODE) {
		ServerCommunication::serverGeneralErrorHandler();
		return;
	}
	else if(respHeader.responseHeaderData.code != Response::ResponseCodes::MESSAGE_SENT_RESP_CODE) {
		ServerCommunication::serverUndefinedResponseHandler();
		return;
	}
	
	client->getContact(destUUID)->setSymmetricKey(symmetricKey);
	std::cout << "Sent the symmetric key message successfuly.\n";

	// Clears the data from the memory.
	std::fill(symmetricKey.begin(), 
		  symmetricKey.end(), 
		  0);
	std::fill(encryptedSymKey.begin(), 
		  encryptedSymKey.end(), 
		  0);
	std::fill(msgContentBuffer.begin(), 
		  msgContentBuffer.end(), 
		  0);
	std::fill(reqPayloadBuffer.begin(), 
		  reqPayloadBuffer.end(), 
		  0);
}

void MessageUClient::sendTextFileHandler(Client* client) {

	std::string uname;
	std::cout << "Please enter target username:\n";
	std::getline(std::cin, uname);

	Types::username_t username = { 0 };
	std::copy(uname.begin(),
		  uname.end(),
		  username.data());
	
	if (!client->isClientExistInList(username)) {
		std::cout << "Error while trying to get user \'" << uname << "\' information.";
		std::cout << "Plesae check the username and try again.\n";
		return;
	}

	Types::uuid_t destUUID = client->getClientUUIDfromClientsList(username);

	if (!client->contactHasSymmetricKey(destUUID)) {
		std::cout << "Error - user \'" << uname << "\' has no symmetric key yet.\n";
		std::cout << "Please ask target user for a symmetric key ";
		std::cout << "or send a new symmetric key to the target user.\n";
		return;
	}

	// Gets the target user Symmetric key.
	unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
	std::string symmetricKey = client->getContact(destUUID)->getSymmetricKey();
	std::copy(symmetricKey.begin(),
		  symmetricKey.end(),
		  key);

	AESWrapper aes(key, AESWrapper::DEFAULT_KEYLENGTH);
	
	std::string filePath;
	std::cout << "Please enter the file's full path and name:\n";
	std::getline(std::cin, filePath);

	if (!Utils::isFileExist(filePath)) {
		std::cout << "File could not be found, please verify its location and name, and try again.\n";
		return;
	}
	
	std::string fileData;
	std::ifstream fileToRead;
	fileToRead.exceptions(std::ifstream::badbit | std::ifstream::failbit);

	try {
		fileToRead.open(filePath, std::ios::binary);
		fileToRead.seekg(0, std::ios::end);

		fileData.reserve(fileToRead.tellg());
		fileToRead.seekg(0, std::ios::beg);

		fileData.assign((std::istreambuf_iterator<char>(fileToRead)),
				 std::istreambuf_iterator<char>());

		fileToRead.close();
	}
	catch (std::ifstream::failure const& e) {
		std::cout << "Error when trying to open file to read data.\n";
		std::cout << e.what() << "\n";
		return;
	}

	if (fileData.length() > Constants::MESSAGE_MAX_LENGTH) {
		std::cout << "File's data length exceeds the maximum length.";
		std::cout << "please try smaller file.\n";
		return;
	}

	std::string encryptedFileData = aes.encrypt(fileData.c_str(),
						    fileData.length());

	Request::RequestHeader reqHeader{client->getUUID(),
					 Constants::VERSION,
					 Request::RequestCodes::SEND_MESSAGE_REQ_CODE,
					 (sizeof(Message::MessageHeader) + encryptedFileData.length()) };

	std::vector<uint8_t> reqHeaderBuffer(reqHeader.buffer.begin(),
					     reqHeader.buffer.end());

	Message::MessageHeader msgHeader{destUUID,
					 Message::MessageTypes::SEND_FILE,
					 encryptedFileData.length() };

	std::vector<uint8_t> msgHeaderBuffer(msgHeader.buffer.begin(),
					     msgHeader.buffer.end());

	std::vector<uint8_t> msgContentBuffer(encryptedFileData.begin(),
					      encryptedFileData.end());

	std::vector<uint8_t> reqPayloadBuffer(msgHeaderBuffer.begin(),
					      msgHeaderBuffer.end());

	reqPayloadBuffer.insert(reqPayloadBuffer.end(),
		                msgContentBuffer.begin(),
		                msgContentBuffer.end());

	std::vector<uint8_t> respHeaderBuffer;
	std::vector<uint8_t> respPayloadBuffer;

	ServerCommunication::sendAndReceive(reqHeaderBuffer,
					    reqPayloadBuffer,
					    respHeaderBuffer,
					    respPayloadBuffer);

	Response::ResponseHeader respHeader;
	respHeader.buffer = { 0 };
	std::copy(respHeaderBuffer.begin(),
		  respHeaderBuffer.end(),
		  respHeader.buffer.data());

	if (respHeader.responseHeaderData.code == Response::ResponseCodes::GENERAL_ERROR_RESP_CODE) {
		ServerCommunication::serverGeneralErrorHandler();
		return;
	}
	else if (respHeader.responseHeaderData.code != Response::ResponseCodes::MESSAGE_SENT_RESP_CODE) {
		ServerCommunication::serverUndefinedResponseHandler();
		return;
	}

	std::cout << "File data delivered successfuly.\n";

	std::fill(fileData.begin(),
		  fileData.end(),
		  0);

}

void MessageUClient::start() {

	bool run = true; // indicates if the client inputs exit command.
	unsigned int command = 0;

	Client* client = Client::getInstance();
	
	// The client perform actions until it exits from the program.
	while (run) {
		Utils::showMenu();
		command = Utils::getCommandInput();

		if (Utils::validateCommand(command)) {
			// registered client cannot preform registeration again. 
			if (client->isRegistered() &&
			    command == UserCommands::REGISTER) {
				std::cout << "Client already Registered.\n";
				continue;
			}
			// non-registered client cannot preform other commands then registeration, instructions and exit. 
			else if (!client->isRegistered() &&
				 command != UserCommands::REGISTER &&
				 command != UserCommands::INSTRUCTIONS &&
				 command != UserCommands::EXIT) {
				std::cout << "Client is not registered.\n";
				std::cout << "Please Perform a registeration to perform the selected command.\n";
				continue;
			}

			switch (command) {

			case UserCommands::INSTRUCTIONS:
				printInstructions();
				break;

			case UserCommands::REGISTER:
				registerHandlder(client);
				break;

			case UserCommands::GET_CLIENTS_LIST:
				getClientsListHandler(client);
				break;

			case UserCommands::GET_PUBLIC_KEY:
				getPublicKeyHandler(client);
				break;

			case UserCommands::GET_WAITING_MESSAGES:
				getWaitingMessagesHandler(client);
				break;

			case UserCommands::SEND_TEXT_MESSAGE:
				sendTextMessagesHandler(client);
				break;

			case UserCommands::ASK_CLIENT_SYM_KEY:
				askForSymmetricKeyHandler(client);
				break;

			case UserCommands::SEND_MY_SYM_KEY:
				sendSymmetricKeyHandler(client);
				break;

			case UserCommands::SEND_TEXT_FILE:
				sendTextFileHandler(client);
				break;

			case UserCommands::EXIT:
				std::cout << "Exiting program...\n";
				run = false;
				break;

			default:
				std::cout << "undefined command number, please try again.\n";
				break;
			}
		}
		else {
			std::cout << "Invalid command, please try again!\n";
		}
	}
	delete client;
} 
