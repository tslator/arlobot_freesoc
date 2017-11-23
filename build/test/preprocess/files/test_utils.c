#include "build/temp/_test_utils.c"
#include "mock_assertion.h"
#include "mock_time.h"
#include "consts.h"
#include "utils.h"
#include "unity.h"


void setUp(void)

{

}



void tearDown(void)

{

}

void test_WhenTwoBytePatternValue_ThenValidUint16Value(void)

{



    UINT8 patterns[(4)][2] = {{0x00, 0x00}, {0xFF, 0xFF}, {0x00, 0x80}, {0xFF, 0x7F}};

    UINT16 values[(4)] = {0, 65535, 32768, 32767};

    UINT8 ii;

    UINT16 value;



    for (ii = 0; ii < (4); ++ii)

    {

        printf("Testing [0x%02X, 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], values[ii]);



        assertion_CMockExpect(38, 1, "bytes is null");





        value = TwoBytesToUint16(patterns[ii]);





        UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((values[ii])), (((void *)0)), (UNITY_UINT)(44), UNITY_DISPLAY_STYLE_INT);

    }

}



void test_WhenTwoBytePatternValue_ThenValidInt16Value(void)

{



    UINT8 patterns[(4)][2] = {{0x00, 0x00}, {0xFF, 0xFF}, {0x00, 0x80}, {0xFF, 0x7F}};

    INT16 values[(4)] = {0, -1, -32768, 32767};

    UINT8 ii;

    INT16 value;



    for (ii = 0; ii < (4); ++ii)

    {

        printf("Testing [0x%02X, 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], values[ii]);



        assertion_CMockExpect(60, 1, "bytes is null");





        value = TwoBytesToInt16(patterns[ii]);





        UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((values[ii])), (((void *)0)), (UNITY_UINT)(66), UNITY_DISPLAY_STYLE_INT);

    }

}



void test_WhenFourBytePatternValue_ThenValidUint32Value(void)

{



    UINT8 patterns[(4)][4] = {{0x00, 0x00, 0x00, 0x00},

                                         {0xFF, 0xFF, 0xFF, 0xFF},

                                         {0x00, 0x00, 0x00, 0x80},

                                         {0xFF, 0xFF, 0xFF, 0x7F}};

    UINT32 values[(4)] = {0, 4294967295UL, 2147483648UL, 2147483647UL};

    UINT8 ii;

    UINT32 value;



    for (ii = 0; ii < (4); ++ii)

    {

        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3], values[ii]);



        assertion_CMockExpect(85, 1, "bytes is null");





        value = FourBytesToUint32(patterns[ii]);





        UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((values[ii])), (((void *)0)), (UNITY_UINT)(91), UNITY_DISPLAY_STYLE_INT);

    }

}



void test_WhenFourBytePatternValue_ThenValidInt32Value(void)

{



    UINT8 patterns[(4)][4] = {{0x00, 0x00, 0x00, 0x00},

                                         {0xFF, 0xFF, 0xFF, 0xFF},

                                         {0x00, 0x00, 0x00, 0x80},

                                         {0xFF, 0xFF, 0xFF, 0x7F}};

    INT32 values[(4)] = {0L, -1L, -2147483647L - 1L, 2147483647L};

    UINT8 ii;

    INT32 value;



    for (ii = 0; ii < (4); ++ii)

    {

        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3], values[ii]);



        assertion_CMockExpect(110, 1, "bytes is null");





        value = FourBytesToInt32(patterns[ii]);





        UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((values[ii])), (((void *)0)), (UNITY_UINT)(116), UNITY_DISPLAY_STYLE_INT);

    }

}



void test_WhenUint32Value_ThenValidFourBytePatternValue(void)

