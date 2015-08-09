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
    visibility(uint32_t distance, distance_unit unit = distance_unit::ft);

    visibility(visibility const& other) = default;
#if (_MSC_VER >= 1900)
    visibility(visibility && other) = default;
#endif // _MSC_VER >= 1900

    visibility& operator=(visibility const& rhs) = default;
#if (_MSC_VER >= 1900)
    visibility& operator=(visibility && rhs) = default;
#endif // _MSC_VER >= 1900

    bool operator== (visibility const& rhs) const;
    bool operator!= (visibility const& rhs) const;

    bool operator<= (visibility const& rhs) const;
    bool operator>= (visibility const& rhs) const;

    bool operator< (visibility const& rhs) const;
    bool operator> (visibility const& rhs) const;

public:
    distance_unit unit;
    uint32_t      distance;
};

//-----------------------------------------------------------------------------

class altimeter
{
public:
    typedef std::shared_ptr<altimeter> pointer;
    typedef std::unique_ptr<altimeter> unique_pointer;

    altimeter();
    altimeter(double pressure, pressure_unit unit = pressure_unit::hPa);

    altimeter(altimeter const& other) = default;
#if (_MSC_VER >= 1900)
    altimeter(altimeter && other) = default;
#endif // _MSC_VER >= 1900

    altimeter& operator= (altimeter const& rhs) = default;
#if (_MSC_VER >= 1900)
    altimeter& operator= (altimeter && rhs) = default;
#endif // _MSC_VER >= 1900

    bool operator== (altimeter const& rhs) const;
    bool operator!= (altimeter const& rhs) const;

    bool operator<= (altimeter const& rhs) const;
    bool operator>= (altimeter const& rhs) const;

    bool operator< (altimeter const& rhs) const;
    bool operator> (altimeter const& rhs) const;

public:
    pressure_unit unit;
    double        pressure;
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
#if (_MSC_VER >= 1900)
    observation_time(observation_time && other) = default;
#endif // _MSC_VER >= 1900

    observation_time& operator= (observation_time const& rhs) = default;
#if (_MSC_VER >= 1900)
    observation_time& operator= (observation_time && rhs) = default;
#endif // _MSC_VER >= 1900

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
#if (_MSC_VER >= 1900)
    wind(wind && other) = default;
#endif // _MSC_VER >= 1900

    wind& operator= (wind const& rhs) = default;
#if (_MSC_VER >= 1900)
    wind& operator= (wind && rhs) = default;
#endif // _MSC_VER >= 1900

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
#if (_MSC_VER >= 1900)
    runway_visual_range(runway_visual_range && other) = default;
#endif // _MSC_VER >= 1900

    runway_visual_range& operator=(runway_visual_range const& rhs) = default;
#if (_MSC_VER >= 1900)
    runway_visual_range& operator=(runway_visual_range && rhs) = default;
#endif // _MSC_VER >= 1900

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
#if (_MSC_VER >= 1900)
    weather(weather && other) = default;
#endif // _MSC_VER >= 1900

    weather& operator= (weather const& rhs) = default;
#if (_MSC_VER >= 1900)
    weather& operator= (weather && rhs) = default;
#endif // _MSC_VER >= 1900

    bool operator== (weather const& rhs) const;
    bool operator!= (weather const& rhs) const;

public:
    weather_intensity              intensity;
    weather_descriptor             descriptor;
    std::vector<weather_phenomena> phenomena;
};

//-----------------------------------------------------------------------------

class cloud_layer
{
public:
    typedef std::shared_ptr<cloud_layer> pointer;
    typedef std::unique_ptr<cloud_layer> unique_pointer;

    cloud_layer();

    cloud_layer(cloud_layer const& other) = default;
#if (_MSC_VER >= 1900)
    cloud_layer(cloud_layer && other) = default;
#endif // _MSC_VER >= 1900

    cloud_layer& operator= (cloud_layer const& rhs) = default;
#if (_MSC_VER >= 1900)
    cloud_layer& operator= (cloud_layer && rhs) = default;
#endif // _MSC_VER >= 1900

    bool operator== (cloud_layer const& rhs) const;
    bool operator!= (cloud_layer const& rhs) const;

public:
    sky_cover_type       sky_cover;
    uint32_t             layer_height;
    sky_cover_cloud_type cloud_type;
};

//-----------------------------------------------------------------------------

class metar_info
{
public:
    typedef std::shared_ptr<metar_info> pointer;
    typedef std::unique_ptr<metar_info> unique_pointer;

    metar_info(std::string const& metar);

    metar_info(metar_info const& other) = default;
#if (_MSC_VER >= 1900)
    metar_info(metar_info && other) = default;
#endif // _MSC_VER >= 1900

    metar_info& operator= (metar_info const& rhs) = default;
#if (_MSC_VER >= 1900)
    metar_info& operator= (metar_info && rhs) = default;
#endif // _MSC_VER >= 1900

    bool operator== (metar_info const& rhs) const;
    bool operator!= (metar_info const& rhs) const;

private:
    void parse();

public:
    std::string                      metar;
    report_type                      type;
    std::string                      station_identifier;
    observation_time                 report_time;
    modifier_type                    modifier;
    wind                             wind_group;
    visibility                       visibility;
    std::vector<runway_visual_range> runway_visual_range_group;
    std::vector<weather>             weather_group;
    std::vector<cloud_layer>         sky_condition_group;
    uint8_t                          temperature;
    uint8_t                          dewpoint;
    altimeter                        altimeter;
    std::string                      remarks;
};

//-----------------------------------------------------------------------------

} // namespace metar
} // namespace aw
