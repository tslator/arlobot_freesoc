#include <stdio.h>
#include "unity.h"
#include "freesoc.h"
#include "utils.h"
#include "consts.h"
#include "mock_time.h"
#include "mock_contract.h"
#include "mock_CyLib.h"



void EnsureAngularVelocity_contract_fulfilled()
{   
    // ENSUREANGULARVELOCITY_V_IS_NOT_NULL();
    // ENSUREANGULARVELOCITY_W_IS_NOT_NULL();
    // ENSUREANGULARVELOCITY_LEFT_IS_NOT_NULL();
    // ENSUREANGULARVELOCITY_RIGHT_IS_NOT_NULL();
    // ENSUREANGULARVELOCITY_LINEAR_IS_NOT_NULL();
    // ENSUREANGULARVELOCITY_ANGULAR_IS_NOT_NULL();
}

void BinaryRangeSearch_contract_fulfilled(void)
{
    // DATA_POINTS_IS_NOT_NULL();
    // LOWER_INDEX_IS_NOT_NULL();
    // UPPER_INDEX_IS_NOT_NULL();
    // NUM_POINTS_GREATER_THAN_ONE();
}

void setUp(void)
{
    on_assert_Ignore();
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
void test_WhenTwoBytePatternValue_ThenValidUint16Value(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][2] = {{0x00, 0x00}, {0xFF, 0xFF}, {0x00, 0x80}, {0xFF, 0x7F}};
    UINT16 values[NUM_PATTERNS] = {0, 65535, 32768, 32767};
    UINT8 ii;
    UINT16 value;
    
    //TEST_IGNORE();
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        printf("Testing [0x%02X, 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], values[ii]);
        
        // When
        value = TwoBytesToUint16(patterns[ii]);
        
        // Then
        TEST_ASSERT_EQUAL(value, values[ii]);
    }
}

void test_WhenTwoBytePatternValue_ThenValidInt16Value(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][2] = {{0x00, 0x00}, {0xFF, 0xFF}, {0x00, 0x80}, {0xFF, 0x7F}};
    INT16 values[NUM_PATTERNS] = {0, -1, -32768, 32767};
    UINT8 ii;
    INT16 value;
    
    //TEST_IGNORE();
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        printf("Testing [0x%02X, 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], values[ii]);

        // When
        value = TwoBytesToInt16(patterns[ii]);
        
        // Then
        TEST_ASSERT_EQUAL(value, values[ii]);
    }
}

void test_WhenFourBytePatternValue_ThenValidUint32Value(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0xFF, 0xFF, 0xFF, 0xFF}, 
                                         {0x00, 0x00, 0x00, 0x80}, 
                                         {0xFF, 0xFF, 0xFF, 0x7F}};
    UINT32 values[NUM_PATTERNS] = {0, 4294967295UL, 2147483648UL, 2147483647UL};
    UINT8 ii;
    UINT32 value;
    
    //TEST_IGNORE();
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3], values[ii]);
        
        // When
        value = FourBytesToUint32(patterns[ii]);
        
        // Then
        TEST_ASSERT_EQUAL(value, values[ii]);
    }
}

void test_WhenFourBytePatternValue_ThenValidInt32Value(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0xFF, 0xFF, 0xFF, 0xFF}, 
                                         {0x00, 0x00, 0x00, 0x80}, 
                                         {0xFF, 0xFF, 0xFF, 0x7F}};
    INT32 values[NUM_PATTERNS] = {0L, -1L, -2147483647L - 1L, 2147483647L};
    UINT8 ii;
    INT32 value;
    
    //TEST_IGNORE();
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3], values[ii]);
        
        // When
        value = FourBytesToInt32(patterns[ii]);
        
        // Then
        TEST_ASSERT_EQUAL(value, values[ii]);
    }
}

void test_WhenUint32Value_ThenValidFourBytePatternValue(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0xFF, 0xFF, 0xFF, 0xFF}, 
                                         {0x00, 0x00, 0x00, 0x80}, 
                                         {0xFF, 0xFF, 0xFF, 0x7F}};
    UINT32 values[NUM_PATTERNS] = {0, 4294967295UL, 2147483648UL, 2147483647UL};
    UINT8 ii;
    UINT8 actual[4];
    
    //TEST_IGNORE();
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3], values[ii]);

        // When
        Uint32ToFourBytes(values[ii], actual);
        
        // Then
        TEST_ASSERT_EQUAL_UINT8_ARRAY(patterns[ii], actual, 4);
    }
}