{



    UINT8 patterns[(4)][4] = {{0x00, 0x00, 0x00, 0x00},

                                         {0xFF, 0xFF, 0xFF, 0xFF},

                                         {0x00, 0x00, 0x00, 0x80},

                                         {0xFF, 0xFF, 0xFF, 0x7F}};

    UINT32 values[(4)] = {0, 4294967295UL, 2147483648UL, 2147483647UL};

    UINT8 ii;

    UINT8 actual[4];



    for (ii = 0; ii < (4); ++ii)

    {

        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3], values[ii]);



        assertion_CMockExpect(135, 1, "bytes is null");





        Uint32ToFourBytes(values[ii], actual);





        UnityAssertEqualIntArray(( const void*)((patterns[ii])), ( const void*)((actual)), (UNITY_UINT32)((4)), (((void *)0)), (UNITY_UINT)(141), UNITY_DISPLAY_STYLE_UINT8, UNITY_ARRAY_TO_ARRAY);

    }

}



void test_WhenInt32Value_ThenValidFourBytesValue(void)

{



    UINT8 patterns[(4)][4] = {{0x00, 0x00, 0x00, 0x00},

                                         {0xFF, 0xFF, 0xFF, 0xFF},

                                         {0x00, 0x00, 0x00, 0x80},

                                         {0xFF, 0xFF, 0xFF, 0x7F}};

    UINT32 values[(4)] = {0L, -1L, -2147483647L - 1L, 2147483647L};

    UINT8 ii;

    UINT8 actual[4];



    for (ii = 0; ii < (4); ++ii)

    {

        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3], values[ii]);



        assertion_CMockExpect(160, 1, "bytes is null");





        Int32ToFourBytes(values[ii], actual);





        UnityAssertEqualIntArray(( const void*)((patterns[ii])), ( const void*)((actual)), (UNITY_UINT32)((4)), (((void *)0)), (UNITY_UINT)(166), UNITY_DISPLAY_STYLE_UINT8, UNITY_ARRAY_TO_ARRAY);

    }

}



void test_WhenFloatValue_ThenValidFourBytesValue(void)

{



    UINT8 patterns[(4)][4] = {{0x00, 0x00, 0x00, 0x00},

                                         {0x00,0x00,0x80,0xBf},

                                         {0xC3, 0xF5, 0x48, 0x40},

                                         {0x4D, 0xF8, 0x2D, 0x40}};

    float values[(4)] = {0, -1, 3.14, 2.71828};

    UINT8 ii;

    UINT8 actual[4];



    for (ii = 0; ii < 4; ++ii)

    {

        printf("Testing [0x%02X 0x%02X 0x%02X 0x%02X] => Expecting %d\n", patterns[ii][0], patterns[ii][1], patterns[ii][2], patterns[ii][3],values[ii]);



        assertion_CMockExpect(185, 1, "bytes is null");





        FloatToFourBytes(values[ii], actual);





        UnityAssertEqualIntArray(( const void*)((patterns[ii])), ( const void*)((actual)), (UNITY_UINT32)((4)), (((void *)0)), (UNITY_UINT)(191), UNITY_DISPLAY_STYLE_HEX8, UNITY_ARRAY_TO_ARRAY);

    }

}











void test_WhenDataPointsIsNull_ThenAssertion(void)

{

    UINT8 lower_index;

    UINT8 upper_index;



    assertion_CMockExpect(204, 0, "data_points is NULL");

    assertion_CMockExpect(205, 1, "lower_index is NULL");

    assertion_CMockExpect(206, 1, "upper_index is NULL");

    assertion_CMockExpect(207, 1, "num_points <= 1");





    BinaryRangeSearch(0, 0, 2, &lower_index, &upper_index);







}



void test_WhenLowerLimitIsNull_ThenAssertion(void)

{

    INT16 data_points[2] = {0, 0};

    UINT8 upper_index;



    assertion_CMockExpect(221, 1, "data_points is NULL");

    assertion_CMockExpect(222, 0, "lower_index is NULL");

    assertion_CMockExpect(223, 1, "upper_index is NULL");

    assertion_CMockExpect(224, 1, "num_points <= 1");





    BinaryRangeSearch(0, data_points, 2, 0, &upper_index);







}



void test_WhenUpperIndexIsNull_ThenAssertion(void)

{

    INT16 data_points[2] = {0, 0};

    UINT8 lower_index;



    assertion_CMockExpect(238, 1, "data_points is NULL");

    assertion_CMockExpect(239, 1, "lower_index is NULL");

    assertion_CMockExpect(240, 0, "upper_index is NULL");

    assertion_CMockExpect(241, 1, "num_points <= 1");





    BinaryRangeSearch(0, data_points, 2, &lower_index, 0);







}



