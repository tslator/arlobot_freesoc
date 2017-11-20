#include "unity.h"
#include "utils.h"
#include "consts.h"
#include "mock_time.h"
#include <stdio.h>

void setUp(void)
{
}

void tearDown(void)
{
}


/*-------------------------------------------------------------------------------------------------
    TwoBytesToUint16
    TwoBytesToInt16
    FourBytesToUint32
    FourBytesToInt32
    Uint32ToFourBytes
    Int32ToFourBytes
    FloatToFourBytes
  -----------------------------------------------------------------------------------------------*/
void test_WhenTwoBytesValue_ThenUint16Value(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][2] = {{0x00, 0x00}, {0xFF, 0xFF}, {0x00, 0x80}, {0xFF, 0x7F}};
    UINT16 values[NUM_PATTERNS] = {0, 65535, 32768, 32767};
    UINT8 ii;
    UINT16 value;
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        // When
        value = TwoBytesToUint16(patterns[ii]);
        
        // Then
        TEST_ASSERT_EQUAL(value, values[ii]);
    }
}

void test_WhenTwoBytesValue_ThenInt16Value(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][2] = {{0x00, 0x00}, {0xFF, 0xFF}, {0x00, 0x80}, {0xFF, 0x7F}};
    INT16 values[NUM_PATTERNS] = {0, -1, -32768, 32767};
    UINT8 ii;
    INT16 value;
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        // When
        value = TwoBytesToInt16(patterns[ii]);
        
        // Then
        TEST_ASSERT_EQUAL(value, values[ii]);
    }
}

void test_WhenFourBytesValue_ThenUint32Value(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0xFF, 0xFF, 0xFF, 0xFF}, 
                                         {0x00, 0x00, 0x00, 0x80}, 
                                         {0xFF, 0xFF, 0xFF, 0x7F}};
    UINT32 values[NUM_PATTERNS] = {0, 4294967295UL, 2147483648UL, 2147483647UL};
    UINT8 ii;
    UINT32 value;
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        // When
        value = FourBytesToUint32(patterns[ii]);
        
        // Then
        TEST_ASSERT_EQUAL(value, values[ii]);
    }
}

void test_WhenFourBytesValue_ThenInt32Value(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0xFF, 0xFF, 0xFF, 0xFF}, 
                                         {0x00, 0x00, 0x00, 0x80}, 
                                         {0xFF, 0xFF, 0xFF, 0x7F}};
    INT32 values[NUM_PATTERNS] = {0L, -1L, -2147483647L - 1L, 2147483647L};
    UINT8 ii;
    INT32 value;
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        // When
        value = FourBytesToInt32(patterns[ii]);
        
        // Then
        TEST_ASSERT_EQUAL(value, values[ii]);
    }
}

void test_WhenUint32Value_ThenFourBytesValue(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0xFF, 0xFF, 0xFF, 0xFF}, 
                                         {0x00, 0x00, 0x00, 0x80}, 
                                         {0xFF, 0xFF, 0xFF, 0x7F}};
    UINT32 values[NUM_PATTERNS] = {0, 4294967295UL, 2147483648UL, 2147483647UL};
    UINT8 ii;
    UINT8 actual[4];
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        // When
        Uint32ToFourBytes(values[ii], actual);
        
        // Then
        TEST_ASSERT_EQUAL_UINT8_ARRAY(patterns[ii], actual, 4);
    }
}

void test_WhenInt32Value_ThenFourBytesValue(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0xFF, 0xFF, 0xFF, 0xFF}, 
                                         {0x00, 0x00, 0x00, 0x80}, 
                                         {0xFF, 0xFF, 0xFF, 0x7F}};
    UINT32 values[NUM_PATTERNS] = {0L, -1L, -2147483647L - 1L, 2147483647L};
    UINT8 ii;
    UINT8 actual[4];
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        // When
        Int32ToFourBytes(values[ii], actual);
        
        // Then
        TEST_ASSERT_EQUAL_UINT8_ARRAY(patterns[ii], actual, 4);
    }
}

