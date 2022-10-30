# Message Data class represents a message with its details and content.
class MessageData:
    ID = 1 # initial value of message ID
    def __init__(self, destUUID, srcUUID, type, content, size):
        self.msgID = MessageData.ID
        MessageData.ID += 1
        self.destUUID = destUUID
        self.srcUUID = srcUUID
        self.type = type
        self.content = content
        self.size = size
        
 # Only getters, since the message details are constant once it is created.

    def getMessageID(self):
        return self.msgID

    def getDestUUID(self):
        return self.destUUID

    def getSrcUUID(self):
        return self.srcUUID

    def getMessageType(self):
        return self.type

    def getMessageContent(self):
        return self.content

    def getMessageSize(self):
        return self.size
