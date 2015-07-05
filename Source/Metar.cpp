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

#include "stdafx.hpp"

#include "Metar.hpp"

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
    { MetarElement::ReportType,        "^(METAR|SPECI) " },
    { MetarElement::StationIdentifier, "(^| )([A-Z0-9]{4}) " },
    { MetarElement::ObservationTime,   " ([0-9]{2})([0-9]{2})([0-9]{2})Z " },
    { MetarElement::ReportModifier,    " (AUTO|COR) " },
    { MetarElement::Wind,              " ([0-9]{3}|VRB)([0-9]{2,3})(G([0-9]{2,3}))?(KT|MPS)( ([0-9]{3})V([0-9]{3}))? " },
    { MetarElement::Visibility,        " (CAVOK|((M?([12]?)[ ]?([0-9])/([0-9]))|([0-9]{1,5}))(SM)?) " },
    { MetarElement::RunwayVisualRange, " R[0-9]{2}[LRC]?/[0-9]{4}(V[0-9]{4})?FT " },
    { MetarElement::WeatherGroup,      " [+-]?[A-Z]{2}([A-Z]{2}){1,3} " },
    { MetarElement::SkyCondition,      " ((SKC|CLR)|(((VV)|([A-Z]{2,3}))([0-9]{3}|///))) " },
    { MetarElement::Temperature,       " (M)?[0-9]{2}/" },
    { MetarElement::Dewpoint,          "/(M)?[0-9]{2} " },
    { MetarElement::Altimeter,         " (Q|A)([0-9]{4}) " },
    { MetarElement::Remarks,           " RMK (PK WND [0-9]{5,6}/([0-9]{2}|[0-9]{4}))?.*$" }
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
    struct tm *now = localtime(&t);

    m_dayOfMonth   = now->tm_mon;
    m_hourOfDay    = now->tm_hour;
    m_minuteOfHour = now->tm_min;
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
        char designator,
        int visibility,
        int visibilityVariation)
    {
        m_unit = unit;
        m_runwayNumber = runwayNumber;
        m_designator = designator;
        m_visibility = visibility;
        m_visibilityVariation = visibilityVariation;
    }

    void SetUnit(std::string const& unit)                { m_unit = unit;                               }
    void SetRunwayNumber(unsigned int runwayNumber)      { m_runwayNumber = runwayNumber;               }
    void SetDesignator(char designator)                  { m_designator = designator;                   }
    void SetVisibility(int visibility)                   { m_visibility = visibility;                   }
    void SetVisibilityVariation(int visibilityVariation) { m_visibilityVariation = visibilityVariation; }
};

MetarRunwayVisualRange::MetarRunwayVisualRange() :
    m_unit("KT"),
    m_runwayNumber(0U),
    m_designator('C'),
    m_visibility(MAX_VISIBILITY),
    m_visibilityVariation(0)
{};

//-----------------------------------------------------------------------------

class MetarWeatherGroupImpl : public MetarWeatherGroup
{
public:
    MetarWeatherGroupImpl() :
        MetarWeatherGroup()
    {}

    MetarWeatherGroupImpl(
        WeatherIntensity intensity,
        WeatherDescriptor descriptor)
    {
        m_intensity = intensity;
        m_descriptor = descriptor;
    }

    void SetIntensity(WeatherIntensity intensity)    { m_intensity = intensity;      }
    void SetDescriptor(WeatherDescriptor descriptor) { m_descriptor = descriptor;    }
    void AddPhenomena(WeatherPhenomena phenomena)    { m_types.push_back(phenomena); }
};

MetarWeatherGroup::MetarWeatherGroup() :
    m_intensity(WeatherIntensity::None),
    m_descriptor(WeatherDescriptor::None)
{};

//-----------------------------------------------------------------------------

class MetarSkyConditionGroupImpl : public MetarSkyConditionGroup
{
public:
    MetarSkyConditionGroupImpl() :
        MetarSkyConditionGroup()
    {}

