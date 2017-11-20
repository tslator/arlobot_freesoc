#include "build/temp/_test_utils.c"
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

void test_WhenTwoBytesValue_ThenUint16Value(void)

{



    UINT8 patterns[(4)][2] = {{0x00, 0x00}, {0xFF, 0xFF}, {0x00, 0x80}, {0xFF, 0x7F}};

    UINT16 values[(4)] = {0, 65535, 32768, 32767};

    UINT8 ii;

    UINT16 value;



    for (ii = 0; ii < (4); ++ii)

    {



        value = TwoBytesToUint16(patterns[ii]);





        UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((values[ii])), (((void *)0)), (UNITY_UINT)(39), UNITY_DISPLAY_STYLE_INT);

    }

}



void test_WhenTwoBytesValue_ThenInt16Value(void)

{



    UINT8 patterns[(4)][2] = {{0x00, 0x00}, {0xFF, 0xFF}, {0x00, 0x80}, {0xFF, 0x7F}};

    INT16 values[(4)] = {0, -1, -32768, 32767};

    UINT8 ii;

    INT16 value;



    for (ii = 0; ii < (4); ++ii)

    {



        value = TwoBytesToInt16(patterns[ii]);





        UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((values[ii])), (((void *)0)), (UNITY_UINT)(57), UNITY_DISPLAY_STYLE_INT);

    }

}



void test_WhenFourBytesValue_ThenUint32Value(void)

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



        value = FourBytesToUint32(patterns[ii]);





        UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((values[ii])), (((void *)0)), (UNITY_UINT)(78), UNITY_DISPLAY_STYLE_INT);

    }

}



void test_WhenFourBytesValue_ThenInt32Value(void)

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



        value = FourBytesToInt32(patterns[ii]);





        UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((values[ii])), (((void *)0)), (UNITY_UINT)(99), UNITY_DISPLAY_STYLE_INT);

    }

}



void test_WhenUint32Value_ThenFourBytesValue(void)

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



        Uint32ToFourBytes(values[ii], actual);





        UnityAssertEqualIntArray(( const void*)((patterns[ii])), ( const void*)((actual)), (UNITY_UINT32)((4)), (((void *)0)), (UNITY_UINT)(120), UNITY_DISPLAY_STYLE_UINT8, UNITY_ARRAY_TO_ARRAY);

    }

}



void test_WhenInt32Value_ThenFourBytesValue(void)

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



        Int32ToFourBytes(values[ii], actual);





        UnityAssertEqualIntArray(( const void*)((patterns[ii])), ( const void*)((actual)), (UNITY_UINT32)((4)), (((void *)0)), (UNITY_UINT)(141), UNITY_DISPLAY_STYLE_UINT8, UNITY_ARRAY_TO_ARRAY);

    }

}



void test_WhenFloatValue_ThenFourBytesValue(void)

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



        FloatToFourBytes(values[ii], actual);





        UnityAssertEqualIntArray(( const void*)((patterns[ii])), ( const void*)((actual)), (UNITY_UINT32)((4)), (((void *)0)), (UNITY_UINT)(162), UNITY_DISPLAY_STYLE_HEX8, UNITY_ARRAY_TO_ARRAY);

    }

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



    UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((result)), (((void *)0)), (UNITY_UINT)(181), UNITY_DISPLAY_STYLE_INT);

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



    UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((result)), (((void *)0)), (UNITY_UINT)(195), UNITY_DISPLAY_STYLE_INT);



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





    UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((result)), (((void *)0)), (UNITY_UINT)(212), UNITY_DISPLAY_STYLE_INT);



}





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



    UnityAssertEqualNumber((UNITY_INT)((value)), (UNITY_INT)((result)), (((void *)0)), (UNITY_UINT)(231), UNITY_DISPLAY_STYLE_INT);

}











void test_WhenLRCountEqual_ThenReturnZero(void)

{

    FLOAT heading;





    heading = CalcHeading(2000, 2000, 1.0, 1.0, 1, 1.0);



    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((heading)), (((void *)0)), (UNITY_UINT)(245), UNITY_DISPLAY_STYLE_INT);

}



void test_WhenLRCountDiffEqualsOne_ThenReturnPi(void)

