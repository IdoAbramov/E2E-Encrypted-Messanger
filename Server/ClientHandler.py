import socket
import struct
import threading
from uuid import *
from Definitions import *
from ClientData import *
from MessageData import *

class ClientsHandler:
    clientsList = {} # Contains the server's clients
    messages = {} # Contains the server's messages received from clients

    def __init__(self):
        self.sem = threading.Semaphore()

    def sync(func):
        def inner(self, *args, **kwargs):
            self.sem.acquire()
            try:
                return func(self, *args, **kwargs)
            except:
                print("Error while trying to perform function:", func.__name__)
            finally:
                self.sem.release()
        return inner

    # Adds new client into the server's clients list.
    @sync
    def addNewClient(self, newClient:ClientData, clientUUID:UUID):
        if clientUUID not in self.clientsList:
            self.clientsList[clientUUID] = newClient; # clientUUID is the key. newClient holds username + public key.

    # Returns true if the username is already registered to the server. returns false otherwise.
    @sync
    def isUsernameExistsAlready(self, username):
        for uuid in self.clientsList:
            if self.clientsList[uuid].username == username:
                return True
        return False

    # Returns the client username by its UUID from the server's clients list.
    @sync
    def getClientUsernameByUUID(self, clientUUID:UUID):
        return self.clientsList[clientUUID].username

    # Adds new message received from a client into the server's messages list.
    @sync
    def addMessageToList(self, newMessage:MessageData):
        self.messages[newMessage.getMessageID()] = newMessage # the key for dict is destination client UUID.

    # Sends general error response code to the client.
    def returnGeneralError(self, clientSocket):
        version = VERSION
        responseCode = ResponseCodes.GENERAL_ERROR_RESP
        payloadSize = 0
        responseHeader = struct.pack('<BHL', version, responseCode, payloadSize)
        clientSocket.sendall(responseHeader)

    # ----- Server handlers. each request has its handler. ----- #
    def registerRequestHandler(self, clientSocket, payloadSize):

        # Gets the payload of the register request which contains the username and the publickey.
        try:
            registerRequestPayloadBuffer = clientSocket.recv(payloadSize)
        except:
            print("Error while trying to get request's payload from the client.")

        # If the payload size is less then excepted
        if payloadSize < (USERNAME_LEN + PUBLIC_KEY_LEN):
            self.returnGeneralError(clientSocket)
            return

        # if the payload received is less then the payload size.
        if len(registerRequestPayloadBuffer) != payloadSize:
            self.returnGeneralError(clientSocket)
            return

        # Unpack the payload into username and public key.
        username, publicKey = struct.unpack(f'<{USERNAME_LEN}s{PUBLIC_KEY_LEN}s', registerRequestPayloadBuffer)

        # Checks if the username is already exists in the server clients list.
        if self.isUsernameExistsAlready(username):
            self.returnGeneralError(clientSocket)
            return

        # Sets the values of the response header.
        version = VERSION
        responseCode = ResponseCodes.REGISTER_SUCCESS_RESP
        payloadSize = UUID_LEN

        # Pack the data into one stream of bytes and send it to the client.
        responseHeader = struct.pack('<BHL', version, responseCode, payloadSize)

        try:
            clientSocket.sendall(responseHeader)
        except:
            print("Error while trying to send response's header to the client.")

        # Generates new randomly UUID.
        generatedUUID = uuid4()
        # Adds the new client to the server clients list
        self.addNewClient(ClientData(username, publicKey),generatedUUID)

        # Converts the UUID into stream of bytes.
        clientUUID = generatedUUID.bytes

        # Pack the UUID and sends it as the response payload and send it to the requesting client.
        responsePayload = struct.pack(f'<{UUID_LEN}s', clientUUID)
        try:
            clientSocket.sendall(responsePayload)
        except:
            print("Error while trying to send response's payload to the client.")

    def clientsListRequestHandler(self, clientSocket, srcUUID):
        # No request's payload.

        # Checks if the requesting client is exist in the server clients list.
        if not self.isUsernameExistsAlready(self.getClientUsernameByUUID(UUID(bytes=srcUUID))):
            self.returnGeneralError(clientSocket)
            return

        # Sums the client record size.
        clientRecordLen = UUID_LEN + USERNAME_LEN

        # Sets the values of the response header.
        version = VERSION
        responseCode = ResponseCodes.CLIENTS_LIST_RESP

        # sums the number of clients multiplied by the size of a client record.
        payloadSize = len(self.clientsList) * clientRecordLen

        # Pack the data into one stream of bytes and send it to the client.
        responseHeader = struct.pack("<BHL", version, responseCode, payloadSize)

        try:
            clientSocket.sendall(responseHeader)
        except:
            print("Error while trying to send response's header to the client.")

        # Gets all clients from the server clients list and concatenate them into one stream of bytes.
        clientsListToSend = bytes()
        for clientUUID in self.clientsList:
            clientUsername = self.getClientUsernameByUUID(clientUUID)
            clientUUIDBytes = clientUUID.bytes
            clientsListToSend += struct.pack(f'<{UUID_LEN}s{USERNAME_LEN}s',clientUUIDBytes , clientUsername)

        # Send the whole list bytes to the requesting client as the response payload.
        try:
            clientSocket.sendall(clientsListToSend)
        except:
            print("Error while trying to send response's payload to the client.")

    def getPublicKeyRequest(self, clientSocket, payloadSize, srcUUID):

        # Receives the request payload which is the target client's UUID
        try:
            getPubkeyRequestPayloadBuffer = clientSocket.recv(payloadSize)
        except:
            print("Error while trying to get request's payload from the client.")

        # Checks if the asking client exists in the server clients list.
        if not self.isUsernameExistsAlready(self.getClientUsernameByUUID(UUID(bytes=srcUUID))):
            self.returnGeneralError(clientSocket)
            return

        # Checks if the payload size is in the excepted size of UUID length.
        if payloadSize < UUID_LEN:
            self.returnGeneralError(clientSocket)
            return

        # Checks if the actual received buffer is in the excepted size.
        if len(getPubkeyRequestPayloadBuffer) != payloadSize:
            self.returnGeneralError(clientSocket)
            return

        # Unpacks the received payload into requested client's public key.
        destUUID, = struct.unpack(f'<{UUID_LEN}s', getPubkeyRequestPayloadBuffer)

        # Gets the public key of the requested client from the server clients list.
        destClient = self.clientsList.get(UUID(bytes=destUUID))
        destPubkey = destClient.getPublicKey()

        # Sets the values of the response header, packs it into one stream of bytes and send to the requesting client.
        version = VERSION
        responseCode = ResponseCodes.GET_PUBLIC_KEY_RESP
        payloadSize = UUID_LEN + PUBLIC_KEY_LEN
        responseHeader = struct.pack('<BHL', version, responseCode, payloadSize)
        try:
            clientSocket.sendall(responseHeader)
        except:
            print("Error while trying to send response's header to the client.")

        # Packs the response payload into stream of bytes and send to the requesting client.
        responsePayload = struct.pack(f'<{UUID_LEN}s{PUBLIC_KEY_LEN}s', destUUID, destPubkey)
        try:
            clientSocket.sendall(responsePayload)
        except:
            print("Error while trying to send response's payload to the client.")

    def sendMessageRequestHandler(self,clientSocket, payloadSize, srcUUID):

        try:
            messageBuffer = clientSocket.recv(payloadSize)
        except:
            print("Error while trying to get request's payload from the client.")

        # Sums the message header size.
        messageHeaderSize = UUID_LEN + MESSAGE_TYPE_LEN + MESSAGE_CONTENT_LEN

        # Unpacks the message and split it to header and content.
        # Header contains target client's UUID, message type and its content size.
        (destUUID, messageType, contentSize), messageContent = struct.unpack(f'<{UUID_LEN}sBL',\
                                            messageBuffer[0:messageHeaderSize]),\
                                             messageBuffer[messageHeaderSize:]

        # Checks if the excepted payload of the request meets the actual size.
        if payloadSize < (UUID_LEN + MESSAGE_TYPE_LEN + MESSAGE_CONTENT_LEN):
            self.returnGeneralError(clientSocket)
            return

        # Checks if the requesting client is registered in the server.
        if not self.isUsernameExistsAlready(self.getClientUsernameByUUID(UUID(bytes=srcUUID))):
            self.returnGeneralError(clientSocket)
            return

        # Checks if the message type is an available one.
        if messageType != MessageTypes.ASK_SYM_KEY and\
           messageType != MessageTypes.SEND_SYM_KEY and\
           messageType != MessageTypes.SEND_TEXT_MSG and\
           messageType != MessageTypes.SEND_TEXT_FILE:
            self.returnGeneralError(clientSocket)
            return

        # Creates new message and adds it to the server messages list.
        newMessage = MessageData(destUUID, srcUUID, messageType,messageContent, contentSize)
        self.addMessageToList(newMessage)

        messageID = newMessage.getMessageID()

        # Sets the values of the response header.
        version = VERSION
        responseCode = ResponseCodes.MESSAGE_SENT_RESP
        payloadSize = UUID_LEN + MESSAGE_ID_LEN

        # Packs the response's header into stream of bytes and send it to the requesting client.
        responseHeader = struct.pack('<BHL', version, responseCode, payloadSize)
        try:
            clientSocket.sendall(responseHeader)
        except:
            print("Error while trying to send response's header to the client.")

        # Packs the response's payload into stream of bytes and send it to the requesting client.
        responsePayload = struct.pack(f'<{UUID_LEN}sL', destUUID, messageID)
        try:
            clientSocket.sendall(responsePayload)
        except:
            print("Error while trying to send response's payload to the client.")

    def getWaitingMessages(self, clientSocket, srcUUID,destUUID):
        # No request's payload.

        # Checks if the requesting client is registered in the server.
        if not self.isUsernameExistsAlready(self.getClientUsernameByUUID(UUID(bytes=srcUUID))):
            self.returnGeneralError(clientSocket)
            return

        # Calculates a message header size.
        messageHeaderSize = UUID_LEN + MESSAGE_ID_LEN + MESSAGE_TYPE_LEN + MESSAGE_CONTENT_LEN

        messageListToSend = bytes() # The stream of bytes which will contain all messages which are for the requesting client.
        responsePayloadSize = 0
        messagesToRemove = [] # Each message sent to the client is to be removed from the servers messages list.

        # Iterating over the server messages list, packing those matches to the requesting client as destination client.
        for id in self.messages:
            if self.messages[id].getDestUUID() == destUUID:

                # Packs the header of the message.
                messageListToSend += struct.pack(f'<{UUID_LEN}sLBL',\
                                     self.messages[id].getSrcUUID(),\
                                     self.messages[id].getMessageID(), \
                                     self.messages[id].getMessageType(), \
                                     self.messages[id].getMessageSize())

                # Packs the content of the message.
                messageListToSend += struct.pack(f'<{self.messages[id].getMessageSize()}s',\
                                                 self.messages[id].getMessageContent())

                # Sums the response payload size with the current message header and content size.
                responsePayloadSize += messageHeaderSize + self.messages[id].getMessageSize()
                # Adds the message to the messages to be removed list.
                messagesToRemove.append(id)

        # Iterating the message to be removed list, removing the message from the server messages list.
        for id in messagesToRemove:
            del self.messages[id]

        # Sets the values of the response header.
        version = VERSION
        responseCode = ResponseCodes.PULL_WAITING_MESSAGES_RESP
        # Pack the response's header into stream of bytes and sending it to the requesting client.
        responseHeader = struct.pack("<BHL", version, responseCode, responsePayloadSize)
        try:
           clientSocket.sendall(responseHeader)
        except:
           print("Error while trying to send response's header to the client.")

        # Sending the requesting client's list of messages as a respose payload, to the requesting client.
        try:
            clientSocket.sendall(messageListToSend)
        except:
            print("Error while trying to send response's payload to the client.")

    # Main handler of clients requests.
    # Roots the requests to the correct handler by the received request code.
    def clientRequestHandler(self, clientSocket):
            try:
                # Receives a client request's header.
                requestHeaderBuffer = clientSocket.recv(REQUEST_HEADER_LEN)

            except:
                print("Error while trying to get request's header from the client.")
                self.returnGeneralError(clientSocket)
                clientSocket.close()

            # Validating the request's header length meets with the excepted length.
            if len(requestHeaderBuffer) != REQUEST_HEADER_LEN:
                self.returnGeneralError(clientSocket)
                clientSocket.close()
                return

            # Unpacks the request's header into the excepted values.
            try:
                uuid, version, code, payloadSize = struct.unpack(f'<{UUID_LEN}sBHL', requestHeaderBuffer)
                    
            except struct.error:
                print("Unable to unpack data.")
                self.returnGeneralError(clientSocket)
                return
 
            # Refers to the right handler for the client's request
            if code == RequestCodes.REGISTER_REQ:
                 self.registerRequestHandler(clientSocket, payloadSize)

            elif code == RequestCodes.CLIENTS_LIST_REQ:
                self.clientsListRequestHandler(clientSocket, uuid)

            elif code == RequestCodes.GET_PUBLIC_KEY_REQ:
                self.getPublicKeyRequest(clientSocket, payloadSize, uuid)

            elif code == RequestCodes.SEND_MESSAGE_REQ:
                self.sendMessageRequestHandler(clientSocket, payloadSize, uuid)

            elif code == RequestCodes.PULL_WAITING_MESSAGES_REQ:
                self.getWaitingMessages(clientSocket, uuid, uuid)

            # If the client sent an unknown request code, return general error from the server.
            else:
                self.returnGeneralError(clientSocket)
