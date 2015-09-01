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
#include <vector>

#include <AviationWeather/types.h>

//-----------------------------------------------------------------------------

namespace aw
{
namespace metar
{

//-----------------------------------------------------------------------------

enum class element_type
{
    report_type,
    station_identifier,
    observation_time,
    report_modifier,
    wind,
    visibility,
    runway_visual_range,
    weather,
    sky_condition,
    temperature_dewpoint,
    altimeter,
    remarks
};

enum class report_type
{
    metar,
    special
};

enum class modifier_type
{
    automatic,
    corrected
};

enum class runway_designator_type
{
    none,
    left,
    right,
    center
};

enum class visibility_modifier_type
{
    none,
    less_than,
    greater_than
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

class altimeter
{
public:
    typedef std::shared_ptr<altimeter> pointer;
    typedef std::unique_ptr<altimeter> unique_pointer;

    altimeter();
    altimeter(double pressure, pressure_unit unit);

    altimeter(altimeter const& other) = default;
    altimeter(altimeter && other);

    altimeter& operator= (altimeter const& rhs) = default;
    altimeter& operator= (altimeter && rhs);

    bool operator== (altimeter const& rhs) const;
    bool operator!= (altimeter const& rhs) const;

    bool operator<= (altimeter const& rhs) const;
    bool operator>= (altimeter const& rhs) const;

    bool operator< (altimeter const& rhs) const;
    bool operator> (altimeter const& rhs) const;

public:
    pressure_unit unit;     // Unit of pressure
    double        pressure; // Pressure in pressure_units
};

//-----------------------------------------------------------------------------

class observation_time
{
public:
    typedef std::shared_ptr<observation_time> pointer;
    typedef std::unique_ptr<observation_time> unique_pointer;

    observation_time();
    observation_time(uint8_t day, uint8_t hour, uint8_t minute);
    observation_time(time_t time);

    observation_time(observation_time const& other) = default;
    observation_time(observation_time && other);

    observation_time& operator= (observation_time const& rhs) = default;
    observation_time& operator= (observation_time && rhs);

    bool operator== (observation_time const& rhs) const;
    bool operator!= (observation_time const& rhs) const;

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
    speed_unit unit;            // Unit of speed for wind_speed and gust_speed
    uint16_t   direction;       // Primary wind direction
    uint8_t    wind_speed;      // Primary wind speed in speed_units
    uint8_t    gust_speed;      // Gust speed in speed_units
    uint16_t   variation_lower; // Lower wind direction
    uint16_t   variation_upper; // Upper wind direction
};

//-----------------------------------------------------------------------------

class runway_visual_range
{
public:
    typedef std::shared_ptr<runway_visual_range> pointer;
    typedef std::unique_ptr<runway_visual_range> unique_pointer;

    runway_visual_range();

    runway_visual_range(runway_visual_range const& other) = default;
    runway_visual_range(runway_visual_range && other);

    runway_visual_range& operator=(runway_visual_range const& rhs) = default;
    runway_visual_range& operator=(runway_visual_range && rhs);

    bool operator== (runway_visual_range const& rhs) const;
    bool operator!= (runway_visual_range const& rhs) const;

    bool is_variable() const;

public:
    distance_unit            unit;                  // Unit of distance for visibility_min and visibility_max
    uint8_t                  runway_number;         // Runway number
    runway_designator_type   runway_designator;     // Runway designator (L, R, C)
    visibility_modifier_type visibility_modifier;   // Modifier (less than, greater than, etc.)
    uint16_t                 visibility_min;        // Minimum visibility
    uint16_t                 visibility_max;        // Maximum visibility
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

class metar_info
{
public:
    typedef std::shared_ptr<metar_info> pointer;
    typedef std::unique_ptr<metar_info> unique_pointer;

    metar_info(std::string const& metar);

    metar_info(metar_info const& other) = default;
    metar_info(metar_info && other);

    metar_info& operator= (metar_info const& rhs) = default;
    metar_info& operator= (metar_info && rhs);

    bool operator== (metar_info const& rhs) const;
    bool operator!= (metar_info const& rhs) const;

    cloud_layer ceiling() const;

    flight_category flight_category() const;

private:
    void parse();

public:
    std::string                      metar;
    report_type                      type;
    std::string                      station_identifier;
    observation_time                 report_time;
    modifier_type                    modifier;
    wind                             wind_group;
    visibility                       visibility_group;
    std::vector<runway_visual_range> runway_visual_range_group;
    std::vector<weather>             weather_group;
    std::vector<cloud_layer>         sky_condition_group;
    int8_t                           temperature;
    int8_t                           dewpoint;
    altimeter                        altimeter_group;
    std::string                      remarks;
};

//-----------------------------------------------------------------------------

} // namespace metar
} // namespace aw