void test_WhenInt32Value_ThenValidFourBytesValue(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0xFF, 0xFF, 0xFF, 0xFF}, 
                                         {0x00, 0x00, 0x00, 0x80}, 
                                         {0xFF, 0xFF, 0xFF, 0x7F}};
    UINT32 values[NUM_PATTERNS] = {0L, -1L, -2147483647L - 1L, 2147483647L};
    UINT8 ii;
    UINT8 actual[4];
    
    //TEST_IGNORE();
    
    for (ii = 0; ii < NUM_PATTERNS; ++ii)
    {
        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3], values[ii]);

        // When
        Int32ToFourBytes(values[ii], actual);
        
        // Then
        TEST_ASSERT_EQUAL_UINT8_ARRAY(patterns[ii], actual, 4);
    }
}

void test_WhenFloatValue_ThenValidFourBytesValue(void)
{
    #define NUM_PATTERNS (4)
    UINT8 patterns[NUM_PATTERNS][4] = {{0x00, 0x00, 0x00, 0x00}, 
                                         {0x00,0x00,0x80,0xBf},
                                         {0xC3, 0xF5, 0x48, 0x40},
                                         {0x4D, 0xF8, 0x2D, 0x40}};
    float values[NUM_PATTERNS] = {0, -1, 3.14, 2.71828};
    UINT8 ii;
    UINT8 actual[4];
    
    //TEST_IGNORE();
    
    for (ii = 0; ii < 4; ++ii)
    {
        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3],values[ii]);

        // When
        FloatToFourBytes(values[ii], actual);
        
        // Then
        TEST_ASSERT_EQUAL_HEX8_ARRAY(patterns[ii], actual, 4);
    }
}

/*-------------------------------------------------------------------------------------------------
    BinaryRangeSearch
  -----------------------------------------------------------------------------------------------*/

void test_WhenNumPointsEquals2SearchEquals0_ThenLLIs0AndULIs0(void)
{
    INT16 data_points[2] = {0, 1};
    UINT8 lower_index;
    UINT8 upper_index;
    
    //TEST_IGNORE();
    
    // When
    BinaryRangeSearch(0, data_points, 2, &lower_index, &upper_index);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, lower_index);
    TEST_ASSERT_EQUAL_UINT8(0, upper_index);
}

void test_WhenNumPointsEquals3SearchEquals1_ThenLLIs1AndULIs1(void)
{
    INT16 data_points[3] = {0, 1, 2};
    UINT8 lower_index;
    UINT8 upper_index;
    
    //TEST_IGNORE();
    
    // When
    BinaryRangeSearch(1, data_points, 3, &lower_index, &upper_index);

    // Then
    TEST_ASSERT_EQUAL_UINT8(1, lower_index);
    TEST_ASSERT_EQUAL_UINT8(1, upper_index);
}

void test_WhenNumPointsEquals3SearchEquals3_ThenLLIs0AndULIs1(void)
{
    INT16 data_points[3] = {0, 5, 10};
    UINT8 lower_index;
    UINT8 upper_index;
    
    //TEST_IGNORE();
    
    // When
    BinaryRangeSearch(3, data_points, 3, &lower_index, &upper_index);

    // Then
    TEST_ASSERT_EQUAL_UINT8(0, lower_index);
    TEST_ASSERT_EQUAL_UINT8(1, upper_index);
}

void test_WhenNumPointsEquals3SearchEquals7_ThenLLIs1AndULIs2(void)
{
    INT16 data_points[3] = {0, 5, 10};
    UINT8 lower_index;
    UINT8 upper_index;
    
    //TEST_IGNORE();
    
    // When
    BinaryRangeSearch(7, data_points, 3, &lower_index, &upper_index);

    // Then
    TEST_ASSERT_EQUAL_UINT8(1, lower_index);
    TEST_ASSERT_EQUAL_UINT8(2, upper_index);
}

/*-------------------------------------------------------------------------------------------------
    Interpolate
  -----------------------------------------------------------------------------------------------*/

void test_WhenX1EqualsX2AndY1EqualsY2_ThenReturnY1(void)
{
    INT32 x1 = 1;
    INT32 x2 = 1;
    INT32 y1 = 2;
    INT32 y2 = 2;
    INT32 value;
    INT32 result = y1;
    
    //TEST_IGNORE();
    
    value = Interpolate(0, x1, x2, y1, y2);
    
    TEST_ASSERT_EQUAL(value, result);
}

void test_WhenX1EqualsX2AndY1NotEqualY2_ThenReturnY1PlusAvgDiffY2Y1(void)
{
    INT32 x1 = 1;
    INT32 x2 = 1;
    INT32 y1 = 1;
    INT32 y2 = 2;
    INT32 value;
    INT32 result = y1 + ((y2 - y1) / 2);
    
    //TEST_IGNORE();
    
    value = Interpolate(0, x1, x2, y1, y2);
    
    TEST_ASSERT_EQUAL(value, result);

}

