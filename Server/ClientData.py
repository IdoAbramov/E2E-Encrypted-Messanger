# Client Data class represents a client with its username and public key.
class ClientData:
    def __init__(self, username, public_key):
        self.username = username
        self.public_key = public_key

    def get_public_key(self):
        return self.public_key

    def get_username(self):
        return self.username
