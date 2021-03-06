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

#pragma once

#include <cstdint>
#include <memory>
#include <regex>
#include <string>

#include <AviationWeather/metar.h>
#include <AviationWeather/optional.h>

#include "decoders.h"

namespace aw
{
namespace
{

//-----------------------------------------------------------------------------

#define REGEX_REPORT_TYPE      "(METAR|SPECI) "
#define REGEX_STATION_IDENT    "([A-Z0-9]{4}) "
#define REGEX_OBSERVATION_TIME "([0-9]{2})([0-9]{2})([0-9]{2})Z "
#define REGEX_REPORT_MODIFIER  "(AUTO|COR) "
#define REGEX_WIND             "([0-9]{3}|VRB)([0-9]{2,3})(G([0-9]{2,3}))?(KT|MPS)( ([0-9]{3})V([0-9]{3}))? "
#define REGEX_VISIBILITY       "(CAVOK|(((M)?([12]?)[ ]?([0-9])/([0-9]{1,2}))|([0-9]{1,5}))(SM)?) "
#define REGEX_RVR              "R([0-9]{2})([LRC])?/([MP]?)([0-9]{4})(V([MP]?)([0-9]{4}))?FT "
#define REGEX_WEATHER          "([+-]|VC)?((MI|PR|BC|DR|BL)?((DZ|RA|SN|SG|IC|PL|GR|GS|UP){1,3}|(BR|FG|FU|VA|DU|SA|HZ|PY|PO|SQ|FC|SS|DS)) |(SH)((RA|SN|PL|GS|GR){0,3}) |(TS)((RA|SN|PL|GS|GR){0,3}) |(FZ)((FG|DZ|RA){1,3}) )"
#define REGEX_SKY_CONDITION    "((SKC|CLR) )|((VV|FEW|SCT|BKN|OVC)([0-9]{3}|///))(CB|TCU)? "
#define REGEX_TEMP_DEW         "(M)?([0-9]{2})/((M)?([0-9]{2}))? "
#define REGEX_ALTIMETER        "(Q|A)([0-9]{4})( |$)"
#define REGEX_REMARKS          "RMK (.*)$"

//-----------------------------------------------------------------------------

struct pattern
{
    metar_element_type type;
    const char*        regex;
};

constexpr pattern g_patterns[] =
{
    { metar_element_type::report_type,          REGEX_REPORT_TYPE },
    { metar_element_type::station_identifier,   REGEX_STATION_IDENT },
    { metar_element_type::observation_time,     REGEX_OBSERVATION_TIME },
    { metar_element_type::report_modifier,      REGEX_REPORT_MODIFIER },
    { metar_element_type::wind,                 REGEX_WIND },
    { metar_element_type::visibility,           REGEX_VISIBILITY },
    { metar_element_type::runway_visual_range,  REGEX_RVR },
    { metar_element_type::weather,              REGEX_WEATHER },
    { metar_element_type::sky_condition,        REGEX_SKY_CONDITION },
    { metar_element_type::temperature_dewpoint, REGEX_TEMP_DEW },
    { metar_element_type::altimeter,            REGEX_ALTIMETER },
    { metar_element_type::remarks,              REGEX_REMARKS }
};

constexpr const char* get_element_regex_impl(metar_element_type type, const pattern* patterns)
{
    return (patterns->type == type) ? patterns->regex : get_element_regex_impl(type, patterns + 1);
}

constexpr const char* get_element_regex(metar_element_type type)
{
    return get_element_regex_impl(type, g_patterns);
}

//-----------------------------------------------------------------------------

template <typename TLambda>
std::string ParseIfMatch(std::string const& metar, metar_element_type element, TLambda l, bool reverse = false)
{
    auto pattern = get_element_regex(element);
    auto metarString = metar;

    std::regex regexp(pattern, std::regex_constants::icase | std::regex_constants::optimize);
    std::cmatch result;
    if (std::regex_search(metar.c_str(), result, regexp))
    {
        l(result);
        metarString = reverse ? result.prefix().str() : result.suffix().str();
    }
    return metarString;
}

//-----------------------------------------------------------------------------

template <typename TLambda>
std::string ParseForEachMatch(std::string const& metar, metar_element_type element, TLambda l, bool reverse = false)
{
    auto pattern = get_element_regex(element);
    auto metarString = metar;

    std::regex regexp(pattern, std::regex_constants::icase | std::regex_constants::optimize);
    std::cmatch result;
    while (std::regex_search(metarString.c_str(), result, regexp))
    {
        l(result);
        metarString = reverse ? result.prefix().str() : result.suffix().str();
    }
    return metarString;
}

//-----------------------------------------------------------------------------

} // namespace

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_station_identifier(std::string const& segment, TLambda && l)
{
    return ParseIfMatch(segment, metar_element_type::station_identifier, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_IDENT = 1;
        l(regex.str(EXPR_IDENT));
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_time(std::string const& segment, TLambda && l)
{
    return ParseIfMatch(segment, metar_element_type::observation_time, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_DAY = 1;
        static const unsigned short EXPR_HOUR = 2;
        static const unsigned short EXPR_MINUTE = 3;

        try
        {
            auto day = static_cast<uint8_t>(stoi(regex.str(EXPR_DAY)));
            auto hour = static_cast<uint8_t>(stoi(regex.str(EXPR_HOUR)));
            auto minute = static_cast<uint8_t>(stoi(regex.str(EXPR_MINUTE)));

            l(time(day, hour, minute));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_wind(std::string const& segment, TLambda && l)
{
    return ParseIfMatch(segment, metar_element_type::wind, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_DIRECTION = 1;
        static const unsigned short EXPR_SPEED = 2;
        static const unsigned short EXPR_GUST = 3;
        static const unsigned short EXPR_GUST_SPEED = 4;
        static const unsigned short EXPR_UNIT = 5;
        static const unsigned short EXPR_VAR = 6;
        static const unsigned short EXPR_VAR_LOWER = 7;
        static const unsigned short EXPR_VAR_UPPER = 8;

        try
        {
            wind windGroup;

            windGroup.unit = decode_speed_unit(regex[EXPR_UNIT]);
            windGroup.wind_speed = static_cast<uint8_t>(atoi(regex.str(EXPR_SPEED).c_str()));

            uint16_t direction = UINT16_MAX;
            if (regex.str(EXPR_DIRECTION) != "VRB")
            {
                direction = static_cast<uint16_t>(atoi(regex.str(EXPR_DIRECTION).c_str()));
            }
            windGroup.direction = direction;

            uint8_t gustSpeed = 0U;
            if (regex[EXPR_GUST].matched)
            {
                gustSpeed = static_cast<uint8_t>(atoi(regex.str(EXPR_GUST_SPEED).c_str()));
            }
            windGroup.gust_speed = gustSpeed;

            if (regex[EXPR_VAR].matched)
            {
                windGroup.variation_lower = static_cast<uint16_t>(atoi(regex.str(EXPR_VAR_LOWER).c_str()));
                windGroup.variation_upper = static_cast<uint16_t>(atoi(regex.str(EXPR_VAR_UPPER).c_str()));
            }
            l(std::move(windGroup));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_visibility(std::string const& segment, TLambda && l)
{
    return ParseIfMatch(segment, metar_element_type::visibility, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_ALL = 1;
        static const unsigned short EXPR_FRACTIONAL = 3;
        static const unsigned short EXPR_LESS_THAN = 4;
        static const unsigned short EXPR_FRAC_W = 5;
        static const unsigned short EXPR_FRAC_N = 6;
        static const unsigned short EXPR_FRAC_D = 7;
        static const unsigned short EXPR_VISIBILITY = 8;
        static const unsigned short EXPR_STATUTE = 9;

        if (regex.str(EXPR_ALL) == "CAVOK")
        {
            l(visibility(UINT16_MAX, distance_unit::metres));
            return;
        }

        try
        {
            double distance = 0.0;
            auto visibilityGroup = visibility(0U, distance_unit::metres, visibility_modifier_type::none);

            if (regex[EXPR_LESS_THAN].matched)
            {
                visibilityGroup.modifier = visibility_modifier_type::less_than;
            }

            if (regex[EXPR_FRACTIONAL].matched)
            {
                double fracN = atof(regex.str(EXPR_FRAC_N).c_str());
                double fracD = atof(regex.str(EXPR_FRAC_D).c_str());

                if (regex[EXPR_FRAC_W].matched)
                {
                    distance = atof(regex.str(EXPR_FRAC_W).c_str());
                }
                distance += fracN / fracD;
            }
            else
            {
                distance = atof(regex.str(EXPR_VISIBILITY).c_str());
            }

            if (regex[EXPR_STATUTE].matched)
            {
                visibilityGroup.unit = distance_unit::statute_miles;
            }

            visibilityGroup.distance = distance;
            l(std::move(visibilityGroup));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_weather(std::string const& segment, TLambda && l)
{
    return ParseForEachMatch(segment, metar_element_type::weather, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_INTENSITY = 1;
        static const unsigned short EXPR_DESCRIPTOR_ALL = 3;
        static const unsigned short EXPR_PHENOMENA_ALL = 4;
        static const unsigned short EXPR_DESCRIPTOR_SH = 7;
        static const unsigned short EXPR_PHENOMENA_SH = 8;
        static const unsigned short EXPR_DESCRIPTOR_TS = 10;
        static const unsigned short EXPR_PHENOMENA_TS = 11;
        static const unsigned short EXPR_DESCRIPTOR_FZ = 13;
        static const unsigned short EXPR_PHENOMENA_FZ = 14;
        static const unsigned short EXPR_PHENOMENA_NO_SPACE = 4;

        auto intensity = weather_intensity::moderate;
        auto descriptor = weather_descriptor::none;

        // Intensity
        if (regex[EXPR_INTENSITY].matched)
        {
            intensity = decode_weather_intensity(regex.str(EXPR_INTENSITY));
        }

        // Descriptor
        if (regex[EXPR_DESCRIPTOR_ALL].matched)
        {
            descriptor = decode_weather_descriptor(regex.str(EXPR_DESCRIPTOR_ALL));
        }
        else if (regex[EXPR_DESCRIPTOR_SH].matched)
        {
            descriptor = decode_weather_descriptor(regex.str(EXPR_DESCRIPTOR_SH));
        }
        else if (regex[EXPR_DESCRIPTOR_TS].matched)
        {
            descriptor = decode_weather_descriptor(regex.str(EXPR_DESCRIPTOR_TS));
        }
        else if (regex[EXPR_DESCRIPTOR_FZ].matched)
        {
            descriptor = decode_weather_descriptor(regex.str(EXPR_DESCRIPTOR_FZ));
        }

        // Phenomena
        unsigned short matchedPhenomena = 0;
        if (regex[EXPR_PHENOMENA_ALL].matched)
        {
            matchedPhenomena = EXPR_PHENOMENA_ALL;
        }
        else if (regex[EXPR_PHENOMENA_NO_SPACE].matched)
        {
            matchedPhenomena = EXPR_PHENOMENA_NO_SPACE;
        }
        else if (regex[EXPR_PHENOMENA_SH].matched)
        {
            matchedPhenomena = EXPR_PHENOMENA_SH;
        }
        else if (regex[EXPR_PHENOMENA_TS].matched)
        {
            matchedPhenomena = EXPR_PHENOMENA_TS;
        }
        else if (regex[EXPR_PHENOMENA_FZ].matched)
        {
            matchedPhenomena = EXPR_PHENOMENA_FZ;
        }

        weather weatherGroup;
        weatherGroup.intensity = intensity;
        weatherGroup.descriptor = descriptor;

        auto phenomenaGroup = regex.str(matchedPhenomena);
        if (matchedPhenomena != 0 && phenomenaGroup.size() % 2 == 0)
        {
            auto it = phenomenaGroup.begin();
            while (it != phenomenaGroup.end())
            {
                std::string phenomena("");
                phenomena += *it++;
                phenomena += *it++;

                weatherGroup.phenomena.push_back(
                    decode_weather_phenomena(phenomena)
                );
            }
        }

        l(std::move(weatherGroup));
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_sky_condition(std::string const& segment, TLambda && l)
{
    return ParseForEachMatch(segment, metar_element_type::sky_condition, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_CLEAR = 2;
        static const unsigned short EXPR_LAYER = 4;
        static const unsigned short EXPR_LAYER_ALTITUDE = 5;
        static const unsigned short EXPR_MANUAL = 6;

        try
        {
            auto skyCover = sky_cover_type::sky_clear;
            auto altitude = 0U;
            auto cloudType = sky_cover_cloud_type::unspecified;

            if (regex[EXPR_CLEAR].matched)
            {
                altitude = UINT32_MAX;
                cloudType = sky_cover_cloud_type::none;
                if (regex.str(EXPR_CLEAR) == "CLR")
                {
                    skyCover = sky_cover_type::clear_below_12000;
                }
            }
            else
            {
                skyCover = decode_sky_cover(regex.str(EXPR_LAYER).c_str());

                auto altitudeStr = regex.str(EXPR_LAYER_ALTITUDE);
                if (altitudeStr != "///")
                {
                    altitude = atoi(altitudeStr.c_str());
                }
                altitude *= 100;

                if (regex[EXPR_MANUAL].matched)
                {
                    cloudType = decode_sky_cover_cloud_type(regex.str(EXPR_MANUAL).c_str());
                }
            }

            cloud_layer skyCondition;
            skyCondition.sky_cover = skyCover;
            skyCondition.layer_height = altitude;
            skyCondition.cloud_type = cloudType;

            l(std::move(skyCondition));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_metar_report_type(std::string const& segment, TLambda && l)
{
    return ParseIfMatch(segment, metar_element_type::report_type, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_TYPE = 1;

        auto reportType = regex.str(EXPR_TYPE);
        metar_report_type type = (reportType == "METAR") ?
            metar_report_type::metar : metar_report_type::special;

        l(type);
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_metar_modifier(std::string const& segment, TLambda && l)
{
    return ParseIfMatch(segment, metar_element_type::report_modifier, [&](std::cmatch regex)
    {
        static const unsigned short EXPR = 1;

        auto modifierString = regex.str(EXPR);
        metar_modifier_type modifier = metar_modifier_type::none;

        if (modifierString == "AUTO")
        {
            modifier = metar_modifier_type::automatic;
        }
        else if (modifierString == "COR")
        {
            modifier = metar_modifier_type::corrected;
        }
        l(modifier);
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_runway_visual_range(std::string const& segment, TLambda && l)
{
    return ParseForEachMatch(segment, metar_element_type::runway_visual_range, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_RUNWAY_NUM = 1;
        static const unsigned short EXPR_RUNWAY_DESIGNATOR = 2;
        static const unsigned short EXPR_VISIBILITY_MIN_MOD = 3;
        static const unsigned short EXPR_VISIBILITY_MIN = 4;
        static const unsigned short EXPR_VARIABLE = 5;
        static const unsigned short EXPR_VISIBILITY_MAX_MOD = 6;
        static const unsigned short EXPR_VISIBILITY_MAX = 7;

        try
        {
            auto runwayNumber = static_cast<uint8_t>(atoi(regex.str(EXPR_RUNWAY_NUM).c_str()));
            auto runwayDesignator = runway_designator_type::none;
            auto visibilityMinModifier = visibility_modifier_type::none;
            auto visibilityMaxModifier = visibility_modifier_type::none;
            auto visibilityMin = static_cast<uint16_t>(atoi(regex.str(EXPR_VISIBILITY_MIN).c_str()));
            auto visibilityMax = visibilityMin;

            // Runway designator
            auto runwayDesignatorStr = regex.str(EXPR_RUNWAY_DESIGNATOR);
            if (runwayDesignatorStr == "L")
            {
                runwayDesignator = runway_designator_type::left;
            }
            else if (runwayDesignatorStr == "R")
            {
                runwayDesignator = runway_designator_type::right;
            }
            else if (runwayDesignatorStr == "C")
            {
                runwayDesignator = runway_designator_type::center;
            }

            // Minimum visibility modifier
            if (regex[EXPR_VISIBILITY_MIN_MOD].matched)
            {
                auto modifier = regex.str(EXPR_VISIBILITY_MIN_MOD);
                if (modifier == "M")
                {
                    visibilityMinModifier = visibility_modifier_type::less_than;
                }
                else if (modifier == "P")
                {
                    visibilityMinModifier = visibility_modifier_type::greater_than;
                }
            }

            // Maximum visibility modifier
            if (regex[EXPR_VISIBILITY_MAX_MOD].matched)
            {
                auto modifier = regex.str(EXPR_VISIBILITY_MAX_MOD);
                if (modifier == "M")
                {
                    visibilityMaxModifier = visibility_modifier_type::less_than;
                }
                else if (modifier == "P")
                {
                    visibilityMaxModifier = visibility_modifier_type::greater_than;
                }
            }

            // Variable visibility
            if (regex[EXPR_VARIABLE].matched)
            {
                visibilityMax = static_cast<uint16_t>(atoi(regex.str(EXPR_VISIBILITY_MAX).c_str()));
            }

            runway_visual_range rvr;
            rvr.runway_number = runwayNumber;
            rvr.runway_designator = runwayDesignator;
            rvr.visibility_min = visibility(visibilityMin, distance_unit::feet, visibilityMinModifier);
            rvr.visibility_max = visibility(visibilityMax, distance_unit::feet, visibilityMaxModifier);

            l(std::move(rvr));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_temperature_dewpoint(std::string const& segment, TLambda && l)
{
    return ParseIfMatch(segment, metar_element_type::temperature_dewpoint, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_TEMP_IS_MINUS = 1;
        static const unsigned short EXPR_TEMPERATURE = 2;
        static const unsigned short EXPR_DEW_IS_MINUS = 4;
        static const unsigned short EXPR_DEWPOINT = 5;

        util::optional<int8_t> temperature = util::nullopt;
        util::optional<int8_t> dewpoint = util::nullopt;

        if (regex[EXPR_TEMPERATURE].matched)
        {
            auto temperatureStr = regex.str(EXPR_TEMPERATURE);
            int8_t temperatureValue = static_cast<int8_t>(atoi(temperatureStr.c_str()));

            if (regex[EXPR_TEMP_IS_MINUS].matched)
            {
                temperatureValue = -temperatureValue;
            }
            temperature = temperatureValue;
        }

        if (regex[EXPR_DEWPOINT].matched)
        {
            auto dewpointStr = regex.str(EXPR_DEWPOINT);
            int8_t dewpointValue = static_cast<int8_t>(atoi(dewpointStr.c_str()));

            if (regex[EXPR_DEW_IS_MINUS].matched)
            {
                dewpointValue = -dewpointValue;
            }
            dewpoint = dewpointValue;
        }

        l(std::move(temperature), std::move(dewpoint));
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_altimeter(std::string const& segment, TLambda && l)
{
    return ParseIfMatch(segment, metar_element_type::altimeter, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_SETTING = 1;
        static const unsigned short EXPR_ALT = 2;

        try
        {
            altimeter altimeterGroup;
            altimeterGroup.pressure = atof(regex.str(EXPR_ALT).c_str());

            if (regex.str(EXPR_SETTING) == "Q")
            {
                altimeterGroup.unit = pressure_unit::hPa;
            }
            else
            {
                altimeterGroup.pressure /= 100.0;
                altimeterGroup.unit = pressure_unit::inHg;
            }
            l(std::move(altimeterGroup));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
}

//-----------------------------------------------------------------------------

template <class TLambda>
std::string parse_remarks(std::string const& segment, TLambda && l)
{
    return ParseIfMatch(segment, metar_element_type::remarks, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_ALL = 1;
        l(regex.str(EXPR_ALL));
    }, true);
}

//-----------------------------------------------------------------------------

} // namespace aw
