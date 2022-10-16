#pragma once

/*
* @file Response.h
* @author Ido Abramov
*/

#include "Definitions.h"

/*
* Represents a response from the server.
* Response contains a header and (often) a payload.
*/
namespace Response {

#pragma pack(push, 1) // avoiding padding of the data, making sure the struct data saved in as sequence in the memory.

	/*
	* Response header struct.
	* contains the version of the server, the response code and the size of the payload.
	*/
	struct ResponseHeaderData {
		uint8_t version;
		uint16_t code;
		uint32_t payloadSize;
	};

	/*
	* Register response payload struct.
	* contains the UUID (created by the server) of the requesting client.
	*/
	struct registerSuccessResponsePayloadData {
		Types::uuid_t UUID;
	};

	/*
	* Client list response payload struct.
	* Represents a record of a client in the clients list.
	* contains the UUID and username of the client from the list.
	*/
	struct clientsListResponsePayloadData {
		Types::uuid_t UUID;
		Types::username_t username;
	};

	/*
	* Public key response payload struct.
	* contains the UUID of the requested client and its public key.
	*/
	struct publicKeyResponsePayloadData {
		Types::uuid_t UUID;
		Types::pubkey_t publicKey;
	};

	/*
	* Message sent response payload struct.
	* contains the target client's UUID and the message Id from the server.
	*/
	struct messageSentResponsePayloadData {
		Types::uuid_t destUUID;
		uint32_t messageID;
	};

	/*
	* Pulls waiting messages response payload struct.
	* Represents a record of a message in the messages list.
	* contains the UUID of the sender, the message ID, type and size.
	*/
	struct pullWaitingMessagesResponsePayloadData {
		Types::uuid_t UUID;
		uint32_t messageID;
		uint8_t messageType;
		uint32_t messageSize;
	};
#pragma pack(pop)

	/*
	* Union objects used in 2 directions:
	* 1. arrange data as struct and then use it as a stream of bytes for network delivery.
	* 2. to load stream of bytes from the network and represent it as group of variables contains data.
	*/
	union ResponseHeader {
		ResponseHeaderData responseHeaderData;
		std::array<uint8_t, sizeof(responseHeaderData)> buffer;
	};

	union registersuccessResponsePayload {
		registerSuccessResponsePayloadData regReqPayloadData;
		std::array<uint8_t, sizeof(registerSuccessResponsePayloadData)> buffer;
	};

	union clientsListResponsePayload {
		clientsListResponsePayloadData clntsListPayloadData;
		std::array<uint8_t, sizeof(clientsListResponsePayloadData)> buffer;
	};

	union publicKeyResponsePayload {
		publicKeyResponsePayloadData pubkeyPayloadData;
		std::array<uint8_t, sizeof(publicKeyResponsePayloadData)> buffer;
	};

	union messageSentResponsePayload {
		messageSentResponsePayloadData msgSentPayloadData;
		std::array<uint8_t, sizeof(messageSentResponsePayloadData)> buffer;
	};

	union pullWaitingMessagesResponsePayload {
		pullWaitingMessagesResponsePayloadData pullWaitMsgsPayloadData;
		std::array<uint8_t, sizeof(pullWaitingMessagesResponsePayloadData)> buffer;
	};


	/*
	* The response codes which exist in the program.
	*/
	enum ResponseCodes {
		REGISTER_SUCCESS_RESP_CODE = 2100,
		USERS_LIST_RESP_CODE = 2101,
		PUBLIC_KEY_RESP_CODE = 2102,
		MESSAGE_SENT_RESP_CODE = 2103,
		PULL_WAITING_MESSAGES_RESP_CODE = 2104,
		GENERAL_ERROR_RESP_CODE = 9000
	};
}
