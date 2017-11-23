/* AUTOGENERATED FILE. DO NOT EDIT. */
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "unity.h"
#include "cmock.h"
#include "mock_Left_HB25_Enable_Pin.h"

static const char* CMockString_Left_HB25_Enable_Pin_ClearInterrupt = "Left_HB25_Enable_Pin_ClearInterrupt";
static const char* CMockString_Left_HB25_Enable_Pin_Read = "Left_HB25_Enable_Pin_Read";
static const char* CMockString_Left_HB25_Enable_Pin_ReadDataReg = "Left_HB25_Enable_Pin_ReadDataReg";
static const char* CMockString_Left_HB25_Enable_Pin_SetDriveMode = "Left_HB25_Enable_Pin_SetDriveMode";
static const char* CMockString_Left_HB25_Enable_Pin_SetInterruptMode = "Left_HB25_Enable_Pin_SetInterruptMode";
static const char* CMockString_Left_HB25_Enable_Pin_Write = "Left_HB25_Enable_Pin_Write";
static const char* CMockString_mode = "mode";
static const char* CMockString_position = "position";
static const char* CMockString_value = "value";

typedef struct _CMOCK_Left_HB25_Enable_Pin_Write_CALL_INSTANCE
{
  UNITY_LINE_TYPE LineNumber;
  int CallOrder;
  uint8 Expected_value;

} CMOCK_Left_HB25_Enable_Pin_Write_CALL_INSTANCE;

typedef struct _CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALL_INSTANCE
{
  UNITY_LINE_TYPE LineNumber;
  int CallOrder;
  uint8 Expected_mode;

} CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALL_INSTANCE;

typedef struct _CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE
{
  UNITY_LINE_TYPE LineNumber;
  uint8 ReturnVal;
  int CallOrder;

} CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE;

typedef struct _CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE
{
  UNITY_LINE_TYPE LineNumber;
  uint8 ReturnVal;
  int CallOrder;

} CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE;

typedef struct _CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALL_INSTANCE
{
  UNITY_LINE_TYPE LineNumber;
  int CallOrder;
  uint16 Expected_position;
  uint16 Expected_mode;

} CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALL_INSTANCE;

typedef struct _CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE
{
  UNITY_LINE_TYPE LineNumber;
  uint8 ReturnVal;
  int CallOrder;

} CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE;

static struct mock_Left_HB25_Enable_PinInstance
{
  int Left_HB25_Enable_Pin_Write_IgnoreBool;
  CMOCK_Left_HB25_Enable_Pin_Write_CALLBACK Left_HB25_Enable_Pin_Write_CallbackFunctionPointer;
  int Left_HB25_Enable_Pin_Write_CallbackCalls;
  CMOCK_MEM_INDEX_TYPE Left_HB25_Enable_Pin_Write_CallInstance;
  int Left_HB25_Enable_Pin_SetDriveMode_IgnoreBool;
  CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALLBACK Left_HB25_Enable_Pin_SetDriveMode_CallbackFunctionPointer;
  int Left_HB25_Enable_Pin_SetDriveMode_CallbackCalls;
  CMOCK_MEM_INDEX_TYPE Left_HB25_Enable_Pin_SetDriveMode_CallInstance;
  int Left_HB25_Enable_Pin_ReadDataReg_IgnoreBool;
  uint8 Left_HB25_Enable_Pin_ReadDataReg_FinalReturn;
  CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALLBACK Left_HB25_Enable_Pin_ReadDataReg_CallbackFunctionPointer;
  int Left_HB25_Enable_Pin_ReadDataReg_CallbackCalls;
  CMOCK_MEM_INDEX_TYPE Left_HB25_Enable_Pin_ReadDataReg_CallInstance;
  int Left_HB25_Enable_Pin_Read_IgnoreBool;
  uint8 Left_HB25_Enable_Pin_Read_FinalReturn;
  CMOCK_Left_HB25_Enable_Pin_Read_CALLBACK Left_HB25_Enable_Pin_Read_CallbackFunctionPointer;
  int Left_HB25_Enable_Pin_Read_CallbackCalls;
  CMOCK_MEM_INDEX_TYPE Left_HB25_Enable_Pin_Read_CallInstance;
  int Left_HB25_Enable_Pin_SetInterruptMode_IgnoreBool;
  CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALLBACK Left_HB25_Enable_Pin_SetInterruptMode_CallbackFunctionPointer;
  int Left_HB25_Enable_Pin_SetInterruptMode_CallbackCalls;
  CMOCK_MEM_INDEX_TYPE Left_HB25_Enable_Pin_SetInterruptMode_CallInstance;
  int Left_HB25_Enable_Pin_ClearInterrupt_IgnoreBool;
  uint8 Left_HB25_Enable_Pin_ClearInterrupt_FinalReturn;
  CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALLBACK Left_HB25_Enable_Pin_ClearInterrupt_CallbackFunctionPointer;
  int Left_HB25_Enable_Pin_ClearInterrupt_CallbackCalls;
  CMOCK_MEM_INDEX_TYPE Left_HB25_Enable_Pin_ClearInterrupt_CallInstance;
} Mock;

