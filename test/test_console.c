#include "unity.h"
#include "freesoc.h"
#include "console.h"
#include "mock_serial.h"
#include "mock_parser.h"
#include "mock_disp.h"

/* Note: There are two level of testing in this module.  We need to test the activate/inactivate console
   functionality (with mocking) and then integrate that mocking into the setup and tear down for subsequent
   tests.  These state variables are tracking when the activate/inactivate functionality is tested and
   ready.

   Using groups would be a good way to handle this (or separate files).  Group 1 would test only activate
   and inactivate, group 2 would test the balance. But, alas, I don't know how to do groups with ceedling (yet).

*/
static BOOL setup_activated = FALSE;
static BOOL teardown_activated = FALSE;
static BOOL console_active_ready;
static BOOL console_inactive_ready;

static void make_console_active()
{
    CHAR line[127];
    INT8 length = 127;

    memset(line, 0, 127);

    printf("Activating Console ...\n");

    Parser_Init_Expect();
    Disp_Init_Expect();
    Parser_Start_Expect();
    Disp_Start_Expect();
    Disp_IsRunning_ExpectAndReturn(FALSE);
    Ser_ReadLine_ExpectAndReturn(line, TRUE, length, 0);
    Disp_IsRunning_ExpectAndReturn(FALSE);
    Ser_ReadLine_ExpectAndReturn(line, TRUE, length, 0);
    Disp_IsRunning_ExpectAndReturn(FALSE);
    Ser_ReadLine_ExpectAndReturn(line, TRUE, length, 0);
    Ser_WriteLine_Expect("", TRUE);
    Ser_WriteLine_Expect("arlobot> ", FALSE);


    Console_Init();
    Console_Start();
    Console_Update();
    Console_Update();
    Console_Update();

    printf("Console is Active\n");
}

static void make_console_inactive()
{
    INT8 Ser_ReadLine_ReturnsExit(CHAR * const line, BOOL echo, UINT8 max_length,  int cmock_num_calls)
    {
        strcpy(line, "exit");
        return strlen("exit");
    }

    void Parser_Parse_Exit(CHAR* const line, COMMAND_TYPE* const cmd, int cmock_num_calls)
    {
        cmd->is_exit = TRUE;
    }

    printf("Deactivating Console ...\n");

    Disp_IsRunning_ExpectAndReturn(FALSE);
    Ser_ReadLine_StubWithCallback(Ser_ReadLine_ReturnsExit);
    Parser_Parse_StubWithCallback(Parser_Parse_Exit);
    Ser_WriteLine_Expect("", TRUE);
    Ser_WriteLine_Expect("goodbye", TRUE);    
    
    Console_Update();

    printf("Console is Inactive\n");

}

void setUp(void)
{
    printf("\nSetup Called %d %d\n", console_active_ready, setup_activated);
    if (setup_activated)
    {
        make_console_active();
    }
}

void tearDown(void)
{
    printf("Teardown Called %d %d\n", console_inactive_ready, teardown_activated);
    if (teardown_activated)
        make_console_inactive();

    if (console_active_ready && console_inactive_ready)
    {
        setup_activated = TRUE;
        teardown_activated = TRUE;
    }
}

void test_console_WhenConsoleIsInactiveTypeEnterThreeTimes_ConsoleBecomesActiveAndDisplaysPrompt(void)
{
    //TEST_IGNORE();
    printf("Begin WhenConsoleIsInactiveTypeEnterThreeTimes_ConsoleBecomesActiveAndDisplaysPrompt\n");
    make_console_active();
    printf("End WhenConsoleIsInactiveTypeEnterThreeTimes_ConsoleBecomesActiveAndDisplaysPrompt\n");
}

void test_console_WhenConsoleIsActiveTypeExit_ConsoleBecomeInactiveAndDisplaysGoodbye(void)
{
    //TEST_IGNORE();
    printf("Begin WhenConsoleIsActiveTypeExit_ConsoleBecomeInactiveAndDisplaysGoodbye\n");
    make_console_inactive();
    printf("End WhenConsoleIsActiveTypeExit_ConsoleBecomeInactiveAndDisplaysGoodbye\n");

    console_active_ready = TRUE;
    console_inactive_ready = TRUE;
}

