import socket
import os
from threading import Thread
from Definitions import *
from ClientHandler import *

def startReceive():
    # Infinate loop taking accepts and creates each acception of client connection - a thread of clientRequestHandler
    while True:
        try:
            clientSocket, address = server.accept()
            print(address)
            clientsHandler = ClientsHandler()
            clientThread = Thread(target=clientsHandler.clientRequestHandler, args=(clientSocket,))
            clientThread.start()
        except socket.error:
            print("Error - lost connection with the client.")
            clientSocket.close()

if __name__ == '__main__':
    """
    Gets the port from the server port file and open a socket to accept requests from clients.
    """

    # Checks if the files of server port info exists.
    if not os.path.isfile(PORT_INFO_FILE_PATH):
        raise IOError("The port info file couldn't be found. check if exist and retry.")

    # Open the server port info for reading.
    try:
        portInfoFile = open(PORT_INFO_FILE_PATH,"r")
        fileConent = portInfoFile.read().split()
    except:
        print("Server's port file cannot open. please check the file and retry.")
        exit()

    # Checks if the file contains any data.
    if len(fileConent) == 0:
        raise  ValueError("The file is empty. check its content and retry.")

    # Checks if the file content contains any other data except of the port.
    if len(fileConent) > 1:
        raise  ValueError("The file contains more than one argument. check its content and retry.")

    # Gets the port from the file content.
    serverPort = int(fileConent[0])

    # Validate the port number is a valid one.
    if not (serverPort > 0 and serverPort < MAX_PORT_VALUE):
        raise ValueError("The port value is invalid. please check if valid and try again.")

    portInfoFile.close()

    # Creates the server socket and waiting for clients' communications.
    try:
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.bind((HOST, serverPort))
        server.listen()
        print("Server is up !")
        startReceive()
    except:
        print("Server failed. please try again.")