void test_WhenAllZeros_ThenReturnZero(void)
{
    INT32 x1 = 0;
    INT32 x2 = 0;
    INT32 y1 = 0;
    INT32 y2 = 0;
    INT32 value;
    INT32 result = 0;
    
    //TEST_IGNORE();
    
    // When
    value = Interpolate(0, x1, x2, y1, y2);
    
    // Then
    TEST_ASSERT_EQUAL(value, result);

}

/* Considering creating a loop with multiple m/b combinations */
void test_WhenGivenX_ThenReturnYEqualsMxPlusb(void)
{
    FLOAT m = 1;
    FLOAT b = 0;
    INT32 x1 = -1000;
    INT32 x2 = 1000;
    INT32 y1 = m * x1 + b;
    INT32 y2 = m * x2 + b;
    INT32 x = 100;
    INT32 value;
    INT32 result = m * x + b;
    
    //TEST_IGNORE();
    
    value = Interpolate(x, x1, x2, y1, y2);
    
    TEST_ASSERT_EQUAL(value, result);
}

/*-------------------------------------------------------------------------------------------------
    CalcHeading
  -----------------------------------------------------------------------------------------------*/

void test_WhenLRCountEqual_ThenReturnZero(void)
{
    FLOAT heading;
    FLOAT left_count = 2000;
    FLOAT right_count = 2000;
    FLOAT radius = 1.0;
    FLOAT width = 1.0;
    FLOAT count_per_rev = 1;
    FLOAT bias = 1.0;
    
    //TEST_IGNORE();
    
    // When
    heading = CalcHeading(left_count, right_count, radius, width, count_per_rev, bias);
    
    TEST_ASSERT_EQUAL(0, heading);
}

void test_WhenLRCountDiffEqualsOne_ThenReturnPi(void)
{
    FLOAT heading;
    FLOAT left_count = 2000;
    FLOAT right_count = 2001;
    FLOAT radius = 1.0;
    FLOAT width = 1.0;
    FLOAT count_per_rev = 1;
    FLOAT bias = 1.0;
    
    //TEST_IGNORE();
    
    // When
    heading = CalcHeading(left_count, right_count, radius, width, count_per_rev, bias);
    
    // Then
    TEST_ASSERT_EQUAL(PI, heading);
}

void test_WhenLRCountDiffEqualsMinusOne_ThenReturnPi(void)
{
    FLOAT heading;
    FLOAT left_count = 2001;
    FLOAT right_count = 2000;
    FLOAT radius = 1.0;
    FLOAT width = 1.0;
    FLOAT count_per_rev = 1;
    FLOAT bias = 1.0;
    
    //TEST_IGNORE();
    
    // When
    heading = CalcHeading(left_count, right_count, radius, width, count_per_rev, bias);
    
    // Then
    TEST_ASSERT_EQUAL(-PI, heading);
}

/*-------------------------------------------------------------------------------------------------
    NormalizeHeading
  -----------------------------------------------------------------------------------------------*/

void test_WhenHeadingEqualZero_ThenReturnZero(void)
{
    FLOAT heading;
    
    //TEST_IGNORE();
    
    // When
    heading = NormalizeHeading(0.0);
    
    // Then
    TEST_ASSERT_EQUAL(0.0, heading);
}

void test_WhenHeadingEqualPI_ThenReturnPI(void)
{
    FLOAT heading;
    
    //TEST_IGNORE();
    
    // When
    // Note: Passing constant PI results in -PI likely because of idiosyncracies of floating
    // point implementation, so just use a shorter precision test value.
    heading = NormalizeHeading(3.14159);
    
    // Then
    TEST_ASSERT_EQUAL_FLOAT(PI, heading);
}

void test_WhenHeadingEqualMinusPI_ThenReturnMinusPI(void)
{
    FLOAT heading;
    
    //TEST_IGNORE();
    
    // When
    // Note: Passing constant -PI results in PI likely because of idiosyncracies of floating
    // point implementation, so just use a shorter precision test value.
    heading = NormalizeHeading(-3.14159);
    
    // Then
    TEST_ASSERT_EQUAL_FLOAT(-PI, heading);
}

void test_WhenHeadingGreaterThanPI_ThenReturnMinusPIToPI(void)
{
    FLOAT heading;
    
    //TEST_IGNORE();
    
    // When
    heading = NormalizeHeading(1.5 * 3.14159);
    
    // Then
    TEST_ASSERT(heading > -PI && heading < PI);
}

void test_WhenHeadingLessThanMinusPI_ThenReturnMinusPIToPI(void)
{
    FLOAT heading;
    
    //TEST_IGNORE();
    
    // When
    heading = NormalizeHeading(1.5 * -3.14159);
    
    // Then
    TEST_ASSERT(heading > -PI && heading < PI);
}

void test_WhenHeadingMultipleOfPI_ThenReturnMinusPIToPI(void)
{
    FLOAT heading;
    
    //TEST_IGNORE();
    
    // When
    heading = NormalizeHeading(5 * 3.14159);
    
    // Then
    TEST_ASSERT(heading > -PI && heading < PI);
}

