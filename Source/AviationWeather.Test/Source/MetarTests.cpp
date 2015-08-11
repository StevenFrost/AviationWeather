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

#include <string>

#include <AviationWeather/metar.h>
#include <AviationWeather/types.h>

#include "framework.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace aw
{
namespace test
{

TEST_CLASS(MetarTests)
{
public:
    TEST_METHOD(METAR_KSFO)
    {
        aw::metar::metar_info metar("METAR KSFO 172256Z 00000KT 9SM CLR 19/04 A3012");

        Assert::AreEqual(aw::metar::report_type::metar, metar.type);

        Assert::AreEqual(std::string("KSFO"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(17), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(22), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(56), metar.report_time.minute_of_hour);

        Assert::AreEqual(aw::metar::modifier_type::automatic, metar.modifier);

        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint16_t>(0), metar.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.gust_speed);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);

        Assert::AreEqual(aw::distance_unit::statute_miles, metar.visibility_group.unit);
        Assert::AreEqual(9.0, metar.visibility_group.distance);

        Assert::AreEqual(0U, static_cast<uint32_t>(metar.weather_group.size()));

        Assert::AreEqual(1U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto it = metar.sky_condition_group.begin();
        Assert::AreEqual(aw::sky_cover_type::clear, it->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, it->cloud_type);
        Assert::AreEqual(0U, it->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(19), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(4), metar.dewpoint);

        Assert::AreEqual(aw::pressure_unit::inHg, metar.altimeter_group.unit);
        Assert::AreEqual(30.12, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string(""), metar.remarks);
    }

    TEST_METHOD(METAR_CYUL)
    {
        aw::metar::metar_info metar("CYUL 042100Z 20006KT 15SM FEW120 SCT170 BKN240 24/10 A2989 RMK AC1AC2CI4 SLP123 DENSITY ALT 1200FT");

        Assert::AreEqual(aw::metar::report_type::metar, metar.type);

        Assert::AreEqual(std::string("CYUL"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(4), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(21), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(0), metar.report_time.minute_of_hour);

        Assert::AreEqual(aw::metar::modifier_type::automatic, metar.modifier);

        Assert::AreEqual(static_cast<uint8_t>(6), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint16_t>(200), metar.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.gust_speed);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);

        Assert::AreEqual(aw::distance_unit::statute_miles, metar.visibility_group.unit);
        Assert::AreEqual(15.0, metar.visibility_group.distance);

        Assert::AreEqual(0U, static_cast<uint32_t>(metar.weather_group.size()));

        Assert::AreEqual(3U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto it = metar.sky_condition_group.begin();
        Assert::AreEqual(aw::sky_cover_type::few, it->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, it->cloud_type);
        Assert::AreEqual(12000U, it->layer_height);

        ++it;
        Assert::AreEqual(aw::sky_cover_type::scattered, it->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, it->cloud_type);
        Assert::AreEqual(17000U, it->layer_height);

        ++it;
        Assert::AreEqual(aw::sky_cover_type::broken, it->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, it->cloud_type);
        Assert::AreEqual(24000U, it->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(24), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(10), metar.dewpoint);

        Assert::AreEqual(aw::pressure_unit::inHg, metar.altimeter_group.unit);
        Assert::AreEqual(29.89, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AC1AC2CI4 SLP123 DENSITY ALT 1200FT"), metar.remarks);
    }

    TEST_METHOD(METAR_KVNY)
    {
        aw::metar::metar_info metar("KVNY 302351Z COR 14014KT 10SM BKN150 28/14 A2996 RMK AO2 PK WND 16029/2305 SLP135 T02830139 10372 20283 53030");

        Assert::AreEqual(aw::metar::report_type::metar, metar.type);

        Assert::AreEqual(std::string("KVNY"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(30), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(23), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(51), metar.report_time.minute_of_hour);

        Assert::AreEqual(aw::metar::modifier_type::corrected, metar.modifier);

        Assert::AreEqual(static_cast<uint8_t>(14), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint16_t>(140), metar.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.gust_speed);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);

        Assert::AreEqual(aw::distance_unit::statute_miles, metar.visibility_group.unit);
        Assert::AreEqual(10.0, metar.visibility_group.distance);

        Assert::AreEqual(0U, static_cast<uint32_t>(metar.weather_group.size()));

        Assert::AreEqual(1U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto it = metar.sky_condition_group.begin();
        Assert::AreEqual(aw::sky_cover_type::broken, it->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, it->cloud_type);
        Assert::AreEqual(15000U, it->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(28), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(14), metar.dewpoint);

        Assert::AreEqual(aw::pressure_unit::inHg, metar.altimeter_group.unit);
        Assert::AreEqual(29.96, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AO2 PK WND 16029/2305 SLP135 T02830139 10372 20283 53030"), metar.remarks);
    }

    TEST_METHOD(METAR_KLAX)
    {
        aw::metar::metar_info metar("KLAX 011153Z COR 00000KT 10SM FEW007 SCT050 SCT160 BKN250 20/18 A2990 RMK AO2 SLP122 FRQ LTGICCGCA DSNT N CB DSNT N-NE T02000178 10200 20189 50014 $");

        Assert::AreEqual(aw::metar::report_type::metar, metar.type);

        Assert::AreEqual(std::string("KLAX"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(1), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(11), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(53), metar.report_time.minute_of_hour);

        Assert::AreEqual(aw::metar::modifier_type::corrected, metar.modifier);

        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint16_t>(0), metar.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.gust_speed);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);

        Assert::AreEqual(aw::distance_unit::statute_miles, metar.visibility_group.unit);
        Assert::AreEqual(10.0, metar.visibility_group.distance);

        Assert::AreEqual(0U, static_cast<uint32_t>(metar.weather_group.size()));

        Assert::AreEqual(4U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto it = metar.sky_condition_group.begin();
        Assert::AreEqual(aw::sky_cover_type::few, it->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, it->cloud_type);
        Assert::AreEqual(700U, it->layer_height);

        ++it;
        Assert::AreEqual(aw::sky_cover_type::scattered, it->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, it->cloud_type);
        Assert::AreEqual(5000U, it->layer_height);

        ++it;
        Assert::AreEqual(aw::sky_cover_type::scattered, it->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, it->cloud_type);
        Assert::AreEqual(16000U, it->layer_height);

        ++it;
        Assert::AreEqual(aw::sky_cover_type::broken, it->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, it->cloud_type);
        Assert::AreEqual(25000U, it->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(20), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(18), metar.dewpoint);

        Assert::AreEqual(aw::pressure_unit::inHg, metar.altimeter_group.unit);
        Assert::AreEqual(29.90, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AO2 SLP122 FRQ LTGICCGCA DSNT N CB DSNT N-NE T02000178 10200 20189 50014 $"), metar.remarks);
    }

    TEST_METHOD(METAR_KRNO_1)
    {
        aw::metar::metar_info metar("KRNO 030155Z 26015G23KT 10SM TS SCT110CB BKN180 28/13 A3006 RMK AO2 PK WND 31027/0109 RAB03E20 PRESRR SLP119 OCNL LTGIC NW TS NW MOV W CB DSNT N S SH VC SW NW P0000 T02830128");

        Assert::AreEqual(aw::metar::report_type::metar, metar.type);

        Assert::AreEqual(std::string("KRNO"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(3), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(1), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(55), metar.report_time.minute_of_hour);

        Assert::AreEqual(aw::metar::modifier_type::automatic, metar.modifier);

        Assert::AreEqual(static_cast<uint8_t>(15), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint16_t>(260), metar.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(23), metar.wind_group.gust_speed);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);

        Assert::AreEqual(aw::distance_unit::statute_miles, metar.visibility_group.unit);
        Assert::AreEqual(10.0, metar.visibility_group.distance);

        Assert::AreEqual(1U, static_cast<uint32_t>(metar.weather_group.size()));

        auto itWeather = metar.weather_group.begin();
        Assert::AreEqual(aw::weather_intensity::moderate, itWeather->intensity);
        Assert::AreEqual(aw::weather_descriptor::thunderstorm, itWeather->descriptor);
        Assert::AreEqual(0U, static_cast<uint32_t>(itWeather->phenomena.size()));

        Assert::AreEqual(2U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto itSky = metar.sky_condition_group.begin();
        Assert::AreEqual(aw::sky_cover_type::scattered, itSky->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::cumulonimbus, itSky->cloud_type);
        Assert::AreEqual(11000U, itSky->layer_height);

        ++itSky;
        Assert::AreEqual(aw::sky_cover_type::broken, itSky->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, itSky->cloud_type);
        Assert::AreEqual(18000U, itSky->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(28), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(13), metar.dewpoint);

        Assert::AreEqual(aw::pressure_unit::inHg, metar.altimeter_group.unit);
        Assert::AreEqual(30.06, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AO2 PK WND 31027/0109 RAB03E20 PRESRR SLP119 OCNL LTGIC NW TS NW MOV W CB DSNT N S SH VC SW NW P0000 T02830128"), metar.remarks);
    }

    TEST_METHOD(METAR_KRNO_2)
    {
        aw::metar::metar_info metar("KRNO 010155Z 09027G32KT 10SM +TSRA BKN090CB BKN160 BKN250 29/13 A3007 RMK AO2 PK WND 09032/0155 WSHFT 0130 RAB32 TSB02 PRESRR SLP120 FRQ LTGCGIC OHD NE-E TS OHD NE-E MOV N P0000 T02890128");

        Assert::AreEqual(aw::metar::report_type::metar, metar.type);

        Assert::AreEqual(std::string("KRNO"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(1), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(1), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(55), metar.report_time.minute_of_hour);

        Assert::AreEqual(aw::metar::modifier_type::automatic, metar.modifier);

        Assert::AreEqual(static_cast<uint8_t>(27), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint16_t>(90), metar.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(32), metar.wind_group.gust_speed);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
        Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);

        Assert::AreEqual(aw::distance_unit::statute_miles, metar.visibility_group.unit);
        Assert::AreEqual(10.0, metar.visibility_group.distance);

        Assert::AreEqual(1U, static_cast<uint32_t>(metar.weather_group.size()));

        auto itWeather = metar.weather_group.begin();
        Assert::AreEqual(aw::weather_intensity::heavy, itWeather->intensity);
        Assert::AreEqual(aw::weather_descriptor::thunderstorm, itWeather->descriptor);

        Assert::AreEqual(1U, static_cast<uint32_t>(itWeather->phenomena.size()));
        Assert::AreEqual(aw::weather_phenomena::rain, itWeather->phenomena.front());

        Assert::AreEqual(3U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto itSky = metar.sky_condition_group.begin();
        Assert::AreEqual(aw::sky_cover_type::broken, itSky->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::cumulonimbus, itSky->cloud_type);
        Assert::AreEqual(9000U, itSky->layer_height);

        ++itSky;
        Assert::AreEqual(aw::sky_cover_type::broken, itSky->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, itSky->cloud_type);
        Assert::AreEqual(16000U, itSky->layer_height);

        ++itSky;
        Assert::AreEqual(aw::sky_cover_type::broken, itSky->sky_cover);
        Assert::AreEqual(aw::sky_cover_cloud_type::none, itSky->cloud_type);
        Assert::AreEqual(25000U, itSky->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(29), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(13), metar.dewpoint);

        Assert::AreEqual(aw::pressure_unit::inHg, metar.altimeter_group.unit);
        Assert::AreEqual(30.07, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AO2 PK WND 09032/0155 WSHFT 0130 RAB32 TSB02 PRESRR SLP120 FRQ LTGCGIC OHD NE-E TS OHD NE-E MOV N P0000 T02890128"), metar.remarks);
    }

    TEST_METHOD(METAR_VariableWinds)
    {
        aw::metar::metar_info m1("KSFO 112056Z VRB17KT 10SM FEW010 SCT180 22/12 A2994 RMK AO2 SLP137 T02220122 58006");

        Assert::IsTrue(m1.wind_group.is_variable());
        Assert::AreEqual(UINT16_MAX, m1.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(17), m1.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint8_t>(0), m1.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint8_t>(0), m1.wind_group.gust_factor());
        Assert::AreEqual(UINT16_MAX, m1.wind_group.variation_lower);
        Assert::AreEqual(UINT16_MAX, m1.wind_group.variation_upper);

        aw::metar::metar_info m2("KRHV 101451Z 29014G18KT 10SM SKC 27/13 A2990");

        Assert::IsFalse(m2.wind_group.is_variable());
        Assert::AreEqual(static_cast<uint16_t>(290), m2.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(14), m2.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint8_t>(18), m2.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint8_t>(4), m2.wind_group.gust_factor());
        Assert::AreEqual(UINT16_MAX, m2.wind_group.variation_lower);
        Assert::AreEqual(UINT16_MAX, m2.wind_group.variation_upper);

        aw::metar::metar_info m3("KRHV 101850Z VRB14G18KT 10SM SKC 27/13 A2990");

        Assert::IsTrue(m3.wind_group.is_variable());
        Assert::AreEqual(UINT16_MAX, m3.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(14), m3.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint8_t>(18), m3.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint8_t>(4), m3.wind_group.gust_factor());
        Assert::AreEqual(UINT16_MAX, m3.wind_group.variation_lower);
        Assert::AreEqual(UINT16_MAX, m3.wind_group.variation_upper);

        aw::metar::metar_info m4("KRHV 102102Z VRB12G15KT 200V250 10SM SKC 27/13 A2990");

        Assert::IsTrue(m4.wind_group.is_variable());
        Assert::AreEqual(UINT16_MAX, m4.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(12), m4.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint8_t>(15), m4.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint8_t>(3), m4.wind_group.gust_factor());
        Assert::AreEqual(static_cast<uint16_t>(200), m4.wind_group.variation_lower);
        Assert::AreEqual(static_cast<uint16_t>(250), m4.wind_group.variation_upper);

        aw::metar::metar_info m5("KRHV 112150Z 22512G15KT 200V250 10SM SKC 27/13 A2990");

        Assert::IsTrue(m5.wind_group.is_variable());
        Assert::AreEqual(static_cast<uint16_t>(225), m5.wind_group.direction);
        Assert::AreEqual(static_cast<uint8_t>(12), m5.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint8_t>(15), m5.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint8_t>(3), m5.wind_group.gust_factor());
        Assert::AreEqual(static_cast<uint16_t>(200), m5.wind_group.variation_lower);
        Assert::AreEqual(static_cast<uint16_t>(250), m5.wind_group.variation_upper);
    }
};

} // namespace test
} // namespace aw
