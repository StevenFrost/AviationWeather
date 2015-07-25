/****************************** Module Header ******************************\
Module Name:  Metar.cpp
Project:      Meteorology
Copyright (c) 2015 Steven Frost.

This module implements the METAR class used to download and/or process METAR
data from reference (4).
This document currently complies with North-American standards ONLY and may
not successfully decode International METAR information.

References:  (1) http://www.ofcm.gov/fmh-1/pdf/L-CH12.pdf
             (2) http://en.wikipedia.org/wiki/METAR
             (3) http://www.herk-gouge.com/2006/11/understanding-runway-visual-range-rvr.html
             (4) http://www.aviationweather.gov/adds/dataserver

Todo:        (1) Implement structures for International standards

This source is subject to the MIT License.
See http://opensource.org/licenses/MIT

All other rights reserved.
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "MeteorologyPch.h"

#include <Meteorology/Metar.h>

#include <ctime>
#include <regex>
#include <string>
#include <map>

//-----------------------------------------------------------------------------

namespace Meteorology
{

//-----------------------------------------------------------------------------

static const int MAX_VISIBILITY = 9999;

const std::map<MetarElement, std::string> g_patterns =
{
    { MetarElement::ReportType,          "(METAR|SPECI) " },
    { MetarElement::StationIdentifier,   "([A-Z0-9]{4}) " },
    { MetarElement::ObservationTime,     "([0-9]{2})([0-9]{2})([0-9]{2})Z " },
    { MetarElement::ReportModifier,      "(AUTO|COR) " },
    { MetarElement::Wind,                "([0-9]{3}|VRB)([0-9]{2,3})(G([0-9]{2,3}))?(KT|MPS)( ([0-9]{3})V([0-9]{3}))? " },
    { MetarElement::Visibility,          "(CAVOK|((M?([12]?)[ ]?([0-9])/([0-9]))|([0-9]{1,5}))(SM)?) " },
    { MetarElement::RunwayVisualRange,   "R([0-9]{2})([LRC])?/([MP]?)([0-9]{4})(V([0-9]{4}))?FT " },
    { MetarElement::Weather,             "([+-]|VC)?((MI|PR|BC|DR|BL)?((DZ|RA|SN|SG|IC|PL|GR|GS|UP){1,3}|(BR|FG|FU|VA|DU|SA|HZ|PY|PO|SQ|FC|SS|DS)) |(SH)(RA|SN|PL|GS|GR){0,3} |(TS)(RA|SN|PL|GS|GR){0,3} |(FZ)(FG|DZ|RA){1,3} )" },
    { MetarElement::SkyCondition,        "((SKC|CLR) )|((VV|FEW|SCT|BKN|OVC)([0-9]{3}|///))(CB|TCU)? " },
    { MetarElement::TemperatureDewpoint, "(M)?([0-9]{2})/((M)?([0-9]{2}))? " },
    { MetarElement::Altimeter,           "(Q|A)([0-9]{4})( |$)" },
    { MetarElement::Remarks,             "RMK (.*)$" }
};

//-----------------------------------------------------------------------------

class MetarObservationTimeImpl : public MetarObservationTime
{
public:
    MetarObservationTimeImpl() :
        MetarObservationTime()
    {}

    MetarObservationTimeImpl(
        unsigned int dayOfMonth,
        unsigned int hourOfDay,
        unsigned int minuteOfHour)
    {
        m_dayOfMonth = dayOfMonth;
        m_hourOfDay = hourOfDay;
        m_minuteOfHour = minuteOfHour;
    }

    void SetDayOfMonth(unsigned int dayOfMonth)     { m_dayOfMonth = dayOfMonth;     }
    void SetHourOfDay(unsigned int hourOfDay)       { m_hourOfDay = hourOfDay;       }
    void SetMinuteOfHour(unsigned int minuteOfHour) { m_minuteOfHour = minuteOfHour; }
};

MetarObservationTime::MetarObservationTime()
{
    time_t t = time(0);
    struct tm now;
    auto result = localtime_s(&now, &t);

    if (result != EINVAL)
    {
        m_dayOfMonth = now.tm_mon;
        m_hourOfDay = now.tm_hour;
        m_minuteOfHour = now.tm_min;
    }
    else
    {
        m_dayOfMonth = 0U;
        m_hourOfDay = 0U;
        m_minuteOfHour = 0U;
    }
}

//-----------------------------------------------------------------------------

class MetarWindImpl : public MetarWind
{
public:
    MetarWindImpl() :
        MetarWind()
    {}

    MetarWindImpl(
        std::string unit,
        unsigned int direction,
        unsigned int speed,
        unsigned int gustSpeed,
        unsigned int variationLower,
        unsigned int variationUpper)
    {
        m_unit = unit;
        m_direction = direction;
        m_speed = speed;
        m_gustSpeed = gustSpeed;
        m_variationLower = variationLower;
        m_variationUpper = variationUpper;
    }

    void SetUnit(std::string const& unit)               { m_unit = unit;                     }
    void SetDirection(unsigned int direction)           { m_direction = direction;           }
    void SetSpeed(unsigned int speed)                   { m_speed = speed;                   }
    void SetGustSpeed(unsigned int gustSpeed)           { m_gustSpeed = gustSpeed;           }
    void SetVariationLower(unsigned int variationLower) { m_variationLower = variationLower; }
    void SetVariationUpper(unsigned int variationUpper) { m_variationUpper = variationUpper; }
};

MetarWind::MetarWind() :
    m_unit("KT"),
    m_direction(0U),
    m_speed(0U),
    m_gustSpeed(0U),
    m_variationLower(0U),
    m_variationUpper(0U)
{};

//-----------------------------------------------------------------------------

class MetarRunwayVisualRangeImpl : public MetarRunwayVisualRange
{
public:
    MetarRunwayVisualRangeImpl() :
        MetarRunwayVisualRange()
    {}

    MetarRunwayVisualRangeImpl(
        std::string unit,
        unsigned int runwayNumber,
        MetarRunwayDesignator designator,
        MetarVisibilityModifier visibilityModifier,
        int visibility,
        int visibilityVariation)
    {
        m_unit = unit;
        m_runwayNumber = runwayNumber;
        m_designator = designator;
        m_visibilityModifier = visibilityModifier;
        m_visibility = visibility;
        m_visibilityVariation = visibilityVariation;
    }

    void SetUnit(std::string const& unit)                                  { m_unit = unit;                               }
    void SetRunwayNumber(unsigned int runwayNumber)                        { m_runwayNumber = runwayNumber;               }
    void SetDesignator(MetarRunwayDesignator designator)                   { m_designator = designator;                   }
    void SetVisibilityModifier(MetarVisibilityModifier visibilityModifier) { m_visibilityModifier = visibilityModifier;   }
    void SetVisibility(int visibility)                                     { m_visibility = visibility;                   }
    void SetVisibilityVariation(int visibilityVariation)                   { m_visibilityVariation = visibilityVariation; }
};

MetarRunwayVisualRange::MetarRunwayVisualRange() :
    m_unit("FT"),
    m_runwayNumber(0U),
    m_designator(MetarRunwayDesignator::None),
    m_visibilityModifier(MetarVisibilityModifier::None),
    m_visibility(MAX_VISIBILITY),
    m_visibilityVariation(0)
{};

//-----------------------------------------------------------------------------

class MetarWeatherImpl : public MetarWeather
{
public:
    MetarWeatherImpl() :
        MetarWeather()
    {}

    MetarWeatherImpl(
        WeatherIntensity intensity,
        WeatherDescriptor descriptor,
        std::list<WeatherPhenomena>&& phenomenaList)
    {
        m_intensity = intensity;
        m_descriptor = descriptor;
        m_phenomenaList = std::move(phenomenaList);
    }

    void SetIntensity(WeatherIntensity intensity)    { m_intensity = intensity;              }
    void SetDescriptor(WeatherDescriptor descriptor) { m_descriptor = descriptor;            }
    void AddPhenomena(WeatherPhenomena phenomena)    { m_phenomenaList.push_back(phenomena); }
};

MetarWeather::MetarWeather() :
    m_intensity(WeatherIntensity::Moderate),
    m_descriptor(WeatherDescriptor::None)
{};

//-----------------------------------------------------------------------------

class MetarSkyConditionImpl : public MetarSkyCondition
{
public:
    MetarSkyConditionImpl() :
        MetarSkyCondition()
    {}

    MetarSkyConditionImpl(
        SkyCover skyCover,
        unsigned int height,
        SkyCoverClouds cloudType)
    {
        m_skyCover = skyCover;
        m_height = height;
        m_cloudType = cloudType;
    }

    void SetSkyCover(SkyCover skyCover)              { m_skyCover = skyCover;        }
    void SetHeight(unsigned int height)              { m_height = height;            }
    void SetCloudType(SkyCoverClouds skyCoverClouds) { m_cloudType = skyCoverClouds; }
};

MetarSkyCondition::MetarSkyCondition() :
    m_skyCover(SkyCover::Clear),
    m_height(0U),
    m_cloudType(SkyCoverClouds::None)
{};

//-----------------------------------------------------------------------------

Metar::Metar(std::string const& metar) :
    m_reportType(MetarReportType::Metar),
    m_stationIdentifier(""),
    m_observationTime(nullptr),
    m_modifier(MetarModifier::Automatic),
    m_wind(nullptr),
    m_visibility(MAX_VISIBILITY),
    m_temperature(0),
    m_dewpoint(0),
    m_altimeter(0.0),
    m_metar(metar)
{
    Parse();
}

//-----------------------------------------------------------------------------

WeatherIntensity ParseWeatherIntensity(std::string const& symbol)
{
    if (symbol == "-") {
        return WeatherIntensity::Light;
    }
    else if (symbol == "+") {
        return WeatherIntensity::Heavy;
    }
    else if (symbol == "VC") {
        return WeatherIntensity::InTheVicinity;
    }
    else {
        return WeatherIntensity::Moderate;
    }
}

//-----------------------------------------------------------------------------

WeatherDescriptor ParseWeatherDescriptor(std::string const& symbol)
{
    if (symbol == "MI") {
        return WeatherDescriptor::Shallow;
    }
    else if (symbol == "PR") {
        return WeatherDescriptor::Partial;
    }
    else if (symbol == "BC") {
        return WeatherDescriptor::Patches;
    }
    else if (symbol == "DR") {
        return WeatherDescriptor::LowDrifting;
    }
    else if (symbol == "BL") {
        return WeatherDescriptor::Blowing;
    }
    else if (symbol == "SH") {
        return WeatherDescriptor::Showers;
    }
    else if (symbol == "TS") {
        return WeatherDescriptor::Thunderstorm;
    }
    else if (symbol == "FZ") {
        return WeatherDescriptor::Freezing;
    }
    else {
        return WeatherDescriptor::None;
    }
}

//-----------------------------------------------------------------------------

WeatherPhenomena ParseWeatherPhenomena(std::string const& symbol)
{
    if (symbol == "DZ") {
        return WeatherPhenomena::Drizzle;
    }
    else if (symbol == "RA") {
        return WeatherPhenomena::Rain;
    }
    else if (symbol == "SN") {
        return WeatherPhenomena::Snow;
    }
    else if (symbol == "SG") {
        return WeatherPhenomena::SnowGrains;
    }
    else if (symbol == "IC") {
        return WeatherPhenomena::IceCrystals;
    }
    else if (symbol == "PL") {
        return WeatherPhenomena::IcePellets;
    }
    else if (symbol == "GR") {
        return WeatherPhenomena::Hail;
    }
    else if (symbol == "GS") {
        return WeatherPhenomena::SmallHail;
    }
    else if (symbol == "UP") {
        return WeatherPhenomena::SmallHail;
    }
    else if (symbol == "BR") {
        return WeatherPhenomena::Mist;
    }
    else if (symbol == "FG") {
        return WeatherPhenomena::Fog;
    }
    else if (symbol == "FU") {
        return WeatherPhenomena::Smoke;
    }
    else if (symbol == "VA") {
        return WeatherPhenomena::VolcanicAsh;
    }
    else if (symbol == "DU") {
        return WeatherPhenomena::WidespreadDust;
    }
    else if (symbol == "SA") {
        return WeatherPhenomena::Sand;
    }
    else if (symbol == "HZ") {
        return WeatherPhenomena::Haze;
    }
    else if (symbol == "PY") {
        return WeatherPhenomena::Spray;
    }
    else if (symbol == "PO") {
        return WeatherPhenomena::WellDevelopedDustWhirls;
    }
    else if (symbol == "SQ") {
        return WeatherPhenomena::Squalls;
    }
    else if (symbol == "FC") {
        return WeatherPhenomena::FunnelCloudTornadoWaterspout;
    }
    else if (symbol == "SS") {
        return WeatherPhenomena::Sandstorm;
    }
    else if (symbol == "DS") {
        return WeatherPhenomena::Duststorm;
    }
    else {
        return WeatherPhenomena::None;
    }
}

//-----------------------------------------------------------------------------

SkyCover ParseSkyCover(std::string const& symbol)
{
    if (symbol == "VV") {
        return SkyCover::VerticalVisibility;
    }
    else if (symbol == "FEW") {
        return SkyCover::Few;
    }
    else if (symbol == "SCT") {
        return SkyCover::Scattered;
    }
    else if (symbol == "BKN") {
        return SkyCover::Broken;
    }
    else if (symbol == "OVC") {
        return SkyCover::Overcast;
    }
    else {
        return SkyCover::Clear;
    }
}

//-----------------------------------------------------------------------------

SkyCoverClouds ParseSkyCoverClouds(std::string const& symbol)
{
    if (symbol == "CB") {
        return SkyCoverClouds::Cumulonimbus;
    }
    else if (symbol == "TCU") {
        return SkyCoverClouds::ToweringCumulus;
    }
    else {
        return SkyCoverClouds::None;
    }
}

//-----------------------------------------------------------------------------

template <typename TLambda>
std::string ParseIfMatch(std::string const& metar, MetarElement element, TLambda l, bool reverse = false)
{
    auto patternElement = g_patterns.find(element);
    if (patternElement == g_patterns.end())
    {
        return metar;
    }

    auto metarString = metar;
    auto pattern = patternElement->second;

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
std::string ParseForEachMatch(std::string const& metar, MetarElement element, TLambda l, bool reverse = false)
{
    auto patternElement = g_patterns.find(element);
    if (patternElement == g_patterns.end())
    {
        return metar;
    }

    auto metarString = metar;
    auto pattern = patternElement->second;

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

void Metar::ParseReportType(std::string& metar)
{
    auto result = ParseIfMatch(metar, MetarElement::ReportType, [this](std::cmatch regex)
    {
        static const unsigned short EXPR_TYPE = 1;

        auto reportType = regex.str(EXPR_TYPE);
        if (reportType == "METAR")
        {
            m_reportType = MetarReportType::Metar;
        }
        else if (reportType == "SPECI")
        {
            m_reportType = MetarReportType::Special;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseStationIdentifier(std::string& metar)
{
    auto result = ParseIfMatch(metar, MetarElement::StationIdentifier, [this](std::cmatch regex)
    {
        static const unsigned short EXPR_IDENT = 1;

        m_stationIdentifier = regex.str(EXPR_IDENT);
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseObservationTime(std::string& metar)
{
    auto result = ParseIfMatch(metar, MetarElement::ObservationTime, [this](std::cmatch regex)
    {
        static const unsigned short EXPR_DAY = 1;
        static const unsigned short EXPR_HOUR = 2;
        static const unsigned short EXPR_MINUTE = 3;

        try
        {
            auto day = stoi(regex.str(EXPR_DAY));
            auto hour = stoi(regex.str(EXPR_HOUR));
            auto minute = stoi(regex.str(EXPR_MINUTE));

            m_observationTime = std::make_shared<MetarObservationTimeImpl>(day, hour, minute);
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseModifier(std::string& metar)
{
    auto result = ParseIfMatch(metar, MetarElement::ReportModifier, [this](std::cmatch regex)
    {
        static const unsigned short EXPR = 1;

        auto modifier = regex.str(EXPR);
        if (modifier == "AUTO")
        {
            m_modifier = MetarModifier::Automatic;
        }
        else if (modifier == "COR")
        {
            m_modifier = MetarModifier::Corrected;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseWind(std::string& metar)
{
    auto result = ParseIfMatch(metar, MetarElement::Wind, [this](std::cmatch regex)
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
            auto unit = regex[EXPR_UNIT];
            auto direction = atoi(regex.str(EXPR_DIRECTION).c_str());
            auto speed = atoi(regex.str(EXPR_SPEED).c_str());

            unsigned int gustSpeed = 0U;
            if (regex[EXPR_GUST].matched)
            {
                gustSpeed = atoi(regex.str(EXPR_GUST_SPEED).c_str());
            }

            unsigned int variationLower = 0U;
            unsigned int variationUpper = 0U;
            if (regex[EXPR_VAR].matched)
            {
                variationLower = atoi(regex.str(EXPR_VAR_LOWER).c_str());
                variationUpper = atoi(regex.str(EXPR_VAR_UPPER).c_str());
            }

            m_wind = std::make_shared<MetarWindImpl>(
                unit, direction, speed, gustSpeed, variationLower, variationUpper
            );
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseVisibility(std::string& metar)
{
    auto result = ParseIfMatch(metar, MetarElement::Visibility, [this](std::cmatch regex)
    {
        static const unsigned short EXPR_ALL = 1;
        static const unsigned short EXPR_FRACTIONAL = 3;
        static const unsigned short EXPR_FRAC_W = 4;
        static const unsigned short EXPR_FRAC_N = 5;
        static const unsigned short EXPR_FRAC_D = 6;
        static const unsigned short EXPR_VISIBILITY = 7;
        static const unsigned short EXPR_STATUTE = 8;

        if (regex.str(EXPR_ALL) == "CAVOK")
        {
            m_visibility = static_cast<double>(MAX_VISIBILITY);
            return;
        }

        try
        {
            double visibility = 0.0;
            if (regex[EXPR_FRACTIONAL].matched)
            {
                double fracN = atof(regex.str(EXPR_FRAC_N).c_str());
                double fracD = atof(regex.str(EXPR_FRAC_D).c_str());

                if (regex[EXPR_FRAC_W].matched)
                {
                    visibility = atof(regex.str(EXPR_FRAC_W).c_str());
                }
                visibility += fracN / fracD;
            }
            else
            {
                visibility = atof(regex.str(EXPR_VISIBILITY).c_str());
            }

            if (regex[EXPR_STATUTE].matched)
            {
                visibility = MilesToFeet(visibility);
            }
            m_visibility = visibility;
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseRunwayVisualRange(std::string& metar)
{
    auto result = ParseForEachMatch(metar, MetarElement::RunwayVisualRange, [this](std::cmatch regex)
    {
        static const unsigned short EXPR_RUNWAY_NUM = 1;
        static const unsigned short EXPR_RUNWAY_DESIGNATOR = 2;
        static const unsigned short EXPR_VISIBILITY_MOD = 3;
        static const unsigned short EXPR_VISIBILITY_MIN = 4;
        static const unsigned short EXPR_VARIABLE = 5;
        static const unsigned short EXPR_VISIBILITY_MAX = 6;

        try
        {
            auto runwayNumber = atoi(regex.str(EXPR_RUNWAY_NUM).c_str());
            auto runwayDesignator = MetarRunwayDesignator::None;
            auto visibilityModifier = MetarVisibilityModifier::None;
            auto visibilityMin = atoi(regex.str(EXPR_VISIBILITY_MIN).c_str());
            auto visibilityMax = visibilityMin;

            // Runway designator
            auto runwayDesignatorStr = regex.str(EXPR_RUNWAY_DESIGNATOR);
            if (runwayDesignatorStr == "L")
            {
                runwayDesignator = MetarRunwayDesignator::Left;
            }
            else if (runwayDesignatorStr == "R")
            {
                runwayDesignator = MetarRunwayDesignator::Right;
            }
            else if (runwayDesignatorStr == "C")
            {
                runwayDesignator = MetarRunwayDesignator::Center;
            }

            // Visibility modifier
            if (regex[EXPR_VISIBILITY_MOD].matched)
            {
                auto modifier = regex.str(EXPR_VISIBILITY_MOD);
                if (modifier == "M")
                {
                    visibilityModifier = MetarVisibilityModifier::LessThan;
                }
                else if (modifier == "P")
                {
                    visibilityModifier = MetarVisibilityModifier::GreaterThan;
                }
            }

            // Variable visibility
            if (regex[EXPR_VARIABLE].matched)
            {
                visibilityMax = atoi(regex.str(EXPR_VISIBILITY_MAX).c_str());
            }

            m_runwayVisualRangeList.push_back(std::make_shared<MetarRunwayVisualRangeImpl>(
                "FT", runwayNumber, runwayDesignator, visibilityModifier, visibilityMin, visibilityMax
            ));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseWeather(std::string& metar)
{
    auto result = ParseForEachMatch(metar, MetarElement::Weather, [this](std::cmatch regex)
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

        auto intensity = WeatherIntensity::Moderate;
        auto descriptor = WeatherDescriptor::None;
        auto phenomenaList = std::list<WeatherPhenomena>();

        // Intensity
        if (regex[EXPR_INTENSITY].matched)
        {
            intensity = ParseWeatherIntensity(regex.str(EXPR_INTENSITY));
        }

        // Descriptor
        if (regex[EXPR_DESCRIPTOR_ALL].matched)
        {
            descriptor = ParseWeatherDescriptor(regex.str(EXPR_DESCRIPTOR_ALL));
        }
        else if (regex[EXPR_DESCRIPTOR_SH].matched)
        {
            descriptor = ParseWeatherDescriptor(regex.str(EXPR_DESCRIPTOR_SH));
        }
        else if (regex[EXPR_DESCRIPTOR_TS].matched)
        {
            descriptor = ParseWeatherDescriptor(regex.str(EXPR_DESCRIPTOR_TS));
        }
        else if (regex[EXPR_DESCRIPTOR_FZ].matched)
        {
            descriptor = ParseWeatherDescriptor(regex.str(EXPR_DESCRIPTOR_FZ));
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
        
        auto phenomenaGroup = regex.str(matchedPhenomena);
        if (matchedPhenomena != 0 && phenomenaGroup.size() % 2 == 0)
        {
            auto it = phenomenaGroup.begin();
            while (it != phenomenaGroup.end())
            {
                std::string phenomena("");
                phenomena += *it++;
                phenomena += *it++;

                phenomenaList.push_back(ParseWeatherPhenomena(phenomena));
            }
        }

        m_weatherList.push_back(std::make_shared<MetarWeatherImpl>(
            intensity, descriptor, std::move(phenomenaList)
        ));
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseSkyCondition(std::string& metar)
{
    auto result = ParseForEachMatch(metar, MetarElement::SkyCondition, [this](std::cmatch regex)
    {
        static const unsigned short EXPR_CLEAR = 1;
        static const unsigned short EXPR_LAYER = 4;
        static const unsigned short EXPR_LAYER_ALTITUDE = 5;
        static const unsigned short EXPR_MANUAL = 6;

        try
        {
            auto skyCover = SkyCover::Clear;
            auto altitude = 0U;
            auto cloudType = SkyCoverClouds::None;

            if (!regex[EXPR_CLEAR].matched)
            {
                skyCover = ParseSkyCover(regex.str(EXPR_LAYER).c_str());
                
                auto altitudeStr = regex.str(EXPR_LAYER_ALTITUDE);
                if (altitudeStr != "///")
                {
                    altitude = atoi(altitudeStr.c_str());
                }
                altitude *= 100;

                if (regex[EXPR_MANUAL].matched)
                {
                    cloudType = ParseSkyCoverClouds(regex.str(EXPR_MANUAL).c_str());
                }
            }

            m_skyConditionList.push_back(std::make_shared<MetarSkyConditionImpl>(
                skyCover, altitude, cloudType
            ));
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseTemperatureDewPoint(std::string& metar)
{
    auto result = ParseIfMatch(metar, MetarElement::TemperatureDewpoint, [this](std::cmatch regex)
    {
        static const unsigned short EXPR_TEMP_IS_MINUS = 1;
        static const unsigned short EXPR_TEMPERATURE = 2;
        static const unsigned short EXPR_DEW_IS_MINUS = 4;
        static const unsigned short EXPR_DEWPOINT = 5;

        auto temperatureStr = regex.str(EXPR_TEMPERATURE);
        auto dewpointStr = regex.str(EXPR_DEWPOINT);

        auto temperature = atoi(temperatureStr.c_str());
        auto dewpoint = atoi(dewpointStr.c_str());

        if (regex[EXPR_TEMP_IS_MINUS].matched)
        {
            temperature = -temperature;
        }
        if (regex[EXPR_DEW_IS_MINUS].matched)
        {
            dewpoint = -dewpoint;
        }

        m_temperature = temperature;
        m_dewpoint = dewpoint;
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseAltimeter(std::string& metar)
{
    auto result = ParseIfMatch(metar, MetarElement::Altimeter, [this](std::cmatch regex)
    {
        static const unsigned short EXPR_SETTING = 1;
        static const unsigned short EXPR_ALT = 2;

        try
        {
            auto altimeter = atof(regex.str(EXPR_ALT).c_str()) / 100.0;
            if (regex.str(EXPR_SETTING) == "Q")
            {
                altimeter = HpaToInhg(altimeter);
            }

            m_altimeter = altimeter;
        }
        catch (std::invalid_argument const&)
        {
            return;
        }
    });
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::ParseRemarks(std::string& metar)
{
    auto result = ParseIfMatch(metar, MetarElement::Remarks, [this](std::cmatch regex)
    {
        static const unsigned short EXPR_ALL = 1;
        m_remarks = regex.str(EXPR_ALL);
    }, true);
    metar = result;
}

//-----------------------------------------------------------------------------

void Metar::Parse()
{
    std::string metar = m_metar;

    // We parse remarks first to avoid over-matching in earlier groups
    ParseRemarks(metar);

    ParseReportType(metar);
    ParseStationIdentifier(metar);
    ParseObservationTime(metar);
    ParseModifier(metar);
    ParseWind(metar);
    ParseVisibility(metar);
    ParseRunwayVisualRange(metar);
    ParseWeather(metar);
    ParseSkyCondition(metar);
    ParseTemperatureDewPoint(metar);
    ParseAltimeter(metar);
}

//-----------------------------------------------------------------------------

} // namespace Meteorology