void test_WhenHeadingNegMultipleOfPI_ThenReturnMinusPIToPI(void)
{
    FLOAT heading;
    
    //TEST_IGNORE();
    
    // When
    heading = NormalizeHeading(5 * -3.14159);
    
    // Then
    TEST_ASSERT(heading > -PI && heading < PI);
}

/*-------------------------------------------------------------------------------------------------
    CalcTriangularProfile
  -----------------------------------------------------------------------------------------------*/

void test_When3ElementProfileLowerIs1UpperIs3_Then131(void)
{
    UINT8 num_points = 3;
    FLOAT lower_limit = 1.0;
    FLOAT upper_limit = 3.0;
    FLOAT profile[3] = {0.0, 0.0, 0.0};
    FLOAT result[3] = {1.0, 3.0, 1.0};

    //TEST_IGNORE();

    // When
    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result[0], profile[0]);
    TEST_ASSERT_EQUAL_FLOAT(result[1], profile[1]);
    TEST_ASSERT_EQUAL_FLOAT(result[2], profile[2]);
}

void test_When3ElementProfileLowerIsNeg3UpperIsNeg1_ThenNeg131(void)
{
    UINT8 num_points = 3;
    FLOAT lower_limit = -1.0;
    FLOAT upper_limit = -3.0;
    FLOAT profile[3] = {0.0, 0.0, 0.0};
    FLOAT result[3] = {-1.0, -3.0, -1.0};

    //TEST_IGNORE();

    // When
    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result[0], profile[0]);
    TEST_ASSERT_EQUAL_FLOAT(result[1], profile[1]);
    TEST_ASSERT_EQUAL_FLOAT(result[2], profile[2]);
}

void test_When3ElementProfileLowerIsNeg1UpperIsPos1_ThenNeg1Pos1Neg1(void)
{
    UINT8 num_points = 3;
    FLOAT lower_limit = -1.0;
    FLOAT upper_limit = 1.0;
    FLOAT profile[3] = {0.0, 0.0, 0.0};
    FLOAT result[3] = {-1.0, 1.0, -1.0};

    //TEST_IGNORE();

    // When
    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result[0], profile[0]);
    TEST_ASSERT_EQUAL_FLOAT(result[1], profile[1]);
    TEST_ASSERT_EQUAL_FLOAT(result[2], profile[2]);
}

void test_When3ElementProfileLowerIsPos1UpperIsNeg1_ThenPos1Neg1Pos1(void)
{
    UINT8 num_points = 3;
    FLOAT lower_limit = 1.0;
    FLOAT upper_limit = -1.0;
    FLOAT profile[3] = {0.0, 0.0, 0.0};
    FLOAT result[3] = {1.0, -1.0, 1.0};

    //TEST_IGNORE();

    // When
    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);

    // Then
    TEST_ASSERT_EQUAL_FLOAT(result[0], profile[0]);
    TEST_ASSERT_EQUAL_FLOAT(result[1], profile[1]);
    TEST_ASSERT_EQUAL_FLOAT(result[2], profile[2]);
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();

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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();

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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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

    //TEST_IGNORE();
    
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
    
    //TEST_IGNORE();
    
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
    
    //TEST_IGNORE();

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
    
    //TEST_IGNORE();

    // When
    EnsureAngularVelocity(&v, &w);

    // Then
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0, v);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, result, w);
}

/*-------------------------------------------------------------------------------------------------
    AdjustVelocity
  -----------------------------------------------------------------------------------------------*/

void test_WhenNVGreaterThanLVAndNVLessThanMV_ThenNewVelocityAchieved(void)
{
    
    FLOAT result;

    //TEST_IGNORE();

    millis_ExpectAndReturn(100);
    millis_ExpectAndReturn(200);
    
    // When
    result = LimitLinearAccel(0.5, 1.0, 0.1);
    result = LimitLinearAccel(0.5, 1.0, 0.1);
    
    // Then
    TEST_ASSERT_EQUAL_FLOAT(0.5, result);

}

void test_WhenNVLessThanLVAndNVLessThanMV_ThenNewVelocityAchieved(void)
{
    
    FLOAT result;

    //TEST_IGNORE();

    millis_ExpectAndReturn(300);
    millis_ExpectAndReturn(400);

    /* Note: the LimitLinearAccel function maintains state.  The test of
       the deceleration path depends on the previous test to set the 
       last velocity.  Also, millis must continue to be incremented.
     */
    
    // When
    result = LimitLinearAccel(0.0, 1.0, 0.1);
    result = LimitLinearAccel(0.0, 1.0, 0.1);
    
    // Then
    TEST_ASSERT_EQUAL_FLOAT(0.0, result);
}