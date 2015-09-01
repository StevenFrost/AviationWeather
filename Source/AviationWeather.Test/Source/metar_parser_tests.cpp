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
    Assert::AreEqual(metar::report_type::metar, metar.type);

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

    // 120 @ 2kts, variable between 100 and 140
    std::string m2("12002KT 100V140 ");
    metar::parsers::parse_wind(metar, m2);
    Assert::AreEqual(uint16_t(120), metar.wind_group.direction);
    Assert::AreEqual(uint8_t(2), metar.wind_group.wind_speed);
    Assert::AreEqual(uint8_t(0), metar.wind_group.gust_speed);
    Assert::AreEqual(uint16_t(100), metar.wind_group.variation_lower);
    Assert::AreEqual(uint16_t(140), metar.wind_group.variation_upper);
    Assert::AreEqual(speed_unit::kt, metar.wind_group.unit);

    // 120 @ 8kts, gusting 12kts
    std::string m3("12008G12KT ");
    metar::parsers::parse_wind(metar, m3);
    Assert::AreEqual(uint16_t(120), metar.wind_group.direction);
    Assert::AreEqual(uint8_t(8), metar.wind_group.wind_speed);
    Assert::AreEqual(uint8_t(12), metar.wind_group.gust_speed);
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
    // TODO: There are issues with variable visbility in RVR groups, tracked
    //       with bug #33. Once the bug has been fixed additional tests should
    //       be written here.
    metar::metar_info metar("");
    Assert::AreEqual(size_t(0), metar.runway_visual_range_group.size());

    // R09/3000FT
    std::string m1("R09/3000FT ");
    metar::parsers::parse_runway_visual_range(metar, m1);
    Assert::AreEqual(metar::runway_designator_type::none, metar.runway_visual_range_group.at(0).runway_designator);
    Assert::AreEqual(uint8_t(9), metar.runway_visual_range_group.at(0).runway_number);
    Assert::AreEqual(uint16_t(3000), metar.runway_visual_range_group.at(0).visibility_min);
    Assert::AreEqual(uint16_t(3000), metar.runway_visual_range_group.at(0).visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.runway_visual_range_group.at(0).visibility_modifier);
    metar.runway_visual_range_group.clear();

    // R27L/2000FT
    std::string m2("R27L/2000FT ");
    metar::parsers::parse_runway_visual_range(metar, m2);
    Assert::AreEqual(metar::runway_designator_type::left, metar.runway_visual_range_group.at(0).runway_designator);
    Assert::AreEqual(uint8_t(27), metar.runway_visual_range_group.at(0).runway_number);
    Assert::AreEqual(uint16_t(2000), metar.runway_visual_range_group.at(0).visibility_min);
    Assert::AreEqual(uint16_t(2000), metar.runway_visual_range_group.at(0).visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.runway_visual_range_group.at(0).visibility_modifier);
    metar.runway_visual_range_group.clear();

    // R06R/2000V3000FT
    std::string m3("R06R/2000V3000FT ");
    metar::parsers::parse_runway_visual_range(metar, m3);
    Assert::AreEqual(metar::runway_designator_type::right, metar.runway_visual_range_group.at(0).runway_designator);
    Assert::AreEqual(uint8_t(6), metar.runway_visual_range_group.at(0).runway_number);
    Assert::AreEqual(uint16_t(2000), metar.runway_visual_range_group.at(0).visibility_min);
    Assert::AreEqual(uint16_t(3000), metar.runway_visual_range_group.at(0).visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.runway_visual_range_group.at(0).visibility_modifier);
    metar.runway_visual_range_group.clear();

    // R06R/2000V3000FT
    std::string m4("R09L/3500V4500FT R09R/3000V4000FT ");
    metar::parsers::parse_runway_visual_range(metar, m4);
    Assert::AreEqual(metar::runway_designator_type::left, metar.runway_visual_range_group.at(0).runway_designator);
    Assert::AreEqual(uint8_t(9), metar.runway_visual_range_group.at(0).runway_number);
    Assert::AreEqual(uint16_t(3500), metar.runway_visual_range_group.at(0).visibility_min);
    Assert::AreEqual(uint16_t(4500), metar.runway_visual_range_group.at(0).visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.runway_visual_range_group.at(0).visibility_modifier);
    Assert::AreEqual(metar::runway_designator_type::right, metar.runway_visual_range_group.at(1).runway_designator);
    Assert::AreEqual(uint8_t(9), metar.runway_visual_range_group.at(1).runway_number);
    Assert::AreEqual(uint16_t(3000), metar.runway_visual_range_group.at(1).visibility_min);
    Assert::AreEqual(uint16_t(4000), metar.runway_visual_range_group.at(1).visibility_max);
    Assert::AreEqual(metar::visibility_modifier_type::none, metar.runway_visual_range_group.at(1).visibility_modifier);
    metar.runway_visual_range_group.clear();
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Weather()
{
    metar::metar_info metar("");
    Assert::AreEqual(size_t(0), metar.weather_group.size());

    // RA
    std::string m1("RA ");
    metar::parsers::parse_weather(metar, m1);
    Assert::AreEqual(weather_intensity::moderate, metar.weather_group.at(0).intensity);
    Assert::AreEqual(weather_descriptor::none, metar.weather_group.at(0).descriptor);
    Assert::AreEqual(size_t(1), metar.weather_group.at(0).phenomena.size());
    Assert::AreEqual(weather_phenomena::rain, metar.weather_group.at(0).phenomena.at(0));
    metar.weather_group.clear();

    // -TSRA
    std::string m2("-TSRA ");
    metar::parsers::parse_weather(metar, m2);
    Assert::AreEqual(weather_intensity::light, metar.weather_group.at(0).intensity);
    Assert::AreEqual(weather_descriptor::thunderstorm, metar.weather_group.at(0).descriptor);
    Assert::AreEqual(size_t(1), metar.weather_group.at(0).phenomena.size());
    Assert::AreEqual(weather_phenomena::rain, metar.weather_group.at(0).phenomena.at(0));
    metar.weather_group.clear();

    // +SN
    std::string m3("+SN ");
    metar::parsers::parse_weather(metar, m3);
    Assert::AreEqual(weather_intensity::heavy, metar.weather_group.at(0).intensity);
    Assert::AreEqual(weather_descriptor::none, metar.weather_group.at(0).descriptor);
    Assert::AreEqual(size_t(1), metar.weather_group.at(0).phenomena.size());
    Assert::AreEqual(weather_phenomena::snow, metar.weather_group.at(0).phenomena.at(0));
    metar.weather_group.clear();

    // VCFG
    std::string m4("VCFG ");
    metar::parsers::parse_weather(metar, m4);
    Assert::AreEqual(weather_intensity::in_the_vicinity, metar.weather_group.at(0).intensity);
    Assert::AreEqual(weather_descriptor::none, metar.weather_group.at(0).descriptor);
    Assert::AreEqual(size_t(1), metar.weather_group.at(0).phenomena.size());
    Assert::AreEqual(weather_phenomena::fog, metar.weather_group.at(0).phenomena.at(0));
    metar.weather_group.clear();
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_SkyCondition()
{
    metar::metar_info metar("");
    Assert::AreEqual(size_t(0), metar.sky_condition_group.size());

    // CLR
    std::string m1("CLR ");
    metar::parsers::parse_sky_condition(metar, m1);
    Assert::AreEqual(sky_cover_type::clear_below_12000, metar.sky_condition_group.at(0).sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::none, metar.sky_condition_group.at(0).cloud_type);
    Assert::AreEqual(UINT32_MAX, metar.sky_condition_group.at(0).layer_height);
    Assert::AreEqual(distance_unit::feet, metar.sky_condition_group.at(0).unit);
    metar.sky_condition_group.clear();

    // SKC
    std::string m2("SKC ");
    metar::parsers::parse_sky_condition(metar, m2);
    Assert::AreEqual(sky_cover_type::sky_clear, metar.sky_condition_group.at(0).sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::none, metar.sky_condition_group.at(0).cloud_type);
    Assert::AreEqual(UINT32_MAX, metar.sky_condition_group.at(0).layer_height);
    Assert::AreEqual(distance_unit::feet, metar.sky_condition_group.at(0).unit);
    metar.sky_condition_group.clear();

    // VV030
    std::string m3("VV030 ");
    metar::parsers::parse_sky_condition(metar, m3);
    Assert::AreEqual(sky_cover_type::vertical_visibility, metar.sky_condition_group.at(0).sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::unspecified, metar.sky_condition_group.at(0).cloud_type);
    Assert::AreEqual(uint32_t(3000), metar.sky_condition_group.at(0).layer_height);
    Assert::AreEqual(distance_unit::feet, metar.sky_condition_group.at(0).unit);
    metar.sky_condition_group.clear();

    // BKN060CB
    std::string m4("BKN060CB ");
    metar::parsers::parse_sky_condition(metar, m4);
    Assert::AreEqual(sky_cover_type::broken, metar.sky_condition_group.at(0).sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::cumulonimbus, metar.sky_condition_group.at(0).cloud_type);
    Assert::AreEqual(uint32_t(6000), metar.sky_condition_group.at(0).layer_height);
    Assert::AreEqual(distance_unit::feet, metar.sky_condition_group.at(0).unit);
    metar.sky_condition_group.clear();

    // FEW008 SCT030
    std::string m5("FEW008 SCT030 ");
    metar::parsers::parse_sky_condition(metar, m5);
    Assert::AreEqual(sky_cover_type::few, metar.sky_condition_group.at(0).sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::unspecified, metar.sky_condition_group.at(0).cloud_type);
    Assert::AreEqual(uint32_t(800), metar.sky_condition_group.at(0).layer_height);
    Assert::AreEqual(distance_unit::feet, metar.sky_condition_group.at(0).unit);
    Assert::AreEqual(sky_cover_type::scattered, metar.sky_condition_group.at(1).sky_cover);
    Assert::AreEqual(sky_cover_cloud_type::unspecified, metar.sky_condition_group.at(1).cloud_type);
    Assert::AreEqual(uint32_t(3000), metar.sky_condition_group.at(1).layer_height);
    Assert::AreEqual(distance_unit::feet, metar.sky_condition_group.at(1).unit);
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

    // Altimeter 30.29
    std::string m2("A3029");
    metar::parsers::parse_altimeter(metar, m2);
    Assert::AreEqual(30.29, metar.altimeter_group.pressure);
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
