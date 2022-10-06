#include "Contact.h"

Contact::Contact() {
	this->_username = { 0 };
	this->_publicKey = "";
	this->_symmetricKey = "";
}

Contact::Contact(Types::username_t& username) {
	this->_username = username;
}

Contact::~Contact() { }


Types::username_t Contact::getUsername() {
	return this->_username;
}

void Contact::setUsername(const Types::username_t& username) {
	this->_username = username;
}

std::string Contact::getPublicKey() {
	return this->_publicKey;
}

void Contact::setPublicKey(const std::string& publicKey) {
	this->_publicKey = publicKey;
}

std::string Contact::getSymmetricKey() {
	return this->_symmetricKey;
}

void Contact::setSymmetricKey(const std::string& symmetricKey) {
	this->_symmetricKey = symmetricKey;
}