void test_WhenLowerIndexIsNull_ThenAssertion(void)

{

    INT16 data_points[2] = {0, 0};

    UINT8 lower_index;

    UINT8 upper_index;



    assertion_CMockExpect(256, 1, "data_points is NULL");

    assertion_CMockExpect(257, 1, "lower_index is NULL");

    assertion_CMockExpect(258, 1, "upper_index is NULL");

    assertion_CMockExpect(259, 0, "num_points <= 1");





    BinaryRangeSearch(0, data_points, 0, &lower_index, &upper_index);







}



void test_WhenNumPointsEquals2SearchEquals0_ThenLLIs0AndULIs0(void)

{

    INT16 data_points[2] = {0, 1};

    UINT8 lower_index;

    UINT8 upper_index;



    assertion_CMockExpect(274, 1, "data_points is NULL");

    assertion_CMockExpect(275, 1, "lower_index is NULL");

    assertion_CMockExpect(276, 1, "upper_index is NULL");

    assertion_CMockExpect(277, 1, "num_points <= 1");





    BinaryRangeSearch(0, data_points, 2, &lower_index, &upper_index);





    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0)), (UNITY_INT)(UNITY_UINT8 )((lower_index)), (((void *)0)), (UNITY_UINT)(283), UNITY_DISPLAY_STYLE_UINT8);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0)), (UNITY_INT)(UNITY_UINT8 )((upper_index)), (((void *)0)), (UNITY_UINT)(284), UNITY_DISPLAY_STYLE_UINT8);

}



void test_WhenNumPointsEquals3SearchEquals1_ThenLLIs1AndULIs1(void)

{

    INT16 data_points[3] = {0, 1, 2};

    UINT8 lower_index;

    UINT8 upper_index;



    assertion_CMockExpect(293, 1, "data_points is NULL");

    assertion_CMockExpect(294, 1, "lower_index is NULL");

    assertion_CMockExpect(295, 1, "upper_index is NULL");

    assertion_CMockExpect(296, 1, "num_points <= 1");





    BinaryRangeSearch(1, data_points, 3, &lower_index, &upper_index);





    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((lower_index)), (((void *)0)), (UNITY_UINT)(302), UNITY_DISPLAY_STYLE_UINT8);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((upper_index)), (((void *)0)), (UNITY_UINT)(303), UNITY_DISPLAY_STYLE_UINT8);

}



void test_WhenNumPointsEquals3SearchEquals3_ThenLLIs0AndULIs1(void)

{

    INT16 data_points[3] = {0, 5, 10};

    UINT8 lower_index;

    UINT8 upper_index;



    assertion_CMockExpect(312, 1, "data_points is NULL");

    assertion_CMockExpect(313, 1, "lower_index is NULL");

    assertion_CMockExpect(314, 1, "upper_index is NULL");

    assertion_CMockExpect(315, 1, "num_points <= 1");





    BinaryRangeSearch(3, data_points, 3, &lower_index, &upper_index);





    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0)), (UNITY_INT)(UNITY_UINT8 )((lower_index)), (((void *)0)), (UNITY_UINT)(321), UNITY_DISPLAY_STYLE_UINT8);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((upper_index)), (((void *)0)), (UNITY_UINT)(322), UNITY_DISPLAY_STYLE_UINT8);

}



void test_WhenNumPointsEquals3SearchEquals7_ThenLLIs1AndULIs2(void)

{

    INT16 data_points[3] = {0, 5, 10};

    UINT8 lower_index;

    UINT8 upper_index;



    assertion_CMockExpect(331, 1, "data_points is NULL");

    assertion_CMockExpect(332, 1, "lower_index is NULL");

    assertion_CMockExpect(333, 1, "upper_index is NULL");

    assertion_CMockExpect(334, 1, "num_points <= 1");





    BinaryRangeSearch(7, data_points, 3, &lower_index, &upper_index);





    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((lower_index)), (((void *)0)), (UNITY_UINT)(340), UNITY_DISPLAY_STYLE_UINT8);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((2)), (UNITY_INT)(UNITY_UINT8 )((upper_index)), (((void *)0)), (UNITY_UINT)(341), UNITY_DISPLAY_STYLE_UINT8);

}











