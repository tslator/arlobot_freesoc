#include "unity.h"
#include "freesoc.h"
#include "console.h"
#include "mock_serial.h"

static void make_console_active()
{
    CHAR line[64];
    UINT8 length = 64;

    memset(line, 0, 64);

    Ser_ReadLine_ExpectAndReturn(line, TRUE, length, 0);
    Ser_ReadLine_ExpectAndReturn(line, TRUE, length, 0);
    Ser_ReadLine_ExpectAndReturn(line, TRUE, length, 0);
    Ser_WriteLine_Expect("arlobot> ", FALSE);


    Console_Init();
    Console_Start();
    Console_Update();
    Console_Update();
    Console_Update();
}

static void make_console_inactive()
{
    INT8 Ser_ReadLine_ReturnsExit(CHAR * const line, BOOL echo, UINT8 max_length,  int cmock_num_calls)
    {
        printf("calls %d\n", cmock_num_calls);
        strcpy(line, "exit");
        return strlen("exit");
    }

    Ser_ReadLine_StubWithCallback(Ser_ReadLine_ReturnsExit);
    Ser_WriteLine_Expect("goodbye", TRUE);    
    
    Console_Update();
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_console_WhenConsoleIsInactiveTypeEnterThreeTimes_ConsoleBecomesActiveAndDisplaysPrompt(void)
{
    printf("Begin WhenConsoleIsInactiveTypeEnterThreeTimes_ConsoleBecomesActiveAndDisplaysPrompt\n");
    make_console_active();
    printf("End WhenConsoleIsInactiveTypeEnterThreeTimes_ConsoleBecomesActiveAndDisplaysPrompt\n");
}

void test_console_WhenConsoleIsActiveTypeExit_ConsoleBecomeInactiveAndDisplaysGoodbye(void)
{
    printf("Begin WhenConsoleIsActiveTypeExit_ConsoleBecomeInactiveAndDisplaysGoodbye\n");
    make_console_inactive();
    printf("End WhenConsoleIsActiveTypeExit_ConsoleBecomeInactiveAndDisplaysGoodbye\n");
}

void test_console_WhenConsoleIsActiveAndInvalidCommand_ConsoleDisplayUnknownCommandMessageAndPrompt(void)
{
    INT8 Ser_ReadLine_InvalidCommand(CHAR * const line, BOOL echo, UINT8 max_length,  int cmock_num_calls)
    {
        strcpy(line, "invalid command");
        return strlen("invalid command");
    }

    printf("Begin WhenConsoleIsActiveAndInvalidCommand_ConsoleDisplayUnknownCommandMessageAndPrompt\n");

    make_console_active();

    Ser_ReadLine_StubWithCallback(Ser_ReadLine_InvalidCommand);
    Ser_WriteLine_Expect("unknown command", TRUE);    
    Ser_WriteLine_Expect("arlobot> ", FALSE);

    Console_Update();

    printf("End WhenConsoleIsActiveAndInvalidCommand_ConsoleDisplayUnknownCommandMessageAndPrompt\n");
}

void test_console_WhenConsoleIsActiveAndValidCommand_ConsoleDisplaysProcessingMessageAndPrompt(void)
{
    INT8 Ser_ReadLine_InvalidCommand(CHAR * const line, BOOL echo, UINT8 max_length,  int cmock_num_calls)
    {
        strcpy(line, "valid command");
        return strlen("valid command");
    }

    printf("Begin WhenConsoleIsActiveAndValidCommand_ConsoleDisplaysProcessingMessageAndPrompt\n");

    make_console_active();

    Ser_ReadLine_StubWithCallback(Ser_ReadLine_InvalidCommand);
    Ser_WriteLine_Expect("processing command", TRUE);    
    Ser_WriteLine_Expect("arlobot> ", FALSE);

    Console_Update();

    printf("End WhenConsoleIsActiveAndValidCommand_ConsoleDisplaysProcessingMessageAndPrompt\n");
}
