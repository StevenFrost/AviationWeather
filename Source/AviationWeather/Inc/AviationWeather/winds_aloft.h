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

struct report
{
    double wind_direction;
    double wind_speed;
    double temperature;
};

class station_report
{
public:
    optional<report> at(double altitude)
    {
        return optional<report>();
    }

private:
    std::vector<optional<report>> reports;
    std::weak_ptr<winds_aloft_report> parent;
};

class winds_aloft_report
{
public:
    station_report get(std::string const& identifier)
    {
        return station_report();
    }

private:
    std::vector<uint16_t> altitudes;
    std::vector<station_report> reports;
};

} // namespace winds_aloft
} // namespace aw