void test_WhenX1EqualsX2AndY1EqualsY2_ThenReturnY1(void)

{

    INT16 x1 = 1;

    INT16 x2 = 1;

    UINT16 y1 = 2;

    UINT16 y2 = 2;

    INT16 value;

    INT16 result = (INT16) y1;



    value = Interpolate(0, x1, x2, y1, y2);



    UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((result)), (((void *)0)), (UNITY_UINT)(359), UNITY_DISPLAY_STYLE_INT);

}



void test_WhenX1EqualsX2AndY1NotEqualY2_ThenReturnY1PlusAvgDiffY2Y1(void)

{

    INT16 x1 = 1;

    INT16 x2 = 1;

    UINT16 y1 = 1;

    UINT16 y2 = 2;

    INT16 value;

    INT16 result = y1 + ((y2 - y1) / 2);



    value = Interpolate(0, x1, x2, y1, y2);



    UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((result)), (((void *)0)), (UNITY_UINT)(373), UNITY_DISPLAY_STYLE_INT);



}



void test_WhenAllZeros_ThenReturnZero(void)

{

    INT16 x1 = 0;

    INT16 x2 = 0;

    UINT16 y1 = 0;

    UINT16 y2 = 0;

    INT16 value;

    INT16 result = 0;





    value = Interpolate(0, x1, x2, y1, y2);





    UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((result)), (((void *)0)), (UNITY_UINT)(390), UNITY_DISPLAY_STYLE_INT);



}





void test_WhenGivenX_ThenReturnYEqualsMxPlusb(void)

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



    UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((result)), (((void *)0)), (UNITY_UINT)(409), UNITY_DISPLAY_STYLE_INT);

}











void test_WhenLRCountEqual_ThenReturnZero(void)

{

    FLOAT heading;

    UINT16 left_count = 2000;

    UINT16 right_count = 2000;

    FLOAT radius = 1.0;

    FLOAT width = 1.0;

    FLOAT count_per_rev = 1;

    FLOAT bias = 1.0;





    heading = CalcHeading(left_count, right_count, radius, width, count_per_rev, bias);



    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((heading)), (((void *)0)), (UNITY_UINT)(429), UNITY_DISPLAY_STYLE_INT);

}



void test_WhenLRCountDiffEqualsOne_ThenReturnPi(void)

{

    FLOAT heading;

    UINT16 left_count = 2000;

    UINT16 right_count = 2001;

    FLOAT radius = 1.0;

    FLOAT width = 1.0;

    FLOAT count_per_rev = 1;

    FLOAT bias = 1.0;





    heading = CalcHeading(left_count, right_count, radius, width, count_per_rev, bias);





    UnityAssertEqualNumber((UNITY_INT)(((3.14159265358979323846))), (UNITY_INT)((heading)), (((void *)0)), (UNITY_UINT)(446), UNITY_DISPLAY_STYLE_INT);

}



void test_WhenLRCountDiffEqualsMinusOne_ThenReturnPi(void)

{

    FLOAT heading;

    UINT16 left_count = 2001;

    UINT16 right_count = 2000;

    FLOAT radius = 1.0;

    FLOAT width = 1.0;

    FLOAT count_per_rev = 1;

    FLOAT bias = 1.0;





    heading = CalcHeading(left_count, right_count, radius, width, count_per_rev, bias);





    UnityAssertEqualNumber((UNITY_INT)((-(3.14159265358979323846))), (UNITY_INT)((heading)), (((void *)0)), (UNITY_UINT)(463), UNITY_DISPLAY_STYLE_INT);

}











void test_WhenHeadingEqualZero_ThenReturnZero(void)

{

    FLOAT heading;





    heading = NormalizeHeading(0.0);





    UnityAssertEqualNumber((UNITY_INT)((0.0)), (UNITY_INT)((heading)), (((void *)0)), (UNITY_UINT)(478), UNITY_DISPLAY_STYLE_INT);

}



void test_WhenHeadingEqualPI_ThenReturnPI(void)

