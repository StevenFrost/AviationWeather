/**********************************************************************************
 *                                                                                *
 * Copyright (c) 2015 Steven Frost, Orion Lyau. All rights reserved.              *
 *                                                                                *
 * This source is subject to the MIT License.                                     *
 * See http://opensource.org/licenses/MIT                                         *
 *                                                                                *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,    *
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED          *
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.         *
 *                                                                                *
 * NOT TO BE USED AS A SOLE SOURCE OF INFORMATION FOR FLIGHT CRITICAL OPERATIONS. *
 *                                                                                *
 **********************************************************************************/

#include "AviationWeather.TestPch.h"

#include "..\Source\utility.h"

#include "framework.h"

//-----------------------------------------------------------------------------

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//-----------------------------------------------------------------------------

namespace aw
{
namespace test
{

//-----------------------------------------------------------------------------

TEST_CLASS(UtilityTests)
{
public:
    TEST_METHOD(Utility_FloatEquality);
    TEST_METHOD(Utility_FloatGreaterThan);
    TEST_METHOD(Utility_FloatLessThan);
    TEST_METHOD(Utility_DoubleEquality);
    TEST_METHOD(Utility_DoubleGreaterThan);
    TEST_METHOD(Utility_DoubleLessThan);
};

//-----------------------------------------------------------------------------

void UtilityTests::Utility_FloatEquality()
{
    Assert::IsTrue(are_approximately_equal(1.0f, 1.0f));
    Assert::IsFalse(are_approximately_equal(1.0f, 2.0f));
}

//-----------------------------------------------------------------------------

void UtilityTests::Utility_FloatGreaterThan()
{
    Assert::IsTrue(is_greater_than(2.0f, 1.0f));
    Assert::IsFalse(is_greater_than(1.0f, 2.0f));


    Assert::IsTrue(is_greater_than(1.000001f, 1.0f));
    Assert::IsFalse(is_greater_than(1.0f, 1.000001f));
}

//-----------------------------------------------------------------------------

void UtilityTests::Utility_FloatLessThan()
{
    Assert::IsTrue(is_less_than(1.0f, 2.0f));
    Assert::IsFalse(is_less_than(2.0f, 1.0f));

    Assert::IsTrue(is_less_than(1.0f, 1.000001f));
    Assert::IsFalse(is_less_than(1.000001f, 1.0f));
}

//-----------------------------------------------------------------------------

void UtilityTests::Utility_DoubleEquality()
{
    Assert::IsTrue(are_approximately_equal(1.0, 1.0));
    Assert::IsFalse(are_approximately_equal(1.0, 2.0));
}

//-----------------------------------------------------------------------------

void UtilityTests::Utility_DoubleGreaterThan()
{
    Assert::IsTrue(is_greater_than(2.0, 1.0));
    Assert::IsFalse(is_greater_than(1.0, 2.0));


    Assert::IsTrue(is_greater_than(1.000001, 1.0));
    Assert::IsFalse(is_greater_than(1.0, 1.000001));
}

//-----------------------------------------------------------------------------

void UtilityTests::Utility_DoubleLessThan()
{
    Assert::IsTrue(is_less_than(1.0, 2.0));
    Assert::IsFalse(is_less_than(2.0, 1.0));

    Assert::IsTrue(is_less_than(1.0, 1.000001));
    Assert::IsFalse(is_less_than(1.000001, 1.0));
}

//-----------------------------------------------------------------------------

} // namespace test
} // namespace aw
