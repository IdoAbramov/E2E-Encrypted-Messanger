#pragma once

/*
* @file MessageUClient.h
* @author Ido Abramov
*/

#include "Definitions.h"
#include "Request.h"
#include "Response.h"
#include "Utils.h"

namespace ServerCommunication {
	/*
	* Sends request to server and receives its response.
	* Request consists a header and payload.
	* Response consists a header and payload.
	*
	* @param[in] requestHeader - contains the request header data as a stream of bytes to send to the server.
	* @param[in] requestPayload - contains the request payload data as a stream of bytes to send to the server.
	* @param[out] responseHeader - contains the received response header from the server as a stream of bytes.
	* @param[out] responsePayload - contains the received response payload from the server as a stream of bytes.
	*/
	void sendAndReceive(const std::vector<uint8_t>& requestHeader,
						const std::vector<uint8_t>& requestPayload,
						std::vector<uint8_t>& responseHeader,
						std::vector<uint8_t>& responsePayload);

	/*
	* Handles response code of server general error.
	*/
	void serverGeneralErrorHandler();

	/*
	* Handles response code which not matches reasonable response return code.
	*/
	void serverUndefinedResponseHandler();
}

