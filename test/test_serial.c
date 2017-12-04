#include <stdio.h>
#include "unity.h"
#include "serial.h"
#include "consts.h"
#include "mock_USBUART.h"
#include "mock_USBUART_cdc.h"
#include "mock_CyLib.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_WhenGetConfigurationReturns0AndTimeout_ThenCDCInitNotCalled(void)
{
    int ii;
    
    USBUART_Start_Expect(0, 1);
    for (ii = 0; ii < 10; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(0);
        CyDelayUs_Expect(1);
    }
    USBUART_GetConfiguration_ExpectAndReturn(0);

    // When
    Ser_Start();
}

void test_WhenNoTimeout_ThenCDCInitIsCalled(void)
{
    int ii;
    
    USBUART_Start_Expect(0, 1);
    for (ii = 0; ii < 5; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(0);
        CyDelayUs_Expect(1);
    }
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDC_Init_ExpectAndReturn(0);

    // When
    Ser_Start();
}

void test_WhenCDCIsNotReady_ThenNoStringIsOutput(void)
{
    int ii;
    
    for (ii = 0; ii < 10; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(0);
        CyDelayUs_Expect(1);
    }
    
    // When
    Ser_PutString("This is a string");
}

void test_WhenCDCIsReady_ThenStringIsOutput(void)
{
    int ii;
    
    for (ii = 0; ii < 5; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(0);
        CyDelayUs_Expect(1);
    }
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDCIsReady_ExpectAndReturn(1);
    
    USBUART_PutString_Expect("This is a string");
    
    // When
    Ser_PutString("This is a string");
}

void test_WhenInvalidConfiguration_ThenZeroLengthReturned(void)
{
    char data[10];
    int count;
    
    USBUART_GetConfiguration_ExpectAndReturn(0);
    
    // When
    count = Ser_ReadData(data);
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(count, 0);
}

void test_WhenValidConfigurationAndDataNotReady_ThenZeroLengthReturned(void)
{
    char data[10];
    int count;
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(0);
    
    // When
    count = Ser_ReadData(data);
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(count, 0);
}

void test_WhenValidConfigurationAndDataIsReadyAndCountNotZero_ThenCorrectDataAndLengthIsRead(void)
{
    #define TEXT_DATA "this is text"
    char data[64];
    int count;
    int text_data_length = strlen(TEXT_DATA);
    
    uint16 mock_USBUART_GetAll(UINT8 *pdata, int call_count)
    {
        memcpy(pdata, TEXT_DATA, text_data_length);
        return text_data_length;
    }
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetAll_StubWithCallback(mock_USBUART_GetAll);
    
    // When
    count = Ser_ReadData(data);
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(count, text_data_length);
    TEST_ASSERT_EQUAL_STRING(data, TEXT_DATA);
}

void test_WhenValidConfigurationAndDataIsReadyAndCountIsZero_ThenZeroLengthReturned(void)
{
    #define TEXT_DATA "this is text"
    char data[64];
    int count;
    int text_data_length = strlen(TEXT_DATA);
    
    uint16 mock_USBUART_GetAll(UINT8 *pdata, int call_count)
    {
        return 0;
    }
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetAll_StubWithCallback(mock_USBUART_GetAll);
    
    // When
    count = Ser_ReadData(data);
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(count, 0);
}

void test_WhenInvalidConfiguration_ThenZeroReturned(void)
{
    UINT8 ch;
    
    USBUART_GetConfiguration_ExpectAndReturn(0);
    
    // When
    ch = Ser_ReadByte();
    
    // Then
    TEST_ASSERT_EQUAL_UINT8(0, ch);
}

void test_WhenValidConfigurationAndDataNotReady_ThenZeroReturned(void)
{
    UINT8 ch;
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(0);
    
    // When
    ch = Ser_ReadByte();
    
    // Then
    TEST_ASSERT_EQUAL_UINT8(0, ch);
}