{

    FLOAT heading;









    heading = NormalizeHeading(3.14159);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)(((3.14159265358979323846))) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)(((3.14159265358979323846)))), (UNITY_FLOAT)((UNITY_FLOAT)((heading))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(491)));

}



void test_WhenHeadingEqualMinusPI_ThenReturnMinusPI(void)

{

    FLOAT heading;









    heading = NormalizeHeading(-3.14159);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((-(3.14159265358979323846))) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((-(3.14159265358979323846)))), (UNITY_FLOAT)((UNITY_FLOAT)((heading))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(504)));

}



void test_WhenHeadingGreaterThanPI_ThenReturnMinusPIToPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(1.5 * 3.14159);





    if ((heading > -(3.14159265358979323846) && heading < (3.14159265358979323846))) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(515)));};

}



void test_WhenHeadingLessThanMinusPI_ThenReturnMinusPIToPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(1.5 * -3.14159);





    if ((heading > -(3.14159265358979323846) && heading < (3.14159265358979323846))) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(526)));};

}



void test_WhenHeadingMultipleOfPI_ThenReturnMinusPIToPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(5 * 3.14159);





    if ((heading > -(3.14159265358979323846) && heading < (3.14159265358979323846))) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(537)));};

}



void test_WhenHeadingNegMultipleOfPI_ThenReturnMinusPIToPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(5 * -3.14159);





    if ((heading > -(3.14159265358979323846) && heading < (3.14159265358979323846))) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(548)));};

}











void test_WhenNumPointEven_ThenAssert(void)

{

    UINT8 num_points = 2;

    FLOAT lower_limit = 1.0;

    FLOAT upper_limit = 2.0;

    FLOAT profile[2] = {0.0, 0.0};

    FLOAT result[2] = {0.0, 0.0};



    assertion_CMockExpect(563, 0, "num_points is not odd");

    assertion_CMockExpect(564, 1, "lower_limit >= upper_limit");





    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);







}



void test_WhenLowerEqualsUpper_ThenAssert(void)

{

    UINT8 num_points = 3;

    FLOAT lower_limit = 10.0;

    FLOAT upper_limit = 10.0;

    FLOAT profile[2];



    assertion_CMockExpect(580, 1, "num_points is not odd");

    assertion_CMockExpect(581, 0, "lower_limit >= upper_limit");





    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);







}



void test_WhenLowerGreaterThanUpper_ThenAssert(void)

{

    UINT8 num_points = 3;

    FLOAT lower_limit = 10.0;

    FLOAT upper_limit = 0.0;

    FLOAT profile[2];



    assertion_CMockExpect(597, 1, "num_points is not odd");

    assertion_CMockExpect(598, 0, "lower_limit >= upper_limit");





    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);







}



void test_WhenNumPoints7AndRange1To10_ThenAssert(void)

{

    UINT8 num_points = 7;

    FLOAT lower_limit = 1.0;

    FLOAT upper_limit = 10.0;

    FLOAT results[7] = {1.0, 4.0, 7.0, 10.0, 7.0, 4.0, 1.0};

    FLOAT profile[7];



    assertion_CMockExpect(615, 1, "num_points is not odd");

    assertion_CMockExpect(616, 1, "lower_limit >= upper_limit");





    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);





    UnityAssertEqualFloatArray((UNITY_FLOAT*)((results)), (UNITY_FLOAT*)((profile)), (UNITY_UINT32)((7)), (((void *)0)), (UNITY_UINT)(622), UNITY_ARRAY_TO_ARRAY);

}



void test_WhenNumPoints21AndRangeMinus10To10_ThenAssert(void)

{

    UINT8 num_points = 21;

    FLOAT lower_limit = -10.0;

    FLOAT upper_limit = 10.0;

    FLOAT results[21] = {-10.0, -8.0, -6.0, -4.0, -2.0, 0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 8.0, 6.0, 4.0, 2.0, 0.0, -2.0, -4.0, -6.0, -8.0, -10.0};

    FLOAT profile[21];



    assertion_CMockExpect(633, 1, "num_points is not odd");

    assertion_CMockExpect(634, 1, "lower_limit >= upper_limit");





    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);





    UnityAssertEqualFloatArray((UNITY_FLOAT*)((results)), (UNITY_FLOAT*)((profile)), (UNITY_UINT32)((21)), (((void *)0)), (UNITY_UINT)(640), UNITY_ARRAY_TO_ARRAY);

}











