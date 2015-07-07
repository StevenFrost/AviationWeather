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

#include "Meteorology.TestPch.h"

#include <string>

#include <Meteorology/Metar.h>
#include <Meteorology/Meteorology.h>

using namespace Meteorology;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{

TEST_CLASS(MetarTests)
{
public:
    TEST_METHOD(METAR_KSFO)
    {
        auto metar = std::make_shared<Metar>("METAR KSFO 172256Z 00000KT 9SM CLR 19/04 A3012");

        Assert::AreEqual(static_cast<unsigned int>(MetarReportType::Metar), static_cast<unsigned int>(metar->GetReportType()));
        Assert::AreEqual(std::string("KSFO"), metar->GetStationIdentifier());
        Assert::AreEqual(17U, metar->GetObservationTime()->GetDayOfMonth());
        Assert::AreEqual(22U, metar->GetObservationTime()->GetHourOfDay());
        Assert::AreEqual(56U, metar->GetObservationTime()->GetMinuteOfHour());
        Assert::AreEqual(static_cast<unsigned int>(MetarModifier::Automatic), static_cast<unsigned int>(metar->GetModifier()));
        Assert::AreEqual(0U, metar->GetWind()->GetSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetDirection());
        Assert::AreEqual(0U, metar->GetWind()->GetGustSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationLower());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationUpper());
        Assert::AreEqual(MilesToFeet(9.0), metar->GetVisibility(), 0.01);

        auto weatherList = metar->GetWeatherList();
        Assert::AreEqual(0U, static_cast<unsigned int>(weatherList.size()));

        auto skyConditionList = metar->GetSkyConditionList();
        Assert::AreEqual(1U, static_cast<unsigned int>(skyConditionList.size()));

        auto it = skyConditionList.front();
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Clear), static_cast<unsigned int>(it->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(it->GetCloudType()));
        Assert::AreEqual(0U, it->GetHeight());

        Assert::AreEqual(19, metar->GetTemperature());
        Assert::AreEqual(4, metar->GetDewpoint());

        Assert::AreEqual(InhgToHpa(30.12), metar->GetAltimeterhPa(), 0.01);
        Assert::AreEqual(30.12, metar->GetAltimeterinHg(), 0.01);

        Assert::AreEqual(std::string(""), metar->GetRemarks());
    }

    TEST_METHOD(METAR_CYUL)
    {
        auto metar = std::make_shared<Metar>("CYUL 042100Z 20006KT 15SM FEW120 SCT170 BKN240 24/10 A2989 RMK AC1AC2CI4 SLP123 DENSITY ALT 1200FT");

        Assert::AreEqual(static_cast<unsigned int>(MetarReportType::Metar), static_cast<unsigned int>(metar->GetReportType()));
        Assert::AreEqual(std::string("CYUL"), metar->GetStationIdentifier());
        Assert::AreEqual(04U, metar->GetObservationTime()->GetDayOfMonth());
        Assert::AreEqual(21U, metar->GetObservationTime()->GetHourOfDay());
        Assert::AreEqual(00U, metar->GetObservationTime()->GetMinuteOfHour());
        Assert::AreEqual(static_cast<unsigned int>(MetarModifier::Automatic), static_cast<unsigned int>(metar->GetModifier()));
        Assert::AreEqual(6U, metar->GetWind()->GetSpeed());
        Assert::AreEqual(200U, metar->GetWind()->GetDirection());
        Assert::AreEqual(0U, metar->GetWind()->GetGustSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationLower());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationUpper());
        Assert::AreEqual(MilesToFeet(15.0), metar->GetVisibility(), 0.01);

        auto weatherList = metar->GetWeatherList();
        Assert::AreEqual(0U, static_cast<unsigned int>(weatherList.size()));

        auto skyConditionList = metar->GetSkyConditionList();
        Assert::AreEqual(3U, static_cast<unsigned int>(skyConditionList.size()));

        auto it = skyConditionList.begin();
        auto item1 = *it;
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Few), static_cast<unsigned int>(item1->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(item1->GetCloudType()));
        Assert::AreEqual(12000U, item1->GetHeight());
        
        auto item2 = *(++it);
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Scattered), static_cast<unsigned int>(item2->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(item2->GetCloudType()));
        Assert::AreEqual(17000U, item2->GetHeight());

        auto item3 = *(++it);
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Broken), static_cast<unsigned int>(item3->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(item3->GetCloudType()));
        Assert::AreEqual(24000U, item3->GetHeight());

        Assert::AreEqual(24, metar->GetTemperature());
        Assert::AreEqual(10, metar->GetDewpoint());

        Assert::AreEqual(InhgToHpa(29.89), metar->GetAltimeterhPa(), 0.01);
        Assert::AreEqual(29.89, metar->GetAltimeterinHg(), 0.01);

        Assert::AreEqual(std::string("AC1AC2CI4 SLP123 DENSITY ALT 1200FT"), metar->GetRemarks());
    }

    TEST_METHOD(METAR_KVNY)
    {
        auto metar = std::make_shared<Metar>("KVNY 302351Z COR 14014KT 10SM BKN150 28/14 A2996 RMK AO2 PK WND 16029/2305 SLP135 T02830139 10372 20283 53030");

        Assert::AreEqual(static_cast<unsigned int>(MetarReportType::Metar), static_cast<unsigned int>(metar->GetReportType()));
        Assert::AreEqual(std::string("KVNY"), metar->GetStationIdentifier());
        Assert::AreEqual(30U, metar->GetObservationTime()->GetDayOfMonth());
        Assert::AreEqual(23U, metar->GetObservationTime()->GetHourOfDay());
        Assert::AreEqual(51U, metar->GetObservationTime()->GetMinuteOfHour());
        Assert::AreEqual(static_cast<unsigned int>(MetarModifier::Corrected), static_cast<unsigned int>(metar->GetModifier()));
        Assert::AreEqual(14U, metar->GetWind()->GetSpeed());
        Assert::AreEqual(140U, metar->GetWind()->GetDirection());
        Assert::AreEqual(0U, metar->GetWind()->GetGustSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationLower());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationUpper());
        Assert::AreEqual(MilesToFeet(10.0), metar->GetVisibility(), 0.01);

        auto weatherList = metar->GetWeatherList();
        Assert::AreEqual(0U, static_cast<unsigned int>(weatherList.size()));

        auto skyConditionList = metar->GetSkyConditionList();
        Assert::AreEqual(1U, static_cast<unsigned int>(skyConditionList.size()));

        auto it = skyConditionList.front();
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Broken), static_cast<unsigned int>(it->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(it->GetCloudType()));
        Assert::AreEqual(15000U, it->GetHeight());

        Assert::AreEqual(28, metar->GetTemperature());
        Assert::AreEqual(14, metar->GetDewpoint());

        Assert::AreEqual(InhgToHpa(29.96), metar->GetAltimeterhPa(), 0.01);
        Assert::AreEqual(29.96, metar->GetAltimeterinHg(), 0.01);

        Assert::AreEqual(std::string("AO2 PK WND 16029/2305 SLP135 T02830139 10372 20283 53030"), metar->GetRemarks());
    }

    TEST_METHOD(METAR_KLAX)
    {
        auto metar = std::make_shared<Metar>("KLAX 011153Z COR 00000KT 10SM FEW007 SCT050 SCT160 BKN250 20/18 A2990 RMK AO2 SLP122 FRQ LTGICCGCA DSNT N CB DSNT N-NE T02000178 10200 20189 50014 $");

        Assert::AreEqual(static_cast<unsigned int>(MetarReportType::Metar), static_cast<unsigned int>(metar->GetReportType()));
        Assert::AreEqual(std::string("KLAX"), metar->GetStationIdentifier());
        Assert::AreEqual(1U, metar->GetObservationTime()->GetDayOfMonth());
        Assert::AreEqual(11U, metar->GetObservationTime()->GetHourOfDay());
        Assert::AreEqual(53U, metar->GetObservationTime()->GetMinuteOfHour());
        Assert::AreEqual(static_cast<unsigned int>(MetarModifier::Corrected), static_cast<unsigned int>(metar->GetModifier()));
        Assert::AreEqual(0U, metar->GetWind()->GetSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetDirection());
        Assert::AreEqual(0U, metar->GetWind()->GetGustSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationLower());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationUpper());
        Assert::AreEqual(MilesToFeet(10.0), metar->GetVisibility(), 0.01);

        auto weatherList = metar->GetWeatherList();
        Assert::AreEqual(0U, static_cast<unsigned int>(weatherList.size()));

        auto skyConditionList = metar->GetSkyConditionList();
        Assert::AreEqual(4U, static_cast<unsigned int>(skyConditionList.size()));

        auto it = skyConditionList.begin();
        auto item1 = *it;
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Few), static_cast<unsigned int>(item1->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(item1->GetCloudType()));
        Assert::AreEqual(700U, item1->GetHeight());

        auto item2 = *(++it);
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Scattered), static_cast<unsigned int>(item2->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(item2->GetCloudType()));
        Assert::AreEqual(5000U, item2->GetHeight());

        auto item3 = *(++it);
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Scattered), static_cast<unsigned int>(item3->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(item3->GetCloudType()));
        Assert::AreEqual(16000U, item3->GetHeight());

        auto item4 = *(++it);
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Broken), static_cast<unsigned int>(item4->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(item4->GetCloudType()));
        Assert::AreEqual(25000U, item4->GetHeight());

        Assert::AreEqual(20, metar->GetTemperature());
        Assert::AreEqual(18, metar->GetDewpoint());

        Assert::AreEqual(InhgToHpa(29.90), metar->GetAltimeterhPa(), 0.01);
        Assert::AreEqual(29.90, metar->GetAltimeterinHg(), 0.01);

        Assert::AreEqual(std::string("AO2 SLP122 FRQ LTGICCGCA DSNT N CB DSNT N-NE T02000178 10200 20189 50014 $"), metar->GetRemarks());
    }

    TEST_METHOD(METAR_KRNO_1)
    {
        auto metar = std::make_shared<Metar>("KRNO 030155Z 26015G23KT 10SM TS SCT110CB BKN180 28/13 A3006 RMK AO2 PK WND 31027/0109 RAB03E20 PRESRR SLP119 OCNL LTGIC NW TS NW MOV W CB DSNT N S SH VC SW NW P0000 T02830128");

        Assert::AreEqual(static_cast<unsigned int>(MetarReportType::Metar), static_cast<unsigned int>(metar->GetReportType()));
        Assert::AreEqual(std::string("KRNO"), metar->GetStationIdentifier());
        Assert::AreEqual(3U, metar->GetObservationTime()->GetDayOfMonth());
        Assert::AreEqual(1U, metar->GetObservationTime()->GetHourOfDay());
        Assert::AreEqual(55U, metar->GetObservationTime()->GetMinuteOfHour());
        Assert::AreEqual(static_cast<unsigned int>(MetarModifier::Automatic), static_cast<unsigned int>(metar->GetModifier()));
        Assert::AreEqual(15U, metar->GetWind()->GetSpeed());
        Assert::AreEqual(260U, metar->GetWind()->GetDirection());
        Assert::AreEqual(23U, metar->GetWind()->GetGustSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationLower());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationUpper());
        Assert::AreEqual(MilesToFeet(10.0), metar->GetVisibility(), 0.01);

        auto weatherList = metar->GetWeatherList();
        Assert::AreEqual(1U, static_cast<unsigned int>(weatherList.size()));

        auto weatherIt = weatherList.begin();
        auto weatherItem1 = *weatherIt;
        auto weatherItem1PhenomenaList = weatherItem1->GetPhenomenaList();
        Assert::AreEqual(static_cast<unsigned int>(WeatherIntensity::Moderate), static_cast<unsigned int>(weatherItem1->GetIntensity()));
        Assert::AreEqual(static_cast<unsigned int>(WeatherDescriptor::Thunderstorm), static_cast<unsigned int>(weatherItem1->GetDescriptor()));
        Assert::AreEqual(0U, static_cast<unsigned int>(weatherItem1PhenomenaList.size()));

        auto skyConditionList = metar->GetSkyConditionList();
        Assert::AreEqual(2U, static_cast<unsigned int>(skyConditionList.size()));

        auto skyConditionIt = skyConditionList.begin();
        auto skyConditionItem1 = *skyConditionIt;
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Scattered), static_cast<unsigned int>(skyConditionItem1->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::Cumulonimbus), static_cast<unsigned int>(skyConditionItem1->GetCloudType()));
        Assert::AreEqual(11000U, skyConditionItem1->GetHeight());

        auto skyConditionItem2 = *(++skyConditionIt);
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Broken), static_cast<unsigned int>(skyConditionItem2->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(skyConditionItem2->GetCloudType()));
        Assert::AreEqual(18000U, skyConditionItem2->GetHeight());

        Assert::AreEqual(28, metar->GetTemperature());
        Assert::AreEqual(13, metar->GetDewpoint());

        Assert::AreEqual(InhgToHpa(30.06), metar->GetAltimeterhPa(), 0.01);
        Assert::AreEqual(30.06, metar->GetAltimeterinHg(), 0.01);

        Assert::AreEqual(std::string("AO2 PK WND 31027/0109 RAB03E20 PRESRR SLP119 OCNL LTGIC NW TS NW MOV W CB DSNT N S SH VC SW NW P0000 T02830128"), metar->GetRemarks());
    }

    TEST_METHOD(METAR_KRNO_2)
    {
        auto metar = std::make_shared<Metar>("KRNO 010155Z 09027G32KT 10SM +TSRA BKN090CB BKN160 BKN250 29/13 A3007 RMK AO2 PK WND 09032/0155 WSHFT 0130 RAB32 TSB02 PRESRR SLP120 FRQ LTGCGIC OHD NE-E TS OHD NE-E MOV N P0000 T02890128");

        Assert::AreEqual(static_cast<unsigned int>(MetarReportType::Metar), static_cast<unsigned int>(metar->GetReportType()));
        Assert::AreEqual(std::string("KRNO"), metar->GetStationIdentifier());
        Assert::AreEqual(1U, metar->GetObservationTime()->GetDayOfMonth());
        Assert::AreEqual(1U, metar->GetObservationTime()->GetHourOfDay());
        Assert::AreEqual(55U, metar->GetObservationTime()->GetMinuteOfHour());
        Assert::AreEqual(static_cast<unsigned int>(MetarModifier::Automatic), static_cast<unsigned int>(metar->GetModifier()));
        Assert::AreEqual(27U, metar->GetWind()->GetSpeed());
        Assert::AreEqual(90U, metar->GetWind()->GetDirection());
        Assert::AreEqual(32U, metar->GetWind()->GetGustSpeed());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationLower());
        Assert::AreEqual(0U, metar->GetWind()->GetVariationUpper());
        Assert::AreEqual(MilesToFeet(10.0), metar->GetVisibility(), 0.01);

        auto weatherList = metar->GetWeatherList();
        Assert::AreEqual(1U, static_cast<unsigned int>(weatherList.size()));

        auto weatherIt = weatherList.begin();
        auto weatherItem1 = *weatherIt;
        auto weatherItem1PhenomenaList = weatherItem1->GetPhenomenaList();
        Assert::AreEqual(static_cast<unsigned int>(WeatherIntensity::Heavy), static_cast<unsigned int>(weatherItem1->GetIntensity()));
        Assert::AreEqual(static_cast<unsigned int>(WeatherDescriptor::Thunderstorm), static_cast<unsigned int>(weatherItem1->GetDescriptor()));
        Assert::AreEqual(1U, static_cast<unsigned int>(weatherItem1PhenomenaList.size()));
        Assert::AreEqual(static_cast<unsigned int>(WeatherPhenomena::Rain), static_cast<unsigned int>(weatherItem1PhenomenaList.front()));

        auto skyConditionList = metar->GetSkyConditionList();
        Assert::AreEqual(3U, static_cast<unsigned int>(skyConditionList.size()));

        auto skyConditionIt = skyConditionList.begin();
        auto skyConditionItem1 = *skyConditionIt;
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Broken), static_cast<unsigned int>(skyConditionItem1->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::Cumulonimbus), static_cast<unsigned int>(skyConditionItem1->GetCloudType()));
        Assert::AreEqual(9000U, skyConditionItem1->GetHeight());

        auto skyConditionItem2 = *(++skyConditionIt);
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Broken), static_cast<unsigned int>(skyConditionItem2->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(skyConditionItem2->GetCloudType()));
        Assert::AreEqual(16000U, skyConditionItem2->GetHeight());

        auto skyConditionItem3 = *(++skyConditionIt);
        Assert::AreEqual(static_cast<unsigned int>(SkyCover::Broken), static_cast<unsigned int>(skyConditionItem3->GetSkyCover()));
        Assert::AreEqual(static_cast<unsigned int>(SkyCoverClouds::None), static_cast<unsigned int>(skyConditionItem3->GetCloudType()));
        Assert::AreEqual(25000U, skyConditionItem3->GetHeight());

        Assert::AreEqual(29, metar->GetTemperature());
        Assert::AreEqual(13, metar->GetDewpoint());

        Assert::AreEqual(InhgToHpa(30.07), metar->GetAltimeterhPa(), 0.01);
        Assert::AreEqual(30.07, metar->GetAltimeterinHg(), 0.01);

        Assert::AreEqual(std::string("AO2 PK WND 09032/0155 WSHFT 0130 RAB32 TSB02 PRESRR SLP120 FRQ LTGCGIC OHD NE-E TS OHD NE-E MOV N P0000 T02890128"), metar->GetRemarks());
    }
};

} // namespace Tests