void test_WhenFloatValue_ThenFourBytesValue(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0x00,0x00,0x80,0xBf},
                                         {0xC3, 0xF5, 0x48, 0x40},
                                         {0x4D, 0xF8, 0x2D, 0x40}};
    float values[NUM_PATTERNS] = {0, -1, 3.14, 2.71828};
    UINT8 ii;
    UINT8 actual[4];
    
    for (ii = 0; ii < 4; ++ii)
    {
        // When
        FloatToFourBytes(values[ii], actual);
        
        // Then
        TEST_ASSERT_EQUAL_HEX8_ARRAY(patterns[ii], actual, 4);
    }
}

/*-------------------------------------------------------------------------------------------------
    Interpolate
  -----------------------------------------------------------------------------------------------*/

void test_WhenX1EqualsX2AndY1EqualsY2_ThenReturnY1(void)
{
    INT16 x1 = 1;
    INT16 x2 = 1;
    UINT16 y1 = 2;
    UINT16 y2 = 2;
    INT16 value;
    INT16 result = (INT16) y1;
    
    value = Interpolate(0, x1, x2, y1, y2);
    
    TEST_ASSERT_EQUAL(value, result);
}

void test_WhenX1EqualsX2AndY1NotEqualY2_ThenReturnY1PlusAvgDiffY(void)
{
    INT16 x1 = 1;
    INT16 x2 = 1;
    UINT16 y1 = 1;
    UINT16 y2 = 2;
    INT16 value;
    INT16 result = y1 + ((y2 - y1) / 2);
    
    value = Interpolate(0, x1, x2, y1, y2);
    
    TEST_ASSERT_EQUAL(value, result);

}

void test_WhenAllZeros_ThenReturnZero(void)
{
    INT16 x1 = 0;
    INT16 x2 = 0;
    UINT16 y1 = 0;
    UINT16 y2 = 0;
    INT16 value;
    INT16 result = 0;
    
    // When
    value = Interpolate(0, x1, x2, y1, y2);
    
    // Then
    TEST_ASSERT_EQUAL(value, result);

}

/* Considering creating a loop with multiple m/b combinations */
void test_WhenGivenX_ThenReturnY(void)
{
    FLOAT m = 1;
    FLOAT b = 0;
    INT16 x1 = -1000;
    INT16 x2 = 1000;
    UINT16 y1 = m * x1 + b;
    UINT16 y2 = m * x2 + b;
    INT16 x = 100;
    INT16 value;
    INT16 result = m * x + b;
    
    value = Interpolate(x, x1, x2, y1, y2);
    
    TEST_ASSERT_EQUAL(value, result);
}

/*-------------------------------------------------------------------------------------------------
    CalcHeading
  -----------------------------------------------------------------------------------------------*/

void test_WhenLRCountEqual_ThenReturnZero(void)
{
    FLOAT heading;
    
    // When
    heading = CalcHeading(2000, 2000, 1.0, 1.0, 1, 1.0);
    
    TEST_ASSERT_EQUAL(0, heading);
}

void test_WhenLRCountDiffEqualsOne_ThenReturnPi(void)
{
    FLOAT heading;
    
    // When
    heading = CalcHeading(2000, 2001, 1.0, 1.0, 1, 1.0);
    
    // Then
    TEST_ASSERT_EQUAL(PI, heading);
}

void test_WhenLRCountDiffEqualsMinusOne_ThenReturnPi(void)
{
    FLOAT heading;
    
    // When
    heading = CalcHeading(2001, 2000, 1.0, 1.0, 1, 1.0);
    
    // Then
    TEST_ASSERT_EQUAL(-PI, heading);
}

/*-------------------------------------------------------------------------------------------------
    NormalizeHeading
  -----------------------------------------------------------------------------------------------*/

void test_WhenHeadingEqualZero_ThenReturnZero(void)
{
    FLOAT heading;
    
    // When
    heading = NormalizeHeading(0.0);
    
    // Then
    TEST_ASSERT_EQUAL(0.0, heading);
}

void test_WhenHeadingEqualPI_ThenReturnPI(void)
{
    FLOAT heading;
    
    // When
    heading = NormalizeHeading(3.14159);
    
    // Then
    TEST_ASSERT_EQUAL_FLOAT(3.14159, heading);
}

void test_WhenHeadingEqualMinusPI_ThenReturnMinusPI(void)
{
    FLOAT heading;
    
    // When
    heading = NormalizeHeading(-3.14159);
    
    // Then
    TEST_ASSERT_EQUAL_FLOAT(-3.14159, heading);
}

