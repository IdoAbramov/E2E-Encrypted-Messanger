import socket
import os
from threading import Thread
from Definitions import *
from ClientHandler import *

def startReceive():
    clientsHandler = ClientsHandler()
    
    # Starts to receive connections from clients. 
    # Each connection creates its own thread and routes to the appropriate handler.
    while True:
        try:
            clientSocket, address = server.accept()
            print(address)
            clientThread = Thread(target=clientsHandler.clientRequestHandler, args=(clientSocket,))
            clientThread.start()
        except socket.error:
            print("Error - lost connection with the client.")
            clientSocket.close()

def startServer(port):
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.bind((HOST, port))
        server.listen()
        print("Server is up !")
        return server
   
def getPortFromFile(fileFullPath):
    # Checks if the files of server port info exists.
    if not os.path.isfile(fileFullPath):
        print("The port info file couldn't be found. check if exist and retry.")
        return

    # Open the server port info for reading.
    try:
        portInfoFile = open(fileFullPath,"r")
        fileConent = portInfoFile.read().split()
    except:
        print("Server's port file cannot open. please check the file and retry.")
        return

    # Checks if the file contains any data.
    if len(fileConent) == 0:
        print("The file is empty. check its content and retry.")
        return

    # Checks if the file content contains any other data except of the port.
    if len(fileConent) > 1:
        print("The file contains more than one argument. check its content and retry.")
        return

    # Gets the port from the file content.
    port = int(fileConent[0])

    # Validate the port number is a valid one.
    if not (port > 0 and port < MAX_PORT_VALUE):
        print("The port value is invalid. please check if valid and try again.")
        return

    portInfoFile.close()
    return port
    
if __name__ == '__main__':
    serverPort = getPortFromFile(PORT_INFO_FILE_PATH)
    # Creates the server socket and waiting for clients' communications. creates a thread for each client.
    try:
        server = startServer(serverPort)
        startReceive()
    except:
        print("Server failed. please try again.")