void test_WhenValidConfigurationAndDataIsReadyAndCharReturned_ThenValidCharIsReturned(void)
{
    UINT8 ch;
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('c');
    
    // When
    ch = Ser_ReadByte();
    
    // Then
    TEST_ASSERT_EQUAL_UINT8('c', ch);
}

/* No Echo
   1. Test Ser_GetByte returns 0x00, Ser_ReadLine returns 0
   2. Test Ser_ReadByte returns '\n', Ser_ReadLine returns null string
   3. Test Ser_ReadByte returns '\r', Ser_ReadLine returns null string
   4. Test Ser_ReadByte returns "123\n", Ser_ReadLine returns "123\n"
   5. Test Ser_ReadByte returns "123\r", Ser_ReadLine returns "123\r"
   
   Echo
   Repeat 4 and 5, validate data is written to Ser_WriteByte

 */
void test_WhenNullCharReturned_ThenZeroIsReturned(void)
{
    UINT8 result;
    UINT8 data[10];

    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn(0);


    // When
    result = Ser_ReadLine(data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

void test_WhenNewLineReturned_ThenNewLineIsReturned(void)
{
    UINT8 result;
    UINT8 data[10];

    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('\n');


    // When
    result = Ser_ReadLine(data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_STRING("", data);
}

void test_WhenLineReturnReturned_ThenLineReturnReturned(void)
{
    UINT8 result;
    UINT8 data[10];

    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('\r');


    // When
    result = Ser_ReadLine(data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_STRING("", data);
}

void test_WhenLineDataWithNewLine_ThenDataIsReturned(void)
{
    UINT8 result1;
    UINT8 result2;
    UINT8 result3;
    UINT8 result4;
    UINT8 data[10] = {0};
  
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('1');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('2');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('3');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('\n');


    // When
    result1 = Ser_ReadLine(data, FALSE, 10);
    result2 = Ser_ReadLine(data, FALSE, 10);
    result3 = Ser_ReadLine(data, FALSE, 10);
    result4 = Ser_ReadLine(data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result1);
    TEST_ASSERT_EQUAL_UINT8(0, result2);
    TEST_ASSERT_EQUAL_UINT8(0, result3);
    TEST_ASSERT_EQUAL_UINT8(3, result4);
    TEST_ASSERT_EQUAL_STRING("123", data);
}

void test_WhenLineDataWithLineReturn_ThenDataIsReturned(void)
{
    UINT8 result1;
    UINT8 result2;
    UINT8 result3;
    UINT8 result4;
    UINT8 data[10] = {0};
  
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('1');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('2');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('3');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('\r');


    // When
    result1 = Ser_ReadLine(data, FALSE, 10);
    result2 = Ser_ReadLine(data, FALSE, 10);
    result3 = Ser_ReadLine(data, FALSE, 10);
    result4 = Ser_ReadLine(data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result1);
    TEST_ASSERT_EQUAL_UINT8(0, result2);
    TEST_ASSERT_EQUAL_UINT8(0, result3);
    TEST_ASSERT_EQUAL_UINT8(3, result4);
    TEST_ASSERT_EQUAL_STRING("123", data);
}

void test_WhenLineDataWithNewLineAndEcho_ThenDataIsReturnedAndOutput(void)
{
    UINT8 result1;
    UINT8 result2;
    UINT8 result3;
    UINT8 result4;
    UINT8 data[10] = {0};
  
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('1');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDCIsReady_ExpectAndReturn(1);
    USBUART_PutChar_Expect('1');
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('2');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDCIsReady_ExpectAndReturn(1);
    USBUART_PutChar_Expect('2');

    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('3');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDCIsReady_ExpectAndReturn(1);
    USBUART_PutChar_Expect('3');

    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('\n');


    // When
    result1 = Ser_ReadLine(data, TRUE, 10);
    result2 = Ser_ReadLine(data, TRUE, 10);
    result3 = Ser_ReadLine(data, TRUE, 10);
    result4 = Ser_ReadLine(data, TRUE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result1);
    TEST_ASSERT_EQUAL_UINT8(0, result2);
    TEST_ASSERT_EQUAL_UINT8(0, result3);
    TEST_ASSERT_EQUAL_UINT8(3, result4);
    TEST_ASSERT_EQUAL_STRING("123", data);
}

void test_WhenLineDataWithLineReturnAndEcho_ThenDataIsReturnedAndOutput(void)
{
    UINT8 result1;
    UINT8 result2;
    UINT8 result3;
    UINT8 result4;
    UINT8 data[10] = {0};
  
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('1');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDCIsReady_ExpectAndReturn(1);
    USBUART_PutChar_Expect('1');
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('2');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDCIsReady_ExpectAndReturn(1);
    USBUART_PutChar_Expect('2');

    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('3');
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDCIsReady_ExpectAndReturn(1);
    USBUART_PutChar_Expect('3');

    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('\r');


    // When
    result1 = Ser_ReadLine(data, TRUE, 10);
    result2 = Ser_ReadLine(data, TRUE, 10);
    result3 = Ser_ReadLine(data, TRUE, 10);
    result4 = Ser_ReadLine(data, TRUE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result1);
    TEST_ASSERT_EQUAL_UINT8(0, result2);
    TEST_ASSERT_EQUAL_UINT8(0, result3);
    TEST_ASSERT_EQUAL_UINT8(3, result4);
    TEST_ASSERT_EQUAL_STRING("123", data);
}

void test_WhenMaxLengthLessThanMaxPossibleAndDataLengthGreaterThanMaxLength_ThenMaxLengthDataReturned(void)
{
    UINT8 test_data[] = "0123456789012345678901234";
    UINT8 expected_data[] = "0123456784";
    UINT8 results[25];
    UINT8 data[10] = {0};
    int ii;

    for (ii = 0; ii < 25; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(1);
        USBUART_DataIsReady_ExpectAndReturn(1);
        USBUART_GetChar_ExpectAndReturn(test_data[ii]);
        USBUART_GetConfiguration_ExpectAndReturn(1);
        USBUART_CDCIsReady_ExpectAndReturn(1);
        USBUART_PutChar_Expect(test_data[ii]);

    }
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('\n');

    // When
    for (ii = 0; ii < 10; ++ii)
    {
        results[ii] = Ser_ReadLine(data, TRUE, 10);
        printf("Results: %d\n", results[ii]);
        printf("Data: %d\n", data[ii]);
    }

    // Then
    TEST_ASSERT_EACH_EQUAL_UINT8(0, results, 10);
    TEST_ASSERT_EQUAL_UINT8(9, results[9]);
    TEST_ASSERT_EQUAL_STRING(expected_data, data);
}

void test_WhenLineDataExceedsMaxLineLength_ThenNewDataIsDropped(void)
{
    UINT8 test_data[] =     "1234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234";
    UINT8 expected_data[] = "123456781234567812345678123456781234567812345678123456781234567";
    UINT8 results[101];
    UINT8 data[100] = {0};
    int ii;

    for (ii = 0; ii < 100; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(1);
        USBUART_DataIsReady_ExpectAndReturn(1);
        USBUART_GetChar_ExpectAndReturn(test_data[ii]);
        USBUART_GetConfiguration_ExpectAndReturn(1);
        USBUART_CDCIsReady_ExpectAndReturn(1);
        USBUART_PutChar_Expect(test_data[ii]);

    }
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('\n');


    // When
    for (ii = 0; ii < 101; ++ii)
    {
        results[ii] = Ser_ReadLine(data, TRUE, 0);
    }

    // Then
    TEST_ASSERT_EACH_EQUAL_UINT8(0, results, 100);
    TEST_ASSERT_EQUAL_UINT8(63, results[100]);
    TEST_ASSERT_EQUAL_STRING(expected_data, data);
    
}