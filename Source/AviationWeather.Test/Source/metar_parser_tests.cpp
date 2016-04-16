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

#include <AviationWeather/optional.h>

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
    // Empty
    parse_metar_report_type("", [&](metar_report_type type)
    {
        Assert::Fail();
    });

    // METAR
    bool parsed = false;
    parse_metar_report_type("METAR ", [&](metar_report_type type)
    {
        parsed = true;
        Assert::AreEqual(metar_report_type::metar, type);
    });
    Assert::IsTrue(parsed);

    // SPECI
    parsed = false;
    parse_metar_report_type(std::string("SPECI "), [&](metar_report_type type)
    {
        parsed = true;
        Assert::AreEqual(metar_report_type::special, type);
    });
    Assert::IsTrue(parsed);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_StationIdentifier()
{
    // Empty
    parse_station_identifier(std::string(""), [&](std::string const& identifier)
    {
        Assert::Fail();
    });

    // KSFO
    bool parsed = false;
    parse_station_identifier(std::string("KSFO "), [&](std::string const& identifier)
    {
        parsed = true;
        Assert::AreEqual(std::string("KSFO"), identifier);
    });
    Assert::IsTrue(parsed);

    // EBGE
    parsed = false;
    parse_station_identifier(std::string("EGBE "), [&](std::string const& identifier)
    {
        parsed = true;
        Assert::AreEqual(std::string("EGBE"), identifier);
    });
    Assert::IsTrue(parsed);
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_ObservationTime()
{
    // Empty
    parse_time(std::string(""), [&](aw::time && time)
    {
        Assert::Fail();
    });

    // 8th, 17:53
    parse_time(std::string("081753Z "), [&](aw::time && time)
    {
        Assert::AreEqual(uint8_t(8), time.day_of_month);
        Assert::AreEqual(uint8_t(17), time.hour_of_day);
        Assert::AreEqual(uint8_t(53), time.minute_of_hour);
    });

    // 20th, 03:28
    parse_time(std::string("200328Z "), [&](aw::time && time)
    {
        Assert::AreEqual(uint8_t(20), time.day_of_month);
        Assert::AreEqual(uint8_t(3), time.hour_of_day);
        Assert::AreEqual(uint8_t(28), time.minute_of_hour);
    });
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Modifier()
{
    // Empty
    parse_metar_modifier(std::string(""), [&](metar_modifier_type type)
    {
        Assert::Fail();
    });

    // AUTO
    parse_metar_modifier(std::string("AUTO "), [&](metar_modifier_type type)
    {
        Assert::AreEqual(metar_modifier_type::automatic, type);
    });

    // COR
    parse_metar_modifier(std::string("COR "), [&](metar_modifier_type type)
    {
        Assert::AreEqual(metar_modifier_type::corrected, type);
    });
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Wind()
{
    // Empty
    parse_wind(std::string(""), [&](wind && windGroup)
    {
        Assert::Fail();
    });

    // 240 @15kts
    parse_wind(std::string("24015KT "), [&](wind && windGroup)
    {
        Assert::AreEqual(uint16_t(240), windGroup.direction);
        Assert::AreEqual(uint8_t(15), windGroup.wind_speed);
        Assert::AreEqual(uint8_t(0), windGroup.gust_speed);
        Assert::IsFalse(static_cast<bool>(windGroup.variation_lower));
        Assert::IsFalse(static_cast<bool>(windGroup.variation_upper));
        Assert::AreEqual(speed_unit::kt, windGroup.unit);
    });


    // Variable @ 2kts
    parse_wind(std::string("VRB02KT "), [&](wind && windGroup)
    {
        Assert::AreEqual(UINT16_MAX, windGroup.direction);
        Assert::AreEqual(uint8_t(2), windGroup.wind_speed);
        Assert::AreEqual(uint8_t(0), windGroup.gust_speed);
        Assert::IsFalse(static_cast<bool>(windGroup.variation_lower));
        Assert::IsFalse(static_cast<bool>(windGroup.variation_upper));
        Assert::AreEqual(speed_unit::kt, windGroup.unit);
    });

    // 120 @ 10kts, variable between 100 and 140
    parse_wind(std::string("12010KT 100V140 "), [&](wind && windGroup)
    {
        Assert::AreEqual(uint16_t(120), windGroup.direction);
        Assert::AreEqual(uint8_t(10), windGroup.wind_speed);
        Assert::AreEqual(uint8_t(0), windGroup.gust_speed);
        Assert::AreEqual(uint16_t(100), *(windGroup.variation_lower));
        Assert::AreEqual(uint16_t(140), *(windGroup.variation_upper));
        Assert::AreEqual(speed_unit::kt, windGroup.unit);
    });

    // 120 @ 8kts, gusting 12kts
    parse_wind(std::string("12008G12KT "), [&](wind && windGroup)
    {
        Assert::AreEqual(uint16_t(120), windGroup.direction);
        Assert::AreEqual(uint8_t(8), windGroup.wind_speed);
        Assert::AreEqual(uint8_t(12), windGroup.gust_speed);
        Assert::IsFalse(static_cast<bool>(windGroup.variation_lower));
        Assert::IsFalse(static_cast<bool>(windGroup.variation_upper));
        Assert::AreEqual(speed_unit::kt, windGroup.unit);
    });

    // 340 @ 112kts
    parse_wind(std::string("340112KT "), [&](wind && windGroup)
    {
        Assert::AreEqual(uint16_t(340), windGroup.direction);
        Assert::AreEqual(uint8_t(112), windGroup.wind_speed);
        Assert::AreEqual(uint8_t(0), windGroup.gust_speed);
        Assert::IsFalse(static_cast<bool>(windGroup.variation_lower));
        Assert::IsFalse(static_cast<bool>(windGroup.variation_upper));
        Assert::AreEqual(speed_unit::kt, windGroup.unit);
    });

    // 000 @ 0kts
    parse_wind(std::string("00000KT "), [&](wind && windGroup)
    {
        Assert::AreEqual(uint16_t(0), windGroup.direction);
        Assert::AreEqual(uint8_t(0), windGroup.wind_speed);
        Assert::AreEqual(uint8_t(0), windGroup.gust_speed);
        Assert::IsFalse(static_cast<bool>(windGroup.variation_lower));
        Assert::IsFalse(static_cast<bool>(windGroup.variation_upper));
        Assert::AreEqual(speed_unit::kt, windGroup.unit);
    });
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Visibility()
{
    // Empty
    parse_visibility(std::string(""), [&](visibility && visibilityGroup)
    {
        Assert::Fail();
    });

    // M1/4SM
    parse_visibility(std::string("M1/4SM "), [&](visibility && visibilityGroup)
    {
        Assert::AreEqual(distance_unit::statute_miles, visibilityGroup.unit);
        Assert::AreEqual(1.0 / 4.0, visibilityGroup.distance, 0.00001);
        Assert::AreEqual(visibility_modifier_type::less_than, visibilityGroup.modifier);
    });

    // 1/2SM
    parse_visibility(std::string("1/2SM "), [&](visibility && visibilityGroup)
    {
        Assert::AreEqual(distance_unit::statute_miles, visibilityGroup.unit);
        Assert::AreEqual(1.0 / 2.0, visibilityGroup.distance, 0.00001);
        Assert::AreEqual(visibility_modifier_type::none, visibilityGroup.modifier);
    });

    // 1SM
    parse_visibility(std::string("1SM "), [&](visibility && visibilityGroup)
    {
        Assert::AreEqual(distance_unit::statute_miles, visibilityGroup.unit);
        Assert::AreEqual(1.0, visibilityGroup.distance, 0.00001);
        Assert::AreEqual(visibility_modifier_type::none, visibilityGroup.modifier);
    });

    // 1 3/4SM
    parse_visibility(std::string("1 3/4SM "), [&](visibility && visibilityGroup)
    {
        Assert::AreEqual(distance_unit::statute_miles, visibilityGroup.unit);
        Assert::AreEqual(1.0 + (3.0 / 4.0), visibilityGroup.distance, 0.00001);
        Assert::AreEqual(visibility_modifier_type::none, visibilityGroup.modifier);
    });

    // 5/16SM
    parse_visibility(std::string("5/16SM "), [&](visibility && visibilityGroup)
    {
        Assert::AreEqual(distance_unit::statute_miles, visibilityGroup.unit);
        Assert::AreEqual(5.0 / 16.0, visibilityGroup.distance, 0.00001);
        Assert::AreEqual(visibility_modifier_type::none, visibilityGroup.modifier);
    });

    // 3SM
    parse_visibility(std::string("3SM "), [&](visibility && visibilityGroup)
    {
        Assert::AreEqual(distance_unit::statute_miles, visibilityGroup.unit);
        Assert::AreEqual(3.0, visibilityGroup.distance, 0.00001);
        Assert::AreEqual(visibility_modifier_type::none, visibilityGroup.modifier);
    });

    // 15SM
    parse_visibility(std::string("15SM "), [&](visibility && visibilityGroup)
    {
        Assert::AreEqual(distance_unit::statute_miles, visibilityGroup.unit);
        Assert::AreEqual(15.0, visibilityGroup.distance, 0.00001);
        Assert::AreEqual(visibility_modifier_type::none, visibilityGroup.modifier);
    });
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_RunwayVisualRange()
{
    // Empty
    parse_runway_visual_range(std::string(""), [&](runway_visual_range && rvr)
    {
        Assert::Fail();
    });

    // R09/3000FT
    parse_runway_visual_range(std::string("R09/3000FT "), [&](runway_visual_range && rvr)
    {
        Assert::AreEqual(runway_designator_type::none, rvr.runway_designator);
        Assert::AreEqual(uint8_t(9), rvr.runway_number);
        Assert::AreEqual(double(3000), rvr.visibility_min.distance);
        Assert::AreEqual(double(3000), rvr.visibility_max.distance);
        Assert::AreEqual(visibility_modifier_type::none, rvr.visibility_min.modifier);
        Assert::AreEqual(visibility_modifier_type::none, rvr.visibility_max.modifier);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_min.unit);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_max.unit);
    });

    // R27L/2000FT
    parse_runway_visual_range(std::string("R27L/2000FT "), [&](runway_visual_range && rvr)
    {
        Assert::AreEqual(runway_designator_type::left, rvr.runway_designator);
        Assert::AreEqual(uint8_t(27), rvr.runway_number);
        Assert::AreEqual(double(2000), rvr.visibility_min.distance);
        Assert::AreEqual(double(2000), rvr.visibility_max.distance);
        Assert::AreEqual(visibility_modifier_type::none, rvr.visibility_min.modifier);
        Assert::AreEqual(visibility_modifier_type::none, rvr.visibility_max.modifier);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_min.unit);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_max.unit);
    });

    // R06R/2000V3000FT
    parse_runway_visual_range(std::string("R06R/2000V3000FT "), [&](runway_visual_range && rvr)
    {
        Assert::AreEqual(runway_designator_type::right, rvr.runway_designator);
        Assert::AreEqual(uint8_t(6), rvr.runway_number);
        Assert::AreEqual(double(2000), rvr.visibility_min.distance);
        Assert::AreEqual(double(3000), rvr.visibility_max.distance);
        Assert::AreEqual(visibility_modifier_type::none, rvr.visibility_min.modifier);
        Assert::AreEqual(visibility_modifier_type::none, rvr.visibility_max.modifier);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_min.unit);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_max.unit);
    });

    // R06R/2000V3000FT
    {
        std::vector<runway_visual_range> rvrGroup;
        std::vector<runway_visual_range>::iterator it;

        parse_runway_visual_range(std::string("R09L/3500V4500FT R09R/3000V4000FT "), [&](runway_visual_range && rvr)
        {
            rvrGroup.push_back(rvr);
        });

        Assert::AreEqual(size_t(2), rvrGroup.size());

        it = rvrGroup.begin();
        Assert::AreEqual(runway_designator_type::left, it->runway_designator);
        Assert::AreEqual(uint8_t(9), it->runway_number);
        Assert::AreEqual(double(3500), it->visibility_min.distance);
        Assert::AreEqual(double(4500), it->visibility_max.distance);
        Assert::AreEqual(visibility_modifier_type::none, it->visibility_min.modifier);
        Assert::AreEqual(visibility_modifier_type::none, it->visibility_max.modifier);
        Assert::AreEqual(distance_unit::feet, it->visibility_min.unit);
        Assert::AreEqual(distance_unit::feet, it->visibility_max.unit);
        ++it;
        Assert::AreEqual(runway_designator_type::right, it->runway_designator);
        Assert::AreEqual(uint8_t(9), it->runway_number);
        Assert::AreEqual(double(3000), it->visibility_min.distance);
        Assert::AreEqual(double(4000), it->visibility_max.distance);
        Assert::AreEqual(visibility_modifier_type::none, it->visibility_min.modifier);
        Assert::AreEqual(visibility_modifier_type::none, it->visibility_max.modifier);
        Assert::AreEqual(distance_unit::feet, it->visibility_min.unit);
        Assert::AreEqual(distance_unit::feet, it->visibility_max.unit);
    }

    // R01L/M0600FT
    parse_runway_visual_range(std::string("R01L/M0600FT "), [&](runway_visual_range && rvr)
    {
        Assert::AreEqual(runway_designator_type::left, rvr.runway_designator);
        Assert::AreEqual(uint8_t(1), rvr.runway_number);
        Assert::AreEqual(double(600), rvr.visibility_min.distance);
        Assert::AreEqual(double(600), rvr.visibility_max.distance);
        Assert::AreEqual(visibility_modifier_type::less_than, rvr.visibility_min.modifier);
        Assert::AreEqual(visibility_modifier_type::none, rvr.visibility_max.modifier);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_min.unit);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_max.unit);
    });

    // R27/P6000FT
    parse_runway_visual_range(std::string("R27/P6000FT "), [&](runway_visual_range && rvr)
    {
        Assert::AreEqual(runway_designator_type::none, rvr.runway_designator);
        Assert::AreEqual(uint8_t(27), rvr.runway_number);
        Assert::AreEqual(double(6000), rvr.visibility_min.distance);
        Assert::AreEqual(double(6000), rvr.visibility_max.distance);
        Assert::AreEqual(visibility_modifier_type::greater_than, rvr.visibility_min.modifier);
        Assert::AreEqual(visibility_modifier_type::none, rvr.visibility_max.modifier);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_min.unit);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_max.unit);
    });

    // R01L/M0600VP6000FT
    parse_runway_visual_range(std::string("R01L/M0600VP6000FT "), [&](runway_visual_range && rvr)
    {
        Assert::AreEqual(runway_designator_type::left, rvr.runway_designator);
        Assert::AreEqual(uint8_t(1), rvr.runway_number);
        Assert::AreEqual(double(600), rvr.visibility_min.distance);
        Assert::AreEqual(double(6000), rvr.visibility_max.distance);
        Assert::AreEqual(visibility_modifier_type::less_than, rvr.visibility_min.modifier);
        Assert::AreEqual(visibility_modifier_type::greater_than, rvr.visibility_max.modifier);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_min.unit);
        Assert::AreEqual(distance_unit::feet, rvr.visibility_max.unit);
    });
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Weather()
{
    // Empty
    parse_weather(std::string(""), [&](weather && rvr)
    {
        Assert::Fail();
    });

    // -DZ
    parse_weather(std::string("-DZ "), [&](weather && weatherGroup)
    {
        Assert::AreEqual(weather_intensity::light, weatherGroup.intensity);
        Assert::AreEqual(weather_descriptor::none, weatherGroup.descriptor);
        Assert::AreEqual(size_t(1), weatherGroup.phenomena.size());
        Assert::AreEqual(weather_phenomena::drizzle, weatherGroup.phenomena.at(0));
    });

    // -RASN
    parse_weather(std::string("-RASN "), [&](weather && weatherGroup)
    {
        Assert::AreEqual(weather_intensity::light, weatherGroup.intensity);
        Assert::AreEqual(weather_descriptor::none, weatherGroup.descriptor);
        Assert::AreEqual(size_t(2), weatherGroup.phenomena.size());
        Assert::AreEqual(weather_phenomena::rain, weatherGroup.phenomena.at(0));
        Assert::AreEqual(weather_phenomena::snow, weatherGroup.phenomena.at(1));
    });

    // SN BR
    {
        std::vector<weather> weatherGroups;
        std::vector<weather>::iterator it;

        parse_weather(std::string("SN BR "), [&](weather && weatherGroup)
        {
            weatherGroups.push_back(weatherGroup);
        });

        Assert::AreEqual(size_t(2), weatherGroups.size());

        it = weatherGroups.begin();
        Assert::AreEqual(weather_intensity::moderate, it->intensity);
        Assert::AreEqual(weather_descriptor::none, it->descriptor);
        Assert::AreEqual(size_t(1), it->phenomena.size());
        Assert::AreEqual(weather_phenomena::snow, it->phenomena.at(0));
        ++it;
        Assert::AreEqual(weather_intensity::moderate, it->intensity);
        Assert::AreEqual(weather_descriptor::none, it->descriptor);
        Assert::AreEqual(size_t(1), it->phenomena.size());
        Assert::AreEqual(weather_phenomena::mist, it->phenomena.at(0));
    }

    // -FZRA FG
    {
        std::vector<weather> weatherGroups;
        std::vector<weather>::iterator it;

        parse_weather(std::string("-FZRA FG "), [&](weather && weatherGroup)
        {
            weatherGroups.push_back(weatherGroup);
        });

        Assert::AreEqual(size_t(2), weatherGroups.size());

        it = weatherGroups.begin();
        Assert::AreEqual(weather_intensity::light, it->intensity);
        Assert::AreEqual(weather_descriptor::freezing, it->descriptor);
        Assert::AreEqual(size_t(1), it->phenomena.size());
        Assert::AreEqual(weather_phenomena::rain, it->phenomena.at(0));
        ++it;
        Assert::AreEqual(weather_intensity::moderate, it->intensity);
        Assert::AreEqual(weather_descriptor::none, it->descriptor);
        Assert::AreEqual(size_t(1), it->phenomena.size());
        Assert::AreEqual(weather_phenomena::fog, it->phenomena.at(0));
    }

    // SHRA
    parse_weather(std::string("SHRA "), [&](weather && weatherGroup)
    {
        Assert::AreEqual(weather_intensity::moderate, weatherGroup.intensity);
        Assert::AreEqual(weather_descriptor::showers, weatherGroup.descriptor);
        Assert::AreEqual(size_t(1), weatherGroup.phenomena.size());
        Assert::AreEqual(weather_phenomena::rain, weatherGroup.phenomena.at(0));
    });

    // VCBLSA
    parse_weather(std::string("VCBLSA "), [&](weather && weatherGroup)
    {
        Assert::AreEqual(weather_intensity::in_the_vicinity, weatherGroup.intensity);
        Assert::AreEqual(weather_descriptor::blowing, weatherGroup.descriptor);
        Assert::AreEqual(size_t(1), weatherGroup.phenomena.size());
        Assert::AreEqual(weather_phenomena::sand, weatherGroup.phenomena.at(0));
    });

    // -RASN FG HZ
    {
        std::vector<weather> weatherGroups;
        std::vector<weather>::iterator it;

        parse_weather(std::string("-RASN FG HZ "), [&](weather && weatherGroup)
        {
            weatherGroups.push_back(weatherGroup);
        });

        Assert::AreEqual(size_t(3), weatherGroups.size());

        it = weatherGroups.begin();
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
    }

    // TS
    parse_weather(std::string("TS "), [&](weather && weatherGroup)
    {
        Assert::AreEqual(weather_intensity::moderate, weatherGroup.intensity);
        Assert::AreEqual(weather_descriptor::thunderstorm, weatherGroup.descriptor);
        Assert::AreEqual(size_t(0), weatherGroup.phenomena.size());
    });

    // +TSRA
    parse_weather(std::string("+TSRA "), [&](weather && weatherGroup)
    {
        Assert::AreEqual(weather_intensity::heavy, weatherGroup.intensity);
        Assert::AreEqual(weather_descriptor::thunderstorm, weatherGroup.descriptor);
        Assert::AreEqual(size_t(1), weatherGroup.phenomena.size());
        Assert::AreEqual(weather_phenomena::rain, weatherGroup.phenomena.at(0));
    });

    // +FC TSRAGR BR
    {
        std::vector<weather> weatherGroups;
        std::vector<weather>::iterator it;

        parse_weather(std::string("+FC TSRAGR BR "), [&](weather && weatherGroup)
        {
            weatherGroups.push_back(weatherGroup);
        });

        Assert::AreEqual(size_t(3), weatherGroups.size());

        it = weatherGroups.begin();
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
    }
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_SkyCondition()
{
    // Empty
    parse_sky_condition(std::string(""), [&](cloud_layer && cloudLayer)
    {
        Assert::Fail();
    });

    // CLR
    parse_sky_condition(std::string("CLR "), [&](cloud_layer && cloudLayer)
    {
        Assert::AreEqual(sky_cover_type::clear_below_12000, cloudLayer.sky_cover);
        Assert::AreEqual(sky_cover_cloud_type::none, cloudLayer.cloud_type);
        Assert::AreEqual(UINT32_MAX, cloudLayer.layer_height);
        Assert::AreEqual(distance_unit::feet, cloudLayer.unit);
    });

    // SKC
    parse_sky_condition(std::string("SKC "), [&](cloud_layer && cloudLayer)
    {
        Assert::AreEqual(sky_cover_type::sky_clear, cloudLayer.sky_cover);
        Assert::AreEqual(sky_cover_cloud_type::none, cloudLayer.cloud_type);
        Assert::AreEqual(UINT32_MAX, cloudLayer.layer_height);
        Assert::AreEqual(distance_unit::feet, cloudLayer.unit);
    });

    // VV003
    parse_sky_condition(std::string("VV003 "), [&](cloud_layer && cloudLayer)
    {
        Assert::AreEqual(sky_cover_type::vertical_visibility, cloudLayer.sky_cover);
        Assert::AreEqual(sky_cover_cloud_type::unspecified, cloudLayer.cloud_type);
        Assert::AreEqual(uint32_t(300), cloudLayer.layer_height);
        Assert::AreEqual(distance_unit::feet, cloudLayer.unit);
    });

    // BKN060CB
    parse_sky_condition(std::string("BKN060CB "), [&](cloud_layer && cloudLayer)
    {
        Assert::AreEqual(sky_cover_type::broken, cloudLayer.sky_cover);
        Assert::AreEqual(sky_cover_cloud_type::cumulonimbus, cloudLayer.cloud_type);
        Assert::AreEqual(uint32_t(6000), cloudLayer.layer_height);
        Assert::AreEqual(distance_unit::feet, cloudLayer.unit);
    });

    // FEW008 SCT030
    {
        std::vector<cloud_layer> skyConditionGroup;
        std::vector<cloud_layer>::iterator it;

        parse_sky_condition(std::string("FEW008 SCT030 "), [&](cloud_layer && cloudLayer)
        {
            skyConditionGroup.push_back(cloudLayer);
        });

        Assert::AreEqual(size_t(2), skyConditionGroup.size());

        it = skyConditionGroup.begin();
        Assert::AreEqual(sky_cover_type::few, it->sky_cover);
        Assert::AreEqual(sky_cover_cloud_type::unspecified, it->cloud_type);
        Assert::AreEqual(uint32_t(800), it->layer_height);
        Assert::AreEqual(distance_unit::feet, it->unit);
        ++it;
        Assert::AreEqual(sky_cover_type::scattered, it->sky_cover);
        Assert::AreEqual(sky_cover_cloud_type::unspecified, it->cloud_type);
        Assert::AreEqual(uint32_t(3000), it->layer_height);
        Assert::AreEqual(distance_unit::feet, it->unit);
    }
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_TemperatureDewpoint()
{
    // Empty
    parse_temperature_dewpoint(std::string(""), [&](util::optional<int8_t> temperature, util::optional<int8_t> dewpoint)
    {
        Assert::Fail();
    });

    // Temperature: 19C, Dewpoint: 4C
    parse_temperature_dewpoint(std::string("19/04 "), [&](util::optional<int8_t> temperature, util::optional<int8_t> dewpoint)
    {
        Assert::AreEqual(int8_t(19), *(temperature));
        Assert::AreEqual(int8_t(4), *(dewpoint));
    });

    // Temperature: -3C, Dewpoint: -9C
    parse_temperature_dewpoint(std::string("M03/M09 "), [&](util::optional<int8_t> temperature, util::optional<int8_t> dewpoint)
    {
        Assert::AreEqual(int8_t(-3), *(temperature));
        Assert::AreEqual(int8_t(-9), *(dewpoint));
    });
    
    // Temperature: 17C, Dewpoint: missing
    parse_temperature_dewpoint(std::string("17/ "), [&](util::optional<int8_t> temperature, util::optional<int8_t> dewpoint)
    {
        Assert::AreEqual(int8_t(17), *(temperature));
        Assert::IsFalse(static_cast<bool>(dewpoint));
    });
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Altimeter()
{
    // Empty
    parse_altimeter(std::string(""), [&](altimeter && altimeterGroup)
    {
        Assert::Fail();
    });

    // QNH 1013
    parse_altimeter(std::string("Q1013"), [&](altimeter && altimeterGroup)
    {
        Assert::AreEqual(1013.0, altimeterGroup.pressure);
        Assert::AreEqual(pressure_unit::hPa, altimeterGroup.unit);
    });

    // Altimeter 29.92
    parse_altimeter(std::string("A2992"), [&](altimeter && altimeterGroup)
    {
        Assert::AreEqual(29.92, altimeterGroup.pressure);
        Assert::AreEqual(pressure_unit::inHg, altimeterGroup.unit);
    });
}

//-----------------------------------------------------------------------------

void MetarParserTests::METAR_Parser_Remarks()
{
    // Empty
    parse_remarks(std::string(""), [&](std::string const& remarks)
    {
        Assert::Fail();
    });

    // RMK AO1
    parse_remarks(std::string("RMK AO1"), [&](std::string const& remarks)
    {
        Assert::AreEqual(std::string("AO1"), remarks);
    });
}

//-----------------------------------------------------------------------------

} // namespace test
} // namespace aw
