/****************************** Module Header ******************************\
Module Name:  TestMetar.cpp
Project:      Meteorology
Copyright (c) 2014 Steven Frost.

This module defines and implements various tests for the Metar class.

This source is subject to the MIT License.
See http://opensource.org/licenses/MIT

All other rights reserved.
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "stdafx.hpp"
#include "CppUnitTest.h"
#include "../Source/Metar.hpp"
#include "../Source/Meteorology.hpp"

using namespace Meteorology;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {
	TEST_CLASS(TestMetar) {
	private:
		const double SM_TO_FT = 5280;
		const double SM_TO_M = 1609.344;

	public:
		TEST_METHOD(StandardMetar) {
			Metar *metar = new Metar("METAR KSFO 172256Z 00000KT 9SM CLR 19/04 A3012 ");

			Assert::AreEqual(Metar::StationIdentifier("KSFO"), metar->getStationIdentifier());
			Assert::AreEqual(17U, metar->getObservationTime()->dayOfMonth);
			Assert::AreEqual(22U, metar->getObservationTime()->hourOfDay);
			Assert::AreEqual(56U, metar->getObservationTime()->minuteOfHour);
			Assert::AreEqual(0U, metar->getWind()->speed);
			Assert::AreEqual(0U, metar->getWind()->direction);
			Assert::AreEqual(0U, metar->getWind()->gustSpeed);
			Assert::AreEqual(0U, metar->getWind()->variationLower);
			Assert::AreEqual(0U, metar->getWind()->variationUpper);
			Assert::AreEqual(9.0 * SM_TO_FT, metar->getVisibilityF(), 0.01);
			Assert::AreEqual(9.0 * SM_TO_M, metar->getVisibilityM(), 0.01);
			Assert::AreEqual(30.12, metar->getAltimeterinHg(), 0.01);
			Assert::AreEqual(1019.98, metar->getAltimeterhPa(), 0.01);
		}

		TEST_METHOD(AltimeterTests) {
			Metar *metar;
			Metar::MetarInfo data;

			metar = new Metar("METAR KSFO 172256Z 00000KT 9SM CLR 19/04 A3012 ");
			//metar->getMetarStruct(data);
			Assert::AreEqual(30.12, data.altimeter, 0.005);

			metar = new Metar("METAR KSFO 172256Z 00000KT 9SM CLR 19/04 Q1013 ");
			//metar->getMetarStruct(data);
			Assert::AreEqual(29.92, data.altimeter, 0.005);
		}
	};
}