extern jmp_buf AbortFrame;
extern int GlobalExpectCount;
extern int GlobalVerifyOrder;

void mock_Left_HB25_Enable_Pin_Verify(void)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  if (Mock.Left_HB25_Enable_Pin_Write_IgnoreBool)
    Mock.Left_HB25_Enable_Pin_Write_CallInstance = CMOCK_GUTS_NONE;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_Write);
  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.Left_HB25_Enable_Pin_Write_CallInstance, cmock_line, CMockStringCalledLess);
  if (Mock.Left_HB25_Enable_Pin_Write_CallbackFunctionPointer != NULL)
    Mock.Left_HB25_Enable_Pin_Write_CallInstance = CMOCK_GUTS_NONE;
  if (Mock.Left_HB25_Enable_Pin_SetDriveMode_IgnoreBool)
    Mock.Left_HB25_Enable_Pin_SetDriveMode_CallInstance = CMOCK_GUTS_NONE;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_SetDriveMode);
  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.Left_HB25_Enable_Pin_SetDriveMode_CallInstance, cmock_line, CMockStringCalledLess);
  if (Mock.Left_HB25_Enable_Pin_SetDriveMode_CallbackFunctionPointer != NULL)
    Mock.Left_HB25_Enable_Pin_SetDriveMode_CallInstance = CMOCK_GUTS_NONE;
  if (Mock.Left_HB25_Enable_Pin_ReadDataReg_IgnoreBool)
    Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance = CMOCK_GUTS_NONE;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_ReadDataReg);
  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance, cmock_line, CMockStringCalledLess);
  if (Mock.Left_HB25_Enable_Pin_ReadDataReg_CallbackFunctionPointer != NULL)
    Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance = CMOCK_GUTS_NONE;
  if (Mock.Left_HB25_Enable_Pin_Read_IgnoreBool)
    Mock.Left_HB25_Enable_Pin_Read_CallInstance = CMOCK_GUTS_NONE;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_Read);
  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.Left_HB25_Enable_Pin_Read_CallInstance, cmock_line, CMockStringCalledLess);
  if (Mock.Left_HB25_Enable_Pin_Read_CallbackFunctionPointer != NULL)
    Mock.Left_HB25_Enable_Pin_Read_CallInstance = CMOCK_GUTS_NONE;
  if (Mock.Left_HB25_Enable_Pin_SetInterruptMode_IgnoreBool)
    Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallInstance = CMOCK_GUTS_NONE;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_SetInterruptMode);
  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallInstance, cmock_line, CMockStringCalledLess);
  if (Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallbackFunctionPointer != NULL)
    Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallInstance = CMOCK_GUTS_NONE;
  if (Mock.Left_HB25_Enable_Pin_ClearInterrupt_IgnoreBool)
    Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance = CMOCK_GUTS_NONE;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_ClearInterrupt);
  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance, cmock_line, CMockStringCalledLess);
  if (Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallbackFunctionPointer != NULL)
    Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance = CMOCK_GUTS_NONE;
}

void mock_Left_HB25_Enable_Pin_Init(void)
{
  mock_Left_HB25_Enable_Pin_Destroy();
}