void test_WhenHeadingGreaterThanPI_ThenReturnMinusPIToPI(void)
{
    FLOAT heading;
    
    // When
    heading = NormalizeHeading(1.5 * 3.14159);
    
    // Then
    TEST_ASSERT(heading > -3.14159 && heading < 3.14159);
}

void test_WhenHeadingLessThanMinusPI_ThenReturnMinusPIToPI(void)
{
    FLOAT heading;
    
    // When
    heading = NormalizeHeading(1.5 * -3.14159);
    
    // Then
    TEST_ASSERT(heading > -3.14159 && heading < 3.14159);
}

void test_WhenHeadingMultipleOfPI_ThenReturnMinusPIToPI(void)
{
    FLOAT heading;
    
    // When
    heading = NormalizeHeading(5 * 3.14159);
    
    // Then
    TEST_ASSERT(heading > -3.14159 && heading < 3.14159);
}

void test_WhenHeadingNegMultipleOfPI_ThenReturnMinusPIToPI(void)
{
    FLOAT heading;
    
    // When
    heading = NormalizeHeading(5 * -3.14159);
    
    // Then
    TEST_ASSERT(heading > -3.14159 && heading < 3.14159);
}

/*-------------------------------------------------------------------------------------------------
    CalcTriangularProfile
  -----------------------------------------------------------------------------------------------*/

void test_WhenNumPointEven_ThenAssert(void)
{
    UINT8 num_points = 2;
    FLOAT lower_limit = 1.0;
    FLOAT upper_limit = 2.0;
    FLOAT profile[2];

    // Would like to know how to test that assertions are correctly triggered
    // Created ASSERT macro to print a message, but it would nice to capture that
    // message and check it here.  That is, if the message matches, the test passes.
    // How do we do that?

    // When
    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);

    // Then
    TEST_ASSERT_MESSAGE(1 == 1, "Expecting assertion");
}

void test_WhenLowerEqualsUpper_ThenAssert(void)
{
    UINT8 num_points = 3;
    FLOAT lower_limit = 10.0;
    FLOAT upper_limit = 10.0;
    FLOAT profile[2];

    // When
    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);

    // Then
    TEST_ASSERT_MESSAGE(1 == 1, "Expecting assertion");
}

void test_WhenLowerGreaterThanUpper_ThenAssert(void)
{
    UINT8 num_points = 3;
    FLOAT lower_limit = 10.0;
    FLOAT upper_limit = 0.0;
    FLOAT profile[2];

    // When
    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);

    // Then
    TEST_ASSERT_MESSAGE(1 == 1, "Expecting assertion");
}

void test_WhenNumPoints7AndRange1To10_ThenAssert(void)
{
    UINT8 num_points = 7;
    FLOAT lower_limit = 1.0;
    FLOAT upper_limit = 10.0;
    FLOAT results[7] = {1.0, 4.0, 7.0, 10.0, 7.0, 4.0, 1.0};
    FLOAT profile[7];

    // When
    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);

    // Then
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(results, profile, 7);
}

void test_WhenNumPoints21AndRangeMinus10To10_ThenAssert(void)
{
    UINT8 num_points = 21;
    FLOAT lower_limit = -10.0;
    FLOAT upper_limit = 10.0;
    FLOAT results[21] = {-10.0, -8.0, -6.0, -4.0, -2.0, 0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 8.0, 6.0, 4.0, 2.0, 0.0, -2.0, -4.0, -6.0, -8.0, -10.0};
    FLOAT profile[21];

    // When
    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);

    // Then
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(results, profile, 21);
}

/*-------------------------------------------------------------------------------------------------
    UniToDiff
  -----------------------------------------------------------------------------------------------*/

void test_WhenLinearAngularZero_ThenLeftRightZero(void)
{
    float linear = 0.0;
    float angular = 0.0;
    float left;
    float right;

    // When
    UniToDiff(linear, angular, &left, &right);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(0.0, left);
    TEST_ASSERT_EQUAL_FLOAT(0.0, right);
}

