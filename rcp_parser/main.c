#include <stdio.h>
#include <stdlib.h>
#include "RcpParser.h"
#include <string.h>

typedef struct ResultCodeDefinition
{
    int16_t code;
    char* definition;
} ResultCodeDefinition_t;

ResultCodeDefinition_t ResultCodeDefinitions[] = {
    {RCP_SUCCESS, "RCP_SUCCESS"},
    {RCP_OK, "RCP_OK"},
    {RCP_UNEXPECTED_DELIMITER, "RCP_UNEXPECTED_DELIMITER"},
    {RCP_UNEXPECTED_TERMINATOR, "RCP_UNEXPECTED_TERMINATOR"},
    {RCP_TOO_MANY_BYTES, "RCP_TOO_MANY_BYTES"},
    {RCP_CHECKSUM_ERROR, "RCP_CHECKSUM_ERROR"},
    {RCP_UNASSIGNED, "RCP_UNASSIGNED"},
    {RCP_WAITING_FOR_START_DELIMITER, "RCP_WAITING_FOR_START_DELIMITER"},
    {RCP_BUFFER_OVERFLOW, "RCP_BUFFER_OVERFLOW"},
    {RCP_UNEXPECTED_CHARACTER, "RCP_UNEXPECTED_CHARACTER"},
    {RCP_INCOMPLETE_MESSAGE, "RCP_INCOMPLETE_MESSAGE"}
};

bool TestCommand(uint8_t testNumber, char* testString, RcpParser_Result expectedResult);

RcpParser_Instance rcp_parser_instance;

