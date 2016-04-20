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

#include <memory>
#include <string>

#include <AviationWeather/optional.h>
#include <AviationWeather/types.h>

namespace aw
{

//-----------------------------------------------------------------------------

typedef std::string station_identifier;

//-----------------------------------------------------------------------------

class time
{
public:
    typedef std::shared_ptr<time> pointer;
    typedef std::unique_ptr<time> unique_pointer;

    time();
    time(uint8_t day, uint8_t hour, uint8_t minute);
    time(time_t time);

    time(time const& other) = default;
    time(time && other);

    time& operator= (time const& rhs) = default;
    time& operator= (time && rhs);

    bool operator== (time const& rhs) const;
    bool operator!= (time const& rhs) const;

public:
    uint8_t day_of_month;   // Day of month (1 - 31)
    uint8_t hour_of_day;    // Hour of day (0 - 23)
    uint8_t minute_of_hour; // Minute of hour (0 - 59)
};

//-----------------------------------------------------------------------------

class wind
{
public:
    typedef std::shared_ptr<wind> pointer;
    typedef std::unique_ptr<wind> unique_pointer;

    wind();

    wind(wind const& other) = default;
    wind(wind && other);

    wind& operator= (wind const& rhs) = default;
    wind& operator= (wind && rhs);

    bool operator== (wind const& rhs) const;
    bool operator!= (wind const& rhs) const;

    bool is_variable() const;

    uint8_t gust_factor() const;

    double headwind_component(double heading, bool useGusts = false) const;
    double crosswind_component(double heading, bool useGusts = false) const;

public:
    speed_unit               unit;            // Unit of speed for wind_speed and gust_speed
    uint16_t                 direction;       // Primary wind direction. Variable wind is encoded as UINT16_MAX
    uint8_t                  wind_speed;      // Primary wind speed in speed_units
    uint8_t                  gust_speed;      // Gust speed in speed_units
    util::optional<uint16_t> variation_lower; // Lower wind direction
    util::optional<uint16_t> variation_upper; // Upper wind direction
};

//-----------------------------------------------------------------------------

class visibility
{
public:
    typedef std::shared_ptr<visibility> pointer;
    typedef std::unique_ptr<visibility> unique_pointer;

    visibility();
    visibility(double distance, distance_unit unit, visibility_modifier_type modifier = visibility_modifier_type::none);

    visibility(visibility const& other) = default;
    visibility(visibility && other);

    visibility& operator=(visibility const& rhs) = default;
    visibility& operator=(visibility && rhs);

    bool operator== (visibility const& rhs) const;
    bool operator!= (visibility const& rhs) const;

    bool operator<= (visibility const& rhs) const;
    bool operator>= (visibility const& rhs) const;

    bool operator< (visibility const& rhs) const;
    bool operator> (visibility const& rhs) const;

public:
    distance_unit            unit;     // Unit of distance
    double                   distance; // Visibility distance in distance_units
    visibility_modifier_type modifier; // Visibility modifier (only none, less_than are valid)
};

//-----------------------------------------------------------------------------

class weather
{
public:
    typedef std::shared_ptr<weather> pointer;
    typedef std::unique_ptr<weather> unique_pointer;

    weather();

    weather(weather const& other) = default;
    weather(weather && other);

    weather& operator= (weather const& rhs) = default;
    weather& operator= (weather && rhs);

    bool operator== (weather const& rhs) const;
    bool operator!= (weather const& rhs) const;

public:
    weather_intensity              intensity;   // Weather intensity (moderate, heavy, etc.)
    weather_descriptor             descriptor;  // Weather desciptor
    std::vector<weather_phenomena> phenomena;   // List of present weather phenomena
};

//-----------------------------------------------------------------------------

class cloud_layer
{
public:
    typedef std::shared_ptr<cloud_layer> pointer;
    typedef std::unique_ptr<cloud_layer> unique_pointer;

    cloud_layer();

    cloud_layer(cloud_layer const& other) = default;
    cloud_layer(cloud_layer && other);

    cloud_layer& operator= (cloud_layer const& rhs) = default;
    cloud_layer& operator= (cloud_layer && rhs);

    bool operator== (cloud_layer const& rhs) const;
    bool operator!= (cloud_layer const& rhs) const;

    bool is_unlimited() const;

public:
    distance_unit        unit;          // Unit of layer_height
    sky_cover_type       sky_cover;     // Sky cloud coverage
    uint32_t             layer_height;  // Cloud layer height above ground level
    sky_cover_cloud_type cloud_type;    // Layer cloud type
};

//-----------------------------------------------------------------------------

} // namespace aw
