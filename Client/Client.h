#pragma once

/*
* @file Client.h
* @author Ido Abramov
*/

#include "Definitions.h"
#include "Contact.h"

class Contact;

/*
* @class Client
* @details represents a Client and its attributes:
* @var _insatance - contains the one and only instance of Client instance (Singleton).
* @var _UUID - contains the client's UUID.
* @var _username - contains the client's username.
* @var _privateKey - contains the client's private key.
* @var _registerStatus - saves the client's registeration status.
* @var _clientsList - a map which contains usernames as keys and their matching UUIDs.
* @var _contactsList -a map which contains UUIDs as keys and their matching Contact object.
*/
class Client
{
private:
	static Client* _instance;
	Types::uuid_t _UUID;
	Types::username_t _username;
	std::string _privateKey;
	bool _registerStatus;
	// Note - should have replaced it to unordered_map for lower complexity of insert and find,
	// but it forces me to create hash functions for username_t and uuid_t (both fixed size arrays),
	// so I passed it for now.
	std::map<Types::username_t, Types::uuid_t> _clientsList;
	std::map<Types::uuid_t, Contact*> _contactsList;

private:
	/*
	* @details -  Client can be created only fron getInstance() function, to control that it can be created once.
	*/
	Client(); 

public:
	/*
	* @details - creates new single instance or returns exist one.
	*/
	static Client* getInstance(); 
	~Client();

	/*
	* Getters and Setters of Client's attributes - UUID, username and private key.
	*/
	Types::uuid_t getUUID();
	void setUUID(const Types::uuid_t& uuid);

	Types::username_t getUsername();
	void setUsername(const Types::username_t& username);

	std::string getPrivateKey();
	void setPrivateKey(const std::string& privateKey);

	/*
	* @param status - set the status as the Client is registered or not.
	*/
	void setRegisterStatus(const bool status);

	/*
	* @return the registeration status of the Client.	
	*/
	bool isRegistered();
	
	/*
	* adds a new Contact into Clinet's list of contacts.
	* @param uuid - the UUID of the Contact.
	* @param username - the name of the Cotntact.
	*/
	void addContact(const Types::uuid_t& uuid, Types::username_t& username);

	/*
	* Returns the contact record from the list of contacts by its UUID.
	* @param uuid - the UUID of the Contact to return.
	* @return a pointer to Contact record.
	*/
	Contact* getContact(const Types::uuid_t& uuid);

	/*
	* Returns the UUID of a contact from the list of contacts by its useranme.
	* @param username - the name of the Contact to return its UUID
	* @return the UUID of the Contact from the list.
	*/
	Types::uuid_t getClientUUIDfromClientsList(const Types::username_t& username);

	/*
	* Returns if the client username exists in the Client's clients list.
	* @param username - the name of the client.
	* @return true if exist, otherwise false.
	*/
	bool isClientExistInList(const Types::username_t& username);

	/*
	* Returns true if the contact UUID exists in the Client's contacts list. otherwise false.
	* @param uuid - the UUID of the contact.
	* @return true if exist, else false.
	*/
	bool isContactExistInList(const Types::uuid_t& uuid);

	/*
	* Returns if the Client has the contact's symmetric key.
	* @param uuid - the UUID of the contact.
	* @return true if exist, else false.
	*/
	bool contactHasSymmetricKey(const Types::uuid_t& uuid);

	/*
	* Returns if the Client has the contact's public key.
	* @param uuid - the UUID of the contact.
	* @return true if exist, else false.
	*/
	bool contactHasPublicKey(const Types::uuid_t& uuid);
};