void test_WhenLinearAngularZero_ThenLeftRightZero(void)

{

    float linear = 0.0;

    float angular = 0.0;

    float left;

    float right;



    assertion_CMockExpect(654, 1, "left is NULL");

    assertion_CMockExpect(655, 1, "right is NULL");





    UniToDiff(linear, angular, &left, &right);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((left))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(661)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((right))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(662)));

}



void test_WhenLinearForwardMaxAngularZero_ThenLeftRightMax(void)

{

    float linear = (((95.0) / (60.0)) * ((2 * (3.14159265358979323846))));

    float angular = 0.0;

    float left;

    float right;

    float result = 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (2 * (0.0775));



    assertion_CMockExpect(673, 1, "left is NULL");

    assertion_CMockExpect(674, 1, "right is NULL");





    UniToDiff(linear, angular, &left, &right);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((left))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(680)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((right))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(681)));

}



void test_WhenLinearBackwardMaxAngularZero_ThenLeftRightMax(void)

{

    float linear = -(((95.0) / (60.0)) * ((2 * (3.14159265358979323846))));

    float angular = 0.0;

    float left;

    float right;

    float result = -2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (2 * (0.0775));



    assertion_CMockExpect(692, 1, "left is NULL");

    assertion_CMockExpect(693, 1, "right is NULL");





    UniToDiff(linear, angular, &left, &right);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((left))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(699)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((right))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(700)));

}



void test_WhenAngularCWMaxLinearZero_ThenLeftRightMax(void)

{

    float linear = 0.0;

    float angular = ((((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4))) / ((0.3968) / 2));

    float left;

    float right;

    float left_result = -(((95.0) / (60.0)) * ((2 * (3.14159265358979323846))));

    float right_result = (((95.0) / (60.0)) * ((2 * (3.14159265358979323846))));



    assertion_CMockExpect(712, 1, "left is NULL");

    assertion_CMockExpect(713, 1, "right is NULL");





    UniToDiff(linear, angular, &left, &right);





    UnityAssertFloatsWithin((UNITY_FLOAT)((0.00001f)), (UNITY_FLOAT)((left_result)), (UNITY_FLOAT)((left)), (((void *)0)), (UNITY_UINT)(719));

    UnityAssertFloatsWithin((UNITY_FLOAT)((0.00001f)), (UNITY_FLOAT)((right_result)), (UNITY_FLOAT)((right)), (((void *)0)), (UNITY_UINT)(720));

}











void test_WhenLeftRightZero_ThenLinearAngularZero(void)

{

    float left = 0.0;

    float right = 0.0;

    float linear;

    float angular;

    float result = 0.0;



    assertion_CMockExpect(735, 1, "linear is NULL");

    assertion_CMockExpect(736, 1, "angular is NULL");





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(742)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(743)));

}



void test_WhenLeftRightForwardEqualsOne_ThenLinearForwardWheelRadiusAngularZero(void)

{

    float left = 1.0;

    float right = 1.0;

    float linear;

    float angular;



    assertion_CMockExpect(753, 1, "linear is NULL");

    assertion_CMockExpect(754, 1, "angular is NULL");





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)(((0.0775))) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)(((0.0775)))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(760)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(761)));

}



void test_WhenLeftRightBackwardEqualsOne_ThenLinearBackwardWheelRadiusAngularZero(void)

{

    float left = -1.0;

    float right = -1.0;

    float linear;

    float angular;



    assertion_CMockExpect(771, 1, "linear is NULL");

    assertion_CMockExpect(772, 1, "angular is NULL");





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((-(0.0775))) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((-(0.0775)))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(778)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(779)));

}



void test_WhenLeftRightForwardEqual_ThenLinearForwardNWheelRadiusAngularZero(void)

