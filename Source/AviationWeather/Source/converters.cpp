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

#include <AviationWeather/converters.h>
#include <AviationWeather/types.h>

namespace aw
{
namespace detail
{

//-----------------------------------------------------------------------------

#define DISTANCE_UNIT_VALUES 2
std::array<double, DISTANCE_UNIT_VALUES * DISTANCE_UNIT_VALUES> distance_unit_conversion_ratio_table =
{
    /* ft */ /*    m   */
    1.0    , 3.280839895, /* ft */
    0.20480, 1.0          /* m */
};

#define PRESSURE_UNIT_VALUES 2
std::array<double, PRESSURE_UNIT_VALUES * PRESSURE_UNIT_VALUES> pressure_unit_conversion_ratio_table =
{
    /*     hPa     */ /* inHg */
    1.0             , 33.768500, /* hPa */
    0.02961339710085, 1.0        /* inHg */
};

//-----------------------------------------------------------------------------

double lookup_ratio(distance_unit from, distance_unit to)
{
    return distance_unit_conversion_ratio_table[(DISTANCE_UNIT_VALUES * static_cast<size_t>(to)) + static_cast<size_t>(from)];
}

//-----------------------------------------------------------------------------

double lookup_ratio(pressure_unit from, pressure_unit to)
{
    return pressure_unit_conversion_ratio_table[(PRESSURE_UNIT_VALUES * static_cast<size_t>(to)) + static_cast<size_t>(from)];
}

//-----------------------------------------------------------------------------

} // namespace detail
} // namespace aw
