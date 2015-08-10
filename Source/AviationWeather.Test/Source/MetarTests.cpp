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

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::report_type::metar), static_cast<uint8_t>(metar.type));

        Assert::AreEqual(std::string("KSFO"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(17), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(22), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(56), metar.report_time.minute_of_hour);

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::modifier_type::automatic), static_cast<uint8_t>(metar.modifier));

        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.direction));
        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_lower));
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_upper));

        Assert::AreEqual(static_cast<uint8_t>(aw::distance_unit::statute_miles), static_cast<uint8_t>(metar.visibility_group.unit));
        Assert::AreEqual(9.0, metar.visibility_group.distance);

        Assert::AreEqual(0U, static_cast<uint32_t>(metar.weather_group.size()));

        Assert::AreEqual(1U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto it = metar.sky_condition_group.begin();
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::clear), static_cast<uint8_t>(it->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(it->cloud_type));
        Assert::AreEqual(0U, it->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(19), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(4), metar.dewpoint);

        Assert::AreEqual(static_cast<uint8_t>(aw::pressure_unit::inHg), static_cast<uint8_t>(metar.altimeter_group.unit));
        Assert::AreEqual(30.12, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string(""), metar.remarks);
    }

    TEST_METHOD(METAR_CYUL)
    {
        aw::metar::metar_info metar("CYUL 042100Z 20006KT 15SM FEW120 SCT170 BKN240 24/10 A2989 RMK AC1AC2CI4 SLP123 DENSITY ALT 1200FT");

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::report_type::metar), static_cast<uint8_t>(metar.type));

        Assert::AreEqual(std::string("CYUL"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(4), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(21), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(0), metar.report_time.minute_of_hour);

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::modifier_type::automatic), static_cast<uint8_t>(metar.modifier));

        Assert::AreEqual(static_cast<uint8_t>(6), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint32_t>(200), static_cast<uint32_t>(metar.wind_group.direction));
        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_lower));
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_upper));

        Assert::AreEqual(static_cast<uint8_t>(aw::distance_unit::statute_miles), static_cast<uint8_t>(metar.visibility_group.unit));
        Assert::AreEqual(15.0, metar.visibility_group.distance);

        Assert::AreEqual(0U, static_cast<uint32_t>(metar.weather_group.size()));

        Assert::AreEqual(3U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto it = metar.sky_condition_group.begin();
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::few), static_cast<uint8_t>(it->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(it->cloud_type));
        Assert::AreEqual(12000U, it->layer_height);

        ++it;
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::scattered), static_cast<uint8_t>(it->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(it->cloud_type));
        Assert::AreEqual(17000U, it->layer_height);

        ++it;
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::broken), static_cast<uint8_t>(it->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(it->cloud_type));
        Assert::AreEqual(24000U, it->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(24), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(10), metar.dewpoint);

        Assert::AreEqual(static_cast<uint8_t>(aw::pressure_unit::inHg), static_cast<uint8_t>(metar.altimeter_group.unit));
        Assert::AreEqual(29.89, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AC1AC2CI4 SLP123 DENSITY ALT 1200FT"), metar.remarks);
    }

    TEST_METHOD(METAR_KVNY)
    {
        aw::metar::metar_info metar("KVNY 302351Z COR 14014KT 10SM BKN150 28/14 A2996 RMK AO2 PK WND 16029/2305 SLP135 T02830139 10372 20283 53030");

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::report_type::metar), static_cast<uint8_t>(metar.type));

        Assert::AreEqual(std::string("KVNY"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(30), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(23), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(51), metar.report_time.minute_of_hour);

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::modifier_type::corrected), static_cast<uint8_t>(metar.modifier));

        Assert::AreEqual(static_cast<uint8_t>(14), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint32_t>(140), static_cast<uint32_t>(metar.wind_group.direction));
        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_lower));
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_upper));

        Assert::AreEqual(static_cast<uint8_t>(aw::distance_unit::statute_miles), static_cast<uint8_t>(metar.visibility_group.unit));
        Assert::AreEqual(10.0, metar.visibility_group.distance);

        Assert::AreEqual(0U, static_cast<uint32_t>(metar.weather_group.size()));

        Assert::AreEqual(1U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto it = metar.sky_condition_group.begin();
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::broken), static_cast<uint8_t>(it->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(it->cloud_type));
        Assert::AreEqual(15000U, it->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(28), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(14), metar.dewpoint);

        Assert::AreEqual(static_cast<uint8_t>(aw::pressure_unit::inHg), static_cast<uint8_t>(metar.altimeter_group.unit));
        Assert::AreEqual(29.96, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AO2 PK WND 16029/2305 SLP135 T02830139 10372 20283 53030"), metar.remarks);
    }

    TEST_METHOD(METAR_KLAX)
    {
        aw::metar::metar_info metar("KLAX 011153Z COR 00000KT 10SM FEW007 SCT050 SCT160 BKN250 20/18 A2990 RMK AO2 SLP122 FRQ LTGICCGCA DSNT N CB DSNT N-NE T02000178 10200 20189 50014 $");

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::report_type::metar), static_cast<uint8_t>(metar.type));

        Assert::AreEqual(std::string("KLAX"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(1), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(11), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(53), metar.report_time.minute_of_hour);

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::modifier_type::corrected), static_cast<uint8_t>(metar.modifier));

        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.direction));
        Assert::AreEqual(static_cast<uint8_t>(0), metar.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_lower));
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_upper));

        Assert::AreEqual(static_cast<uint8_t>(aw::distance_unit::statute_miles), static_cast<uint8_t>(metar.visibility_group.unit));
        Assert::AreEqual(10.0, metar.visibility_group.distance);

        Assert::AreEqual(0U, static_cast<uint32_t>(metar.weather_group.size()));

        Assert::AreEqual(4U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto it = metar.sky_condition_group.begin();
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::few), static_cast<uint8_t>(it->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(it->cloud_type));
        Assert::AreEqual(700U, it->layer_height);

        ++it;
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::scattered), static_cast<uint8_t>(it->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(it->cloud_type));
        Assert::AreEqual(5000U, it->layer_height);

        ++it;
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::scattered), static_cast<uint8_t>(it->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(it->cloud_type));
        Assert::AreEqual(16000U, it->layer_height);

        ++it;
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::broken), static_cast<uint8_t>(it->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(it->cloud_type));
        Assert::AreEqual(25000U, it->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(20), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(18), metar.dewpoint);

        Assert::AreEqual(static_cast<uint8_t>(aw::pressure_unit::inHg), static_cast<uint8_t>(metar.altimeter_group.unit));
        Assert::AreEqual(29.90, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AO2 SLP122 FRQ LTGICCGCA DSNT N CB DSNT N-NE T02000178 10200 20189 50014 $"), metar.remarks);
    }

    TEST_METHOD(METAR_KRNO_1)
    {
        aw::metar::metar_info metar("KRNO 030155Z 26015G23KT 10SM TS SCT110CB BKN180 28/13 A3006 RMK AO2 PK WND 31027/0109 RAB03E20 PRESRR SLP119 OCNL LTGIC NW TS NW MOV W CB DSNT N S SH VC SW NW P0000 T02830128");

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::report_type::metar), static_cast<uint8_t>(metar.type));

        Assert::AreEqual(std::string("KRNO"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(3), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(1), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(55), metar.report_time.minute_of_hour);

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::modifier_type::automatic), static_cast<uint8_t>(metar.modifier));

        Assert::AreEqual(static_cast<uint8_t>(15), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint32_t>(260), static_cast<uint32_t>(metar.wind_group.direction));
        Assert::AreEqual(static_cast<uint8_t>(23), metar.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_lower));
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_upper));

        Assert::AreEqual(static_cast<uint8_t>(aw::distance_unit::statute_miles), static_cast<uint8_t>(metar.visibility_group.unit));
        Assert::AreEqual(10.0, metar.visibility_group.distance);

        Assert::AreEqual(1U, static_cast<uint32_t>(metar.weather_group.size()));

        auto itWeather = metar.weather_group.begin();
        Assert::AreEqual(static_cast<uint8_t>(aw::weather_intensity::moderate), static_cast<uint8_t>(itWeather->intensity));
        Assert::AreEqual(static_cast<uint8_t>(aw::weather_descriptor::thunderstorm), static_cast<uint8_t>(itWeather->descriptor));
        Assert::AreEqual(0U, static_cast<uint32_t>(itWeather->phenomena.size()));

        Assert::AreEqual(2U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto itSky = metar.sky_condition_group.begin();
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::scattered), static_cast<uint8_t>(itSky->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::cumulonimbus), static_cast<uint8_t>(itSky->cloud_type));
        Assert::AreEqual(11000U, itSky->layer_height);

        ++itSky;
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::broken), static_cast<uint8_t>(itSky->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(itSky->cloud_type));
        Assert::AreEqual(18000U, itSky->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(28), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(13), metar.dewpoint);

        Assert::AreEqual(static_cast<uint8_t>(aw::pressure_unit::inHg), static_cast<uint8_t>(metar.altimeter_group.unit));
        Assert::AreEqual(30.06, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AO2 PK WND 31027/0109 RAB03E20 PRESRR SLP119 OCNL LTGIC NW TS NW MOV W CB DSNT N S SH VC SW NW P0000 T02830128"), metar.remarks);
    }

    TEST_METHOD(METAR_KRNO_2)
    {
        aw::metar::metar_info metar("KRNO 010155Z 09027G32KT 10SM +TSRA BKN090CB BKN160 BKN250 29/13 A3007 RMK AO2 PK WND 09032/0155 WSHFT 0130 RAB32 TSB02 PRESRR SLP120 FRQ LTGCGIC OHD NE-E TS OHD NE-E MOV N P0000 T02890128");

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::report_type::metar), static_cast<uint8_t>(metar.type));

        Assert::AreEqual(std::string("KRNO"), metar.station_identifier);

        Assert::AreEqual(static_cast<uint8_t>(1), metar.report_time.day_of_month);
        Assert::AreEqual(static_cast<uint8_t>(1), metar.report_time.hour_of_day);
        Assert::AreEqual(static_cast<uint8_t>(55), metar.report_time.minute_of_hour);

        Assert::AreEqual(static_cast<uint8_t>(aw::metar::modifier_type::automatic), static_cast<uint8_t>(metar.modifier));

        Assert::AreEqual(static_cast<uint8_t>(27), metar.wind_group.wind_speed);
        Assert::AreEqual(static_cast<uint32_t>(90), static_cast<uint32_t>(metar.wind_group.direction));
        Assert::AreEqual(static_cast<uint8_t>(32), metar.wind_group.gust_speed);
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_lower));
        Assert::AreEqual(static_cast<uint32_t>(0), static_cast<uint32_t>(metar.wind_group.variation_upper));

        Assert::AreEqual(static_cast<uint8_t>(aw::distance_unit::statute_miles), static_cast<uint8_t>(metar.visibility_group.unit));
        Assert::AreEqual(10.0, metar.visibility_group.distance);

        Assert::AreEqual(1U, static_cast<uint32_t>(metar.weather_group.size()));

        auto itWeather = metar.weather_group.begin();
        Assert::AreEqual(static_cast<uint8_t>(aw::weather_intensity::heavy), static_cast<uint8_t>(itWeather->intensity));
        Assert::AreEqual(static_cast<uint8_t>(aw::weather_descriptor::thunderstorm), static_cast<uint8_t>(itWeather->descriptor));

        Assert::AreEqual(1U, static_cast<uint32_t>(itWeather->phenomena.size()));
        Assert::AreEqual(static_cast<uint8_t>(aw::weather_phenomena::rain), static_cast<uint8_t>(itWeather->phenomena.front()));

        Assert::AreEqual(3U, static_cast<uint32_t>(metar.sky_condition_group.size()));

        auto itSky = metar.sky_condition_group.begin();
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::broken), static_cast<uint8_t>(itSky->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::cumulonimbus), static_cast<uint8_t>(itSky->cloud_type));
        Assert::AreEqual(9000U, itSky->layer_height);

        ++itSky;
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::broken), static_cast<uint8_t>(itSky->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(itSky->cloud_type));
        Assert::AreEqual(16000U, itSky->layer_height);

        ++itSky;
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_type::broken), static_cast<uint8_t>(itSky->sky_cover));
        Assert::AreEqual(static_cast<uint8_t>(aw::sky_cover_cloud_type::none), static_cast<uint8_t>(itSky->cloud_type));
        Assert::AreEqual(25000U, itSky->layer_height);

        Assert::AreEqual(static_cast<uint8_t>(29), metar.temperature);
        Assert::AreEqual(static_cast<uint8_t>(13), metar.dewpoint);

        Assert::AreEqual(static_cast<uint8_t>(aw::pressure_unit::inHg), static_cast<uint8_t>(metar.altimeter_group.unit));
        Assert::AreEqual(30.07, metar.altimeter_group.pressure, 0.01);

        Assert::AreEqual(std::string("AO2 PK WND 09032/0155 WSHFT 0130 RAB32 TSB02 PRESRR SLP120 FRQ LTGCGIC OHD NE-E TS OHD NE-E MOV N P0000 T02890128"), metar.remarks);
    }
};

} // namespace test
} // namespace aw