void test_WhenLinearForwardMaxAngularZero_ThenLeftRightMax(void)
{
    float linear = MAX_WHEEL_RADIAN_PER_SECOND;
    float angular = 0.0;
    float left;
    float right;
    float result = 2 * MAX_WHEEL_RADIAN_PER_SECOND / WHEEL_DIAMETER;

    // When
    UniToDiff(linear, angular, &left, &right);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result, left);
    TEST_ASSERT_EQUAL_FLOAT(result, right);
}

void test_WhenLinearBackwardMaxAngularZero_ThenLeftRightMax(void)
{
    float linear = -MAX_WHEEL_RADIAN_PER_SECOND;
    float angular = 0.0;
    float left;
    float right;
    float result = -2 * MAX_WHEEL_RADIAN_PER_SECOND / WHEEL_DIAMETER;

    // When
    UniToDiff(linear, angular, &left, &right);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result, left);
    TEST_ASSERT_EQUAL_FLOAT(result, right);
}

void test_WhenAngularCWMaxLinearZero_ThenLeftRightMax(void)
{
    float linear = 0.0;
    float angular = MAX_ROBOT_RADIAN_PER_SECOND;
    float left;
    float right;
    float left_result = -MAX_WHEEL_RADIAN_PER_SECOND;
    float right_result = MAX_WHEEL_RADIAN_PER_SECOND;

    // When
    UniToDiff(linear, angular, &left, &right);

    // Then
    TEST_ASSERT_FLOAT_WITHIN(0.00001f, left_result, left);
    TEST_ASSERT_FLOAT_WITHIN(0.00001f, right_result, right);
}

/*-------------------------------------------------------------------------------------------------
    DiffToUni
  -----------------------------------------------------------------------------------------------*/

void test_WhenLeftRightZero_ThenLinearAngularZero(void)
{
    float left = 0.0;
    float right = 0.0;
    float linear;
    float angular;
    float result = 0.0;

    // When
    DiffToUni(left, right, &linear, &angular);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result, linear);
    TEST_ASSERT_EQUAL_FLOAT(result, angular);
}

void test_WhenLeftRightForwardEqualsOne_ThenLinearForwardWheelRadiusAngularZero(void)
{
    float left = 1.0;
    float right = 1.0;
    float linear;
    float angular;

    // When
    DiffToUni(left, right, &linear, &angular);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(WHEEL_RADIUS, linear);
    TEST_ASSERT_EQUAL_FLOAT(0.0, angular);
}

void test_WhenLeftRightBackwardEqualsOne_ThenLinearBackwardWheelRadiusAngularZero(void)
{
    float left = -1.0;
    float right = -1.0;
    float linear;
    float angular;

    // When
    DiffToUni(left, right, &linear, &angular);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(-WHEEL_RADIUS, linear);
    TEST_ASSERT_EQUAL_FLOAT(0.0, angular);
}

void test_WhenLeftRightForwardEqual_ThenLinearForwardNWheelRadiusAngularZero(void)
{
    float left = 5.0;
    float right = 5.0;
    float linear;
    float angular;
    float result = left * WHEEL_RADIUS;

    // When
    DiffToUni(left, right, &linear, &angular);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result, linear);
    TEST_ASSERT_EQUAL_FLOAT(0.0, angular);
}

void test_WhenLeftRightBackwardEqual_ThenLinearBackwardNWheelRadiusAngularZero(void)
{
    float left = -5.0;
    float right = -5.0;
    float linear;
    float angular;
    float result = left * WHEEL_RADIUS;

    // When
    DiffToUni(left, right, &linear, &angular);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result, linear);
    TEST_ASSERT_EQUAL_FLOAT(0.0, angular);
}

void test_WhenLeftZeroRightTwo_ThenLinearWheelRadiusAngular2WheelRadiusOverTrackWidth(void)
{
    float left = 0.0;
    float right = 2.0;
    float linear;
    float angular;
    float linear_result = WHEEL_RADIUS;
    float angular_result = 2 * WHEEL_RADIUS / TRACK_WIDTH;

    // When
    DiffToUni(left, right, &linear, &angular);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(linear_result, linear);
    TEST_ASSERT_EQUAL_FLOAT(angular_result, angular);
}

