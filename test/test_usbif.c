#include "unity.h"
#include "usbif.h"
#include "mock_USBUART.h"
#include "mock_USBUART_cdc.h"
#include "mock_CyLib.h"


void setUp(void)
{
    USBIF_Init();
}

void tearDown(void)
{
}

void test_WhenGetConfigurationReturns0AndTimeout_ThenCDCInitNotCalled(void)
{
    int ii;
    
    // Given
    USBUART_Start_Expect(0, 1);
    for (ii = 0; ii < 10; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(0);
        CyDelayUs_Expect(1);
    }
    USBUART_GetConfiguration_ExpectAndReturn(0);

    // When
    USBIF_Start();
}

void test_WhenNoTimeout_ThenCDCInitIsCalled(void)
{
    int ii;
    
    // Given
    USBUART_Start_Expect(0, 1);
    for (ii = 0; ii < 5; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(0);
        CyDelayUs_Expect(1);
    }
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDC_Init_ExpectAndReturn(0);

    // When
    USBIF_Start();
}

void test_WhenCDCIsNotReady_ThenNoStringIsOutput(void)
{
    int ii;
    
    // Given
    for (ii = 0; ii < 10; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(0);
        CyDelayUs_Expect(1);
    }
    
    // When
    USBIF_PutString("This is a string");
}

void test_WhenCDCIsReady_ThenStringIsOutput(void)
{
    int ii;
    
    // Given
    for (ii = 0; ii < 5; ++ii)
    {
        USBUART_GetConfiguration_ExpectAndReturn(0);
        CyDelayUs_Expect(1);
    }
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_CDCIsReady_ExpectAndReturn(1);
    
    USBUART_PutString_Expect("This is a string");
    
    // When
    USBIF_PutString("This is a string");
}


void test_WhenInvalidConfiguration_ThenZeroLengthReturned(void)
{
    char data[10];
    int count;
    
    // Given
    USBUART_GetConfiguration_ExpectAndReturn(0);
    
    // When
    count = USBIF_GetAll(data);
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(count, 0);
}

void test_WhenValidConfigurationAndDataNotReady_ThenZeroLengthReturned(void)
{
    char data[10];
    int count;
    
    // Given
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(0);
    
    // When
    count = USBIF_GetAll(data);
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(count, 0);
}

void test_WhenValidConfigurationAndDataIsReadyAndCountNotZero_ThenCorrectDataAndLengthIsRead(void)
{
    #define TEXT_DATA "this is text"
    char data[64];
    int count;
    int text_data_length = strlen(TEXT_DATA);
    
    // Given
    uint16 mock_USBUART_GetAll(UINT8 *pdata, int call_count)
    {
        memcpy(pdata, TEXT_DATA, text_data_length);
        return text_data_length;
    }
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetAll_StubWithCallback(mock_USBUART_GetAll);
    
    // When
    count = USBIF_GetAll(data);
    
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
    
    // Given
    uint16 mock_USBUART_GetAll(UINT8 *pdata, int call_count)
    {
        return 0;
    }
    
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetAll_StubWithCallback(mock_USBUART_GetAll);
    
    // When
    count = USBIF_GetAll(data);
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(count, 0);
}

void test_WhenInvalidConfiguration_ThenZeroReturned(void)
{
    UINT8 ch;
    
    // Given
    USBUART_GetConfiguration_ExpectAndReturn(0);
    
    // When
    ch = USBIF_GetChar();
    
    // Then
    TEST_ASSERT_EQUAL_UINT8(0, ch);
}

void test_WhenValidConfigurationAndDataNotReady_ThenZeroReturned(void)
{
    UINT8 ch;
    
    // Given
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(0);
    
    // When
    ch = USBIF_GetChar();
    
    // Then
    TEST_ASSERT_EQUAL_UINT8(0, ch);
}

void test_WhenValidConfigurationAndDataIsReadyAndCharReturned_ThenValidCharIsReturned(void)
{
    UINT8 ch;
    
    // Given
    USBUART_GetConfiguration_ExpectAndReturn(1);
    USBUART_DataIsReady_ExpectAndReturn(1);
    USBUART_GetChar_ExpectAndReturn('c');
    
    // When
    ch = USBIF_GetChar();
    
    // Then
    TEST_ASSERT_EQUAL_UINT8('c', ch);
}
