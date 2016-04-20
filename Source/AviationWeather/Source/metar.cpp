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

//-----------------------------------------------------------------------------

namespace aw
{
namespace metar
{

//-----------------------------------------------------------------------------

template <class TExpected = double, class TEnum, class TUnit, class TVal, class TLambda>
bool comparison_conversion_helper(TEnum to, TUnit leftUnit, TUnit rightUnit, TVal const& leftVal, TVal const& rightVal, TLambda && l)
{
    auto lhs = leftUnit == to ? static_cast<TExpected>(leftVal) : convert<TExpected>(leftVal, leftUnit, to);
    auto rhs = rightUnit == to ? static_cast<TExpected>(rightVal) : convert<TExpected>(rightVal, rightUnit, to);

    return l(std::forward<TExpected>(lhs), std::forward<TExpected>(rhs));
}

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
    auto distanceEqual = comparison_conversion_helper<double>(distance_unit::feet, unit, rhs.unit, distance, rhs.distance,
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

observation_time::observation_time() :
    observation_time(time(NULL))
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
        day_of_month = static_cast<uint8_t>(t.tm_mday);
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
    variation_lower(util::nullopt),
    variation_upper(util::nullopt)
{}

wind::wind(wind && other) :
    unit(speed_unit::kt),
    direction(UINT16_MAX),
    wind_speed(0),
    gust_speed(0),
    variation_lower(util::nullopt),
    variation_upper(util::nullopt)
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
        variation_lower = std::move(rhs.variation_lower);
        variation_upper = std::move(rhs.variation_upper);

        rhs.unit = speed_unit::kt;
        rhs.direction = UINT16_MAX;
        rhs.wind_speed = 0;
        rhs.gust_speed = 0;
        rhs.variation_lower = util::nullopt;
        rhs.variation_upper = util::nullopt;
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
    return (direction == UINT16_MAX) || (variation_lower && variation_upper);
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
    sky_cover(sky_cover_type::sky_clear),
    layer_height(UINT32_MAX),
    cloud_type(sky_cover_cloud_type::unspecified)
{}

cloud_layer::cloud_layer(cloud_layer && other) :
    unit(distance_unit::feet),
    sky_cover(sky_cover_type::sky_clear),
    layer_height(UINT32_MAX),
    cloud_type(sky_cover_cloud_type::unspecified)
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
        rhs.sky_cover = sky_cover_type::sky_clear;
        rhs.layer_height = UINT32_MAX;
        rhs.cloud_type = sky_cover_cloud_type::unspecified;
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
    modifier(modifier_type::none),
    temperature(util::nullopt),
    dewpoint(util::nullopt),
    remarks("")
{
    parse();
}

metar_info::metar_info(metar_info && other) :
    metar(""),
    type(report_type::metar),
    station_identifier(""),
    modifier(modifier_type::none),
    temperature(util::nullopt),
    dewpoint(util::nullopt),
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
        rhs.modifier = modifier_type::none;
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
    parsers::parse_remarks(*this, baseMetar);

    parsers::parse_report_type(*this, baseMetar);
    parsers::parse_station_identifier(*this, baseMetar);
    parsers::parse_observation_time(*this, baseMetar);
    parsers::parse_modifier(*this, baseMetar);
    parsers::parse_wind(*this, baseMetar);
    parsers::parse_visibility(*this, baseMetar);
    parsers::parse_runway_visual_range(*this, baseMetar);
    parsers::parse_weather(*this, baseMetar);
    parsers::parse_sky_condition(*this, baseMetar);
    parsers::parse_temperature_dewpoint(*this, baseMetar);
    parsers::parse_altimeter(*this, baseMetar);
}

cloud_layer metar_info::ceiling_nothrow() const
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

cloud_layer metar_info::ceiling() const
{
    if (sky_condition_group.empty())
    {
        throw aw_exception("Sky condition missing");
    }
    return ceiling_nothrow();
}

flight_category metar_info::flight_category() const
{
    if (!visibility_group || sky_condition_group.empty())
    {
        return flight_category::unknown;
    }

    cloud_layer ceiling = metar_info::ceiling_nothrow();

    auto distanceSM = aw::convert(visibility_group->distance, visibility_group->unit, distance_unit::statute_miles);

    if (distanceSM >= 3.0 && ceiling.layer_height >= 1000L)
    {
        return (distanceSM > 5.0 && ceiling.layer_height > 3000L) ? flight_category::vfr : flight_category::mvfr;
    }
    return (distanceSM >= 1.0 && ceiling.layer_height >= 500L) ? flight_category::ifr : flight_category::lifr;
}

int16_t metar_info::temperature_dewpoint_spread() const
{
    if (!temperature || !dewpoint)
    {
        throw aw_exception("Missing temperature or dewpoint");
    }
    return *temperature - *dewpoint;
}

//-----------------------------------------------------------------------------

} // namespace metar
} // namespace aw