int main()
{
    uint16_t i, string_length;
    int16_t result;
    int16_t num_passed_tests = 0;
    int16_t num_failed_tests = 0;
    RcpParser_ParsedInfo* parsed_info;
    char* test_string_1 = "#$G@M:G:POT1:0:\n";
    char* test_string_2 = "#G@M:G:POT1:0:\n";
    char* test_string_3 = "#$GM:G:POT1:0:\n";
    char* test_string;
    char parser_buffer[128];
    char builder_buffer[128];
    uint8_t rcp_command_length;

    printf("Welcome to the RCP Parser Tester!\n");

    RcpParser_Init(&rcp_parser_instance, parser_buffer, 128);

    if (TestCommand(1, "#$G@M:G:POT1:0:\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(2, "#G@M:G:POT1:0:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(3, "#$GM:G:POT1:0:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(4, "#$G@MG:POT1:0:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(5, "#$G@M:GPOT1:0:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(6, "#$G@M:G:POT10:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(7, "#$G@M:G:POT1:0:8\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(8, "#$G1@M:G:POT1:0:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(9, "#$G@M1:G:POT1:0:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(10, "#$G@M:G1:POT1:0:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(11, "#$G@M:G:POT12345:0:\n", RCP_TOO_MANY_BYTES)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(12, "#$G@M:G:POT1:0123456:\n", RCP_TOO_MANY_BYTES)) num_passed_tests++; else num_failed_tests++;

    if (TestCommand(20, "$G@M:G:POT1:0:\n", RCP_WAITING_FOR_START_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(21, "#@M:G:POT1:0:\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(22, "#$:G:POT1:0:\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(23, "#:G@M:G:POT1:0:\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(24, "#$:G@M:G:POT1:0:\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(25, "#$G:@M:G:POT1:0:\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(26, "#$G@:M:G:POT1:0:\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(27, "#$G@M::G:POT1:0:\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(28, "#$G@M:G::POT1:0:\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(29, "#$G@M:G:POT1::0:\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(30, "#$G@M:G:POT1::0::\n", RCP_UNEXPECTED_DELIMITER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(31, "#$G@M:G:P:OT1:0:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;

    if (TestCommand(40, "#\n$G@M:G:POT1:0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(41, "#$\nG@M:G:POT1:0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(42, "#$G\n@M:G:POT1:0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(43, "#$G@M\n:G:POT1:0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(44, "#$G@M:\nG:POT1:0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(45, "#$G@M:G\n:POT1:0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(46, "#$G@M:G:\nPOT1:0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(47, "#$G@M:G:P\nOT1:0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(48, "#$G@M:G:POT1\n:0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(49, "#$G@M:G:POT1:\n0:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(50, "#$G@M:G:POT1:0\n:\n", RCP_INCOMPLETE_MESSAGE)) num_passed_tests++; else num_failed_tests++;

    if (TestCommand(60, "#$G@M:G:POT1:0:*00\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(61, "#$G@M:G:POT1:0:*09\n", RCP_CHECKSUM_ERROR)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(62, "#$G@M:G:POT1:0:*99\n", RCP_CHECKSUM_ERROR)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(63, "#$G@M:G:POT1:0:*8\n", RCP_UNEXPECTED_TERMINATOR)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(64, "#$G@M:G:POT1:0:*\n", RCP_UNEXPECTED_TERMINATOR)) num_passed_tests++; else num_failed_tests++;


    if (TestCommand(70, "#$G@M:S:BIN:0::\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(71, "#$G@M:S:BIN:1:0:\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(72, "#$G@M:S:BIN:10:0123456789:\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(73, "#$G@M:S:BIN:9:01234567:\n", RCP_UNEXPECTED_TERMINATOR)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(74, "#$G@M:S:BIN:99:0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(75, "#$G@M:S:BIN:99:012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678:\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(76, "#$G@M:S:BIN:99:01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;

    if (TestCommand(80, "#$G@M:S:PIR1DAT:0::\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(81, "#$G@M:S:PIR1DAT:1:0:\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(82, "#$G@M:S:PIR1DAT:10:0123456789:\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(83, "#$G@M:S:PIR1DAT:9:012345678:\n", RCP_UNEXPECTED_TERMINATOR)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(84, "#$G@M:S:PIR1DAT:99:0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(85, "#$G@M:S:PIR1DAT:99:012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678:\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;
    if (TestCommand(86, "#$G@M:S:PIR1DAT:99:01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890:\n", RCP_UNEXPECTED_CHARACTER)) num_passed_tests++; else num_failed_tests++;

    if (TestCommand(90, "\r\nHi GS!\r\n", RCP_SUCCESS)) num_passed_tests++; else num_failed_tests++;



    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer),  &rcp_command_length, 'G', 'M', 'G', "1234567", "123456", false, 0, 0);
    printf("Packet 1: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'G', 'N', 'S', "1234567", "123456", true, 0, 0);
    printf("Packet 2: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'M', 'G', 'C', "1234567", "123456", false, 0, 0);
    printf("Packet 3: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'M', 'N', 'G', "1234567", "123456", true, 0, 0);
    printf("Packet 4: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'N', 'G', 'S', "1234567", "123456", false, 0, 0);
    printf("Packet 5: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'N', 'G', 'S', "BIN", "20", false, "01234567890123456789", 20);
    printf("Packet 6: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'N', 'G', 'S', "BIN", "99", true, "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678", 99);
    printf("Packet 7: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'G', 'M', 'G', "12345678", "123456", false, 0, 0);
    printf("Packet 8: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'G', 'M', 'G', "1234567", "1234567", false, 0, 0);
    printf("Packet 9: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'F', 'M', 'G', "1234567", "123456", false, 0, 0);
    printf("Packet 10: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'G', 'F', 'G', "1234567", "123456", false, 0, 0);
    printf("Packet 11: %s\r\n", builder_buffer);
    RcpParser_BuildFullPacket(builder_buffer, sizeof(builder_buffer), &rcp_command_length, 'G', 'M', 'F', "1234567", "123456", false, 0, 0);
    printf("Packet 12: %s\r\n", builder_buffer);


    //TestCommand(100, "#$G@M:G:POT1:0:", "No response");
    printf("Number of Passed Tests: %d\r\n", num_passed_tests);
    printf("Number of Failed Tests: %d\r\n", num_failed_tests);

    while(1);

    return 0;
}

bool TestCommand(uint8_t testNumber, char* testString, RcpParser_Result expectedResult)
{
    char *test_string, *result_string, *expected_result_string = "NULL";
    uint16_t string_length;
    int16_t result, i;

    for (i = 0; i < RCP_LAST_RESULT; i++)
    {
        if (ResultCodeDefinitions[i].code == expectedResult)
        {
            expected_result_string = ResultCodeDefinitions[i].definition;
            break;
        }
    }

    printf("-------------------------------------------\r\n");
    printf("Test Number: %d\r\n", testNumber);
    printf("Testing Command: %s", testString);
    printf("Expected Result: %s\r\n", expected_result_string);

    test_string = testString;
    string_length = strlen(testString);
    for (i = 0; i < string_length; i++)
    {
        result = RcpParser_Parse(&rcp_parser_instance, *test_string);
        if (result == RCP_WAITING_FOR_START_DELIMITER)
        {
            if (result == expectedResult)
            {
                printf("RCP_SUCCESS, Result was RCP_WAITING_FOR_START_DELIMITER\r\n");
            }
            printf("Done, waiting for start delimiter\r\n");
            return true;
        }
        else if (result == RCP_SUCCESS)
        {
            printf("Source: %s\r\n", rcp_parser_instance.parsedInfo.Source);
            printf("Target: %s\r\n", rcp_parser_instance.parsedInfo.Target);
            printf("Command: %s\r\n", rcp_parser_instance.parsedInfo.Command);
            printf("Name: %s\r\n", rcp_parser_instance.parsedInfo.Name);
            printf("Value: %s\r\n", rcp_parser_instance.parsedInfo.Value);

            if (rcp_parser_instance.parsedInfo.BinData != 0)
            {
                printf("BinData: %c\r\n", *((char*)rcp_parser_instance.parsedInfo.BinData));
            }
            return true;
        }
        else if (result != RCP_OK)
        {
            if (result == expectedResult)
            {
                printf("SUCCESS, Result was %s\r\n", expected_result_string);
                return true;
            }
            else
            {
                for (i = 0; i < sizeof(ResultCodeDefinitions)/sizeof(RcpParser_Result); i++)
                {
                    if (ResultCodeDefinitions[i].code == result)
                    {
                        result_string = ResultCodeDefinitions[i].definition;
                    }
                }

                printf("FAIL!!!, Result was %s\r\n", result_string);
                return false;
            }
        }
        test_string++;
    }
}
