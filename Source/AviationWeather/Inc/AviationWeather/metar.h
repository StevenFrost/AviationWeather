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

#include <AviationWeather/components.h>
#include <AviationWeather/optional.h>
#include <AviationWeather/types.h>

//-----------------------------------------------------------------------------

namespace aw
{

//-----------------------------------------------------------------------------

enum class metar_element_type
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

enum class metar_report_type
{
    metar,
    special
};

enum class metar_modifier_type
{
    none,       // Automatic, someone reviewed the report
    automatic,  // Automatic, nobody reviewed the report
    corrected   // Corrected by someone
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
    uint8_t                runway_number;       // Runway number
    runway_designator_type runway_designator;   // Runway designator (L, R, C)
    visibility             visibility_min;      // Minimum visibility
    visibility             visibility_max;      // Maximum visibility
};

//-----------------------------------------------------------------------------

class metar
{
public:
    typedef std::shared_ptr<metar> pointer;
    typedef std::unique_ptr<metar> unique_pointer;

    metar(std::string const& metar);

    metar(metar const& other) = default;
    metar(metar && other);

    metar& operator= (metar const& rhs) = default;
    metar& operator= (metar && rhs);

    bool operator== (metar const& rhs) const;
    bool operator!= (metar const& rhs) const;

    cloud_layer ceiling() const;

    flight_category flight_category() const;
    int16_t temperature_dewpoint_spread() const;

private:
    void parse();
    cloud_layer ceiling_nothrow() const;

public:
    std::string                      raw_data;
    metar_report_type                type;
    station_identifier               identifier;
    time                             report_time;
    metar_modifier_type              modifier;
    util::optional<wind>             wind_group;
    util::optional<visibility>       visibility_group;
    std::vector<runway_visual_range> runway_visual_range_group;
    std::vector<weather>             weather_group;
    std::vector<cloud_layer>         sky_condition_group;
    util::optional<int8_t>           temperature;
    util::optional<int8_t>           dewpoint;
    util::optional<altimeter>        altimeter_group;
    std::string                      remarks;
};

//-----------------------------------------------------------------------------

} // namespace aw
