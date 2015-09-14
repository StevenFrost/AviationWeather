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
namespace winds_aloft
{

class winds_aloft_report;

template <class T>
class optional
{
public:
};

class time
{
public:
    time(uint8_t hour, uint8_t minute);
    time(uint8_t day, uint8_t hour, uint8_t minute);

    time(time const& other) = default;
    time(time && other);

    time& operator= (time const& rhs) = default;
    time& operator= (time && rhs);

    bool operator== (time const& rhs) const;
    bool operator!= (time const& rhs) const;

public:
    optional<uint8_t> day_of_month;
    uint8_t hour_of_day;
    uint8_t minute_of_hour;
};


class report
{
public:
    report(double direction, double speed);
    report(double direction, double speed, double temperature, double isa_deviation);

    report(report const& other) = default;
    report(report && other);

    report& operator= (report const& rhs) = default;
    report& operator= (report && rhs);

    bool operator== (report const& rhs) const;
    bool operator!= (report const& rhs) const;

    bool is_light_and_variable() const;

public:
    double wind_direction;
    double wind_speed;
    optional<double> temperature;
    optional<double> isa_deviation;
};

class location
{
public:
    location(std::string const& text);

    location(location const& other) = default;
    location(location && other);

    location& operator= (location const& rhs) = default;
    location& operator= (location && rhs);

    bool operator== (location const& rhs) const;
    bool operator!= (location const& rhs) const;

    std::string iata_identifier() const;

    optional<report> at(double altitude) const;

private:
    std::string iata_identifier;
    std::vector<optional<report>> reports;
    std::weak_ptr<winds_aloft_report> parent;
};

class winds_aloft_report
{
public:
    winds_aloft_report(std::string const& text);

    winds_aloft_report(winds_aloft_report const& other) = default;
    winds_aloft_report(winds_aloft_report && other);

    winds_aloft_report& operator= (winds_aloft_report const& rhs) = default;
    winds_aloft_report& operator= (winds_aloft_report && rhs);

    bool operator== (winds_aloft_report const& rhs) const;
    bool operator!= (winds_aloft_report const& rhs) const;

    location get(std::string const& location_identifier);

private:
    friend class location;
    std::vector<uint16_t> altitudes;
    std::vector<location> reports;
};

} // namespace winds_aloft
} // namespace aw
