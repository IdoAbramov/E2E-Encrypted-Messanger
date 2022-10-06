#pragma once

/*
* @file Request.h
* @author Ido Abramov
*/

#include "Definitions.h"

/*
* Represents a request to the server.
* Request contains a header and (not always) a payload.
*/
namespace Request {

#pragma pack(push,1) // avoiding padding of the data, making sure the struct data saved in a raw in the memory.

	/*
	* Request header struct.
	* contains the client's UUID (source client), its version, the request code and the size of the payload.
	*/
	struct RequestHeaderData {
		Types::uuid_t UUID;
		uint8_t version;
		uint16_t code;
		uint32_t payloadSize;
	};

	/*
	* Register request payload struct.
	* contains the username of the requesting client and its public key.
	*/
	struct registerRequestPayloadData {
		Types::username_t username;
		Types::pubkey_t publicKey;
	};

	/*
	* Get public key request payload struct.
	* contains the target client UUID which its public key requested.
	*/
	struct getPublicKeyRequestPayloadData {
		Types::uuid_t destUUID;
	};
#pragma pack(pop)

	/*
	* Union objects used in 2 directions:
	* 1. arrange data as struct and then use it as a stream of bytes for network delivery.
	* 2. to load stream of bytes from the network and represent it as group of variables contains data.
	*/
	union RequestHeader {
		RequestHeaderData requestHeaderData;
		std::array<uint8_t, sizeof(RequestHeaderData)> buffer;
	};

	union registerRequestPayload {
		registerRequestPayloadData regReqPayloadData;
		std::array<uint8_t, sizeof(registerRequestPayloadData)> buffer;
	};

	union getPublicKeyRequestPayload {
		getPublicKeyRequestPayloadData pubkeyReqPayloadData;
		std::array<uint8_t, sizeof(getPublicKeyRequestPayloadData)> buffer;
	};


	/*
	* The request codes which exist in the program.
	*/
	enum RequestCodes {
		REGISTER_REQ_CODE = 1100,
		USERS_LIST_REQ_CODE = 1101,
		GET_PUBLIC_KEY_REQ_CODE = 1102,
		SEND_MESSAGE_REQ_CODE = 1103,
		PULL_WAITING_MESSAGES_REQ_CODE = 1104
	};
}