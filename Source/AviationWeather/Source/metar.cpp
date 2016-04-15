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

#include <AviationWeather/metar.h>

#include <algorithm>
#include <ctime>
#include <regex>
#include <string>
#include <map>

#include <AviationWeather/converters.h>
#include <AviationWeather/optional.h>

#include "parsers.h"
#include "utility.h"

//-----------------------------------------------------------------------------

namespace aw
{

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
    return comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
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
    return comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
        [](double && l, double && r)
    {
        return l <= r;
    });
}

bool altimeter::operator>=(altimeter const& rhs) const
{
    return comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
        [](double && l, double && r)
    {
        return l >= r;
    });
}

bool altimeter::operator<(altimeter const& rhs) const
{
    return comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
        [](double && l, double && r)
    {
        return l < r;
    });
}

bool altimeter::operator>(altimeter const& rhs) const
{
    return comparison_conversion_helper<double>(pressure_unit::hPa, unit, rhs.unit, pressure, rhs.pressure,
        [](double && l, double && r)
    {
        return l > r;
    });
}

//-----------------------------------------------------------------------------

runway_visual_range::runway_visual_range() :
    runway_number(0U),
    runway_designator(runway_designator_type::none)
{}

runway_visual_range::runway_visual_range(runway_visual_range && other) :
    runway_number(0U),
    runway_designator(runway_designator_type::none)
{
    *this = std::move(other);
}

runway_visual_range& runway_visual_range::operator=(runway_visual_range && rhs)
{
    if (this != &rhs)
    {
        runway_number = rhs.runway_number;
        runway_designator = rhs.runway_designator;
        visibility_min = std::move(rhs.visibility_min);
        visibility_max = std::move(rhs.visibility_max);

        rhs.runway_number = 0U;
        rhs.runway_designator = runway_designator_type::none;
        rhs.visibility_min = visibility();
        rhs.visibility_max = visibility();
    }
    return *this;
}

bool runway_visual_range::operator== (runway_visual_range const& rhs) const
{
    return (runway_number == rhs.runway_number) &&
        (runway_designator == rhs.runway_designator) &&
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

metar::metar(std::string const& metar) :
    raw_data(metar),
    type(metar_report_type::metar),
    identifier(""),
    modifier(metar_modifier_type::none),
    temperature(util::nullopt),
    dewpoint(util::nullopt),
    remarks("")
{
    parse();
}

metar::metar(metar && other) :
    raw_data(""),
    type(metar_report_type::metar),
    identifier(""),
    modifier(metar_modifier_type::none),
    temperature(util::nullopt),
    dewpoint(util::nullopt),
    remarks("")
{
    *this = std::move(other);
}

metar& metar::operator=(metar && rhs)
{
    if (this != &rhs)
    {
        raw_data = std::move(rhs.raw_data);
        type = rhs.type;
        identifier = std::move(rhs.identifier);
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

        rhs.raw_data = "";
        rhs.type = metar_report_type::metar;
        rhs.identifier = "";
        rhs.report_time = time();
        rhs.modifier = metar_modifier_type::none;
        rhs.wind_group = util::nullopt;
        rhs.visibility_group = util::nullopt;
        rhs.runway_visual_range_group.clear();
        rhs.weather_group.clear();
        rhs.sky_condition_group.clear();
        rhs.temperature = util::nullopt;
        rhs.dewpoint = util::nullopt;
        rhs.altimeter_group = util::nullopt;
        rhs.remarks = "";
    }
    return *this;
}

bool metar::operator== (metar const& rhs) const
{
    return (type == rhs.type) &&
        (identifier == rhs.identifier) &&
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

bool metar::operator!= (metar const& rhs) const
{
    return !(*this == rhs);
}

void metar::parse()
{
    std::string baseMetar = raw_data;

    // We parse remarks first to avoid over-matching in earlier groups
    parse_remarks(*this, baseMetar);

    parse_report_type(*this, baseMetar);
    parse_station_identifier(*this, baseMetar);
    parse_observation_time(*this, baseMetar);
    parse_modifier(*this, baseMetar);
    parse_wind(*this, baseMetar);
    parse_visibility(*this, baseMetar);
    parse_runway_visual_range(*this, baseMetar);
    parse_weather(*this, baseMetar);
    parse_sky_condition(*this, baseMetar);
    parse_temperature_dewpoint(*this, baseMetar);
    parse_altimeter(*this, baseMetar);
}

cloud_layer metar::ceiling_nothrow() const
{
    auto result = std::find_if(sky_condition_group.begin(), sky_condition_group.end(), [](cloud_layer layer)
    {
        return layer.sky_cover == sky_cover_type::broken ||
            layer.sky_cover == sky_cover_type::overcast ||
            layer.sky_cover == sky_cover_type::vertical_visibility ||
            layer.sky_cover == sky_cover_type::sky_clear ||
            layer.sky_cover == sky_cover_type::clear_below_12000;
    });
    return result != sky_condition_group.end() ? *result : cloud_layer();
}

cloud_layer metar::ceiling() const
{
    if (sky_condition_group.empty())
    {
        throw aw_exception("Sky condition missing");
    }
    return ceiling_nothrow();
}

flight_category metar::flight_category() const
{
    if (!visibility_group || sky_condition_group.empty())
    {
        return flight_category::unknown;
    }

    cloud_layer ceiling = ceiling_nothrow();

    auto distanceSM = aw::convert(visibility_group->distance, visibility_group->unit, distance_unit::statute_miles);

    if (distanceSM >= 3.0 && ceiling.layer_height >= 1000L)
    {
        return (distanceSM > 5.0 && ceiling.layer_height > 3000L) ? flight_category::vfr : flight_category::mvfr;
    }
    return (distanceSM >= 1.0 && ceiling.layer_height >= 500L) ? flight_category::ifr : flight_category::lifr;
}

int16_t metar::temperature_dewpoint_spread() const
{
    if (!temperature || !dewpoint)
    {
        throw aw_exception("Missing temperature or dewpoint");
    }
    return *temperature - *dewpoint;
}

//-----------------------------------------------------------------------------

} // namespace aw