void mock_Left_HB25_Enable_Pin_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
  Mock.Left_HB25_Enable_Pin_Write_CallbackFunctionPointer = NULL;
  Mock.Left_HB25_Enable_Pin_Write_CallbackCalls = 0;
  Mock.Left_HB25_Enable_Pin_SetDriveMode_CallbackFunctionPointer = NULL;
  Mock.Left_HB25_Enable_Pin_SetDriveMode_CallbackCalls = 0;
  Mock.Left_HB25_Enable_Pin_ReadDataReg_CallbackFunctionPointer = NULL;
  Mock.Left_HB25_Enable_Pin_ReadDataReg_CallbackCalls = 0;
  Mock.Left_HB25_Enable_Pin_Read_CallbackFunctionPointer = NULL;
  Mock.Left_HB25_Enable_Pin_Read_CallbackCalls = 0;
  Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallbackFunctionPointer = NULL;
  Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallbackCalls = 0;
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallbackFunctionPointer = NULL;
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallbackCalls = 0;
  GlobalExpectCount = 0;
  GlobalVerifyOrder = 0;
}

void Left_HB25_Enable_Pin_Write(uint8 value)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  CMOCK_Left_HB25_Enable_Pin_Write_CALL_INSTANCE* cmock_call_instance;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_Write);
  cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_Write_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.Left_HB25_Enable_Pin_Write_CallInstance);
  Mock.Left_HB25_Enable_Pin_Write_CallInstance = CMock_Guts_MemNext(Mock.Left_HB25_Enable_Pin_Write_CallInstance);
  if (Mock.Left_HB25_Enable_Pin_Write_IgnoreBool)
  {
    UNITY_CLR_DETAILS();
    return;
  }
  if (Mock.Left_HB25_Enable_Pin_Write_CallbackFunctionPointer != NULL)
  {
    Mock.Left_HB25_Enable_Pin_Write_CallbackFunctionPointer(value, Mock.Left_HB25_Enable_Pin_Write_CallbackCalls++);
    return;
  }
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
  cmock_line = cmock_call_instance->LineNumber;
  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);
  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);
  {
    UNITY_SET_DETAILS(CMockString_Left_HB25_Enable_Pin_Write,CMockString_value);
    UNITY_TEST_ASSERT_EQUAL_HEX8(cmock_call_instance->Expected_value, value, cmock_line, CMockStringMismatch);
  }
  UNITY_CLR_DETAILS();
}

void CMockExpectParameters_Left_HB25_Enable_Pin_Write(CMOCK_Left_HB25_Enable_Pin_Write_CALL_INSTANCE* cmock_call_instance, uint8 value)
{
  cmock_call_instance->Expected_value = value;
}

void Left_HB25_Enable_Pin_Write_CMockIgnore(void)
{
  Mock.Left_HB25_Enable_Pin_Write_IgnoreBool = (int)1;
}

void Left_HB25_Enable_Pin_Write_CMockExpect(UNITY_LINE_TYPE cmock_line, uint8 value)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Left_HB25_Enable_Pin_Write_CALL_INSTANCE));
  CMOCK_Left_HB25_Enable_Pin_Write_CALL_INSTANCE* cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_Write_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.Left_HB25_Enable_Pin_Write_CallInstance = CMock_Guts_MemChain(Mock.Left_HB25_Enable_Pin_Write_CallInstance, cmock_guts_index);
  Mock.Left_HB25_Enable_Pin_Write_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->CallOrder = ++GlobalExpectCount;
  CMockExpectParameters_Left_HB25_Enable_Pin_Write(cmock_call_instance, value);
  UNITY_CLR_DETAILS();
}

void Left_HB25_Enable_Pin_Write_StubWithCallback(CMOCK_Left_HB25_Enable_Pin_Write_CALLBACK Callback)
{
  Mock.Left_HB25_Enable_Pin_Write_IgnoreBool = (int)0;
  Mock.Left_HB25_Enable_Pin_Write_CallbackFunctionPointer = Callback;
}