void test_console_WhenConsoleIsActiveAndEnterPressed_NewPromptDisplayed(void)
{
    INT8 Ser_ReadLine_ReturnsNewLine(CHAR * const line, BOOL echo, UINT8 max_length,  int cmock_num_calls)
    {
        strcpy(line, "");
        return 0;
    }
    
    //TEST_IGNORE();
    
    printf("Begin WhenConsoleIsActiveAndEnterPressed_NewPromptDisplayed\n");
    Disp_IsRunning_ExpectAndReturn(FALSE);
    Ser_ReadLine_StubWithCallback(Ser_ReadLine_ReturnsNewLine);
    Ser_WriteLine_Expect("", TRUE);
    Ser_WriteLine_Expect("arlobot> ", FALSE);

    Console_Update();
    printf("End WhenConsoleIsActiveAndEnterPressed_NewPromptDisplayed\n");
}

void test_console_WhenConsoleIsActiveAndNotParsedCommand_ConsoleDisplayUnknownCommandMessageAndPrompt(void)
{
    INT8 Ser_ReadLine_InvalidCommand(CHAR * const line, BOOL echo, UINT8 max_length,  int cmock_num_calls)
    {
        strcpy(line, "invalid command");
        return strlen("invalid command");
    }

    void Parser_Parse_InvalidCommand(CHAR* const line, COMMAND_TYPE* const cmd, int cmock_num_calls)
    {
        cmd->is_parsed = FALSE;
    }

    //TEST_IGNORE();

    printf("Begin WhenConsoleIsActiveAndNotParsedCommand_ConsoleDisplayUnknownCommandMessageAndPrompt\n");

    Disp_IsRunning_ExpectAndReturn(FALSE);
    Ser_ReadLine_StubWithCallback(Ser_ReadLine_InvalidCommand);
    Ser_WriteLine_Expect("", TRUE);
    Parser_Parse_StubWithCallback(Parser_Parse_InvalidCommand);
    Ser_WriteLine_Expect("unknown command", TRUE);    
    Ser_WriteLine_Expect("", TRUE);
    Ser_WriteLine_Expect("arlobot> ", FALSE);

    Console_Update();

    printf("End WhenConsoleIsActiveAndNotParsedCommand_ConsoleDisplayUnknownCommandMessageAndPrompt\n");
}

void test_console_WhenConsoleIsActiveAndInvalidCommand_ConsoleDisplaysProcessingMessageAndPrompt(void)
{
    INT8 Ser_ReadLine_ValidCommand(CHAR * const line, BOOL echo, UINT8 max_length,  int cmock_num_calls)
    {
        strcpy(line, "valid command");
        return strlen("valid command");
    }

    void Parser_Parse_ValidCommand(CHAR* const line, COMMAND_TYPE* const cmd, int cmock_num_calls)
    {
        cmd->is_parsed = TRUE;
    }

    void Disp_Dispatch_InvalidCommand(COMMAND_TYPE* const command, int cmock_num_calls)
    {
        command->is_valid = FALSE;
    }

    //TEST_IGNORE();

    printf("Begin WhenConsoleIsActiveAndInvalidCommand_ConsoleDisplaysProcessingMessageAndPrompt\n");

    make_console_active();

    Disp_IsRunning_ExpectAndReturn(FALSE);
    Ser_ReadLine_StubWithCallback(Ser_ReadLine_ValidCommand);
    Ser_WriteLine_Expect("", TRUE);
    Parser_Parse_StubWithCallback(Parser_Parse_ValidCommand);
    Disp_Dispatch_StubWithCallback(Disp_Dispatch_InvalidCommand);
    Ser_WriteLine_Expect("invalid command parameters", TRUE);
    Ser_WriteLine_Expect("", TRUE);
    Ser_WriteLine_Expect("arlobot> ", FALSE);

    Console_Update();

    printf("End WhenConsoleIsActiveAndInvalidCommand_ConsoleDisplaysProcessingMessageAndPrompt\n");
}
