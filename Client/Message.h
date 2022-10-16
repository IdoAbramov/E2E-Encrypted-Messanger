#pragma once

/*
* @file Message.h
* @author Ido Abramov
*/

#include "Definitions.h"

/*
* Represents a message object.
* Message is a payload of Request and Response objects.
* Message contains a header and (often) a content.
*/
namespace Message {

#pragma pack(push, 1) // avoiding padding of the data, making sure the struct data saved as sequence in the memory.

	/*
	* Each message object sent to the server contains :
	* destination UUID(receiver client), its typeand content size.
	*/
	struct MessageHeaderData {
		Types::uuid_t destUUID;
		uint8_t messageType;
		uint32_t contentSize;
	};

	/*
	* Each message object received from the server contains:
	* destination UUID (receiver client), its ID, type and content size.*
	*/
	struct MessageRecordHeaderData {
		Types::uuid_t destUUID;
		uint32_t messageID;
		uint8_t messageType;
		uint32_t contentSize;
	};
#pragma pack(pop)

	/*
	* Union objects used in 2 directions:
	* 1. arrange data as struct and then use it as a stream of bytes for network delivery.
	* 2. to load stream of bytes from the network and represent it as group of variables contains data.
	*/
	union MessageHeader {
		MessageHeaderData msgHeaderData;
		std::array<uint8_t, sizeof(MessageHeaderData)> buffer;
	};

	union MessageRecordHeader {
		MessageRecordHeaderData msgHeaderData;
		std::array<uint8_t, sizeof(MessageRecordHeaderData)> buffer;
	};

	/*
	* The message types which exist in the program.
	*/
	enum MessageTypes {
		ASK_SYM_KEY = 1,
		SEND_SYM_KEY = 2,
		SEND_TEXT_MSG = 3,
		SEND_FILE = 4
	};
}