void Left_HB25_Enable_Pin_SetDriveMode(uint8 mode)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALL_INSTANCE* cmock_call_instance;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_SetDriveMode);
  cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.Left_HB25_Enable_Pin_SetDriveMode_CallInstance);
  Mock.Left_HB25_Enable_Pin_SetDriveMode_CallInstance = CMock_Guts_MemNext(Mock.Left_HB25_Enable_Pin_SetDriveMode_CallInstance);
  if (Mock.Left_HB25_Enable_Pin_SetDriveMode_IgnoreBool)
  {
    UNITY_CLR_DETAILS();
    return;
  }
  if (Mock.Left_HB25_Enable_Pin_SetDriveMode_CallbackFunctionPointer != NULL)
  {
    Mock.Left_HB25_Enable_Pin_SetDriveMode_CallbackFunctionPointer(mode, Mock.Left_HB25_Enable_Pin_SetDriveMode_CallbackCalls++);
    return;
  }
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
  cmock_line = cmock_call_instance->LineNumber;
  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);
  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);
  {
    UNITY_SET_DETAILS(CMockString_Left_HB25_Enable_Pin_SetDriveMode,CMockString_mode);
    UNITY_TEST_ASSERT_EQUAL_HEX8(cmock_call_instance->Expected_mode, mode, cmock_line, CMockStringMismatch);
  }
  UNITY_CLR_DETAILS();
}

void CMockExpectParameters_Left_HB25_Enable_Pin_SetDriveMode(CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALL_INSTANCE* cmock_call_instance, uint8 mode)
{
  cmock_call_instance->Expected_mode = mode;
}

void Left_HB25_Enable_Pin_SetDriveMode_CMockIgnore(void)
{
  Mock.Left_HB25_Enable_Pin_SetDriveMode_IgnoreBool = (int)1;
}

void Left_HB25_Enable_Pin_SetDriveMode_CMockExpect(UNITY_LINE_TYPE cmock_line, uint8 mode)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALL_INSTANCE));
  CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALL_INSTANCE* cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.Left_HB25_Enable_Pin_SetDriveMode_CallInstance = CMock_Guts_MemChain(Mock.Left_HB25_Enable_Pin_SetDriveMode_CallInstance, cmock_guts_index);
  Mock.Left_HB25_Enable_Pin_SetDriveMode_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->CallOrder = ++GlobalExpectCount;
  CMockExpectParameters_Left_HB25_Enable_Pin_SetDriveMode(cmock_call_instance, mode);
  UNITY_CLR_DETAILS();
}

void Left_HB25_Enable_Pin_SetDriveMode_StubWithCallback(CMOCK_Left_HB25_Enable_Pin_SetDriveMode_CALLBACK Callback)
{
  Mock.Left_HB25_Enable_Pin_SetDriveMode_IgnoreBool = (int)0;
  Mock.Left_HB25_Enable_Pin_SetDriveMode_CallbackFunctionPointer = Callback;
}

uint8 Left_HB25_Enable_Pin_ReadDataReg(void)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE* cmock_call_instance;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_ReadDataReg);
  cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance);
  Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance = CMock_Guts_MemNext(Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance);
  if (Mock.Left_HB25_Enable_Pin_ReadDataReg_IgnoreBool)
  {
    UNITY_CLR_DETAILS();
    if (cmock_call_instance == NULL)
      return Mock.Left_HB25_Enable_Pin_ReadDataReg_FinalReturn;
    Mock.Left_HB25_Enable_Pin_ReadDataReg_FinalReturn = cmock_call_instance->ReturnVal;
    return cmock_call_instance->ReturnVal;
  }
  if (Mock.Left_HB25_Enable_Pin_ReadDataReg_CallbackFunctionPointer != NULL)
  {
    return Mock.Left_HB25_Enable_Pin_ReadDataReg_CallbackFunctionPointer(Mock.Left_HB25_Enable_Pin_ReadDataReg_CallbackCalls++);
  }
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
  cmock_line = cmock_call_instance->LineNumber;
  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);
  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);
  UNITY_CLR_DETAILS();
  return cmock_call_instance->ReturnVal;
}

void Left_HB25_Enable_Pin_ReadDataReg_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, uint8 cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE));
  CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE* cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance = CMock_Guts_MemChain(Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance, cmock_guts_index);
  Mock.Left_HB25_Enable_Pin_ReadDataReg_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->ReturnVal = cmock_to_return;
  Mock.Left_HB25_Enable_Pin_ReadDataReg_IgnoreBool = (int)1;
}

