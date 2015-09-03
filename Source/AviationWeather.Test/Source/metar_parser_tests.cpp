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

#include "../Source/parsers.h"

#include "framework.h"

//-----------------------------------------------------------------------------

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//-----------------------------------------------------------------------------

namespace aw
{
namespace test
{

//-----------------------------------------------------------------------------

TEST_CLASS(MetarParserTests)
{
public:
    TEST_METHOD(METAR_Parser_ReportType);
    TEST_METHOD(METAR_Parser_StationIdentifier);
    TEST_METHOD(METAR_Parser_ObservationTime);
    TEST_METHOD(METAR_Parser_Modifier);
    TEST_METHOD(METAR_Parser_Wind);
    TEST_METHOD(METAR_Parser_Visibility);
    TEST_METHOD(METAR_Parser_RunwayVisualRange);
    TEST_METHOD(METAR_Parser_Weather);
    TEST_METHOD(METAR_Parser_SkyCondition);
    TEST_METHOD(METAR_Parser_TemperatureDewpoint);
    TEST_METHOD(METAR_Parser_Altimeter);
    TEST_METHOD(METAR_Parser_Remarks);
};

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_ReportType()
{
    metar::metar_info metar("");
    Assert::AreEqual(metar::report_type::none, metar.type);

    // METAR
    std::string m1("METAR ");
    metar::parsers::parse_report_type(metar, m1);
    Assert::AreEqual(metar::report_type::metar, metar.type);

    // SPECI
    std::string m2("SPECI ");
    metar::parsers::parse_report_type(metar, m2);
    Assert::AreEqual(metar::report_type::special, metar.type);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_StationIdentifier()
{
    metar::metar_info metar("");
    Assert::AreEqual(std::string(""), metar.station_identifier);

    // KSFO
    std::string m1("KSFO ");
    metar::parsers::parse_station_identifier(metar, m1);
    Assert::AreEqual(std::string("KSFO"), metar.station_identifier);

    // EBGE
    std::string m2("EGBE ");
    metar::parsers::parse_station_identifier(metar, m2);
    Assert::AreEqual(std::string("EGBE"), metar.station_identifier);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_ObservationTime()
{
    metar::metar_info metar("");
    Assert::AreEqual(uint8_t(1), metar.report_time.day_of_month);
    Assert::AreEqual(uint8_t(0), metar.report_time.hour_of_day);
    Assert::AreEqual(uint8_t(0), metar.report_time.minute_of_hour);

    // 8th, 17:53
    std::string m1("081753Z ");
    metar::parsers::parse_observation_time(metar, m1);
    Assert::AreEqual(uint8_t(8), metar.report_time.day_of_month);
    Assert::AreEqual(uint8_t(17), metar.report_time.hour_of_day);
    Assert::AreEqual(uint8_t(53), metar.report_time.minute_of_hour);

    // 20th, 03:28
    std::string m2("200328Z ");
    metar::parsers::parse_observation_time(metar, m2);
    Assert::AreEqual(uint8_t(20), metar.report_time.day_of_month);
    Assert::AreEqual(uint8_t(3), metar.report_time.hour_of_day);
    Assert::AreEqual(uint8_t(28), metar.report_time.minute_of_hour);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Modifier()
{
    metar::metar_info metar("");
    Assert::AreEqual(metar::modifier_type::automatic, metar.modifier);

    // AUTO
    std::string m1("AUTO ");
    metar::parsers::parse_modifier(metar, m1);
    Assert::AreEqual(metar::modifier_type::automatic, metar.modifier);

    // COR
    std::string m2("COR ");
    metar::parsers::parse_modifier(metar, m2);
    Assert::AreEqual(metar::modifier_type::corrected, metar.modifier);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Wind()
{
    metar::metar_info metar("");
    Assert::AreEqual(UINT16_MAX, metar.wind_group.direction);
    Assert::AreEqual(uint8_t(0), metar.wind_group.wind_speed);
    Assert::AreEqual(uint8_t(0), metar.wind_group.gust_speed);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);
    Assert::AreEqual(speed_unit::kt, metar.wind_group.unit);

    // 240 @15kts
    std::string m1("24015KT ");
    metar::parsers::parse_wind(metar, m1);
    Assert::AreEqual(uint16_t(240), metar.wind_group.direction);
    Assert::AreEqual(uint8_t(15), metar.wind_group.wind_speed);
    Assert::AreEqual(uint8_t(0), metar.wind_group.gust_speed);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);
    Assert::AreEqual(speed_unit::kt, metar.wind_group.unit);

    // Variable @ 2kts
    std::string m2("VRB02KT ");
    metar::parsers::parse_wind(metar, m2);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.direction);
    Assert::AreEqual(uint8_t(2), metar.wind_group.wind_speed);
    Assert::AreEqual(uint8_t(0), metar.wind_group.gust_speed);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);
    Assert::AreEqual(speed_unit::kt, metar.wind_group.unit);

    // 120 @ 10kts, variable between 100 and 140
    std::string m3("12010KT 100V140 ");
    metar::parsers::parse_wind(metar, m3);
    Assert::AreEqual(uint16_t(120), metar.wind_group.direction);
    Assert::AreEqual(uint8_t(10), metar.wind_group.wind_speed);
    Assert::AreEqual(uint8_t(0), metar.wind_group.gust_speed);
    Assert::AreEqual(uint16_t(100), metar.wind_group.variation_lower);
    Assert::AreEqual(uint16_t(140), metar.wind_group.variation_upper);
    Assert::AreEqual(speed_unit::kt, metar.wind_group.unit);

    // 120 @ 8kts, gusting 12kts
    std::string m4("12008G12KT ");
    metar::parsers::parse_wind(metar, m4);
    Assert::AreEqual(uint16_t(120), metar.wind_group.direction);
    Assert::AreEqual(uint8_t(8), metar.wind_group.wind_speed);
    Assert::AreEqual(uint8_t(12), metar.wind_group.gust_speed);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);
    Assert::AreEqual(speed_unit::kt, metar.wind_group.unit);