{

    FLOAT heading;





    heading = CalcHeading(2000, 2001, 1.0, 1.0, 1, 1.0);





    UnityAssertEqualNumber((UNITY_INT)(((3.14159265358979323846))), (UNITY_INT)((heading)), (((void *)0)), (UNITY_UINT)(256), UNITY_DISPLAY_STYLE_INT);

}



void test_WhenLRCountDiffEqualsMinusOne_ThenReturnPi(void)

{

    FLOAT heading;





    heading = CalcHeading(2001, 2000, 1.0, 1.0, 1, 1.0);





    UnityAssertEqualNumber((UNITY_INT)((-(3.14159265358979323846))), (UNITY_INT)((heading)), (((void *)0)), (UNITY_UINT)(267), UNITY_DISPLAY_STYLE_INT);

}











void test_WhenHeadingEqualZero_ThenReturnZero(void)

{

    FLOAT heading;





    heading = NormalizeHeading(0.0);





    UnityAssertEqualNumber((UNITY_INT)((0.0)), (UNITY_INT)((heading)), (((void *)0)), (UNITY_UINT)(282), UNITY_DISPLAY_STYLE_INT);

}



void test_WhenHeadingEqualPI_ThenReturnPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(3.14159);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((3.14159)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((3.14159))), (UNITY_FLOAT)((UNITY_FLOAT)((heading))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(293)));

}



void test_WhenHeadingEqualMinusPI_ThenReturnMinusPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(-3.14159);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((-3.14159)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((-3.14159))), (UNITY_FLOAT)((UNITY_FLOAT)((heading))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(304)));

}



void test_WhenHeadingGreaterThanPI_ThenReturnMinusPIToPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(1.5 * 3.14159);





    if ((heading > -3.14159 && heading < 3.14159)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(315)));};

}



void test_WhenHeadingLessThanMinusPI_ThenReturnMinusPIToPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(1.5 * -3.14159);





    if ((heading > -3.14159 && heading < 3.14159)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(326)));};

}



void test_WhenHeadingMultipleOfPI_ThenReturnMinusPIToPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(5 * 3.14159);





    if ((heading > -3.14159 && heading < 3.14159)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(337)));};

}



void test_WhenHeadingNegMultipleOfPI_ThenReturnMinusPIToPI(void)

{

    FLOAT heading;





    heading = NormalizeHeading(5 * -3.14159);





    if ((heading > -3.14159 && heading < 3.14159)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(348)));};

}











void test_WhenNumPointEven_ThenAssert(void)

{

    UINT8 num_points = 2;

    FLOAT lower_limit = 1.0;

    FLOAT upper_limit = 2.0;

    FLOAT profile[2];















    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);





    if ((1 == 1)) {} else {UnityFail( ((("Expecting assertion"))), (UNITY_UINT)((UNITY_UINT)(371)));};

}



void test_WhenLowerEqualsUpper_ThenAssert(void)

{

    UINT8 num_points = 3;

    FLOAT lower_limit = 10.0;

    FLOAT upper_limit = 10.0;

    FLOAT profile[2];





    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);





    if ((1 == 1)) {} else {UnityFail( ((("Expecting assertion"))), (UNITY_UINT)((UNITY_UINT)(385)));};

}



void test_WhenLowerGreaterThanUpper_ThenAssert(void)

{

    UINT8 num_points = 3;

    FLOAT lower_limit = 10.0;

    FLOAT upper_limit = 0.0;

    FLOAT profile[2];





    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);





    if ((1 == 1)) {} else {UnityFail( ((("Expecting assertion"))), (UNITY_UINT)((UNITY_UINT)(399)));};

}



void test_WhenNumPoints7AndRange1To10_ThenAssert(void)

{

    UINT8 num_points = 7;

    FLOAT lower_limit = 1.0;

    FLOAT upper_limit = 10.0;

    FLOAT results[7] = {1.0, 4.0, 7.0, 10.0, 7.0, 4.0, 1.0};

    FLOAT profile[7];





    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);





    UnityAssertEqualFloatArray((UNITY_FLOAT*)((results)), (UNITY_FLOAT*)((profile)), (UNITY_UINT32)((7)), (((void *)0)), (UNITY_UINT)(414), UNITY_ARRAY_TO_ARRAY);

}



void test_WhenNumPoints21AndRangeMinus10To10_ThenAssert(void)