{

    float left = 5.0;

    float right = 5.0;

    float linear;

    float angular;

    float result = left * (0.0775);



    assertion_CMockExpect(790, 1, "linear is NULL");

    assertion_CMockExpect(791, 1, "angular is NULL");





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(797)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(798)));

}



void test_WhenLeftRightBackwardEqual_ThenLinearBackwardNWheelRadiusAngularZero(void)

{

    float left = -5.0;

    float right = -5.0;

    float linear;

    float angular;

    float result = left * (0.0775);



    assertion_CMockExpect(809, 1, "linear is NULL");

    assertion_CMockExpect(810, 1, "angular is NULL");





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(816)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(817)));

}



void test_WhenLeftZeroRightTwo_ThenLinearWheelRadiusAngular2WheelRadiusOverTrackWidth(void)

{

    float left = 0.0;

    float right = 2.0;

    float linear;

    float angular;

    float linear_result = (0.0775);

    float angular_result = 2 * (0.0775) / (0.3968);



    assertion_CMockExpect(829, 1, "linear is NULL");

    assertion_CMockExpect(830, 1, "angular is NULL");





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((linear_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((linear_result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(836)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((angular_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((angular_result))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(837)));

}



void test_WhenLeftMinusTwoRightZero_ThenLinearWheelRadiusAngularMinus2WheelRadiusOverTrackWidth(void)

{

    float left = 0.0;

    float right = -2.0;

    float linear;

    float angular;

    float linear_result = -(0.0775);

    float angular_result = -2 * (0.0775) / (0.3968);



    assertion_CMockExpect(849, 1, "linear is NULL");

    assertion_CMockExpect(850, 1, "angular is NULL");





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((linear_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((linear_result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(856)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((angular_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((angular_result))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(857)));

}











void test_WhenVWEqualZero_ThenEnsureVWEqualZero(void)

{

    float v = 0.0;

    float w = 0.0;



    assertion_CMockExpect(869, 1, "v is NULL");

    assertion_CMockExpect(870, 1, "w is NULL");

    assertion_CMockExpect(871, 1, "left is NULL");

    assertion_CMockExpect(872, 1, "right is NULL");





    EnsureAngularVelocity(&v, &w);





    if ((v == 0.0 && w == 0.0)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(878)));};

}



void test_WhenMaxForwardVZeroW_ThenEnsureMaxForwardV(void)

{

    float v = (((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));

    float w = 0.0;

    float result = (((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));



    assertion_CMockExpect(887, 1, "v is NULL");

    assertion_CMockExpect(888, 1, "w is NULL");

    assertion_CMockExpect(889, 1, "left is NULL");

    assertion_CMockExpect(890, 1, "right is NULL");





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(896)));

}



void test_WhenMaxBackwardVZeroW_ThenEnsureMaxBackwardV(void)

{

    float v = -(((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));

    float w = 0.0;

    float result = -(((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));



    assertion_CMockExpect(905, 1, "v is NULL");

    assertion_CMockExpect(906, 1, "w is NULL");

    assertion_CMockExpect(907, 1, "left is NULL");

    assertion_CMockExpect(908, 1, "right is NULL");





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(914)));

}



void test_WhenVForwardMaxAndMatchingW_ThenEnsureVIsZeroWUnchanged(void)

{

    float v = (((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));

    float w = 2 * v / (0.3968);

    float linear_result = 0.0;

    float angular_result = 2 * v / (0.3968);



    assertion_CMockExpect(924, 1, "v is NULL");

    assertion_CMockExpect(925, 1, "w is NULL");

    assertion_CMockExpect(926, 1, "left is NULL");

    assertion_CMockExpect(927, 1, "right is NULL");

    assertion_CMockExpect(928, 1, "linear is NULL");

    assertion_CMockExpect(929, 1, "angular is NULL");





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((linear_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((linear_result))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(935)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((angular_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((angular_result))), (UNITY_FLOAT)((UNITY_FLOAT)((w))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(936)));

}



void test_WhenVBackwardMaxAndMatchingW_ThenEnsureVIsZeroWUnchanged(void)

{

    float v = -(((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));

    float w = 2 * v / (0.3968);

    float linear_result = 0.0;

    float angular_result = 2 * v / (0.3968);



    assertion_CMockExpect(946, 1, "v is NULL");

    assertion_CMockExpect(947, 1, "w is NULL");

    assertion_CMockExpect(948, 1, "left is NULL");

    assertion_CMockExpect(949, 1, "right is NULL");

    assertion_CMockExpect(950, 1, "linear is NULL");

    assertion_CMockExpect(951, 1, "angular is NULL");





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((linear_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((linear_result))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(957)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((angular_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((angular_result))), (UNITY_FLOAT)((UNITY_FLOAT)((w))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(958)));

}





void test_WhenForwardVZeroCWWMatch_ThenEnsureVIs0WUnchanged(void)

{

    float v = 0.0;

    float w = (0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);

    float result = (0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);



    assertion_CMockExpect(968, 1, "v is NULL");

    assertion_CMockExpect(969, 1, "w is NULL");

    assertion_CMockExpect(970, 1, "left is NULL");

    assertion_CMockExpect(971, 1, "right is NULL");





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(977)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((w))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(978)));

}



void test_WhenForwardVGreaterThanZeroCWWMatch_ThenEnsureVIs0WUnchanged(void)

{

    float v = 1000.0;

    float w = (0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);

    float result = (0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);



    assertion_CMockExpect(987, 1, "v is NULL");

    assertion_CMockExpect(988, 1, "w is NULL");

    assertion_CMockExpect(989, 1, "left is NULL");

    assertion_CMockExpect(990, 1, "right is NULL");

    assertion_CMockExpect(991, 1, "linear is NULL");

    assertion_CMockExpect(992, 1, "angular is NULL");





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((0.0001f)), (UNITY_FLOAT)((0.0)), (UNITY_FLOAT)((v)), (((void *)0)), (UNITY_UINT)(998));

    UnityAssertFloatsWithin((UNITY_FLOAT)((0.0001f)), (UNITY_FLOAT)((result)), (UNITY_FLOAT)((w)), (((void *)0)), (UNITY_UINT)(999));

}



void test_WhenBackwardVGreaterThanZeroCWWMatch_ThenEnsureVIs0WUnchanged(void)

{

    float v = -1000.0;

    float w = -(0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);

    float result = -(0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);



    assertion_CMockExpect(1008, 1, "v is NULL");

    assertion_CMockExpect(1009, 1, "w is NULL");

    assertion_CMockExpect(1010, 1, "left is NULL");

    assertion_CMockExpect(1011, 1, "right is NULL");

    assertion_CMockExpect(1012, 1, "linear is NULL");

    assertion_CMockExpect(1013, 1, "angular is NULL");





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((0.0001f)), (UNITY_FLOAT)((0.0)), (UNITY_FLOAT)((v)), (((void *)0)), (UNITY_UINT)(1019));

    UnityAssertFloatsWithin((UNITY_FLOAT)((0.0001f)), (UNITY_FLOAT)((result)), (UNITY_FLOAT)((w)), (((void *)0)), (UNITY_UINT)(1020));

}











void test_WhenNVGreaterThanLVAndNVLessThanMV_ThenNewVelocityAchieved(void)

{





    FLOAT result;



    assertion_CMockExpect(1033, 1, "last_time is NULL");

    millis_CMockExpectAndReturn(1034, 100);

    assertion_CMockExpect(1035, 1, "last_time is NULL");

    millis_CMockExpectAndReturn(1036, 200);





    result = LimitLinearAccel(0.5, 1.0, 0.1);

    result = LimitLinearAccel(0.5, 1.0, 0.1);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.5)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.5))), (UNITY_FLOAT)((UNITY_FLOAT)((result))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(1043)));



}



void test_WhenNVLessThanLVAndNVLessThanMV_ThenNewVelocityAchieved(void)

{





    FLOAT result;



    assertion_CMockExpect(1053, 1, "last_time is NULL");

    millis_CMockExpectAndReturn(1054, 300);

    assertion_CMockExpect(1055, 1, "last_time is NULL");

    millis_CMockExpectAndReturn(1056, 400);















    result = LimitLinearAccel(0.0, 1.0, 0.1);

    result = LimitLinearAccel(0.0, 1.0, 0.1);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((result))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(1068)));

}
