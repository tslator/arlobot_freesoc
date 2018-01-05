#include <stdio.h>
#include "unity.h"
#include "freesoc.h"
#include "serial.h"
#include "mock_usbif.h"

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

    //TEST_IGNORE();

    // Given
    USBIF_GetChar_ExpectAndReturn(0);

    // When
    result = Ser_ReadLine(data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(-1, result);
}

void test_WhenNewLineReturned_ThenNewLineIsReturned(void)
{
    INT8 result;
    UINT8 data[10];

    //TEST_IGNORE();

    // Given
    USBIF_GetChar_ExpectAndReturn('\n');

    // When
    result = Ser_ReadLine(data, FALSE, 10);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_STRING("", data);
}

void test_WhenLineReturnReturned_ThenLineReturnReturned(void)
{
    INT8 result;
    UINT8 data[10];

    //TEST_IGNORE();
    
    // Given
    USBIF_GetChar_ExpectAndReturn('\r');

    // When
    result = Ser_ReadLine(data, FALSE, 10);

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

    //TEST_IGNORE();
    
    // Given
    USBIF_GetChar_ExpectAndReturn('1');
    USBIF_GetChar_ExpectAndReturn('2');
    USBIF_GetChar_ExpectAndReturn('3');
    USBIF_GetChar_ExpectAndReturn('\n');

    // When
    result1 = Ser_ReadLine(data, FALSE, 10);
    result2 = Ser_ReadLine(data, FALSE, 10);
    result3 = Ser_ReadLine(data, FALSE, 10);
    result4 = Ser_ReadLine(data, FALSE, 10);

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
  
    //TEST_IGNORE();
    
    // Given
    USBIF_GetChar_ExpectAndReturn('1');
    USBIF_GetChar_ExpectAndReturn('2');
    USBIF_GetChar_ExpectAndReturn('3');
    USBIF_GetChar_ExpectAndReturn('\r');

    // When
    result1 = Ser_ReadLine(data, FALSE, 10);
    result2 = Ser_ReadLine(data, FALSE, 10);
    result3 = Ser_ReadLine(data, FALSE, 10);
    result4 = Ser_ReadLine(data, FALSE, 10);

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
  
    //TEST_IGNORE();
    
    // Given
    USBIF_GetChar_ExpectAndReturn('1');
    USBIF_PutChar_Expect('1');
    USBIF_GetChar_ExpectAndReturn('2');
    USBIF_PutChar_Expect('2');
    USBIF_GetChar_ExpectAndReturn('3');
    USBIF_PutChar_Expect('3');
    USBIF_GetChar_ExpectAndReturn('\n');

    // When
    result1 = Ser_ReadLine(data, TRUE, 10);
    result2 = Ser_ReadLine(data, TRUE, 10);
    result3 = Ser_ReadLine(data, TRUE, 10);
    result4 = Ser_ReadLine(data, TRUE, 10);

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
  
    //TEST_IGNORE();
    
    // Given
    USBIF_GetChar_ExpectAndReturn('1');
    USBIF_PutChar_Expect('1');
    USBIF_GetChar_ExpectAndReturn('2');
    USBIF_PutChar_Expect('2');
    USBIF_GetChar_ExpectAndReturn('3');
    USBIF_PutChar_Expect('3');
    USBIF_GetChar_ExpectAndReturn('\n');

    // When
    result1 = Ser_ReadLine(data, TRUE, 10);
    result2 = Ser_ReadLine(data, TRUE, 10);
    result3 = Ser_ReadLine(data, TRUE, 10);
    result4 = Ser_ReadLine(data, TRUE, 10);

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

    //TEST_IGNORE();
    
    // Given
    for (ii = 0; ii <= 27; ++ii)
    {
        USBIF_GetChar_ExpectAndReturn(test_data[ii]);
        USBIF_PutChar_Expect(test_data[ii]);
    }
    
    // When
    for (ii = 0; ii <= 27; ++ii)
    {
        results[ii] = Ser_ReadLine(data, TRUE, 10);
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

    //TEST_IGNORE();
    
    // Given
    for (ii = 0; ii < 100; ++ii)
    {
        USBIF_GetChar_ExpectAndReturn(test_data[ii]);
        USBIF_PutChar_Expect(test_data[ii]);
    }    
    USBIF_GetChar_ExpectAndReturn(test_data[ii]);

    // When
    for (ii = 0; ii < TEST_MAX_LINE_LENGTH; ++ii)
    {
        results[ii] = Ser_ReadLine(data, TRUE, TEST_MAX_LINE_LENGTH);
    }
    memset(data, 0, sizeof data);
    memset(results, 0, sizeof results);
    for (ii = 0; ii < 37; ++ii)
    {
        results[ii] = Ser_ReadLine(data, TRUE, TEST_MAX_LINE_LENGTH);
    }

    // Then
    TEST_ASSERT_EACH_EQUAL_INT8(-1, results, 35);
    TEST_ASSERT_EQUAL_INT8(37, results[36]);
    TEST_ASSERT_EQUAL_STRING(expected_data, data);
    
}

/* Note: Serial line length has been extended to 127.  Add new test for exceeding 127 */