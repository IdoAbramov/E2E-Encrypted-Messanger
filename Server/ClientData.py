# Client Data class represents a client with its username and public key.
class ClientData:
    def __init__(self, username, publicKey):
        self.username = username
        self.publicKey = publicKey

    def getUsername(self):
        return self.username
    
    def setUsername(self, uname):
        self.username = uname;

    def getPublicKey(self):
        return self.publicKey
    
    def setPublicKey(self, pubkey):
        self.publicKey = pubkey;

