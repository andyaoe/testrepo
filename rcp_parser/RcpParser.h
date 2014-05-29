#ifndef RCP_PARSER
#define RCP_PARSER

#include "stdint.h"

#define MAX_RCP_NAME_SIZE   7
#define MAX_RCP_VALUE_SIZE  6
#define MAX_RCP_BIN_SIZE    99

typedef enum RcpParser_State
{
    IDLE = 0,
    START,
    RECEIVE_SOURCE,
    SOURCE_RECEIVED,
    RECEIVE_TARGET,
    TARGET_RECEIVED,
    RECEIVE_COMMAND,
    COMMAND_RECEIVED,
    RECEIVE_NAME,
    NAME_RECEIVED,
    RECEIVE_VALUE,
    VALUE_RECEIVED,
    RECEIVE_CHECKSUM_DELIMITER,
    RECEIVE_CHECKSUM,
    CHECKSUM_RECEIVED,
    RECEIVE_BIN_DATA,
    BIN_DATA_RECEIVED,
    ALL_BIN_DATA_RECEIVED
} RcpParser_State;

typedef enum RcpParser_Result
{
    RCP_OK = 0,
    RCP_SUCCESS = 1,
    RCP_UNEXPECTED_DELIMITER = 2,
    RCP_UNEXPECTED_TERMINATOR = 3,
    RCP_TOO_MANY_BYTES = 4,
    RCP_CHECKSUM_ERROR = 5,
    RCP_UNASSIGNED = 6,
    RCP_WAITING_FOR_START_DELIMITER = 7,
    RCP_BUFFER_OVERFLOW = 8,
    RCP_UNEXPECTED_CHARACTER = 9,
    RCP_INCOMPLETE_MESSAGE = 10,
    RCP_BUFFER_TOO_SMALL,
    RCP_BIN_DATA_LENGTH_TOO_LONG,
    RCP_SOURCE_INCORRECT,
    RCP_TARGET_INCORRECT,
    RCP_COMMAND_INCORRECT,
    RCP_NAME_INCORRECT,
    RCP_VALUE_INCORRECT,
    RCP_LAST_RESULT
} RcpParser_Result;

typedef struct RcpParser_ParsedInfo
{
    void *Source;
    void *Target;
    void *Command;
    void *Name;
    void *Value;
    void *BinData;
    void *Checksum;
} RcpParser_ParsedInfo;

typedef struct RcpParser_Instance
{
    void *parserBufferStart;
    void *parserBufferEnd;
    char *writePointer;
    char *readPointer;
    RcpParser_State state;
    uint16_t numberOfBinBytesLeft;
    bool receivedSource;
    bool receivedTarget;
    bool receivedCommand;
    bool receivedName;
    bool receivedValue;
    RcpParser_ParsedInfo parsedInfo;
    uint8_t byteCounter;
    uint8_t calculatedChecksum;
    uint8_t messageChecksum;
    uint8_t runningChecksum;
} RcpParser_Instance;

RcpParser_Result RcpParser_Init(RcpParser_Instance *rcpParserInstance, void* parserBuffer, uint16_t bufferSize);
RcpParser_Result RcpParser_Parse(RcpParser_Instance *rcpParserInstance, uint8_t byte);
RcpParser_Result RcpParser_BuildFullPacket(char *commandReturnBuffer,
                                           uint8_t commandReturnBufferSize,
                                           uint8_t *commandReturnBufferDataLength,
                                           char source,
                                           char target,
                                           char command,
                                           char *name,
                                           char *value,
                                           bool addChecksum,
                                           char *binData,
                                           uint16_t binDataLength);


#endif // RCP_PARSER