{

    UINT8 num_points = 21;

    FLOAT lower_limit = -10.0;

    FLOAT upper_limit = 10.0;

    FLOAT results[21] = {-10.0, -8.0, -6.0, -4.0, -2.0, 0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 8.0, 6.0, 4.0, 2.0, 0.0, -2.0, -4.0, -6.0, -8.0, -10.0};

    FLOAT profile[21];





    CalcTriangularProfile(num_points, lower_limit, upper_limit, profile);





    UnityAssertEqualFloatArray((UNITY_FLOAT*)((results)), (UNITY_FLOAT*)((profile)), (UNITY_UINT32)((21)), (((void *)0)), (UNITY_UINT)(429), UNITY_ARRAY_TO_ARRAY);

}











void test_WhenLinearAngularZero_ThenLeftRightZero(void)

{

    float linear = 0.0;

    float angular = 0.0;

    float left;

    float right;





    UniToDiff(linear, angular, &left, &right);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((left))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(447)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((right))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(448)));

}



void test_WhenLinearForwardMaxAngularZero_ThenLeftRightMax(void)

{

    float linear = (((95.0) / (60.0)) * ((2 * (3.14159265358979323846))));

    float angular = 0.0;

    float left;

    float right;

    float result = 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (2 * (0.0775));





    UniToDiff(linear, angular, &left, &right);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((left))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(463)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((right))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(464)));

}



void test_WhenLinearBackwardMaxAngularZero_ThenLeftRightMax(void)

{

    float linear = -(((95.0) / (60.0)) * ((2 * (3.14159265358979323846))));

    float angular = 0.0;

    float left;

    float right;

    float result = -2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (2 * (0.0775));





    UniToDiff(linear, angular, &left, &right);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((left))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(479)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((right))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(480)));

}



void test_WhenAngularCWMaxLinearZero_ThenLeftRightMax(void)

{

    float linear = 0.0;

    float angular = ((((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4))) / ((0.3968) / 2));

    float left;

    float right;

    float left_result = -(((95.0) / (60.0)) * ((2 * (3.14159265358979323846))));

    float right_result = (((95.0) / (60.0)) * ((2 * (3.14159265358979323846))));





    UniToDiff(linear, angular, &left, &right);





    UnityAssertFloatsWithin((UNITY_FLOAT)((0.00001f)), (UNITY_FLOAT)((left_result)), (UNITY_FLOAT)((left)), (((void *)0)), (UNITY_UINT)(496));

    UnityAssertFloatsWithin((UNITY_FLOAT)((0.00001f)), (UNITY_FLOAT)((right_result)), (UNITY_FLOAT)((right)), (((void *)0)), (UNITY_UINT)(497));

}











void test_WhenLeftRightZero_ThenLinearAngularZero(void)

{

    float left = 0.0;

    float right = 0.0;

    float linear;

    float angular;

    float result = 0.0;





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(516)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(517)));

}



void test_WhenLeftRightForwardEqualsOne_ThenLinearForwardWheelRadiusAngularZero(void)

{

    float left = 1.0;

    float right = 1.0;

    float linear;

    float angular;





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)(((0.0775))) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)(((0.0775)))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(531)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(532)));

}



void test_WhenLeftRightBackwardEqualsOne_ThenLinearBackwardWheelRadiusAngularZero(void)

{

    float left = -1.0;

    float right = -1.0;

    float linear;

    float angular;





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((-(0.0775))) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((-(0.0775)))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(546)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(547)));

}



void test_WhenLeftRightForwardEqual_ThenLinearForwardNWheelRadiusAngularZero(void)

{

    float left = 5.0;

    float right = 5.0;

    float linear;

    float angular;

    float result = left * (0.0775);





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(562)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(563)));

}



void test_WhenLeftRightBackwardEqual_ThenLinearBackwardNWheelRadiusAngularZero(void)

{

    float left = -5.0;

    float right = -5.0;

    float linear;

    float angular;

    float result = left * (0.0775);





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(578)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(579)));

}



void test_WhenLeftZeroRightTwo_ThenLinearWheelRadiusAngular2WheelRadiusOverTrackWidth(void)