void Left_HB25_Enable_Pin_ReadDataReg_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8 cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE));
  CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE* cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance = CMock_Guts_MemChain(Mock.Left_HB25_Enable_Pin_ReadDataReg_CallInstance, cmock_guts_index);
  Mock.Left_HB25_Enable_Pin_ReadDataReg_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->CallOrder = ++GlobalExpectCount;
  cmock_call_instance->ReturnVal = cmock_to_return;
  UNITY_CLR_DETAILS();
}

void Left_HB25_Enable_Pin_ReadDataReg_StubWithCallback(CMOCK_Left_HB25_Enable_Pin_ReadDataReg_CALLBACK Callback)
{
  Mock.Left_HB25_Enable_Pin_ReadDataReg_IgnoreBool = (int)0;
  Mock.Left_HB25_Enable_Pin_ReadDataReg_CallbackFunctionPointer = Callback;
}

uint8 Left_HB25_Enable_Pin_Read(void)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE* cmock_call_instance;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_Read);
  cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.Left_HB25_Enable_Pin_Read_CallInstance);
  Mock.Left_HB25_Enable_Pin_Read_CallInstance = CMock_Guts_MemNext(Mock.Left_HB25_Enable_Pin_Read_CallInstance);
  if (Mock.Left_HB25_Enable_Pin_Read_IgnoreBool)
  {
    UNITY_CLR_DETAILS();
    if (cmock_call_instance == NULL)
      return Mock.Left_HB25_Enable_Pin_Read_FinalReturn;
    Mock.Left_HB25_Enable_Pin_Read_FinalReturn = cmock_call_instance->ReturnVal;
    return cmock_call_instance->ReturnVal;
  }
  if (Mock.Left_HB25_Enable_Pin_Read_CallbackFunctionPointer != NULL)
  {
    return Mock.Left_HB25_Enable_Pin_Read_CallbackFunctionPointer(Mock.Left_HB25_Enable_Pin_Read_CallbackCalls++);
  }
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
  cmock_line = cmock_call_instance->LineNumber;
  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);
  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);
  UNITY_CLR_DETAILS();
  return cmock_call_instance->ReturnVal;
}

void Left_HB25_Enable_Pin_Read_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, uint8 cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE));
  CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE* cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.Left_HB25_Enable_Pin_Read_CallInstance = CMock_Guts_MemChain(Mock.Left_HB25_Enable_Pin_Read_CallInstance, cmock_guts_index);
  Mock.Left_HB25_Enable_Pin_Read_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->ReturnVal = cmock_to_return;
  Mock.Left_HB25_Enable_Pin_Read_IgnoreBool = (int)1;
}

void Left_HB25_Enable_Pin_Read_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8 cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE));
  CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE* cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_Read_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.Left_HB25_Enable_Pin_Read_CallInstance = CMock_Guts_MemChain(Mock.Left_HB25_Enable_Pin_Read_CallInstance, cmock_guts_index);
  Mock.Left_HB25_Enable_Pin_Read_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->CallOrder = ++GlobalExpectCount;
  cmock_call_instance->ReturnVal = cmock_to_return;
  UNITY_CLR_DETAILS();
}

void Left_HB25_Enable_Pin_Read_StubWithCallback(CMOCK_Left_HB25_Enable_Pin_Read_CALLBACK Callback)
{
  Mock.Left_HB25_Enable_Pin_Read_IgnoreBool = (int)0;
  Mock.Left_HB25_Enable_Pin_Read_CallbackFunctionPointer = Callback;
}

void Left_HB25_Enable_Pin_SetInterruptMode(uint16 position, uint16 mode)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALL_INSTANCE* cmock_call_instance;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_SetInterruptMode);
  cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallInstance);
  Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallInstance = CMock_Guts_MemNext(Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallInstance);
  if (Mock.Left_HB25_Enable_Pin_SetInterruptMode_IgnoreBool)
  {
    UNITY_CLR_DETAILS();
    return;
  }
  if (Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallbackFunctionPointer != NULL)
  {
    Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallbackFunctionPointer(position, mode, Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallbackCalls++);
    return;
  }
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
  cmock_line = cmock_call_instance->LineNumber;
  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);
  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);
  {
    UNITY_SET_DETAILS(CMockString_Left_HB25_Enable_Pin_SetInterruptMode,CMockString_position);
    UNITY_TEST_ASSERT_EQUAL_HEX16(cmock_call_instance->Expected_position, position, cmock_line, CMockStringMismatch);
  }
  {
    UNITY_SET_DETAILS(CMockString_Left_HB25_Enable_Pin_SetInterruptMode,CMockString_mode);
    UNITY_TEST_ASSERT_EQUAL_HEX16(cmock_call_instance->Expected_mode, mode, cmock_line, CMockStringMismatch);
  }
  UNITY_CLR_DETAILS();
}

