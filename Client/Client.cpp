#include "Client.h"

Client* Client::_instance = NULL;

Client* Client::getInstance() {
	if (_instance == NULL) {
		_instance = new Client();
	}
	return _instance;
}

Client::Client() {
	this->_UUID = { 0 };
	this->_username = { 0 };
	this->_privateKey = "";
	this->_registerStatus = false; 
}

Client::~Client() {
	std::map<Types::uuid_t, Contact*>::iterator contact;
	for(contact = this->_contactsList.begin();
	    contact != this->_contactsList.end();
		contact++) {
		delete (contact->second);
	}
}

Types::uuid_t Client::getUUID() {
	return this->_UUID;
}
void Client::setUUID(const Types::uuid_t& uuid) {
	this->_UUID = uuid;
}

Types::username_t Client::getUsername() {
	return this->_username;
}
void Client::setUsername(const Types::username_t& username) {
	this->_username = username;
}

std::string Client::getPrivateKey() {
	return this->_privateKey;
}
void Client::setPrivateKey(const std::string& privateKey) {
	this->_privateKey = privateKey;
}

void Client::setRegisterStatus(const bool status) {
	this->_registerStatus = status;
}

bool Client::isRegistered() {
	return this->_registerStatus;
}

void Client::addContact(const Types::uuid_t& uuid, Types::username_t& username) {

	std::map<Types::username_t, Types::uuid_t>::iterator newContactUsername = this->_clientsList.find(username);

	if (newContactUsername == this->_clientsList.end()) { // add to client's clients list.
		this->_clientsList.map::insert(std::make_pair(username, uuid));
	}

	std::map<Types::uuid_t, Contact*>::iterator newContactUUID = this->_contactsList.find(uuid);

	if (newContactUUID == this->_contactsList.end()) { // add to client's contacts list.
		Contact* contact = new Contact(username); 
		this->_contactsList.map::insert(std::make_pair(uuid, contact));
	}
}

Contact* Client::getContact(const Types::uuid_t& uuid) {
	return this->_contactsList.map::find(uuid)->second;
}

Types::uuid_t Client::getClientUUIDfromClientsList(const Types::username_t& username) {
	return this->_clientsList.map::find(username)->second;
}

bool Client::isClientExistInList(const Types::username_t& username) {
	std::map<Types::username_t, Types::uuid_t>::iterator clientUsername = this->_clientsList.find(username);
	if (clientUsername == this->_clientsList.end()) { // if the client not found in the clients list.
		return false;
	}
	return true;
}

bool Client::isContactExistInList(const Types::uuid_t& uuid) {
	std::map<Types::uuid_t, Contact*>::iterator contactUUID = this->_contactsList.find(uuid);
	if (contactUUID == this->_contactsList.end()) { // if the client not found in the contacts list.
		return false;
	}
	return true;
}

bool Client::contactHasSymmetricKey(const Types::uuid_t& uuid){
std::string symKey = this->_contactsList.map::find(uuid)->second->getSymmetricKey();
	if (symKey.empty()) {
		return false;
	}
	return true;
}

bool Client::contactHasPublicKey(const Types::uuid_t& uuid) {
	std::string pubKey = this->_contactsList.map::find(uuid)->second->getPublicKey();
	if (pubKey.empty()) {
		return false;
	}
	return true;
}