void test_WhenLeftMinusTwoRightZero_ThenLinearWheelRadiusAngularMinus2WheelRadiusOverTrackWidth(void)
{
    float left = 0.0;
    float right = -2.0;
    float linear;
    float angular;
    float linear_result = -WHEEL_RADIUS;
    float angular_result = -2 * WHEEL_RADIUS / TRACK_WIDTH;

    // When
    DiffToUni(left, right, &linear, &angular);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(linear_result, linear);
    TEST_ASSERT_EQUAL_FLOAT(angular_result, angular);
}

/*-------------------------------------------------------------------------------------------------
    EnsureAngularVelocity
  -----------------------------------------------------------------------------------------------*/

void test_WhenVWEqualZero_ThenEnsureVWEqualZero(void)
{
    float v = 0.0;
    float w = 0.0;

    // When
    EnsureAngularVelocity(&v, &w);

    // Then
    TEST_ASSERT(v == 0.0 && w == 0.0);
}

void test_WhenMaxForwardVZeroW_ThenEnsureMaxForwardV(void)
{
    float v = MAX_WHEEL_METER_PER_SECOND;
    float w = 0.0;
    float result = MAX_WHEEL_METER_PER_SECOND;

    // When
    EnsureAngularVelocity(&v, &w);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result, v);
}

void test_WhenMaxBackwardVZeroW_ThenEnsureMaxBackwardV(void)
{
    float v = -MAX_WHEEL_METER_PER_SECOND;
    float w = 0.0;
    float result = -MAX_WHEEL_METER_PER_SECOND;

    // When
    EnsureAngularVelocity(&v, &w);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result, v);
}

void test_WhenVForwardMaxAndMatchingW_ThenEnsureVIsZeroWUnchanged(void)
{
    float v = MAX_WHEEL_METER_PER_SECOND;
    float w = 2 * v / TRACK_WIDTH;
    float linear_result = 0.0;
    float angular_result = 2 * v / TRACK_WIDTH;

    // When
    EnsureAngularVelocity(&v, &w);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(linear_result, v);
    TEST_ASSERT_EQUAL_FLOAT(angular_result, w);
}

void test_WhenVBackwardMaxAndMatchingW_ThenEnsureVIsZeroWUnchanged(void)
{
    float v = -MAX_WHEEL_METER_PER_SECOND;
    float w = 2 * v / TRACK_WIDTH;
    float linear_result = 0.0;
    float angular_result = 2 * v / TRACK_WIDTH;

    // When
    EnsureAngularVelocity(&v, &w);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(linear_result, v);
    TEST_ASSERT_EQUAL_FLOAT(angular_result, w);
}


void test_WhenForwardVZeroCWWMatch_ThenEnsureVIs0WUnchanged(void)
{
    float v = 0.0;
    float w = WHEEL_RADIUS * 2 * MAX_WHEEL_RADIAN_PER_SECOND / TRACK_WIDTH;
    float result = WHEEL_RADIUS * 2 * MAX_WHEEL_RADIAN_PER_SECOND / TRACK_WIDTH;
    
    // When
    EnsureAngularVelocity(&v, &w);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(0.0, v);
    TEST_ASSERT_EQUAL_FLOAT(result, w);
}

void test_WhenForwardVGreaterThanZeroCWWMatch_ThenEnsureVIs0WUnchanged(void)
{
    float v = 1000.0;
    float w = WHEEL_RADIUS * 2 * MAX_WHEEL_RADIAN_PER_SECOND / TRACK_WIDTH;
    float result = WHEEL_RADIUS * 2 * MAX_WHEEL_RADIAN_PER_SECOND / TRACK_WIDTH;
    
    // When
    EnsureAngularVelocity(&v, &w);

    // Then
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0, v);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, result, w);
}

void test_WhenBackwardVGreaterThanZeroCWWMatch_ThenEnsureVIs0WUnchanged(void)
{
    float v = -1000.0;
    float w = -WHEEL_RADIUS * 2 * MAX_WHEEL_RADIAN_PER_SECOND / TRACK_WIDTH;
    float result = -WHEEL_RADIUS * 2 * MAX_WHEEL_RADIAN_PER_SECOND / TRACK_WIDTH;
    
    // When
    EnsureAngularVelocity(&v, &w);

    // Then
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0, v);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, result, w);
}
