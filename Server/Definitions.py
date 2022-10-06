# all defenitions
from enum import *

# Constants

# Values
VERSION = 1
PORT_INFO_FILE_PATH = "port.info"
HOST = "127.0.0.1"  # localhost
MAX_PORT_VALUE = 65535
DATABASE_NAME = "server.db"

# Lengths
UUID_LEN = 16
USERNAME_LEN = 255
PUBLIC_KEY_LEN = 160
VERSION_LEN = 1
CODE_LEN = 2
PAYLOADSIZE_LEN = 4
MESSAGE_TYPE_LEN = 1
MESSAGE_CONTENT_LEN = 4
MESSAGE_ID_LEN = 4
REQUEST_HEADER_LEN = UUID_LEN + VERSION_LEN + CODE_LEN + PAYLOADSIZE_LEN
RESPONSE_HEADER_LEN = VERSION_LEN + CODE_LEN + PAYLOADSIZE_LEN

# The available request codes which are taking care by the server.
class RequestCodes(IntEnum):
    REGISTER_REQ = 1100,
    CLIENTS_LIST_REQ = 1101,
    GET_PUBLIC_KEY_REQ = 1102,
    SEND_MESSAGE_REQ = 1103,
    PULL_WAITING_MESSAGES_REQ = 1104

# The available response codes which the server can send to a client.
class ResponseCodes(IntEnum):
    REGISTER_SUCCESS_RESP = 2100,
    CLIENTS_LIST_RESP = 2101,
    GET_PUBLIC_KEY_RESP = 2102,
    MESSAGE_SENT_RESP = 2103,
    PULL_WAITING_MESSAGES_RESP = 2104,
    GENERAL_ERROR_RESP = 9000

# The available messages' types the server is taking care of.
class MessageTypes(IntEnum):
    ASK_SYM_KEY = 1,
    SEND_SYM_KEY = 2,
    SEND_TEXT_MSG = 3,
    SEND_TEXT_FILE = 4