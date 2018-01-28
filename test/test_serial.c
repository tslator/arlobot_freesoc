#include <stdio.h>
#include "unity.h"
#include "freesoc.h"
#include "serial.h"


static void PutString(CHAR * const str) {}
static UINT8 GetAll(CHAR * const data) { return 0; }
static UINT8 GetChar(void) { return 0; }
static void PutChar(CHAR value) {}

static SERIAL_DEVICE_TYPE device = {
    PutString,
    GetAll,
    GetChar,
    PutChar
};

void setUp(void)
{
    Ser_Init();
}

void tearDown(void)
{
}

void test_WhenNullCharReturned_ThenZeroIsReturned(void)
{
    INT8 result;
    UINT8 data[10];

    UINT8 TestGetCharReturns0()
    {
        return 0;
    }

    //TEST_IGNORE();

    // Given
    device.get_char = TestGetCharReturns0;
    
    // When
    result = Ser_ReadLine(device, data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(-1, result);
}

void test_WhenNewLineReturned_ThenNewLineIsReturned(void)
{
    INT8 result;
    UINT8 data[10];

    UINT8 TestGetCharReturnsNewLine()
    {
        return '\n';
    }

    //TEST_IGNORE();

    // Given
    device.get_char = TestGetCharReturnsNewLine;

    // When
    result = Ser_ReadLine(device, data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_STRING("", data);
}

void test_WhenLineReturnReturned_ThenLineReturnReturned(void)
{
    INT8 result;
    UINT8 data[10];

    UINT8 TestGetCharReturnsReturnCarriage()
    {
        return '\r';
    }

    //TEST_IGNORE();

    // Given
    device.get_char = TestGetCharReturnsReturnCarriage;

    // When
    result = Ser_ReadLine(device, data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_STRING("", data);
}

void test_WhenLineDataWithNewLine_ThenDataIsReturned(void)
{
    INT8 result1;
    INT8 result2;
    INT8 result3;
    INT8 result4;
    UINT8 data[10] = {0};

    UINT8 TestGetCharReturns123NewLine()
    {
        static CHAR chars[] = {'1', '2', '3', '\n'};
        static int ii = 0;
        return chars[ii++];
    }

    //TEST_IGNORE();

    // Given
    device.get_char = TestGetCharReturns123NewLine;

    // When
    result1 = Ser_ReadLine(device, data, FALSE, 10);
    result2 = Ser_ReadLine(device, data, FALSE, 10);
    result3 = Ser_ReadLine(device, data, FALSE, 10);
    result4 = Ser_ReadLine(device, data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_INT8(-1, result1);
    TEST_ASSERT_EQUAL_INT8(-1, result2);
    TEST_ASSERT_EQUAL_INT8(-1, result3);
    TEST_ASSERT_EQUAL_INT8(3, result4);
    TEST_ASSERT_EQUAL_STRING("123", data);
}

void test_WhenLineDataWithLineReturn_ThenDataIsReturned(void)
{
    INT8 result1;
    INT8 result2;
    INT8 result3;
    INT8 result4;
    UINT8 data[10] = {0};
  
    UINT8 TestGetCharReturns123CarriageReturn()
    {
        static CHAR chars[] = {'1', '2', '3', '\r'};
        static int ii = 0;
        return chars[ii++];
    }

    //TEST_IGNORE();

    // Given
    device.get_char = TestGetCharReturns123CarriageReturn;

    // When
    result1 = Ser_ReadLine(device, data, FALSE, 10);
    result2 = Ser_ReadLine(device, data, FALSE, 10);
    result3 = Ser_ReadLine(device, data, FALSE, 10);
    result4 = Ser_ReadLine(device, data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_INT8(-1, result1);
    TEST_ASSERT_EQUAL_INT8(-1, result2);
    TEST_ASSERT_EQUAL_INT8(-1, result3);
    TEST_ASSERT_EQUAL_INT8(3, result4);
    TEST_ASSERT_EQUAL_STRING("123", data);
}

void test_WhenLineDataWithNewLineAndEcho_ThenDataIsReturnedAndOutput(void)
{
    INT8 result1;
    INT8 result2;
    INT8 result3;
    INT8 result4;
    UINT8 data[10] = {0};
  
    UINT8 TestGetCharReturns123NewLine()
    {
        static CHAR chars[] = {'1', '2', '3', '\n'};
        static int ii = 0;
        return chars[ii++];
    }

    //TEST_IGNORE();

    // Given
    device.get_char = TestGetCharReturns123NewLine;

    // When
    result1 = Ser_ReadLine(device, data, TRUE, 10);
    result2 = Ser_ReadLine(device, data, TRUE, 10);
    result3 = Ser_ReadLine(device, data, TRUE, 10);
    result4 = Ser_ReadLine(device, data, TRUE, 10);

    // Then
    TEST_ASSERT_EQUAL_INT8(-1, result1);
    TEST_ASSERT_EQUAL_INT8(-1, result2);
    TEST_ASSERT_EQUAL_INT8(-1, result3);
    TEST_ASSERT_EQUAL_INT8(3, result4);
    TEST_ASSERT_EQUAL_STRING("123", data);
}

void test_WhenLineDataWithLineReturnAndEcho_ThenDataIsReturnedAndOutput(void)
{
    INT8 result1;
    INT8 result2;
    INT8 result3;
    INT8 result4;
    UINT8 data[10] = {0};
  
    UINT8 TestGetCharReturns123CarriageReturn()
    {
        static CHAR chars[] = {'1', '2', '3', '\r'};
        static int ii = 0;
        return chars[ii++];
    }

    //TEST_IGNORE();

    // Given
    device.get_char = TestGetCharReturns123CarriageReturn;

    // When
    result1 = Ser_ReadLine(device, data, TRUE, 10);
    result2 = Ser_ReadLine(device, data, TRUE, 10);
    result3 = Ser_ReadLine(device, data, TRUE, 10);
    result4 = Ser_ReadLine(device, data, TRUE, 10);

    // Then
    TEST_ASSERT_EQUAL_INT8(-1, result1);
    TEST_ASSERT_EQUAL_INT8(-1, result2);
    TEST_ASSERT_EQUAL_INT8(-1, result3);
    TEST_ASSERT_EQUAL_INT8(3, result4);
    TEST_ASSERT_EQUAL_STRING("123", data);
}

void test_WhenMaxLengthLessThanMaxPossibleAndDataLengthGreaterThanMaxLength_ThenMaxLengthDataReturned(void)
{
    UINT8 test_data[] = "012345678901234567890123456\n";
    UINT8 expected_data[] = "890123456";
    INT8 results[28];
    UINT8 data[10] = {0};
    int ii;

    UINT8 TestGetCharReturnsDataNewLine()
    {
        static int ii = 0;
        return test_data[ii++];
    }

    //TEST_IGNORE();

    // Given
    device.get_char = TestGetCharReturnsDataNewLine;

    // When
    for (ii = 0; ii <= 27; ++ii)
    {
        results[ii] = Ser_ReadLine(device, data, TRUE, 10);
    }

    // Then
    TEST_ASSERT_EACH_EQUAL_INT8(-1, &results[19], 6);
    TEST_ASSERT_EQUAL_INT8(9, results[27]);
    TEST_ASSERT_EQUAL_STRING(expected_data, data);
}

void test_WhenLineDataExceedsMaxLineLength_ThenNewDataIsDropped(void)
{
    #define TEST_MAX_LINE_LENGTH (64)
    UINT8 test_data[] =     "1234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234\n";
    UINT8 expected_data[] = "8123456781234567812345678123456781234";
    INT8 results[101] = {0};
    UINT8 data[TEST_MAX_LINE_LENGTH] = {0};
    int ii;
    int count = 0;

    UINT8 TestGetCharReturnsData()
    {
        static int ii = 0;
        return test_data[ii++];
    }

    //TEST_IGNORE();

    // Given
    device.get_char = TestGetCharReturnsData;
    
    // When
    for (ii = 0; ii < TEST_MAX_LINE_LENGTH; ++ii)
    {
        results[ii] = Ser_ReadLine(device, data, TRUE, TEST_MAX_LINE_LENGTH);
    }
    memset(data, 0, sizeof data);
    memset(results, 0, sizeof results);
    for (ii = 0; ii < 37; ++ii)
    {
        results[ii] = Ser_ReadLine(device, data, TRUE, TEST_MAX_LINE_LENGTH);
    }

    // Then
    TEST_ASSERT_EACH_EQUAL_INT8(-1, results, 35);
    TEST_ASSERT_EQUAL_INT8(37, results[36]);
    TEST_ASSERT_EQUAL_STRING(expected_data, data);
    
}

/* Note: Serial line length has been extended to 300.  Add new test for exceeding 300 */