    // 340 @ 112kts
    std::string m5("340112KT ");
    metar::parsers::parse_wind(metar, m5);
    Assert::AreEqual(uint16_t(340), metar.wind_group.direction);
    Assert::AreEqual(uint8_t(112), metar.wind_group.wind_speed);
    Assert::AreEqual(uint8_t(0), metar.wind_group.gust_speed);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);
    Assert::AreEqual(speed_unit::kt, metar.wind_group.unit);

    // 000 @ 0kts
    std::string m6("00000KT ");
    metar::parsers::parse_wind(metar, m6);
    Assert::AreEqual(uint16_t(0), metar.wind_group.direction);
    Assert::AreEqual(uint8_t(0), metar.wind_group.wind_speed);
    Assert::AreEqual(uint8_t(0), metar.wind_group.gust_speed);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_lower);
    Assert::AreEqual(UINT16_MAX, metar.wind_group.variation_upper);
    Assert::AreEqual(speed_unit::kt, metar.wind_group.unit);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Visibility()
{
    metar::metar_info metar("");
    Assert::AreEqual(distance_unit::feet, metar.visibility_group.unit);
    Assert::AreEqual(static_cast<double>(UINT32_MAX), metar.visibility_group.distance);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.visibility_group.modifier);

    // M1/4SM
    std::string m1("M1/4SM ");
    metar::parsers::parse_visibility(metar, m1);
    Assert::AreEqual(distance_unit::statute_miles, metar.visibility_group.unit);
    Assert::AreEqual(1.0 / 4.0, metar.visibility_group.distance, 0.00001);
    Assert::AreEqual(metar::visibility_modifier_type::less_than, metar.visibility_group.modifier);

    // 1/2SM
    std::string m2("1/2SM ");
    metar::parsers::parse_visibility(metar, m2);
    Assert::AreEqual(distance_unit::statute_miles, metar.visibility_group.unit);
    Assert::AreEqual(1.0 / 2.0, metar.visibility_group.distance, 0.00001);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.visibility_group.modifier);

    // 1SM
    std::string m3("1SM ");
    metar::parsers::parse_visibility(metar, m3);
    Assert::AreEqual(distance_unit::statute_miles, metar.visibility_group.unit);
    Assert::AreEqual(1.0, metar.visibility_group.distance, 0.00001);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.visibility_group.modifier);

    // 1 3/4SM
    std::string m4("1 3/4SM ");
    metar::parsers::parse_visibility(metar, m4);
    Assert::AreEqual(distance_unit::statute_miles, metar.visibility_group.unit);
    Assert::AreEqual(1.0 + (3.0 / 4.0), metar.visibility_group.distance, 0.00001);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.visibility_group.modifier);

    // 5/16SM
    std::string m5("5/16SM ");
    metar::parsers::parse_visibility(metar, m5);
    Assert::AreEqual(distance_unit::statute_miles, metar.visibility_group.unit);
    Assert::AreEqual(5.0 / 16.0, metar.visibility_group.distance, 0.00001);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.visibility_group.modifier);

    // 3SM
    std::string m6("3SM ");
    metar::parsers::parse_visibility(metar, m6);
    Assert::AreEqual(distance_unit::statute_miles, metar.visibility_group.unit);
    Assert::AreEqual(3.0, metar.visibility_group.distance, 0.00001);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.visibility_group.modifier);

    // 15SM
    std::string m7("15SM ");
    metar::parsers::parse_visibility(metar, m7);
    Assert::AreEqual(distance_unit::statute_miles, metar.visibility_group.unit);
    Assert::AreEqual(15.0, metar.visibility_group.distance, 0.00001);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.visibility_group.modifier);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_RunwayVisualRange()
{
    metar::metar_info metar("");
    Assert::AreEqual(size_t(0), metar.runway_visual_range_group.size());

    std::vector<metar::runway_visual_range>::iterator it;

    // R09/3000FT
    std::string m1("R09/3000FT ");
    metar::parsers::parse_runway_visual_range(metar, m1);
    Assert::AreEqual(size_t(1), metar.runway_visual_range_group.size());

    it = metar.runway_visual_range_group.begin();
    Assert::AreEqual(metar::runway_designator_type::none, it->runway_designator);
    Assert::AreEqual(uint8_t(9), it->runway_number);
    Assert::AreEqual(uint16_t(3000), it->visibility_min);
    Assert::AreEqual(uint16_t(3000), it->visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_min_modifier);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_max_modifier);
    metar.runway_visual_range_group.clear();

    // R27L/2000FT
    std::string m2("R27L/2000FT ");
    metar::parsers::parse_runway_visual_range(metar, m2);
    Assert::AreEqual(size_t(1), metar.runway_visual_range_group.size());

    it = metar.runway_visual_range_group.begin();
    Assert::AreEqual(metar::runway_designator_type::left, it->runway_designator);
    Assert::AreEqual(uint8_t(27), it->runway_number);
    Assert::AreEqual(uint16_t(2000), it->visibility_min);
    Assert::AreEqual(uint16_t(2000), it->visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_min_modifier);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_max_modifier);
    metar.runway_visual_range_group.clear();

    // R06R/2000V3000FT
    std::string m3("R06R/2000V3000FT ");
    metar::parsers::parse_runway_visual_range(metar, m3);
    Assert::AreEqual(size_t(1), metar.runway_visual_range_group.size());

    it = metar.runway_visual_range_group.begin();
    Assert::AreEqual(metar::runway_designator_type::right, it->runway_designator);
    Assert::AreEqual(uint8_t(6), it->runway_number);
    Assert::AreEqual(uint16_t(2000), it->visibility_min);
    Assert::AreEqual(uint16_t(3000), it->visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_min_modifier);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_max_modifier);
    metar.runway_visual_range_group.clear();

    // R06R/2000V3000FT
    std::string m4("R09L/3500V4500FT R09R/3000V4000FT ");
    metar::parsers::parse_runway_visual_range(metar, m4);
    Assert::AreEqual(size_t(2), metar.runway_visual_range_group.size());

    it = metar.runway_visual_range_group.begin();
    Assert::AreEqual(metar::runway_designator_type::left, it->runway_designator);
    Assert::AreEqual(uint8_t(9), it->runway_number);
    Assert::AreEqual(uint16_t(3500), it->visibility_min);
    Assert::AreEqual(uint16_t(4500), it->visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_min_modifier);
    ++it;
    Assert::AreEqual(metar::runway_designator_type::right, it->runway_designator);
    Assert::AreEqual(uint8_t(9), it->runway_number);
    Assert::AreEqual(uint16_t(3000), it->visibility_min);
    Assert::AreEqual(uint16_t(4000), it->visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_min_modifier);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_max_modifier);
    metar.runway_visual_range_group.clear();

    // R01L/M0600FT
    std::string m5("R01L/M0600FT ");
    metar::parsers::parse_runway_visual_range(metar, m5);
    Assert::AreEqual(size_t(1), metar.runway_visual_range_group.size());

    it = metar.runway_visual_range_group.begin();
    Assert::AreEqual(metar::runway_designator_type::left, it->runway_designator);
    Assert::AreEqual(uint8_t(1), it->runway_number);
    Assert::AreEqual(uint16_t(600), it->visibility_min);
    Assert::AreEqual(uint16_t(600), it->visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::less_than, it->visibility_min_modifier);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_max_modifier);
    metar.runway_visual_range_group.clear();

    // R27/P6000FT
    std::string m6("R27/P6000FT ");
    metar::parsers::parse_runway_visual_range(metar, m6);
    Assert::AreEqual(size_t(1), metar.runway_visual_range_group.size());

    it = metar.runway_visual_range_group.begin();
    Assert::AreEqual(metar::runway_designator_type::none, it->runway_designator);
    Assert::AreEqual(uint8_t(27), it->runway_number);
    Assert::AreEqual(uint16_t(6000), it->visibility_min);
    Assert::AreEqual(uint16_t(6000), it->visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::greater_than, it->visibility_min_modifier);
    Assert::AreEqual(metar::visibility_modifier_type::none, it->visibility_max_modifier);
    metar.runway_visual_range_group.clear();

    // R01L/L0600VM1000FT
    std::string m7("R01L/M0600VP1000FT ");
    metar::parsers::parse_runway_visual_range(metar, m7);
    Assert::AreEqual(size_t(1), metar.runway_visual_range_group.size());

    it = metar.runway_visual_range_group.begin();
    Assert::AreEqual(metar::runway_designator_type::left, it->runway_designator);
    Assert::AreEqual(uint8_t(1), it->runway_number);
    Assert::AreEqual(uint16_t(600), it->visibility_min);
    Assert::AreEqual(uint16_t(1000), it->visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::less_than, it->visibility_min_modifier);
    Assert::AreEqual(metar::visibility_modifier_type::greater_than, it->visibility_max_modifier);
    metar.runway_visual_range_group.clear();
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Weather()
{
    metar::metar_info metar("");
    Assert::AreEqual(size_t(0), metar.weather_group.size());

    std::vector<metar::weather>::iterator it;

    // -DZ
    std::string m1("-DZ ");
    metar::parsers::parse_weather(metar, m1);
    Assert::AreEqual(size_t(1), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::light, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::drizzle, it->phenomena.at(0));
    metar.weather_group.clear();

    // -RASN
    std::string m2("-RASN ");
    metar::parsers::parse_weather(metar, m2);
    Assert::AreEqual(size_t(1), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::light, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(2), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::rain, it->phenomena.at(0));
    Assert::AreEqual(weather_phenomena::snow, it->phenomena.at(1));
    metar.weather_group.clear();

    // SN BR
    std::string m3("SN BR ");
    metar::parsers::parse_weather(metar, m3);
    Assert::AreEqual(size_t(2), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::moderate, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::snow, it->phenomena.at(0));
    ++it;
    Assert::AreEqual(weather_intensity::moderate, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::mist, it->phenomena.at(0));
    metar.weather_group.clear();

    // -FZRA FG
    std::string m4("-FZRA FG ");
    metar::parsers::parse_weather(metar, m4);
    Assert::AreEqual(size_t(2), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::light, it->intensity);
    Assert::AreEqual(weather_descriptor::freezing, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::rain, it->phenomena.at(0));
    ++it;
    Assert::AreEqual(weather_intensity::moderate, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::fog, it->phenomena.at(0));
    metar.weather_group.clear();

    // SHRA
    std::string m5("SHRA ");
    metar::parsers::parse_weather(metar, m5);
    Assert::AreEqual(size_t(1), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::moderate, it->intensity);
    Assert::AreEqual(weather_descriptor::showers, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::rain, it->phenomena.at(0));
    metar.weather_group.clear();

    // VCBLSA
    std::string m6("VCBLSA ");
    metar::parsers::parse_weather(metar, m6);
    Assert::AreEqual(size_t(1), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::in_the_vicinity, it->intensity);
    Assert::AreEqual(weather_descriptor::blowing, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::sand, it->phenomena.at(0));
    metar.weather_group.clear();

    // -RASN FG HZ
    std::string m7("-RASN FG HZ ");
    metar::parsers::parse_weather(metar, m7);
    Assert::AreEqual(size_t(3), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::light, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(2), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::rain, it->phenomena.at(0));
    Assert::AreEqual(weather_phenomena::snow, it->phenomena.at(1));
    ++it;
    Assert::AreEqual(weather_intensity::moderate, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::fog, it->phenomena.at(0));
    ++it;
    Assert::AreEqual(weather_intensity::moderate, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::haze, it->phenomena.at(0));
    metar.weather_group.clear();

    // TS
    std::string m8("TS ");
    metar::parsers::parse_weather(metar, m8);
    Assert::AreEqual(size_t(1), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::moderate, it->intensity);
    Assert::AreEqual(weather_descriptor::thunderstorm, it->descriptor);
    Assert::AreEqual(size_t(0), it->phenomena.size());
    metar.weather_group.clear();

    // +TSRA
    std::string m9("+TSRA ");
    metar::parsers::parse_weather(metar, m9);
    Assert::AreEqual(size_t(1), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::heavy, it->intensity);
    Assert::AreEqual(weather_descriptor::thunderstorm, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::rain, it->phenomena.at(0));
    metar.weather_group.clear();

    // +FC TSRAGR BR
    std::string m10("+FC TSRAGR BR ");
    metar::parsers::parse_weather(metar, m10);
    Assert::AreEqual(size_t(3), metar.weather_group.size());

    it = metar.weather_group.begin();
    Assert::AreEqual(weather_intensity::heavy, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::funnel_cloud_tornado_waterspout, it->phenomena.at(0));
    ++it;
    Assert::AreEqual(weather_intensity::moderate, it->intensity);
    Assert::AreEqual(weather_descriptor::thunderstorm, it->descriptor);
    Assert::AreEqual(size_t(2), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::rain, it->phenomena.at(0));
    Assert::AreEqual(weather_phenomena::hail, it->phenomena.at(1));
    ++it;
    Assert::AreEqual(weather_intensity::moderate, it->intensity);
    Assert::AreEqual(weather_descriptor::none, it->descriptor);
    Assert::AreEqual(size_t(1), it->phenomena.size());
    Assert::AreEqual(weather_phenomena::mist, it->phenomena.at(0));
    metar.weather_group.clear();
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_SkyCondition()
{
    metar::metar_info metar("");
    Assert::AreEqual(size_t(0), metar.sky_condition_group.size());

    std::vector<metar::cloud_layer>::iterator it;

    // CLR
    std::string m1("CLR ");
    metar::parsers::parse_sky_condition(metar, m1);
    Assert::AreEqual(size_t(1), metar.sky_condition_group.size());

    it = metar.sky_condition_group.begin();
    Assert::AreEqual(sky_cover_type::clear_below_12000, it->sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::none, it->cloud_type);
    Assert::AreEqual(UINT32_MAX, it->layer_height);
    Assert::AreEqual(distance_unit::feet, it->unit);
    metar.sky_condition_group.clear();

    // SKC
    std::string m2("SKC ");
    metar::parsers::parse_sky_condition(metar, m2);
    Assert::AreEqual(size_t(1), metar.sky_condition_group.size());

    it = metar.sky_condition_group.begin();
    Assert::AreEqual(sky_cover_type::sky_clear, it->sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::none, it->cloud_type);
    Assert::AreEqual(UINT32_MAX, it->layer_height);
    Assert::AreEqual(distance_unit::feet, it->unit);
    metar.sky_condition_group.clear();

    // VV003
    std::string m3("VV003 ");
    metar::parsers::parse_sky_condition(metar, m3);
    Assert::AreEqual(size_t(1), metar.sky_condition_group.size());

    it = metar.sky_condition_group.begin();
    Assert::AreEqual(sky_cover_type::vertical_visibility, it->sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::unspecified, it->cloud_type);
    Assert::AreEqual(uint32_t(300), it->layer_height);
    Assert::AreEqual(distance_unit::feet, it->unit);
    metar.sky_condition_group.clear();

    // BKN060CB
    std::string m4("BKN060CB ");
    metar::parsers::parse_sky_condition(metar, m4);
    Assert::AreEqual(size_t(1), metar.sky_condition_group.size());

    it = metar.sky_condition_group.begin();
    Assert::AreEqual(sky_cover_type::broken, it->sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::cumulonimbus, it->cloud_type);
    Assert::AreEqual(uint32_t(6000), it->layer_height);
    Assert::AreEqual(distance_unit::feet, it->unit);
    metar.sky_condition_group.clear();

    // FEW008 SCT030
    std::string m5("FEW008 SCT030 ");
    metar::parsers::parse_sky_condition(metar, m5);
    Assert::AreEqual(size_t(2), metar.sky_condition_group.size());

    it = metar.sky_condition_group.begin();
    Assert::AreEqual(sky_cover_type::few, it->sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::unspecified, it->cloud_type);
    Assert::AreEqual(uint32_t(800), it->layer_height);
    Assert::AreEqual(distance_unit::feet, it->unit);
    ++it;
    Assert::AreEqual(sky_cover_type::scattered, it->sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::unspecified, it->cloud_type);
    Assert::AreEqual(uint32_t(3000), it->layer_height);
    Assert::AreEqual(distance_unit::feet, it->unit);
    metar.sky_condition_group.clear();
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_TemperatureDewpoint()
{
    metar::metar_info metar("");
    Assert::AreEqual(int8_t(INT8_MAX), metar.temperature);
    Assert::AreEqual(int8_t(INT8_MAX), metar.dewpoint);

    // Temperature: 19C, Dewpoint: 4C
    std::string m1("19/04 ");
    metar::parsers::parse_temperature_dewpoint(metar, m1);
    Assert::AreEqual(int8_t(19), metar.temperature);
    Assert::AreEqual(int8_t(4), metar.dewpoint);

    // Temperature: -3C, Dewpoint: -9C
    std::string m2("M03/M09 ");
    metar::parsers::parse_temperature_dewpoint(metar, m2);
    Assert::AreEqual(int8_t(-3), metar.temperature);
    Assert::AreEqual(int8_t(-9), metar.dewpoint);

    // Temperature: 17C, Dewpoint: missing
    std::string m3("17/ ");
    metar::parsers::parse_temperature_dewpoint(metar, m3);
    Assert::AreEqual(int8_t(17), metar.temperature);
    Assert::AreEqual(int8_t(INT8_MAX), metar.dewpoint);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Altimeter()
{
    metar::metar_info metar("");
    Assert::AreEqual(0.0, metar.altimeter_group.pressure);
    Assert::AreEqual(pressure_unit::hPa, metar.altimeter_group.unit);

    // QNH 1013
    std::string m1("Q1013");
    metar::parsers::parse_altimeter(metar, m1);
    Assert::AreEqual(1013.0, metar.altimeter_group.pressure);
    Assert::AreEqual(pressure_unit::hPa, metar.altimeter_group.unit);

    // Altimeter 29.92
    std::string m2("A2992");
    metar::parsers::parse_altimeter(metar, m2);
    Assert::AreEqual(29.92, metar.altimeter_group.pressure);
    Assert::AreEqual(pressure_unit::inHg, metar.altimeter_group.unit);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Remarks()
{
    metar::metar_info metar("");
    Assert::AreEqual(std::string(""), metar.remarks);

    // RMK AO1
    std::string m1("RMK AO1");
    metar::parsers::parse_remarks(metar, m1);
    Assert::AreEqual(std::string("AO1"), metar.remarks);
}

//-----------------------------------------------------------------------------

} // namespace test
} // namespace aw
