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

#include <AviationWeather/components.h>
#include <AviationWeather/converters.h>

#include "utility.h"

namespace aw
{

//-----------------------------------------------------------------------------

time::time() :
    time(time(NULL))
{}

time::time(uint8_t day, uint8_t hour, uint8_t minute) :
    day_of_month(day),
    hour_of_day(hour),
    minute_of_hour(minute)
{}

time::time(time_t time) :
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

time::time(time && other) :
    day_of_month(1),
    hour_of_day(0),
    minute_of_hour(0)
{
    *this = std::move(other);
}

time& time::operator=(time && rhs)
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

bool time::operator== (time const& rhs) const
{
    return (day_of_month == rhs.day_of_month) &&
        (hour_of_day == rhs.hour_of_day) &&
        (minute_of_hour == rhs.minute_of_hour);
}

bool time::operator!= (time const& rhs) const
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

} // namespace aw
