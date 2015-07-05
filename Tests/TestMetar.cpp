/****************************** Module Header ******************************\
Module Name:  TestMetar.cpp
Project:      Meteorology
Copyright (c) 2015 Steven Frost.

This module defines and implements various tests for the Metar class.

This source is subject to the MIT License.
See http://opensource.org/licenses/MIT

All other rights reserved.
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "stdafx.hpp"
#include <iostream>
#include <fstream>
#include "CppUnitTest.h"
#include "../Source/Metar.hpp"
#include "../Source/Meteorology.hpp"

using namespace Meteorology;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{

TEST_CLASS(MetarTests)
{
private:
    const double SM_TO_FT = 5280;
    const double SM_TO_M = 1609.344;

public:
    TEST_METHOD(METAR_Basic_KSFO)
    {
        auto metar = std::make_shared<Metar>("METAR KSFO 172256Z 00000KT 9SM CLR 19/04 A3012 ");

        Assert::AreEqual(std::string("KSFO"), metar->GetStationIdentifier());
        Assert::AreEqual(17U, metar->GetObservationTime()->GetDayOfMonth());
        Assert::AreEqual(22U, metar->GetObservationTime()->GetHourOfDay());
        Assert::AreEqual(56U, metar->GetObservationTime()->GetMinuteOfHour());
        Assert::AreEqual(0U, metar->GetWind()->GetSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetDirection());
        Assert::AreEqual(0U, metar->GetWind()->GetGustSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationLower());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationUpper());
        Assert::AreEqual(9.0 * SM_TO_M, metar->GetVisibility(), 0.01);
        Assert::AreEqual(1019.98, metar->GetAltimeter(), 0.01);
    }
};

} // namespace Tests
