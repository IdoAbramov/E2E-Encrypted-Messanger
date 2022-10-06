#include "ServerCommunication.h"

#include <boost/asio.hpp>

void ServerCommunication::sendAndReceive(const std::vector<uint8_t>& requestHeader,
	const std::vector<uint8_t>& requestPayload,
	std::vector<uint8_t>& responseHeader,
	std::vector<uint8_t>& responsePayload) {

	std::string serverIP;
	std::string serverPort;

	// Extracts server's IP and port data.
	if (Utils::isFileExist(Constants::SERVER_DATA_FILE_PATH)) {
		std::fstream serverInfoFile;
		serverInfoFile.open(Constants::SERVER_DATA_FILE_PATH);

		std::string serverInfo;
		std::getline(serverInfoFile, serverInfo);

		serverInfoFile.close();

		int pos = serverInfo.find_first_of(':');

		serverIP = serverInfo.substr(0, pos);
		serverPort = serverInfo.substr(pos + 1);
	}
	else {
		std::cout << "Error while trying to get server info..." << std::endl;
		std::cout << "Please insure \"server.info\" file is in the Client's folder." << std::endl;
		return;
	}

	// Creates the socket and connects to the server.
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::socket socket(io_context);
	boost::asio::ip::tcp::resolver resolver(io_context);
	try {
		boost::asio::connect(socket, resolver.resolve(serverIP, serverPort));
	}
	catch (std::exception& e) {
		std::cout << "Error while trying to connect to the server." << std::endl;
		std::cout << e.what() << std::endl;
		return;
	}

	try {
		boost::asio::write(socket, boost::asio::buffer(requestHeader.data(), sizeof(Request::RequestHeaderData)));
	}
	catch (std::exception& e) {
		std::cout << "Error while trying to send a request header to the server." << std::endl;
		std::cout << e.what() << std::endl;
		return;
	}

	Request::RequestHeader tempReqHeader;
	tempReqHeader.buffer = { 0 };
	std::copy(requestHeader.begin(),
		requestHeader.end(),
		tempReqHeader.buffer.data());

	if (tempReqHeader.requestHeaderData.payloadSize > 0) {
		try {
			boost::asio::write(socket, boost::asio::buffer(requestPayload.data(), requestPayload.size()));
		}
		catch (std::exception& e) {
			std::cout << "Error while trying to send a request payload to the server." << std::endl;
			std::cout << e.what() << std::endl;
			return;
		}
	}

	responseHeader.resize(sizeof(Response::ResponseHeaderData));
	try {
		boost::asio::read(socket, boost::asio::buffer(responseHeader, sizeof(Response::ResponseHeaderData)));
	}
	catch (std::exception& e) {
		std::cout << "Error while trying to get a response header from the server." << std::endl;
		std::cout << e.what() << std::endl;
		return;
	}

	Response::ResponseHeader tempRespHeader;
	tempRespHeader.buffer = { 0 };
	std::copy(responseHeader.begin(),
		responseHeader.end(),
		tempRespHeader.buffer.data());

	uint32_t respPayloadSize = tempRespHeader.responseHeaderData.payloadSize;

	if (respPayloadSize > 0)
	{
		responsePayload.resize(respPayloadSize);
		try {
			boost::asio::read(socket, boost::asio::buffer(responsePayload, respPayloadSize));
		}
		catch (std::exception& e) {
			std::cout << "Error while trying to get a response payload from the server." << std::endl;
			std::cout << e.what() << std::endl;
			return;
		}
	}
	socket.close();
}

void ServerCommunication::serverGeneralErrorHandler() {
	std::cout << "General error from the server. please try again." << std::endl;
}

void ServerCommunication::serverUndefinedResponseHandler() {
	std::cout << "Undefined return code error occured. please try again." << std::endl;
}