    MetarSkyConditionGroupImpl(
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

MetarSkyConditionGroup::MetarSkyConditionGroup() :
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
    m_temperature(0.0),
    m_dewpoint(0.0),
    m_altimeter(0.0),
    m_metar(metar)
{
    Parse();
}

//-----------------------------------------------------------------------------

template <typename TLambda>
void ParseIfMatch(std::string const& metar, MetarElement element, TLambda l)
{
    auto patternElement = g_patterns.find(element);
    if (patternElement == g_patterns.end())
    {
        return;
    }

    std::regex regexp(patternElement->second);
    std::cmatch result;
    if (std::regex_search(metar.c_str(), result, regexp))
    {
        l(result);
    }
}

//-----------------------------------------------------------------------------

void Metar::ParseReportType()
{
    ParseIfMatch(m_metar, MetarElement::ReportType, [this](std::cmatch regex)
    {
        static const unsigned int EXPR = 1;

        auto reportType = regex.str(EXPR);
        if (reportType == "METAR")
        {
            m_reportType = MetarReportType::Metar;
        }
        else if (reportType == "SPECI")
        {
            m_reportType = MetarReportType::Special;
        }
    });
}

//-----------------------------------------------------------------------------

void Metar::ParseStationIdentifier()
{
    ParseIfMatch(m_metar, MetarElement::StationIdentifier, [this](std::cmatch regex)
    {
        static const unsigned int IDENT = 2;

        m_stationIdentifier = regex.str(IDENT);
    });
}

//-----------------------------------------------------------------------------

void Metar::ParseObservationTime()
{
    ParseIfMatch(m_metar, MetarElement::ObservationTime, [this](std::cmatch regex)
    {
        static const unsigned int EXPR_DAY = 1;
        static const unsigned int EXPR_HOUR = 2;
        static const unsigned int EXPR_MINUTE = 3;

        try
        {
            auto day = stoi(regex.str(EXPR_DAY));
            auto hour = stoi(regex.str(EXPR_HOUR));
            auto minute = stoi(regex.str(EXPR_MINUTE));

            m_observationTime = std::make_shared<MetarObservationTimeImpl>(day, hour, minute);
        }
        catch (std::invalid_argument const&)
        {
            // TODO
        }
    });
}

//-----------------------------------------------------------------------------

void Metar::ParseModifier()
{
    ParseIfMatch(m_metar, MetarElement::ReportModifier, [this](std::cmatch regex)
    {
        static const unsigned int EXPR = 1;

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
}

//-----------------------------------------------------------------------------

void Metar::ParseWind()
{
    ParseIfMatch(m_metar, MetarElement::Wind, [this](std::cmatch regex)
    {
        static const unsigned int EXPR_DIRECTION = 1;
        static const unsigned int EXPR_SPEED = 2;
        static const unsigned int EXPR_GUST = 3;
        static const unsigned int EXPR_GUST_SPEED = 4;
        static const unsigned int EXPR_UNIT = 5;
        static const unsigned int EXPR_VAR = 6;
        static const unsigned int EXPR_VAR_LOWER = 7;
        static const unsigned int EXPR_VAR_UPPER = 8;

        try
        {
            auto direction = atoi(regex.str(EXPR_DIRECTION).c_str());
            auto speed = atoi(regex.str(EXPR_SPEED).c_str());
            auto gustSpeed = ((regex[EXPR_GUST].matched) ? (atoi(regex.str(EXPR_GUST_SPEED).c_str())) : 0);
            auto unit = regex[EXPR_UNIT];

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
            // TODO
        }
    });
}

//-----------------------------------------------------------------------------

void Metar::ParseVisibility()
{
    ParseIfMatch(m_metar, MetarElement::Visibility, [this](std::cmatch regex)
    {
        static const int EXPR = 1;
        static const int FRACTIONAL = 3;
        static const int FRAC_W = 4;
        static const int FRAC_N = 5;
        static const int FRAC_D = 6;
        static const int VISIBILITY = 7;
        static const int STATUTE = 8;
        
        if (regex.str(EXPR) == "CAVOK")
        {
            m_visibility = static_cast<double>(MAX_VISIBILITY);
        }
        else
        {
            if (regex[FRACTIONAL].matched)
            {
                m_visibility = (regex.length(FRAC_W) ? (atof(regex.str(FRAC_W).c_str())) : 0.0);
                m_visibility += atof(regex.str(FRAC_N).c_str()) / atof(regex.str(FRAC_D).c_str());
            }
            else
            {
                m_visibility = atof(regex.str(VISIBILITY).c_str());
            }
        
            if (regex[STATUTE].matched)
            {
                m_visibility *= 1609.344;
            }
        }
    });
}

//-----------------------------------------------------------------------------

void Metar::ParseAltimeter()
{
    ParseIfMatch(m_metar, MetarElement::Altimeter, [this](std::cmatch regex)
    {
        static const unsigned int EXPR_QNH = 1;
        static const unsigned int EXPR_ALT = 2;

        try
        {
            // This is most likely incorrect
            auto alt = atof(regex.str(EXPR_ALT).c_str());
            if (regex.str(EXPR_QNH) == "Q")
            {
                m_altimeter = alt;
            }
            else
            {
                m_altimeter = InhgToHpa(alt);
            }
            m_altimeter /= 100.0;
        }
        catch (std::invalid_argument const&)
        {
            // TODO
        }
    });
}

//-----------------------------------------------------------------------------

void Metar::Parse()
{
    ParseReportType();
    ParseStationIdentifier();
    ParseObservationTime();
    ParseModifier();
    ParseWind();
    ParseVisibility();
    ParseAltimeter();
}

//-----------------------------------------------------------------------------

} // namespace Meteorology
