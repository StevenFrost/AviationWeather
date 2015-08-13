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

#include "AviationWeatherPch.h"

#include <AviationWeather/converters.h>
#include <AviationWeather/metar.h>

#include <algorithm>
#include <ctime>
#include <regex>
#include <string>
#include <map>

//-----------------------------------------------------------------------------

namespace aw
{
namespace metar
{

//-----------------------------------------------------------------------------

namespace detail
{

//-----------------------------------------------------------------------------

static const int MAX_VISIBILITY = 9999;

#define REGEX_REPORT_TYPE      "(METAR|SPECI) "
#define REGEX_STATION_IDENT    "([A-Z0-9]{4}) "
#define REGEX_OBSERVATION_TIME "([0-9]{2})([0-9]{2})([0-9]{2})Z "
#define REGEX_REPORT_MODIFIER  "(AUTO|COR) "
#define REGEX_WIND             "([0-9]{3}|VRB)([0-9]{2,3})(G([0-9]{2,3}))?(KT|MPS)( ([0-9]{3})V([0-9]{3}))? "
#define REGEX_VISIBILITY       "(CAVOK|(((M)?([12]?)[ ]?([0-9])/([0-9]))|([0-9]{1,5}))(SM)?) "
#define REGEX_RVR              "R([0-9]{2})([LRC])?/([MP]?)([0-9]{4})(V([0-9]{4}))?FT "
#define REGEX_WEATHER          "([+-]|VC)?((MI|PR|BC|DR|BL)?((DZ|RA|SN|SG|IC|PL|GR|GS|UP){1,3}|(BR|FG|FU|VA|DU|SA|HZ|PY|PO|SQ|FC|SS|DS)) |(SH)(RA|SN|PL|GS|GR){0,3} |(TS)(RA|SN|PL|GS|GR){0,3} |(FZ)(FG|DZ|RA){1,3} )"
#define REGEX_SKY_CONDITION    "((SKC|CLR) )|((VV|FEW|SCT|BKN|OVC)([0-9]{3}|///))(CB|TCU)? "
#define REGEX_TEMP_DEW         "(M)?([0-9]{2})/((M)?([0-9]{2}))? "
#define REGEX_ALTIMETER        "(Q|A)([0-9]{4})( |$)"
#define REGEX_REMARKS          "RMK (.*)$"

//-----------------------------------------------------------------------------

#if (_MSC_VER >= 1900)
struct pattern
{
    element_type type;
    const char*  regex;
};

constexpr pattern g_patterns[] =
{
    { element_type::report_type,          REGEX_REPORT_TYPE },
    { element_type::station_identifier,   REGEX_STATION_IDENT },
    { element_type::observation_time,     REGEX_OBSERVATION_TIME },
    { element_type::report_modifier,      REGEX_REPORT_MODIFIER },
    { element_type::wind,                 REGEX_WIND },
    { element_type::visibility,           REGEX_VISIBILITY },
    { element_type::runway_visual_range,  REGEX_RVR },
    { element_type::weather,              REGEX_WEATHER },
    { element_type::sky_condition,        REGEX_SKY_CONDITION },
    { element_type::temperature_dewpoint, REGEX_TEMP_DEW },
    { element_type::altimeter,            REGEX_ALTIMETER },
    { element_type::remarks,              REGEX_REMARKS }
};

constexpr const char* get_element_regex_impl(element_type type, const pattern* patterns)
{
    return (patterns->type == type) ? patterns->regex : get_element_regex_impl(type, patterns + 1);
}

constexpr const char* get_element_regex(element_type type)
{
    return get_element_regex_impl(type, g_patterns);
}

#else

const std::map<element_type, std::string> g_patterns =
{
    { element_type::report_type,          REGEX_REPORT_TYPE },
    { element_type::station_identifier,   REGEX_STATION_IDENT },
    { element_type::observation_time,     REGEX_OBSERVATION_TIME },
    { element_type::report_modifier,      REGEX_REPORT_MODIFIER },
    { element_type::wind,                 REGEX_WIND },
    { element_type::visibility,           REGEX_VISIBILITY },
    { element_type::runway_visual_range,  REGEX_RVR },
    { element_type::weather,              REGEX_WEATHER },
    { element_type::sky_condition,        REGEX_SKY_CONDITION },
    { element_type::temperature_dewpoint, REGEX_TEMP_DEW },
    { element_type::altimeter,            REGEX_ALTIMETER },
    { element_type::remarks,              REGEX_REMARKS }
};

std::string get_element_regex(element_type type)
{
    auto pattern = g_patterns.find(type);
    if (pattern == g_patterns.end())
    {
        return "";
    }
    return pattern->second;
}
#endif // _MSC_VER == 1900

//-----------------------------------------------------------------------------

weather_intensity decode_weather_intensity(std::string const& symbol)
{
    if (symbol == "-") {
        return weather_intensity::light;
    }
    else if (symbol == "+") {
        return weather_intensity::heavy;
    }
    else if (symbol == "VC") {
        return weather_intensity::in_the_vicinity;
    }
    else {
        return weather_intensity::moderate;
    }
}

//-----------------------------------------------------------------------------

weather_descriptor decode_weather_descriptor(std::string const& symbol)
{
    if (symbol == "MI") {
        return weather_descriptor::shallow;
    }
    else if (symbol == "PR") {
        return weather_descriptor::partial;
    }
    else if (symbol == "BC") {
        return weather_descriptor::patches;
    }
    else if (symbol == "DR") {
        return weather_descriptor::low_drifting;
    }
    else if (symbol == "BL") {
        return weather_descriptor::blowing;
    }
    else if (symbol == "SH") {
        return weather_descriptor::showers;
    }
    else if (symbol == "TS") {
        return weather_descriptor::thunderstorm;
    }
    else if (symbol == "FZ") {
        return weather_descriptor::freezing;
    }
    else {
        return weather_descriptor::none;
    }
}

//-----------------------------------------------------------------------------

weather_phenomena decode_weather_phenomena(std::string const& symbol)
{
    if (symbol == "DZ") {
        return weather_phenomena::drizzle;
    }
    else if (symbol == "RA") {
        return weather_phenomena::rain;
    }
    else if (symbol == "SN") {
        return weather_phenomena::snow;
    }
    else if (symbol == "SG") {
        return weather_phenomena::snow_grains;
    }
    else if (symbol == "IC") {
        return weather_phenomena::ice_crystals;
    }
    else if (symbol == "PL") {
        return weather_phenomena::ice_pellets;
    }
    else if (symbol == "GR") {
        return weather_phenomena::hail;
    }
    else if (symbol == "GS") {
        return weather_phenomena::small_hail;
    }
    else if (symbol == "UP") {
        return weather_phenomena::small_hail;
    }
    else if (symbol == "BR") {
        return weather_phenomena::mist;
    }
    else if (symbol == "FG") {
        return weather_phenomena::fog;
    }
    else if (symbol == "FU") {
        return weather_phenomena::smoke;
    }
    else if (symbol == "VA") {
        return weather_phenomena::volcanic_ash;
    }
    else if (symbol == "DU") {
        return weather_phenomena::widespread_dust;
    }
    else if (symbol == "SA") {
        return weather_phenomena::sand;
    }
    else if (symbol == "HZ") {
        return weather_phenomena::haze;
    }
    else if (symbol == "PY") {
        return weather_phenomena::spray;
    }
    else if (symbol == "PO") {
        return weather_phenomena::well_developed_dust_whirls;
    }
    else if (symbol == "SQ") {
        return weather_phenomena::squalls;
    }
    else if (symbol == "FC") {
        return weather_phenomena::funnel_cloud_tornado_waterspout;
    }
    else if (symbol == "SS") {
        return weather_phenomena::sandstorm;
    }
    else if (symbol == "DS") {
        return weather_phenomena::duststorm;
    }
    else {
        return weather_phenomena::none;
    }
}

//-----------------------------------------------------------------------------

sky_cover_type decode_sky_cover(std::string const& symbol)
{
    if (symbol == "VV") {
        return sky_cover_type::vertical_visibility;
    }
    else if (symbol == "FEW") {
        return sky_cover_type::few;
    }
    else if (symbol == "SCT") {
        return sky_cover_type::scattered;
    }
    else if (symbol == "BKN") {
        return sky_cover_type::broken;
    }
    else if (symbol == "OVC") {
        return sky_cover_type::overcast;
    }
    else {
        return sky_cover_type::clear;
    }
}

//-----------------------------------------------------------------------------

sky_cover_cloud_type decode_sky_cover_cloud_type(std::string const& symbol)
{
    if (symbol == "CB") {
        return sky_cover_cloud_type::cumulonimbus;
    }
    else if (symbol == "TCU") {
        return sky_cover_cloud_type::towering_cumulus;
    }
    else {
        return sky_cover_cloud_type::none;
    }
}

//-----------------------------------------------------------------------------

distance_unit decode_distance_unit(std::string const& symbol)
{
    if (symbol == "FT") {
        return distance_unit::feet;
    }
    else {
        return distance_unit::metres;
    }
}

//-----------------------------------------------------------------------------

speed_unit decode_speed_unit(std::string const& symbol)
{
    if (symbol == "KT") {
        return speed_unit::kt;
    }
    else {
        return speed_unit::mph;
    }
}

//-----------------------------------------------------------------------------

template <class TExpected = double, class TEnum, class TUnit, class TVal, class TLambda>
bool comparison_conversion_helper(TEnum to, TUnit leftUnit, TUnit rightUnit, TVal const& leftVal, TVal const& rightVal, TLambda && l)
{
    auto lhs = leftUnit == to ? static_cast<TExpected>(leftVal) : convert<TExpected>(leftVal, leftUnit, to);
    auto rhs = rightUnit == to ? static_cast<TExpected>(rightVal) : convert<TExpected>(rightVal, rightUnit, to);

    return l(std::forward<TExpected>(lhs), std::forward<TExpected>(rhs));
}

//-----------------------------------------------------------------------------

template <typename TLambda>
std::string ParseIfMatch(std::string const& metar, element_type element, TLambda l, bool reverse = false)
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
std::string ParseForEachMatch(std::string const& metar, element_type element, TLambda l, bool reverse = false)
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

void parse_report_type(metar_info& info, std::string& metar)
{
    auto result = ParseIfMatch(metar, element_type::report_type, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_TYPE = 1;

        auto reportType = regex.str(EXPR_TYPE);
        if (reportType == "METAR")
        {
            info.type = report_type::metar;
        }
        else if (reportType == "SPECI")
        {
            info.type = report_type::special;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_station_identifier(metar_info& info, std::string& metar)
{
    auto result = ParseIfMatch(metar, element_type::station_identifier, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_IDENT = 1;

        info.station_identifier = regex.str(EXPR_IDENT);
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_observation_time(metar_info& info, std::string& metar)
{
    auto result = ParseIfMatch(metar, element_type::observation_time, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_DAY = 1;
        static const unsigned short EXPR_HOUR = 2;
        static const unsigned short EXPR_MINUTE = 3;

        try
        {
            auto day = static_cast<uint8_t>(stoi(regex.str(EXPR_DAY)));
            auto hour = static_cast<uint8_t>(stoi(regex.str(EXPR_HOUR)));
            auto minute = static_cast<uint8_t>(stoi(regex.str(EXPR_MINUTE)));

            info.report_time = observation_time(day, hour, minute);
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_modifier(metar_info& info, std::string& metar)
{
    auto result = ParseIfMatch(metar, element_type::report_modifier, [&](std::cmatch regex)
    {
        static const unsigned short EXPR = 1;

        auto modifier = regex.str(EXPR);
        if (modifier == "AUTO")
        {
            info.modifier = modifier_type::automatic;
        }
        else if (modifier == "COR")
        {
            info.modifier = modifier_type::corrected;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_wind(metar_info& info, std::string& metar)
{
    auto result = ParseIfMatch(metar, element_type::wind, [&](std::cmatch regex)
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
            auto unit = decode_speed_unit(regex[EXPR_UNIT]);
            auto speed = static_cast<uint8_t>(atoi(regex.str(EXPR_SPEED).c_str()));

            uint16_t direction = UINT16_MAX;
            if (regex.str(EXPR_DIRECTION) != "VRB")
            {
                direction = static_cast<uint16_t>(atoi(regex.str(EXPR_DIRECTION).c_str()));
            }

            uint8_t gustSpeed = 0U;
            if (regex[EXPR_GUST].matched)
            {
                gustSpeed = static_cast<uint8_t>(atoi(regex.str(EXPR_GUST_SPEED).c_str()));
            }

            uint16_t variationLower = UINT16_MAX;
            uint16_t variationUpper = UINT16_MAX;
            if (regex[EXPR_VAR].matched)
            {
                variationLower = static_cast<uint16_t>(atoi(regex.str(EXPR_VAR_LOWER).c_str()));
                variationUpper = static_cast<uint16_t>(atoi(regex.str(EXPR_VAR_UPPER).c_str()));
            }

            wind windGroup;
            windGroup.unit = unit;
            windGroup.direction = direction;
            windGroup.wind_speed = speed;
            windGroup.gust_speed = gustSpeed;
            windGroup.variation_lower = variationLower;
            windGroup.variation_upper = variationUpper;

            info.wind_group = std::move(windGroup);
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_visibility(metar_info& info, std::string& metar)
{
    auto result = ParseIfMatch(metar, element_type::visibility, [&](std::cmatch regex)
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
            info.visibility_group = visibility(UINT16_MAX, distance_unit::metres);
            return;
        }

        try
        {
            double hpv = 0.0;
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
                    hpv = atof(regex.str(EXPR_FRAC_W).c_str());
                }
                hpv += fracN / fracD;
            }
            else
            {
                hpv = atof(regex.str(EXPR_VISIBILITY).c_str());
            }

            if (regex[EXPR_STATUTE].matched)
            {
                visibilityGroup.unit = distance_unit::statute_miles;
            }

            visibilityGroup.distance = hpv;
            info.visibility_group = visibilityGroup;
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_runway_visual_range(metar_info& info, std::string& metar)
{
    auto result = ParseForEachMatch(metar, element_type::runway_visual_range, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_RUNWAY_NUM = 1;
        static const unsigned short EXPR_RUNWAY_DESIGNATOR = 2;
        static const unsigned short EXPR_VISIBILITY_MOD = 3;
        static const unsigned short EXPR_VISIBILITY_MIN = 4;
        static const unsigned short EXPR_VARIABLE = 5;
        static const unsigned short EXPR_VISIBILITY_MAX = 6;

        try
        {
            auto runwayNumber = static_cast<uint8_t>(atoi(regex.str(EXPR_RUNWAY_NUM).c_str()));
            auto runwayDesignator = runway_designator_type::none;
            auto visibilityModifier = visibility_modifier_type::none;
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

            // Visibility modifier
            if (regex[EXPR_VISIBILITY_MOD].matched)
            {
                auto modifier = regex.str(EXPR_VISIBILITY_MOD);
                if (modifier == "M")
                {
                    visibilityModifier = visibility_modifier_type::less_than;
                }
                else if (modifier == "P")
                {
                    visibilityModifier = visibility_modifier_type::greater_than;
                }
            }

            // Variable visibility
            if (regex[EXPR_VARIABLE].matched)
            {
                visibilityMax = static_cast<uint16_t>(atoi(regex.str(EXPR_VISIBILITY_MAX).c_str()));
            }

            runway_visual_range rvr;
            rvr.unit = distance_unit::feet;
            rvr.runway_number = runwayNumber;
            rvr.runway_designator = runwayDesignator;
            rvr.visibility_modifier = visibilityModifier;
            rvr.visibility_min = visibilityMin;
            rvr.visibility_max = visibilityMax;

            info.runway_visual_range_group.push_back(std::move(rvr));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_weather(metar_info& info, std::string& metar)
{
    auto result = ParseForEachMatch(metar, element_type::weather, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_INTENSITY = 1;
        static const unsigned short EXPR_DESCRIPTOR_ALL = 3;
        static const unsigned short EXPR_PHENOMENA_ALL = 5;
        static const unsigned short EXPR_DESCRIPTOR_SH = 7;
        static const unsigned short EXPR_PHENOMENA_SH = 8;
        static const unsigned short EXPR_DESCRIPTOR_TS = 9;
        static const unsigned short EXPR_PHENOMENA_TS = 10;
        static const unsigned short EXPR_DESCRIPTOR_FZ = 9;
        static const unsigned short EXPR_PHENOMENA_FZ = 12;

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

        info.weather_group.push_back(std::move(weatherGroup));
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_sky_condition(metar_info& info, std::string& metar)
{
    auto result = ParseForEachMatch(metar, element_type::sky_condition, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_CLEAR = 1;
        static const unsigned short EXPR_LAYER = 4;
        static const unsigned short EXPR_LAYER_ALTITUDE = 5;
        static const unsigned short EXPR_MANUAL = 6;

        try
        {
            auto skyCover = sky_cover_type::clear;
            auto altitude = 0U;
            auto cloudType = sky_cover_cloud_type::none;

            if (!regex[EXPR_CLEAR].matched)
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

            info.sky_condition_group.push_back(std::move(skyCondition));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_temperature_dewpoint(metar_info& info, std::string& metar)
{
    auto result = ParseIfMatch(metar, element_type::temperature_dewpoint, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_TEMP_IS_MINUS = 1;
        static const unsigned short EXPR_TEMPERATURE = 2;
        static const unsigned short EXPR_DEW_IS_MINUS = 4;
        static const unsigned short EXPR_DEWPOINT = 5;

        auto temperatureStr = regex.str(EXPR_TEMPERATURE);
        auto dewpointStr = regex.str(EXPR_DEWPOINT);

        auto temperature = static_cast<uint8_t>(atoi(temperatureStr.c_str()));
        auto dewpoint = static_cast<uint8_t>(atoi(dewpointStr.c_str()));

        if (regex[EXPR_TEMP_IS_MINUS].matched)
        {
            temperature = -temperature;
        }
        if (regex[EXPR_DEW_IS_MINUS].matched)
        {
            dewpoint = -dewpoint;
        }

        info.temperature = temperature;
        info.dewpoint = dewpoint;
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_altimeter(metar_info& info, std::string& metar)
{
    auto result = ParseIfMatch(metar, element_type::altimeter, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_SETTING = 1;
        static const unsigned short EXPR_ALT = 2;

        try
        {
            altimeter altimeterGroup;
            altimeterGroup.pressure = atof(regex.str(EXPR_ALT).c_str()) / 100.0;

            if (regex.str(EXPR_SETTING) == "Q")
            {
                altimeterGroup.unit = pressure_unit::hPa;
            }
            else
            {
                altimeterGroup.unit = pressure_unit::inHg;
            }

            info.altimeter_group = altimeterGroup;
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void parse_remarks(metar_info& info, std::string& metar)
{
    auto result = ParseIfMatch(metar, element_type::remarks, [&](std::cmatch regex)
    {
        static const unsigned short EXPR_ALL = 1;
        info.remarks = regex.str(EXPR_ALL);
    }, true);
    metar = result;
}

//-----------------------------------------------------------------------------

} // namespace detail

//-----------------------------------------------------------------------------

visibility::visibility() :
    unit(distance_unit::feet),
    distance(UINT32_MAX),
    modifier(visibility_modifier_type::none)
{}

visibility::visibility(double distance, distance_unit unit, visibility_modifier_type modifier) :
    unit(unit),
    distance(distance),
    modifier(modifier)
{}

visibility::visibility(visibility && other) :
    unit(distance_unit::feet),
    distance(UINT32_MAX),
    modifier(visibility_modifier_type::none)
{
    *this = std::move(other);
}

visibility& visibility::operator=(visibility && rhs)
{
    if (this != &rhs)
    {
        unit = rhs.unit;
        distance = rhs.distance;
        modifier = rhs.modifier;

        rhs.unit = distance_unit::feet;
        rhs.distance = UINT32_MAX;
        rhs.modifier = visibility_modifier_type::none;
    }
    return *this;
}

bool visibility::operator==(visibility const& rhs) const
{
    auto distanceEqual = detail::comparison_conversion_helper<double>(distance_unit::feet, unit, rhs.unit, distance, rhs.distance,
        [](double && l, double && r)
    {
        return l == r;
    });

    return distanceEqual && (modifier == rhs.modifier);
}

bool visibility::operator!=(visibility const& rhs) const
{
    return !(*this == rhs);
}

bool visibility::operator<=(visibility const& rhs) const
{
    auto distanceLeft = convert(distance, unit, distance_unit::feet);
    auto distanceRight = convert(rhs.distance, rhs.unit, distance_unit::feet);

    return (distanceLeft == distanceRight) && (modifier == rhs.modifier) ||
        (distanceLeft == distanceRight) && (modifier == visibility_modifier_type::less_than) ||
        (distanceLeft < distanceRight);
}

bool visibility::operator>=(visibility const& rhs) const
{
    auto distanceLeft = convert(distance, unit, distance_unit::feet);
    auto distanceRight = convert(rhs.distance, rhs.unit, distance_unit::feet);

    return (distanceLeft == distanceRight) && (modifier == rhs.modifier) ||
        (distanceLeft == distanceRight) && (rhs.modifier == visibility_modifier_type::less_than) ||
        (distanceLeft > distanceRight);
}

bool visibility::operator<(visibility const& rhs) const
{
    auto distanceLeft = convert(distance, unit, distance_unit::feet);
    auto distanceRight = convert(rhs.distance, rhs.unit, distance_unit::feet);

    return (distanceLeft == distanceRight) && (modifier == visibility_modifier_type::less_than) ||
        (distanceLeft < distanceRight);
}

bool visibility::operator>(visibility const& rhs) const
{
    auto distanceLeft = convert(distance, unit, distance_unit::feet);
    auto distanceRight = convert(rhs.distance, rhs.unit, distance_unit::feet);

    return (distanceLeft == distanceRight) && (rhs.modifier == visibility_modifier_type::less_than) ||
        (distanceLeft > distanceRight);
}

//-----------------------------------------------------------------------------

altimeter::altimeter() :
    unit(pressure_unit::hPa),
    pressure(0.0)
{}

altimeter::altimeter(double pressure, pressure_unit unit) :
    unit(unit),
    pressure(pressure)
{}

altimeter::altimeter(altimeter && other) :
    unit(pressure_unit::hPa),
    pressure(0.0)
{
    *this = std::move(other);
}

altimeter& altimeter::operator=(altimeter && rhs)
{
    if (this != &rhs)
    {
        unit = rhs.unit;
        pressure = rhs.pressure;

        rhs.unit = pressure_unit::hPa;
        rhs.pressure = 0.0;
    }
    return *this;
}

bool altimeter::operator==(altimeter const& rhs) const
{
    return detail::comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
        [](double && l, double && r)
    {
        return l == r;
    });
}

bool altimeter::operator!=(altimeter const& rhs) const
{
    return !(*this == rhs);
}

bool altimeter::operator<=(altimeter const& rhs) const
{
    return detail::comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
        [](double && l, double && r)
    {
        return l <= r;
    });
}

bool altimeter::operator>=(altimeter const& rhs) const
{
    return detail::comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
        [](double && l, double && r)
    {
        return l >= r;
    });
}

bool altimeter::operator<(altimeter const& rhs) const
{
    return detail::comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
        [](double && l, double && r)
    {
        return l < r;
    });
}

bool altimeter::operator>(altimeter const& rhs) const
{
    return detail::comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
        [](double && l, double && r)
    {
        return l > r;
    });
}

//-----------------------------------------------------------------------------

observation_time::observation_time() :
    observation_time(0)
{}

observation_time::observation_time(uint8_t day, uint8_t hour, uint8_t minute) :
    day_of_month(day),
    hour_of_day(hour),
    minute_of_hour(minute)
{}

observation_time::observation_time(time_t time) :
    day_of_month(1),
    hour_of_day(0),
    minute_of_hour(0)
{
    struct tm t;
    auto result = localtime_s(&t, &time);

    if (result != EINVAL)
    {
        day_of_month = static_cast<uint8_t>(t.tm_mon);
        hour_of_day = static_cast<uint8_t>(t.tm_hour);
        minute_of_hour = static_cast<uint8_t>(t.tm_min);
    }
}

observation_time::observation_time(observation_time && other) :
    day_of_month(1),
    hour_of_day(0),
    minute_of_hour(0)
{
    *this = std::move(other);
}

observation_time& observation_time::operator=(observation_time && rhs)
{
    if (this != &rhs)
    {
        day_of_month = rhs.day_of_month;
        hour_of_day = rhs.hour_of_day;
        minute_of_hour = rhs.minute_of_hour;

        rhs.day_of_month = 1;
        rhs.hour_of_day = 0;
        rhs.minute_of_hour = 0;
    }
    return *this;
}

bool observation_time::operator== (observation_time const& rhs) const
{
    return (day_of_month == rhs.day_of_month) &&
        (hour_of_day == rhs.hour_of_day) &&
        (minute_of_hour == rhs.minute_of_hour);
}

bool observation_time::operator!= (observation_time const& rhs) const
{
    return !(*this == rhs);
}

//-----------------------------------------------------------------------------

wind::wind() :
    unit(speed_unit::kt),
    direction(UINT16_MAX),
    wind_speed(0),
    gust_speed(0),
    variation_lower(UINT16_MAX),
    variation_upper(UINT16_MAX)
{}

wind::wind(wind && other) :
    unit(speed_unit::kt),
    direction(UINT16_MAX),
    wind_speed(0),
    gust_speed(0),
    variation_lower(UINT16_MAX),
    variation_upper(UINT16_MAX)
{
    *this = std::move(other);
}

wind& wind::operator=(wind && rhs)
{
    if (this != &rhs)
    {
        unit = rhs.unit;
        direction = rhs.direction;
        wind_speed = rhs.wind_speed;
        gust_speed = rhs.gust_speed;
        variation_lower = rhs.variation_lower;
        variation_upper = rhs.variation_upper;

        rhs.unit = speed_unit::kt;
        rhs.direction = UINT16_MAX;
        rhs.wind_speed = 0;
        rhs.gust_speed = 0;
        rhs.variation_lower = UINT16_MAX;
        rhs.variation_upper = UINT16_MAX;
    }
    return *this;
}

bool wind::operator== (wind const& rhs) const
{
    return (unit == rhs.unit) &&
        (direction == rhs.direction) &&
        (wind_speed == rhs.wind_speed) &&
        (gust_speed == rhs.gust_speed) &&
        (variation_lower == rhs.variation_lower) &&
        (variation_upper == rhs.variation_upper);
}

bool wind::operator!= (wind const& rhs) const
{
    return !(*this == rhs);
}

bool wind::is_variable() const
{
    return (direction == UINT16_MAX) || (variation_lower != UINT16_MAX && variation_upper != UINT16_MAX);
}

uint8_t wind::gust_factor() const
{
    return gust_speed <= wind_speed ? 0 : gust_speed - wind_speed;
}

double wind::headwind_component(double heading, bool useGusts) const
{
    return (useGusts ? gust_speed : wind_speed) * cos(direction - heading);
}

double wind::crosswind_component(double heading, bool useGusts) const
{
    return (useGusts ? gust_speed : wind_speed) * sin(direction - heading);
}

//-----------------------------------------------------------------------------

runway_visual_range::runway_visual_range() :
    unit(distance_unit::feet),
    runway_number(0U),
    runway_designator(runway_designator_type::none),
    visibility_modifier(visibility_modifier_type::none),
    visibility_min(UINT16_MAX),
    visibility_max(UINT16_MAX)
{}

runway_visual_range::runway_visual_range(runway_visual_range && other) :
    unit(distance_unit::feet),
    runway_number(0U),
    runway_designator(runway_designator_type::none),
    visibility_modifier(visibility_modifier_type::none),
    visibility_min(UINT16_MAX),
    visibility_max(UINT16_MAX)
{
    *this = std::move(other);
}

runway_visual_range& runway_visual_range::operator=(runway_visual_range && rhs)
{
    if (this != &rhs)
    {
        unit = rhs.unit;
        runway_number = rhs.runway_number;
        runway_designator = rhs.runway_designator;
        visibility_modifier = rhs.visibility_modifier;
        visibility_min = rhs.visibility_min;
        visibility_max = rhs.visibility_max;

        unit = distance_unit::feet;
        runway_number = 0U;
        runway_designator = runway_designator_type::none;
        visibility_modifier = visibility_modifier_type::none;
        visibility_min = UINT16_MAX;
        visibility_max = UINT16_MAX;
    }
    return *this;
}

bool runway_visual_range::operator== (runway_visual_range const& rhs) const
{
    return (unit == rhs.unit) &&
        (runway_number == rhs.runway_number) &&
        (runway_designator == rhs.runway_designator) &&
        (visibility_modifier == rhs.visibility_modifier) &&
        (visibility_min == rhs.visibility_min) &&
        (visibility_max == rhs.visibility_max);
}

bool runway_visual_range::operator!= (runway_visual_range const& rhs) const
{
    return !(*this == rhs);
}

bool runway_visual_range::is_variable() const
{
    return visibility_min != visibility_max;
}

//-----------------------------------------------------------------------------

weather::weather() :
    intensity(weather_intensity::moderate),
    descriptor(weather_descriptor::none)
{}

weather::weather(weather && other) :
    intensity(weather_intensity::moderate),
    descriptor(weather_descriptor::none)
{
    *this = std::move(other);
}

weather& weather::operator=(weather && rhs)
{
    if (this != &rhs)
    {
        intensity = rhs.intensity;
        descriptor = rhs.descriptor;
        phenomena = std::move(rhs.phenomena);

        rhs.intensity = weather_intensity::moderate;
        rhs.descriptor = weather_descriptor::none;
        rhs.phenomena.clear();
    }
    return *this;
}

bool weather::operator== (weather const& rhs) const
{
    return (intensity == rhs.intensity) &&
        (descriptor == rhs.descriptor) &&
        (phenomena == rhs.phenomena);
}

bool weather::operator!= (weather const& rhs) const
{
    return !(*this == rhs);
}

//-----------------------------------------------------------------------------

cloud_layer::cloud_layer() :
    unit(distance_unit::feet),
    sky_cover(sky_cover_type::clear),
    layer_height(UINT32_MAX),
    cloud_type(sky_cover_cloud_type::none)
{}

cloud_layer::cloud_layer(cloud_layer && other) :
    unit(distance_unit::feet),
    sky_cover(sky_cover_type::clear),
    layer_height(UINT32_MAX),
    cloud_type(sky_cover_cloud_type::none)
{
    *this = std::move(other);
}

cloud_layer& cloud_layer::operator=(cloud_layer && rhs)
{
    if (this != &rhs)
    {
        unit = rhs.unit;
        sky_cover = rhs.sky_cover;
        layer_height = rhs.layer_height;
        cloud_type = rhs.cloud_type;

        rhs.unit = distance_unit::feet;
        rhs.sky_cover = sky_cover_type::clear;
        rhs.layer_height = UINT32_MAX;
        rhs.cloud_type = sky_cover_cloud_type::none;
    }
    return *this;
}

bool cloud_layer::operator== (cloud_layer const& rhs) const
{
    return (sky_cover == rhs.sky_cover) &&
        (layer_height == rhs.layer_height) &&
        (cloud_type == rhs.cloud_type);
}

bool cloud_layer::operator!= (cloud_layer const& rhs) const
{
    return !(*this == rhs);
}

bool cloud_layer::is_unlimited() const
{
    return layer_height == UINT32_MAX;
}

//-----------------------------------------------------------------------------

metar_info::metar_info(std::string const& metar) :
    metar(metar),
    type(report_type::metar),
    station_identifier(""),
    modifier(modifier_type::automatic),
    temperature(0),
    dewpoint(0),
    remarks("")
{
    parse();
}

metar_info::metar_info(metar_info && other) :
    metar(""),
    type(report_type::metar),
    station_identifier(""),
    modifier(modifier_type::automatic),
    temperature(0),
    dewpoint(0),
    remarks("")
{
    *this = std::move(other);
}

metar_info& metar_info::operator=(metar_info && rhs)
{
    if (this != &rhs)
    {
        metar = std::move(rhs.metar);
        type = rhs.type;
        station_identifier = std::move(rhs.station_identifier);
        report_time = std::move(rhs.report_time);
        modifier = rhs.modifier;
        wind_group = std::move(rhs.wind_group);
        visibility_group = std::move(rhs.visibility_group);
        runway_visual_range_group = std::move(rhs.runway_visual_range_group);
        weather_group = std::move(rhs.weather_group);
        sky_condition_group = std::move(rhs.sky_condition_group);
        temperature = rhs.temperature;
        dewpoint = rhs.dewpoint;
        altimeter_group = std::move(rhs.altimeter_group);
        remarks = std::move(rhs.remarks);

        rhs.metar = "";
        rhs.type = report_type::metar;
        rhs.station_identifier = "";
        rhs.report_time = observation_time();
        rhs.modifier = modifier_type::automatic;
        rhs.wind_group = wind();
        rhs.visibility_group = visibility();
        rhs.runway_visual_range_group.clear();
        rhs.weather_group.clear();
        rhs.sky_condition_group.clear();
        rhs.temperature = 0;
        rhs.dewpoint = 0;
        rhs.altimeter_group = altimeter();
        rhs.remarks = "";
    }
    return *this;
}

bool metar_info::operator== (metar_info const& rhs) const
{
    return (type == rhs.type) &&
        (station_identifier == rhs.station_identifier) &&
        (report_time == rhs.report_time) &&
        (modifier == rhs.modifier) &&
        (wind_group == rhs.wind_group) &&
        (visibility_group == rhs.visibility_group) &&
        (runway_visual_range_group == rhs.runway_visual_range_group) &&
        (weather_group == rhs.weather_group) &&
        (sky_condition_group == rhs.sky_condition_group) &&
        (temperature == rhs.temperature) &&
        (dewpoint == rhs.dewpoint) &&
        (altimeter_group == rhs.altimeter_group) &&
        (remarks == rhs.remarks);
}

bool metar_info::operator!= (metar_info const& rhs) const
{
    return !(*this == rhs);
}

void metar_info::parse()
{
    std::string baseMetar = metar;

    // We parse remarks first to avoid over-matching in earlier groups
    detail::parse_remarks(*this, baseMetar);

    detail::parse_report_type(*this, baseMetar);
    detail::parse_station_identifier(*this, baseMetar);
    detail::parse_observation_time(*this, baseMetar);
    detail::parse_modifier(*this, baseMetar);
    detail::parse_wind(*this, baseMetar);
    detail::parse_visibility(*this, baseMetar);
    detail::parse_runway_visual_range(*this, baseMetar);
    detail::parse_weather(*this, baseMetar);
    detail::parse_sky_condition(*this, baseMetar);
    detail::parse_temperature_dewpoint(*this, baseMetar);
    detail::parse_altimeter(*this, baseMetar);
}

cloud_layer metar_info::ceiling() const
{
    auto result = std::find_if(sky_condition_group.begin(), sky_condition_group.end(), [](cloud_layer layer)
    {
        return layer.sky_cover == sky_cover_type::broken || layer.sky_cover == sky_cover_type::overcast || layer.sky_cover == sky_cover_type::vertical_visibility;
    });
    return result != sky_condition_group.end() ? *result : cloud_layer();
}

flight_category metar_info::flight_category() const
{
    auto ceiling = metar_info::ceiling();
    if (aw::convert(visibility_group.distance, visibility_group.unit, distance_unit::statute_miles) >= 3.0 && ceiling.layer_height >= 1000L)
    {
        return (aw::convert(visibility_group.distance, visibility_group.unit, distance_unit::statute_miles) >= 5.0 && ceiling.layer_height >= 3000L) ? flight_category::vfr : flight_category::mvfr;
    }
    return (aw::convert(visibility_group.distance, visibility_group.unit, distance_unit::statute_miles) >= 1.0 && ceiling.layer_height >= 500L) ? flight_category::ifr : flight_category::lifr;
}

//-----------------------------------------------------------------------------

} // namespace metar
} // namespace aw
