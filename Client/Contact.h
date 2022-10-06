#pragma once

/*
* @file Contact.h
* @author Ido Abramov
*/

#include "Definitions.h"

/*
* @class Contact
* @details represents a Contact and its attributes:
* @var _username - contains the contact's username.
* @var _publickey - contains the contact's public key.
* @var _symmetricKey - contains the contact's symmetric key.
*/
class Contact
{
friend class Client; 

private:
	Types::username_t _username;
	std::string _publicKey;
	std::string _symmetricKey;

public:
	/*
	* Constructors and a destructor.
	*/
	Contact();
	Contact(Types::username_t& username);
	virtual ~Contact();

	/*
	* Getters and Setters of Contact's attributes - username, public key and symmetric key.
	*/
	Types::username_t getUsername();
	void setUsername(const Types::username_t& username);

	std::string getPublicKey();
	void setPublicKey(const std::string& publicKey);

	std::string getSymmetricKey();
	void setSymmetricKey(const std::string& symmetricKey);
};