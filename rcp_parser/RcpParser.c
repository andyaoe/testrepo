#include "RcpParser.h"
#include <stdlib.h>
#include <string.h>
#include "str_utils.h"

RcpParser_Result RcpParser_Init(RcpParser_Instance *rcpParserInstance, void* parserBuffer, uint16_t bufferSize)
{
	RcpParser_Result return_code = RCP_OK;
    rcpParserInstance->parserBufferStart = parserBuffer;
    rcpParserInstance->parserBufferEnd = (char*)parserBuffer + bufferSize;
    rcpParserInstance->writePointer = rcpParserInstance->parserBufferStart;
    rcpParserInstance->state = IDLE;
    return return_code;
}

RcpParser_Result RcpParser_Parse(RcpParser_Instance *rcpParserInstance, uint8_t byte)
{
    RcpParser_Result return_code = RCP_UNASSIGNED;
    bool no_delimiters_received = false;

    //Save the byte and increment buffer pointers
    rcpParserInstance->readPointer = rcpParserInstance->writePointer;
    *rcpParserInstance->writePointer = byte;
    rcpParserInstance->writePointer++;
    if (rcpParserInstance->writePointer == rcpParserInstance->parserBufferEnd)
    {
        rcpParserInstance->state = IDLE;
        rcpParserInstance->writePointer = rcpParserInstance->parserBufferStart;
        return_code = RCP_BUFFER_OVERFLOW;
        return return_code;
    }

    //Add to Checksum
    rcpParserInstance->runningChecksum += byte;

    //enter state machine (reference Confluence for Flow Diagram)
    if (rcpParserInstance->state == RECEIVE_BIN_DATA)
    {
        if (rcpParserInstance->numberOfBinBytesLeft == 0)
        {
            rcpParserInstance->state = ALL_BIN_DATA_RECEIVED;
        }
        else
        {
            rcpParserInstance->numberOfBinBytesLeft--;
            return_code = RCP_OK;
            return return_code;
        }
    }

    if (*rcpParserInstance->readPointer == '#')
    {
        rcpParserInstance->state = START;
        return_code = RCP_OK;
        rcpParserInstance->writePointer = rcpParserInstance->parserBufferStart;
        *rcpParserInstance->writePointer = '#';
        rcpParserInstance->writePointer++;
        rcpParserInstance->runningChecksum = '#';
        rcpParserInstance->receivedSource = false;
        rcpParserInstance->receivedTarget = false;
        rcpParserInstance->receivedCommand = false;
        rcpParserInstance->receivedName = false;
        rcpParserInstance->receivedValue = false;
        rcpParserInstance->parsedInfo.BinData = 0;
        rcpParserInstance->numberOfBinBytesLeft = 0;
        return return_code;
    }
    else if (rcpParserInstance->state == IDLE)
    {
        return_code = RCP_WAITING_FOR_START_DELIMITER;
        return return_code;
    }

    switch (byte)
    {
    case '$':
        if (rcpParserInstance->state == START)
        {
            rcpParserInstance->state = RECEIVE_SOURCE;
            return_code = RCP_OK;
            rcpParserInstance->byteCounter = 0;
            return return_code;
        }
        else
        {
            return_code = RCP_UNEXPECTED_DELIMITER;
            rcpParserInstance->state = IDLE;
            return return_code;
        }
        break;
    case '@':
        if (rcpParserInstance->state == SOURCE_RECEIVED)
        {
            *rcpParserInstance->readPointer = 0;
            rcpParserInstance->state = RECEIVE_TARGET;
            rcpParserInstance->receivedSource = true;
            rcpParserInstance->byteCounter = 0;
            return_code = RCP_OK;
            return return_code;
        }
        else
        {
            return_code = RCP_UNEXPECTED_DELIMITER;
            rcpParserInstance->state = IDLE;
            return return_code;
        }
        break;
    case ':':
        switch (rcpParserInstance->state)
        {
        case TARGET_RECEIVED:
            *rcpParserInstance->readPointer = 0;
            rcpParserInstance->state = RECEIVE_COMMAND;
            rcpParserInstance->receivedTarget = true;
            rcpParserInstance->byteCounter = 0;
            return_code = RCP_OK;
            return return_code;
            break;
        case COMMAND_RECEIVED:
            *rcpParserInstance->readPointer = 0;
            rcpParserInstance->state = RECEIVE_NAME;
            rcpParserInstance->receivedCommand = true;
            rcpParserInstance->byteCounter = 0;
            return_code = RCP_OK;
            return return_code;
            break;
        case NAME_RECEIVED:
            *rcpParserInstance->readPointer = 0;
            rcpParserInstance->state = RECEIVE_VALUE;
            rcpParserInstance->receivedName = true;
            rcpParserInstance->byteCounter = 0;
            return_code = RCP_OK;
            return return_code;
            break;
        case VALUE_RECEIVED:
            *rcpParserInstance->readPointer = 0;
            rcpParserInstance->receivedValue = true;
            if (0 == strcmp("BIN", rcpParserInstance->parsedInfo.Name))
            {
                rcpParserInstance->state = RECEIVE_BIN_DATA;
                rcpParserInstance->numberOfBinBytesLeft = (uint16_t)atoi(rcpParserInstance->parsedInfo.Value);
                rcpParserInstance->parsedInfo.BinData = rcpParserInstance->writePointer;
                return_code = RCP_OK;
                return return_code;
            }
            else if ((0 == strcmp("PIR1DAT", rcpParserInstance->parsedInfo.Name)) ||
                     (0 == strcmp("PIR2DAT", rcpParserInstance->parsedInfo.Name)) ||
                     (0 == strcmp("PIR3DAT", rcpParserInstance->parsedInfo.Name)))
            {
                rcpParserInstance->state = RECEIVE_BIN_DATA;
                rcpParserInstance->numberOfBinBytesLeft = (uint16_t)atoi(rcpParserInstance->parsedInfo.Value);
                rcpParserInstance->parsedInfo.BinData = rcpParserInstance->writePointer;
                return_code = RCP_OK;
                return return_code;
            }
            else
            {
                rcpParserInstance->state = RECEIVE_CHECKSUM_DELIMITER;
                return_code = RCP_OK;
                return return_code;
            }
            break;
        case ALL_BIN_DATA_RECEIVED:
            rcpParserInstance->state = RECEIVE_CHECKSUM_DELIMITER;
            return_code = RCP_OK;
            return return_code;
            break;
        default:
            return_code = RCP_UNEXPECTED_DELIMITER;
            rcpParserInstance->state = IDLE;
            return return_code;
            break;
        }
        break;
    case '*':
        if (rcpParserInstance->state == RECEIVE_CHECKSUM_DELIMITER)
        {
            rcpParserInstance->calculatedChecksum = rcpParserInstance->runningChecksum % 100;
            rcpParserInstance->state = RECEIVE_CHECKSUM;
            rcpParserInstance->byteCounter = 0;
            return_code = RCP_OK;
            return return_code;
        }
        else
        {
            return_code = RCP_UNEXPECTED_DELIMITER;
            rcpParserInstance->state = IDLE;
            return return_code;
        }
        break;
    case '\n':
        if (rcpParserInstance->state == CHECKSUM_RECEIVED)
        {
            *rcpParserInstance->readPointer = 0;
            rcpParserInstance->messageChecksum = (uint16_t)atoi(rcpParserInstance->parsedInfo.Checksum);
            if (rcpParserInstance->calculatedChecksum == rcpParserInstance->messageChecksum)
            {
                if (rcpParserInstance->receivedSource &&
                    rcpParserInstance->receivedTarget &&
                    rcpParserInstance->receivedCommand &&
                    rcpParserInstance->receivedName &&
                    rcpParserInstance->receivedValue)
                {
                    rcpParserInstance->state = IDLE;
                    return_code = RCP_SUCCESS;
                    return return_code;
                }
                else
                {
                    rcpParserInstance->state = IDLE;
                    return_code = RCP_INCOMPLETE_MESSAGE;
                }
            }
            else
            {
                return_code = RCP_CHECKSUM_ERROR;
                rcpParserInstance->state = IDLE;
                return return_code;
            }
        }
        else if (rcpParserInstance->state == RECEIVE_CHECKSUM || rcpParserInstance->state == ALL_BIN_DATA_RECEIVED)
        {
            rcpParserInstance->state = IDLE;
            return_code = RCP_UNEXPECTED_TERMINATOR;
            return return_code;
        }
        else
        {
            if (rcpParserInstance->receivedSource &&
                rcpParserInstance->receivedTarget &&
                rcpParserInstance->receivedCommand &&
                rcpParserInstance->receivedName &&
                rcpParserInstance->receivedValue)
            {
                rcpParserInstance->state = IDLE;
                return_code = RCP_SUCCESS;
                return return_code;
            }
            else
            {
                rcpParserInstance->state = IDLE;
                return_code = RCP_INCOMPLETE_MESSAGE;
            }
            return return_code;
        }
        break;
    default:
        no_delimiters_received = true;
        break;
    }

    if (no_delimiters_received)
    {
        switch (rcpParserInstance->state)
        {
        case RECEIVE_SOURCE:
            rcpParserInstance->byteCounter++;
            if (rcpParserInstance->byteCounter == 1)
            {
                rcpParserInstance->state = SOURCE_RECEIVED;
                rcpParserInstance->parsedInfo.Source = rcpParserInstance->readPointer;
                return_code = RCP_OK;
                return return_code;
            }
            else
            {
                return_code = RCP_TOO_MANY_BYTES;
                rcpParserInstance->state = IDLE;
                return return_code;
            }
            break;
        case RECEIVE_TARGET:
            rcpParserInstance->byteCounter++;
            if (rcpParserInstance->byteCounter == 1)
            {
                rcpParserInstance->state = TARGET_RECEIVED;
                rcpParserInstance->parsedInfo.Target = rcpParserInstance->readPointer;
                return_code = RCP_OK;
                return return_code;
            }
            else
            {
                return_code = RCP_TOO_MANY_BYTES;
                rcpParserInstance->state = IDLE;
                return return_code;
            }
            break;
        case RECEIVE_COMMAND:
            rcpParserInstance->byteCounter++;
            if (rcpParserInstance->byteCounter == 1)
            {
                rcpParserInstance->state = COMMAND_RECEIVED;
                rcpParserInstance->parsedInfo.Command = rcpParserInstance->readPointer;
                return_code = RCP_OK;
                return return_code;
            }
            else
            {
                return_code = RCP_TOO_MANY_BYTES;
                rcpParserInstance->state = IDLE;
                return return_code;
            }
            break;
        case RECEIVE_NAME:
        case NAME_RECEIVED:
            rcpParserInstance->byteCounter++;
            if (rcpParserInstance->byteCounter > 7)
            {
                return_code = RCP_TOO_MANY_BYTES;
                rcpParserInstance->state = IDLE;
                return return_code;
            }
            else
            {
                if (rcpParserInstance->byteCounter == 1)
                {
                    rcpParserInstance->parsedInfo.Name = rcpParserInstance->readPointer;
                    rcpParserInstance->state = NAME_RECEIVED;
                }
                rcpParserInstance->receivedName = true;
                return_code = RCP_OK;
                return return_code;
            }
            break;
        case RECEIVE_VALUE:
        case VALUE_RECEIVED:
            rcpParserInstance->byteCounter++;
            if (rcpParserInstance->byteCounter > 6)
            {
                return_code = RCP_TOO_MANY_BYTES;
                rcpParserInstance->state = IDLE;
                return return_code;
            }
            else
            {
                if (rcpParserInstance->byteCounter == 1)
                {
                    rcpParserInstance->parsedInfo.Value = rcpParserInstance->readPointer;
                    rcpParserInstance->state = VALUE_RECEIVED;
                }
                rcpParserInstance->state = VALUE_RECEIVED;
                return_code = RCP_OK;
                return return_code;
            }
            break;
        case RECEIVE_CHECKSUM:
            rcpParserInstance->byteCounter++;
            if (rcpParserInstance->byteCounter > 2)
            {
                return_code = RCP_TOO_MANY_BYTES;
                rcpParserInstance->state = IDLE;
                return return_code;
            }
            else if (rcpParserInstance->byteCounter == 1)
            {
                rcpParserInstance->parsedInfo.Checksum = rcpParserInstance->readPointer;
                return_code = RCP_OK;
                return return_code;
            }
            else
            {
                rcpParserInstance->state = CHECKSUM_RECEIVED;
                return_code = RCP_OK;
                return return_code;
            }
            break;
        default:
            rcpParserInstance->state = IDLE;
            return_code = RCP_UNEXPECTED_CHARACTER;
            return return_code;
        }
    }

    return return_code;
}



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
                                           uint16_t binDataLength)
{
    uint8_t field_length;
    uint8_t i;
    uint8_t checksum = 0;
    char *buffer = commandReturnBuffer;

    memset(commandReturnBuffer, 0, commandReturnBufferSize);

    //do some checks first
    if (binDataLength > 99)
    {
        return RCP_BIN_DATA_LENGTH_TOO_LONG;
    }

    *buffer = '#';
    checksum += *buffer;
    buffer++;
    *buffer = '$';
    checksum += *buffer;
    buffer++;

    if ((source != 'G') &&
        (source != 'M') &&
        (source != 'N'))
    {
        return RCP_SOURCE_INCORRECT;
    }

    *buffer = source;
    checksum += *buffer;
    buffer++;
    *buffer = '@';
    checksum += *buffer;
    buffer++;

    if ((target != 'G') &&
        (target != 'M') &&
        (target != 'N'))
    {
        return RCP_TARGET_INCORRECT;
    }

    *buffer = target;
    checksum += *buffer;
    buffer++;
    *buffer = ':';
    checksum += *buffer;
    buffer++;

    if ((command != 'G') &&
        (command != 'S') &&
        (command != 'C'))
    {
        return RCP_COMMAND_INCORRECT;
    }

    *buffer = command;
    checksum += *buffer;
    buffer++;
    *buffer = ':';
    checksum += *buffer;
    buffer++;

    //NAME
    field_length = strlen(name);
    if (field_length > 7)
    {
        return RCP_NAME_INCORRECT;
    }

    for (i = 0; i < field_length; i++)
    {
        *buffer = *name;
        checksum += *buffer;
        buffer++;
        name++;
    }

    *buffer = ':';
    checksum += *buffer;
    buffer++;

    //VALUE
    field_length = strlen(value);
    if (field_length > 6)
    {
        return RCP_VALUE_INCORRECT;
    }

    for (i = 0; i < field_length; i++)
    {
        *buffer = *value;
        checksum += *buffer;
        buffer++;
        value++;
    }

    *buffer = ':';
    checksum += *buffer;
    buffer++;

    //BIN
    if (binDataLength > 0)
    {
        for (i = 0; i < binDataLength; i++)
        {
            *buffer = *binData;
            checksum += *buffer;
            buffer++;
            binData++;
        }

        *buffer = ':';
        checksum += *buffer;
        buffer++;
    }

    //Checksum
    if (addChecksum)
    {
        *buffer = '*';
        checksum += *buffer;
        buffer++;

        checksum %= 100;
        if (checksum < 10)
        {
            *buffer = '0';
            buffer++;
            itoa(checksum, buffer, 10);
            buffer++;
        }
        else
        {
            itoa(checksum, buffer, 10);
            buffer += 2;
        }
    }

    *buffer = '\n';
    buffer++;
    *buffer = 0;

    *commandReturnBufferDataLength = (uint8_t)(buffer - commandReturnBuffer);

    return RCP_SUCCESS;
}