{

    float left = 0.0;

    float right = 2.0;

    float linear;

    float angular;

    float linear_result = (0.0775);

    float angular_result = 2 * (0.0775) / (0.3968);





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((linear_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((linear_result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(595)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((angular_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((angular_result))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(596)));

}



void test_WhenLeftMinusTwoRightZero_ThenLinearWheelRadiusAngularMinus2WheelRadiusOverTrackWidth(void)

{

    float left = 0.0;

    float right = -2.0;

    float linear;

    float angular;

    float linear_result = -(0.0775);

    float angular_result = -2 * (0.0775) / (0.3968);





    DiffToUni(left, right, &linear, &angular);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((linear_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((linear_result))), (UNITY_FLOAT)((UNITY_FLOAT)((linear))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(612)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((angular_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((angular_result))), (UNITY_FLOAT)((UNITY_FLOAT)((angular))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(613)));

}











void test_WhenVWEqualZero_ThenEnsureVWEqualZero(void)

{

    float v = 0.0;

    float w = 0.0;





    EnsureAngularVelocity(&v, &w);





    if ((v == 0.0 && w == 0.0)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(629)));};

}



void test_WhenMaxForwardVZeroW_ThenEnsureMaxForwardV(void)

{

    float v = (((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));

    float w = 0.0;

    float result = (((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(642)));

}



void test_WhenMaxBackwardVZeroW_ThenEnsureMaxBackwardV(void)

{

    float v = -(((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));

    float w = 0.0;

    float result = -(((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(655)));

}



void test_WhenVForwardMaxAndMatchingW_ThenEnsureVIsZeroWUnchanged(void)

{

    float v = (((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));

    float w = 2 * v / (0.3968);

    float linear_result = 0.0;

    float angular_result = 2 * v / (0.3968);





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((linear_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((linear_result))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(669)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((angular_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((angular_result))), (UNITY_FLOAT)((UNITY_FLOAT)((w))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(670)));

}



void test_WhenVBackwardMaxAndMatchingW_ThenEnsureVIsZeroWUnchanged(void)

{

    float v = -(((((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) * (((500) * 4) / (2 * (3.14159265358979323846)))) * ((((3.14159265358979323846) * (2 * (0.0775))))/((500) * 4)));

    float w = 2 * v / (0.3968);

    float linear_result = 0.0;

    float angular_result = 2 * v / (0.3968);





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((linear_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((linear_result))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(684)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((angular_result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((angular_result))), (UNITY_FLOAT)((UNITY_FLOAT)((w))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(685)));

}





void test_WhenForwardVZeroCWWMatch_ThenEnsureVIs0WUnchanged(void)

{

    float v = 0.0;

    float w = (0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);

    float result = (0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((0.0)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((0.0))), (UNITY_FLOAT)((UNITY_FLOAT)((v))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(699)));

    UnityAssertFloatsWithin((UNITY_FLOAT)((UNITY_FLOAT)((result)) * (UNITY_FLOAT)(0.00001f)), (UNITY_FLOAT)((UNITY_FLOAT)((result))), (UNITY_FLOAT)((UNITY_FLOAT)((w))), ((((void *)0))), (UNITY_UINT)((UNITY_UINT)(700)));

}



void test_WhenForwardVGreaterThanZeroCWWMatch_ThenEnsureVIs0WUnchanged(void)

{

    float v = 1000.0;

    float w = (0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);

    float result = (0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((0.0001f)), (UNITY_FLOAT)((0.0)), (UNITY_FLOAT)((v)), (((void *)0)), (UNITY_UINT)(713));

    UnityAssertFloatsWithin((UNITY_FLOAT)((0.0001f)), (UNITY_FLOAT)((result)), (UNITY_FLOAT)((w)), (((void *)0)), (UNITY_UINT)(714));

}



void test_WhenBackwardVGreaterThanZeroCWWMatch_ThenEnsureVIs0WUnchanged(void)

{

    float v = -1000.0;

    float w = -(0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);

    float result = -(0.0775) * 2 * (((95.0) / (60.0)) * ((2 * (3.14159265358979323846)))) / (0.3968);





    EnsureAngularVelocity(&v, &w);





    UnityAssertFloatsWithin((UNITY_FLOAT)((0.0001f)), (UNITY_FLOAT)((0.0)), (UNITY_FLOAT)((v)), (((void *)0)), (UNITY_UINT)(727));

    UnityAssertFloatsWithin((UNITY_FLOAT)((0.0001f)), (UNITY_FLOAT)((result)), (UNITY_FLOAT)((w)), (((void *)0)), (UNITY_UINT)(728));

}