void CMockExpectParameters_Left_HB25_Enable_Pin_SetInterruptMode(CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALL_INSTANCE* cmock_call_instance, uint16 position, uint16 mode)
{
  cmock_call_instance->Expected_position = position;
  cmock_call_instance->Expected_mode = mode;
}

void Left_HB25_Enable_Pin_SetInterruptMode_CMockIgnore(void)
{
  Mock.Left_HB25_Enable_Pin_SetInterruptMode_IgnoreBool = (int)1;
}

void Left_HB25_Enable_Pin_SetInterruptMode_CMockExpect(UNITY_LINE_TYPE cmock_line, uint16 position, uint16 mode)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALL_INSTANCE));
  CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALL_INSTANCE* cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallInstance = CMock_Guts_MemChain(Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallInstance, cmock_guts_index);
  Mock.Left_HB25_Enable_Pin_SetInterruptMode_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->CallOrder = ++GlobalExpectCount;
  CMockExpectParameters_Left_HB25_Enable_Pin_SetInterruptMode(cmock_call_instance, position, mode);
  UNITY_CLR_DETAILS();
}

void Left_HB25_Enable_Pin_SetInterruptMode_StubWithCallback(CMOCK_Left_HB25_Enable_Pin_SetInterruptMode_CALLBACK Callback)
{
  Mock.Left_HB25_Enable_Pin_SetInterruptMode_IgnoreBool = (int)0;
  Mock.Left_HB25_Enable_Pin_SetInterruptMode_CallbackFunctionPointer = Callback;
}

uint8 Left_HB25_Enable_Pin_ClearInterrupt(void)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE* cmock_call_instance;
  UNITY_SET_DETAIL(CMockString_Left_HB25_Enable_Pin_ClearInterrupt);
  cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance);
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance = CMock_Guts_MemNext(Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance);
  if (Mock.Left_HB25_Enable_Pin_ClearInterrupt_IgnoreBool)
  {
    UNITY_CLR_DETAILS();
    if (cmock_call_instance == NULL)
      return Mock.Left_HB25_Enable_Pin_ClearInterrupt_FinalReturn;
    Mock.Left_HB25_Enable_Pin_ClearInterrupt_FinalReturn = cmock_call_instance->ReturnVal;
    return cmock_call_instance->ReturnVal;
  }
  if (Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallbackFunctionPointer != NULL)
  {
    return Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallbackFunctionPointer(Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallbackCalls++);
  }
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
  cmock_line = cmock_call_instance->LineNumber;
  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);
  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);
  UNITY_CLR_DETAILS();
  return cmock_call_instance->ReturnVal;
}

void Left_HB25_Enable_Pin_ClearInterrupt_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, uint8 cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE));
  CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE* cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance = CMock_Guts_MemChain(Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance, cmock_guts_index);
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->ReturnVal = cmock_to_return;
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_IgnoreBool = (int)1;
}

void Left_HB25_Enable_Pin_ClearInterrupt_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8 cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE));
  CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE* cmock_call_instance = (CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance = CMock_Guts_MemChain(Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallInstance, cmock_guts_index);
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->CallOrder = ++GlobalExpectCount;
  cmock_call_instance->ReturnVal = cmock_to_return;
  UNITY_CLR_DETAILS();
}

void Left_HB25_Enable_Pin_ClearInterrupt_StubWithCallback(CMOCK_Left_HB25_Enable_Pin_ClearInterrupt_CALLBACK Callback)
{
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_IgnoreBool = (int)0;
  Mock.Left_HB25_Enable_Pin_ClearInterrupt_CallbackFunctionPointer = Callback;
}

